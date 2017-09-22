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

		//TODO:�÷��̾� pool
		PlayerManager _playerPool;

		//TODO:�÷��̾�� Ÿ�������� ���´� Ÿ���� �谡 �ִ��� �������� ������ �ִ�.
		//�÷��̾�� ��ġ����(�ε���)�� ���´�. ��Ŷ�� ���� �������κ��� �÷��̾ �÷��̾�κ��� ��ġ(�ε���)�� �˾Ƴ���.

		//TODO:��ġ Ǯ pool �÷��̾� �ִ���� �ݸ�ŭ �����Ѵ�.
		MatchManager _matchPool;
		//TODO:��ġ�� �÷��̾� ���� ���´�.
		//�������� �÷��̾�� �ƴ� �÷��̾ ����Ѵ�.
		//���� �ٲ�� �������� �÷��̾�� �ƴ��÷��̾ �ٲ۴�.

		//TODO:�÷��̾�� �����ð����� ��Ʈ��Ʈ�� ������ ������ �����Ų��.
	};
}


