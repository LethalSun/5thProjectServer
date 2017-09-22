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

	class PacketProcessor
	{
	public:
		PacketProcessor(const MDServerNetLib::ServerProperty*  prop,
			MDUtillity::LoggerBase* logger,
			MDServerNetLib::PacketQueueConccurency* recvQue,
			MDServerNetLib::PacketQueueConccurency* sendQue);
		~PacketProcessor();



	private:
		MDServerNetLib::PacketQueueConccurency* _recvQue;
		MDServerNetLib::PacketQueueConccurency* _sendQue;

		//TODO:플레이어 pool
		PlayerManager _playerPool;

		//TODO:플레이어는 타일정보를 갖는다 타일은 배가 있는지 없는지의 정보가 있다.
		//플레이어는 매치정보(인덱스)를 갖는다. 패킷이 오면 세션으로부터 플레이어를 플레이어로부터 매치(인덱스)를 알아낸다.

		//TODO:매치 풀 pool 플레이어 최대수의 반만큼 존재한다.
		MatchManager _matchPool;
		//TODO:매치는 플레이어 둘을 갖는다.
		//공격중인 플레이어와 아닌 플레이어를 기록한다.
		//턴이 바뀌면 공격중인 플레이어와 아닌플레이어를 바꾼다.

		//TODO:플레이어는 일정시간동안 하트비트를 보내지 않으면 종료시킨다.
	};
}


