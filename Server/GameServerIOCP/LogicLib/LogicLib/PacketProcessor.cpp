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
		//없다면 빈 플레이어 객체를 가져와서 세션을 플레이어와 1대1 대응 시켜준다.
		//연결은 세션의 인덱스를 플레이어 객체에 기록한다.
		//배치중으로 스테이트를 바꾸고
		//레이팅,승패수를 저장하고 나머지 값(게임 플레이에 필요한 값들)도 초기화를 해준다.
		
		//플레이어가 존재 하는 지 확인한다.
		if (findPlayer(pkt._sessionIdx) != -1)
		{
			//로그인 상태 인데 다시 로그인, 끊어야 하나?
			return false;
		}
		//존재 하지 않는 다면

		//새로운 플레이어를 할당받고
		auto newPlayer = _playerPool.AllocObject();

		//초기화 하고.
		newPlayer->Reset();

		//값을 써준다.

		///패킷을 풀어 주고
		Packet::GAMESEVER_REQ_GAMESERVER_ENTER packet;

		deserialize(pkt._body, &packet);

		///정보를 저장하고 우선 ID만
		newPlayer->SetID(packet.ID);
		newPlayer->SetSessionIdx(pkt._sessionIdx);
		newPlayer->SetState(static_cast<int>(PlayerState::DEPLOY));
		//TODO:레이팅 승패를 db에서 받아와서 세팅해준다.
		//그런데 원스레드인 로직에서 아이오를 하면 안되므로 iocp 워커 스레드에서 받고 로우패킷에 넣어서 보낸다.
		newPlayer->SetInfo(1200,10,10);//temp Info
		///패킷을 보낸다.
		Packet::GAMESEVER_RES_GAMESERVER_ENTER resPacket;

		resPacket.Result = (int)Result::OK;

		SendResPacket(newPlayer->GetSessionIdx(),
			Packet::PacketId::ID_GAMESEVER_RES_GAMESERVER_ENTER,
			&resPacket);
		
		//새로운 유저 입장 공지를 전체 유저에게 보낸다.

		return true;
	}

	bool PacketProcessor::onShipDeployInfoReq(MDServerNetLib::PacketRaw pkt)
	{

		Packet::GAMESEVER_RES_SHIP_DEPLOY_INFO resPacket;
		//요청이오면 세션으로부터 플레이어를 찾는다
		auto playerIndex = findPlayer(pkt._sessionIdx);
		if (playerIndex == -1)
		{
			//로그인 상태 가아니다. 끊어 버려야 된다.
			return false;
		}
		
		if (_playerPool[playerIndex].IsDeploy() != true)
		{
			//상태가 맞지 않는 패킷이 왔으므로 끊어 버려야 된다.
			return false;
		}

		Packet::GAMESEVER_REQ_SHIP_DEPLOY_INFO packet;

		deserialize(pkt._body, &packet);

		//패킷의 문자열로 부터 문자열의 문자 위치에 해당하는 타일 배열상의 위치의 타일에
		//배가 있는지 없는지 기록한다. 배가 있다면 1 없다면 0값을 갖는다.
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
		//게임 레디 요청이오면 세션으로부터 플레이어를 찾는다

		Packet::GAMESEVER_RES_GAME_READY resPacket;
		//요청이오면 세션으로부터 플레이어를 찾는다
		auto playerIndex = findPlayer(pkt._sessionIdx);
		if (playerIndex == -1)
		{
			//로그인 상태 가아니다. 끊어 버려야 된다.
			return false;
		}

		if (_playerPool[playerIndex].IsDeploy() != true)
		{
			//상태가 맞지 않는 패킷이 왔으므로 끊어 버려야 된다.
			return false;
		}

		if (_playerPool[playerIndex].IsDeployCorrect() == false)
		{
			//배치 정보만 이상한경우 내부적으로 뭔가오류가 있는것. 일단 실패 패킷을 보낸다.
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
		//요청이오면 세션으로부터 플레이어를 찾는다
		auto playerIndex = findPlayer(pkt._sessionIdx);
		if (playerIndex == -1)
		{
			//로그인 상태 가아니다. 끊어 버려야 된다.
			return false;
		}

		if (_playerPool[playerIndex].IsPlaying() != true)
		{
			//상태가 맞지 않는 패킷이 왔으므로 끊어 버려야 된다.
			return false;
		}

		auto enemy = _playerPool[playerIndex].GetEnemy();

		if (_playerPool[playerIndex].IsDeployCorrect() == false)
		{
			//배치 정보만 이상한경우 내부적으로 뭔가오류가 있는것. 일단 실패 패킷을 보낸다.
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
		//세션으로부터 플레이어를 찾는다
		auto playerIndex = findPlayer(pkt._sessionIdx);
		if (playerIndex == -1)
		{
			//로그인 상태 가아니다. 끊어 버려야 된다. 근데 끊어져 있다.
			return false;
		}

		if (_playerPool[playerIndex].IsPlaying() == true)
		{
			auto enemy = _playerPool[playerIndex].GetEnemy();

			_playerPool[playerIndex].GetMatchInfo()->Reset();
			_matchPool.FreeObject(_playerPool[playerIndex].GetMatchInfo()->GetIndex());
			
			//상태가 맞지 않는 패킷이 왔으므로 끊어 버려야 된다.
			Packet::GAMESEVER_NTF_GAMEND endNtfPacket;

			endNtfPacket.Result = (int)Result::WIN;

			//TODO:내 정보의 패를 추가해야 한다.

			SendResPacket(enemy->GetSessionIdx(),
				Packet::PacketId::ID_GAMESEVER_NTF_GAMEND,
				&endNtfPacket);
			//플레이어의 매치와 플레이어를 리셋한다.
		}

		_playerPool[playerIndex].Reset();
		//세션을 정리하도록 특정 내부 패킷을 보낸다.

		//IOCP 작업 스레드에서 DB에서 아이디에대한 Authkey를 삭제하고 세션을 정리한다.

		_playerPool.FreeObject(playerIndex);
		return false;
	}

	void PacketProcessor::MakeMatch()
	{
		//매치 큐를 확인한다 큐에2명 이상이 존재하면 매치를 생성한다.

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
			//TODO:내 정보의 패를 추가해야 한다.

			
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

		auto bodySize = body.length() + 1;//널 문자까지.

		MDServerNetLib::PacketRaw resPacket(sessionIdx, packetID, bodySize, body.c_str());

		_sendQue->push(resPacket);
		_logger->Write(MDUtillity::LogType::DEBUG, "%s | Packet SEND, Session(%d), Packet ID(%hd),size(%hd)", __FUNCTION__, sessionIdx, packetID, bodySize);
		return true;
	}

}
