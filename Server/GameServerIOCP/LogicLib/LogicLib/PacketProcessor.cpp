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
				onGameServerInfoReq(pkt);
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
				onUserHeartBeatReq(pkt);
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
		//��������� ������Ʈ�� �ٲٰ�
		//������,���м��� �����ϰ� ������ ��(���� �÷��̿� �ʿ��� ����)�� �ʱ�ȭ�� ���ش�.
		
		//�÷��̾ ���� �ϴ� �� Ȯ���Ѵ�.
		if (findPlayer(pkt._sessionIdx) != -1)
		{
			//�α��� ���� �ε� �ٽ� �α��� ����� �ϳ�?
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

		///��Ŷ�� ������.
		Packet::GAMESEVER_RES_GAMESERVER_ENTER resPacket;

		resPacket.Result = (int)Result::OK;

		SendResPacket(newPlayer->GetSessionIdx(),
			Packet::PacketId::ID_GAMESEVER_RES_GAMESERVER_ENTER,
			&resPacket);
		

		return true;
	}

	int PacketProcessor::findPlayer(int sessionIdx)
	{
		for (int i = 0; i < _playerPool.GetMaxObjectNum(); ++i)
		{
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
