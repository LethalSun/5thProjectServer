#pragma once
#include "../../Common/Packet.h"
#include "../../IOCPNetLib/IOCPNetLib/PacketRaw.h"
#include "../../IOCPNetLib/IOCPNetLib/Logger.h"
#include "../../IOCPNetLib/IOCPNetLib/ServerProperty.h"
#include "ObjectPool.h"
#include "Player.h"
#include "Match.h"
namespace LogicLib
{
	using PlayerManager = MDUtillity::ObjectPool<Player>;
	using MatchManager = MDUtillity::ObjectPool<Match>;

	enum class Result
	{
		Fail = -1,
		OK,
		TIMEOUT,
		HIT,
		WIN,
		YOURTURN,
	};

	class PacketProcessor
	{
	public:
		PacketProcessor(const MDServerNetLib::ServerProperty*  prop,
			MDUtillity::LoggerBase* logger,
			MDServerNetLib::PacketQueueConccurency* sendQue);
		~PacketProcessor();

		bool ProcessPacket(MDServerNetLib::PacketRaw pkt);


		void MakeMatch();

	private:
		bool onGameEnterReq(MDServerNetLib::PacketRaw pkt);
		bool onShipDeployInfoReq(MDServerNetLib::PacketRaw pkt);
		bool onGameReadyReq(MDServerNetLib::PacketRaw pkt);
		bool onBombReq(MDServerNetLib::PacketRaw pkt);
		bool onUserLogoutReq(MDServerNetLib::PacketRaw pkt);
		//bool onGameServerInfoReq(MDServerNetLib::PacketRaw pkt);
		//bool onUserHeartBeatReq(MDServerNetLib::PacketRaw pkt);

		int findPlayer(int sessionIdx);

		bool deserialize(std::string body, Packet::IJsonSerializable * outSturct);
		bool SendResPacket(int sessionId, short packetID, Packet::IJsonSerializable * inSturct);
	private:
		MDServerNetLib::ServerProperty  _property;
		MDUtillity::LoggerBase* _logger;
		MDServerNetLib::PacketQueueConccurency* _sendQue;

		PlayerManager _playerPool;

		std::deque<int> _readyPlayerIndices;

		MatchManager _matchPool;

		//TODO:플레이어는 일정시간동안 하트비트를 보내지 않으면 종료시킨다.


	};
}


