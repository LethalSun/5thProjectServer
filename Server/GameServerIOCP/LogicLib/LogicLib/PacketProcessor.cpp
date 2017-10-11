#include "pch.h"
#include "PacketProcessor.h"
#include "../../Common/Packet.h"
#include "../../IOCPNetLib/IOCPNetLib/PacketRaw.h"
namespace LogicLib
{
	PacketProcessor::PacketProcessor(const MDServerNetLib::ServerProperty*  prop,
		MDUtillity::LoggerBase* logger,
		MDServerNetLib::PacketQueueConccurency* sendQue)
		: _logger{ logger }, _sendQue{ sendQue }, _playerPool{ prop->MaxClientCount },
		_matchPool{ prop->MaxClientCount /2 }
	{
		_property.BackLog = prop->BackLog;
		_property.ExtraClientCount = prop->ExtraClientCount;
		_property.MaxClientCount = prop->MaxClientCount;
		_property.MaxClientRecvBufferSize = prop->MaxClientRecvBufferSize;
		_property.MaxClientSendBufferSize = prop->MaxClientSendBufferSize;
		_property.MAX_PACKET_BODY_SIZE = prop->MAX_PACKET_BODY_SIZE;
		_property.PortNum = prop->PortNum;

		for (int i = 0; i < prop->MaxClientCount; ++i)
		{
			_playerPool[i].InitIndex(i);
		}

		for (int i = 0; i < prop->MaxClientCount / 2; ++i)
		{
			_matchPool[i].InitIndex(i);
		}
	}


	PacketProcessor::~PacketProcessor()
	{

	}

	bool PacketProcessor::ProcessPacket(MDServerNetLib::PacketRaw pkt)
	{
		switch (pkt._packetId)
		{
			case Packet::PacketId::ID_GAMESEVER_REQ_GAMESERVER_ENTER:
			{
				onGameEnterReq(pkt);
				break;
			}
			case Packet::PacketId::ID_GAMESEVER_REQ_SHIP_DEPLOY_INFO:
			{
				onShipDeployInfoReq(pkt);
				break;
			}
			case Packet::PacketId::ID_GAMESEVER_REQ_GAMESERVER_INFO:
			{
				//onGameServerInfoReq(pkt);
				break;
			}
			case Packet::PacketId::ID_GAMESEVER_REQ_GAME_READY:
			{
				onGameReadyReq(pkt);
				break;
			}
			case Packet::PacketId::ID_GAMESEVER_REQ_BOMB:
			{
				onBombReq(pkt);
				break;
			}
			case Packet::PacketId::ID_GAMSERVER_REQ_USER_HEARTBEAT:
			{
				//onUserHeartBeatReq(pkt);
				break;
			}
			case Packet::PacketId::ID_GAMSERVER_REQ_USER_LOGOUT:
			{
				onUserLogoutReq(pkt);
				break;
			}
		}
		return true;
	}


	bool PacketProcessor::onGameEnterReq(MDServerNetLib::PacketRaw pkt)
	{
		//���ٸ� �� �÷��̾� ��ü�� �����ͼ� ������ �÷��̾�� 1��1 ���� �����ش�.
		//������ ������ �ε����� �÷��̾� ��ü�� ����Ѵ�.
		//��ġ������ ������Ʈ�� �ٲٰ�
		//������,���м��� �����ϰ� ������ ��(���� �÷��̿� �ʿ��� ����)�� �ʱ�ȭ�� ���ش�.
		
		//�÷��̾ ���� �ϴ� �� Ȯ���Ѵ�.
		if (findPlayer(pkt._sessionIdx) != -1)
		{
			//�α��� ���� �ε� �ٽ� �α���, ����� �ϳ�?
			return false;
		}
		//���� ���� �ʴ� �ٸ�

		//���ο� �÷��̾ �Ҵ�ް�
		auto newPlayer = _playerPool.AllocObject();

		//�ʱ�ȭ �ϰ�.
		newPlayer->Reset();

		//���� ���ش�.

		///��Ŷ�� Ǯ�� �ְ�
		Packet::GAMESEVER_REQ_GAMESERVER_ENTER packet;

		deserialize(pkt._body, &packet);

		///������ �����ϰ� �켱 ID��
		newPlayer->SetID(packet.ID);
		newPlayer->SetSessionIdx(pkt._sessionIdx);
		newPlayer->SetState(static_cast<int>(PlayerState::DEPLOY));
		//TODO:������ ���и� db���� �޾ƿͼ� �������ش�.
		//�׷��� ���������� �������� ���̿��� �ϸ� �ȵǹǷ� iocp ��Ŀ �����忡�� �ް� �ο���Ŷ�� �־ ������.
		newPlayer->SetInfo(1200,10,10);//temp Info
		///��Ŷ�� ������.
		Packet::GAMESEVER_RES_GAMESERVER_ENTER resPacket;

		resPacket.Result = (int)Result::OK;

		SendResPacket(newPlayer->GetSessionIdx(),
			Packet::PacketId::ID_GAMESEVER_RES_GAMESERVER_ENTER,
			&resPacket);
		
		//���ο� ���� ���� ������ ��ü �������� ������.

		return true;
	}

	bool PacketProcessor::onShipDeployInfoReq(MDServerNetLib::PacketRaw pkt)
	{

		Packet::GAMESEVER_RES_SHIP_DEPLOY_INFO resPacket;
		//��û�̿��� �������κ��� �÷��̾ ã�´�
		auto playerIndex = findPlayer(pkt._sessionIdx);
		if (playerIndex == -1)
		{
			//�α��� ���� ���ƴϴ�. ���� ������ �ȴ�.
			return false;
		}
		
		if (_playerPool[playerIndex].IsDeploy() != true)
		{
			//���°� ���� �ʴ� ��Ŷ�� �����Ƿ� ���� ������ �ȴ�.
			return false;
		}

		Packet::GAMESEVER_REQ_SHIP_DEPLOY_INFO packet;

		deserialize(pkt._body, &packet);

		//��Ŷ�� ���ڿ��� ���� ���ڿ��� ���� ��ġ�� �ش��ϴ� Ÿ�� �迭���� ��ġ�� Ÿ�Ͽ�
		//�谡 �ִ��� ������ ����Ѵ�. �谡 �ִٸ� 1 ���ٸ� 0���� ���´�.
		packet.ShipOnTileInfo;

		if (packet.ShipOnTileInfo.length() != packet.NumOfTile)
		{
			resPacket.Result = (int)Result::Fail;
			SendResPacket(_playerPool[playerIndex].GetSessionIdx(),
				Packet::PacketId::ID_GAMESEVER_RES_SHIP_DEPLOY_INFO,
				&resPacket);
			return false;
		}

		if (_playerPool[playerIndex].SetDeployInfo(packet.ShipOnTileInfo) == false)
		{
			resPacket.Result = (int)Result::Fail;
			SendResPacket(_playerPool[playerIndex].GetSessionIdx(),
				Packet::PacketId::ID_GAMESEVER_RES_SHIP_DEPLOY_INFO,
				&resPacket);
			return false;
		}


		resPacket.Result = (int)Result::OK;

		SendResPacket(_playerPool[playerIndex].GetSessionIdx(),
			Packet::PacketId::ID_GAMESEVER_RES_SHIP_DEPLOY_INFO,
			&resPacket);

		return true;
	}

	bool PacketProcessor::onGameReadyReq(MDServerNetLib::PacketRaw pkt)
	{
		//���� ���� ��û�̿��� �������κ��� �÷��̾ ã�´�

		Packet::GAMESEVER_RES_GAME_READY resPacket;
		//��û�̿��� �������κ��� �÷��̾ ã�´�
		auto playerIndex = findPlayer(pkt._sessionIdx);
		if (playerIndex == -1)
		{
			//�α��� ���� ���ƴϴ�. ���� ������ �ȴ�.
			return false;
		}

		if (_playerPool[playerIndex].IsDeploy() != true)
		{
			//���°� ���� �ʴ� ��Ŷ�� �����Ƿ� ���� ������ �ȴ�.
			return false;
		}

		if (_playerPool[playerIndex].IsDeployCorrect() == false)
		{
			//��ġ ������ �̻��Ѱ�� ���������� ���������� �ִ°�. �ϴ� ���� ��Ŷ�� ������.
			resPacket.Result = (int)Result::Fail;
			SendResPacket(_playerPool[playerIndex].GetSessionIdx(),
				Packet::PacketId::ID_GAMESEVER_RES_GAME_READY,
				&resPacket);
			return false;
		}


		Packet::GAMESEVER_REQ_GAME_READY packet;

		deserialize(pkt._body, &packet);
		
		_playerPool[playerIndex].SetState(static_cast<int>(PlayerState::MATCHING));

		_readyPlayerIndices.push_back(playerIndex);

		resPacket.Result = (int)Result::OK;

		SendResPacket(_playerPool[playerIndex].GetSessionIdx(),
			Packet::PacketId::ID_GAMESEVER_RES_GAME_READY,
			&resPacket);
		return true;
	}

	bool PacketProcessor::onBombReq(MDServerNetLib::PacketRaw pkt)
	{
		Packet::GAMESEVER_RES_BOMB resPacket;
		Packet::GAMESEVER_NTF_BOMB ntfPacket;
		//��û�̿��� �������κ��� �÷��̾ ã�´�
		auto playerIndex = findPlayer(pkt._sessionIdx);
		if (playerIndex == -1)
		{
			//�α��� ���� ���ƴϴ�. ���� ������ �ȴ�.
			return false;
		}

		if (_playerPool[playerIndex].IsPlaying() != true)
		{
			//���°� ���� �ʴ� ��Ŷ�� �����Ƿ� ���� ������ �ȴ�.
			return false;
		}

		auto enemy = _playerPool[playerIndex].GetEnemy();

		if (_playerPool[playerIndex].IsDeployCorrect() == false)
		{
			//��ġ ������ �̻��Ѱ�� ���������� ���������� �ִ°�. �ϴ� ���� ��Ŷ�� ������.
			resPacket.Result = (int)Result::Fail;
			ntfPacket.Result = (int)Result::Fail;
			SendResPacket(_playerPool[playerIndex].GetSessionIdx(),
				Packet::PacketId::ID_GAMESEVER_RES_BOMB,
				&resPacket);

			SendResPacket(enemy->GetSessionIdx(),
				Packet::PacketId::ID_GAMESEVER_NTF_BOMB,
				&ntfPacket);
			return false;
		}

		Packet::GAMESEVER_REQ_BOMB packet;

		deserialize(pkt._body, &packet);

		if (_playerPool[playerIndex].Bomb(packet.BombedTile) == true)
		{
			resPacket.Result = (int)Result::HIT;
			ntfPacket.Result = (int)Result::HIT;
			resPacket.BombedTile = packet.BombedTile;
			ntfPacket.BombedTile = packet.BombedTile;
		}
		else
		{
			resPacket.Result = (int)Result::OK;
			ntfPacket.Result = (int)Result::OK;
			resPacket.BombedTile = packet.BombedTile;
			ntfPacket.BombedTile = packet.BombedTile;
		}

		SendResPacket(_playerPool[playerIndex].GetSessionIdx(),
			Packet::PacketId::ID_GAMESEVER_RES_BOMB,
			&resPacket);

		SendResPacket(enemy->GetSessionIdx(),
			Packet::PacketId::ID_GAMESEVER_NTF_BOMB,
			&ntfPacket);

		if (_playerPool[playerIndex].GetMatchInfo()->IsEndGame() == false)
		{
			return true;
		}

		Packet::GAMESEVER_NTF_GAMEND endNtfPacket;

		endNtfPacket.Result = (int)Result::WIN;

		SendResPacket(_playerPool[playerIndex].GetSessionIdx(),
			Packet::PacketId::ID_GAMESEVER_NTF_GAMEND,
			&endNtfPacket);

		endNtfPacket.Result = (int)Result::OK;

		SendResPacket(enemy->GetSessionIdx(),
			Packet::PacketId::ID_GAMESEVER_NTF_GAMEND,
			&endNtfPacket);

		return true;
	}

	bool PacketProcessor::onUserLogoutReq(MDServerNetLib::PacketRaw pkt)
	{
		//�������κ��� �÷��̾ ã�´�
		auto playerIndex = findPlayer(pkt._sessionIdx);
		if (playerIndex == -1)
		{
			//�α��� ���� ���ƴϴ�. ���� ������ �ȴ�. �ٵ� ������ �ִ�.
			return false;
		}

		if (_playerPool[playerIndex].IsPlaying() == true)
		{
			auto enemy = _playerPool[playerIndex].GetEnemy();

			_playerPool[playerIndex].GetMatchInfo()->Reset();
			_matchPool.FreeObject(_playerPool[playerIndex].GetMatchInfo()->GetIndex());
			
			//���°� ���� �ʴ� ��Ŷ�� �����Ƿ� ���� ������ �ȴ�.
			Packet::GAMESEVER_NTF_GAMEND endNtfPacket;

			endNtfPacket.Result = (int)Result::WIN;

			//TODO:�� ������ �и� �߰��ؾ� �Ѵ�.

			SendResPacket(enemy->GetSessionIdx(),
				Packet::PacketId::ID_GAMESEVER_NTF_GAMEND,
				&endNtfPacket);
			//�÷��̾��� ��ġ�� �÷��̾ �����Ѵ�.
		}

		_playerPool[playerIndex].Reset();
		//������ �����ϵ��� Ư�� ���� ��Ŷ�� ������.

		//IOCP �۾� �����忡�� DB���� ���̵𿡴��� Authkey�� �����ϰ� ������ �����Ѵ�.

		_playerPool.FreeObject(playerIndex);
		return false;
	}

	void PacketProcessor::MakeMatch()
	{
		//��ġ ť�� Ȯ���Ѵ� ť��2�� �̻��� �����ϸ� ��ġ�� �����Ѵ�.

		if (_readyPlayerIndices.size() <= 1)
		{
			return;
		}

		int newMatchCount = _readyPlayerIndices.size();
			

		while (newMatchCount > 1)
		{
			int player1 = _readyPlayerIndices.front();
			_readyPlayerIndices.pop_front();

			int player2 = _readyPlayerIndices.front();
			_readyPlayerIndices.pop_front();

			auto newMatch = _matchPool.AllocObject();

			newMatch->SetPlayers(&_playerPool[player1], &_playerPool[player2]);

			newMatchCount -= 2;

			Packet::GAMESEVER_NTF_GAME_START startNtfPacket;
			Packet::GAMESEVER_NTF_GAME_START startNtfPacket2;
			startNtfPacket.Result = (int)Result::YOURTURN;
			startNtfPacket2.Result = (int)Result::OK;
			if (newMatch->IsMyTurn(&_playerPool[player1]) == true)
			{
				SendResPacket(_playerPool[player1].GetSessionIdx(),
					Packet::PacketId::ID_GAMESEVER_NTF_GAME_START,
					&startNtfPacket);

				SendResPacket(_playerPool[player2].GetSessionIdx(),
					Packet::PacketId::ID_GAMESEVER_NTF_GAME_START,
					&startNtfPacket2);
			}
			else
			{
				SendResPacket(_playerPool[player1].GetSessionIdx(),
					Packet::PacketId::ID_GAMESEVER_NTF_GAME_START,
					&startNtfPacket2);

				SendResPacket(_playerPool[player2].GetSessionIdx(),
					Packet::PacketId::ID_GAMESEVER_NTF_GAME_START,
					&startNtfPacket);
			}
			//TODO:�� ������ �и� �߰��ؾ� �Ѵ�.

			
		}
	}

	int PacketProcessor::findPlayer(int sessionIdx)
	{
		for (int i = 0; i < _playerPool.GetMaxObjectNum(); ++i)
		{
			if (_playerPool[i].IsUsed() != true)
			{
				continue;
			}

			if (_playerPool[i].GetSessionIdx() == sessionIdx)
			{
				return i;
			}
		}
		return -1;
	}

	bool PacketProcessor::deserialize(std::string body, Packet::IJsonSerializable * packet)
	{
		Json::Value root;
		Json::Reader reader;

		reader.parse(body.c_str(), root);

		packet->Deserialize(root);
		return true;
	}

	bool PacketProcessor::SendResPacket(int sessionIdx, short packetID, Packet::IJsonSerializable * inSturct)
	{
		auto body = std::string();

		Packet::CJsonSerializer::Serialize(inSturct, body);

		auto bodySize = body.length() + 1;//�� ���ڱ���.

		MDServerNetLib::PacketRaw resPacket(sessionIdx, packetID, bodySize, body.c_str());

		_sendQue->push(resPacket);
		_logger->Write(MDUtillity::LogType::DEBUG, "%s | Packet SEND, Session(%d), Packet ID(%hd),size(%hd)", __FUNCTION__, sessionIdx, packetID, bodySize);
		return true;
	}

}
