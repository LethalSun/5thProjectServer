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
		//없다면 빈 플레이어 객체를 가져와서 세션을 플레이어와 1대1 대응 시켜준다.
		//연결은 세션의 인덱스를 플레이어 객체에 기록한다.
		//사용중으로 스테이트를 바꾸고
		//레이팅,승패수를 저장하고 나머지 값(게임 플레이에 필요한 값들)도 초기화를 해준다.
		
		//플레이어가 존재 하는 지 확인한다.
		if (findPlayer(pkt._sessionIdx) != -1)
		{
			//로그인 상태 인데 다시 로그인 끊어야 하나?
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

		///패킷을 보낸다.
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

		auto bodySize = body.length() + 1;//널 문자까지.

		MDServerNetLib::PacketRaw resPacket(sessionIdx, packetID, bodySize, body.c_str());

		_sendQue->push(resPacket);
		_logger->Write(MDUtillity::LogType::DEBUG, "%s | Packet SEND, Session(%d), Packet ID(%hd),size(%hd)", __FUNCTION__, sessionIdx, packetID, bodySize);
		return true;
	}

}
