#pragma once

#include "../../IOCPNetLib/IOCPNetLib/Logger.h"
#include "../../IOCPNetLib/IOCPNetLib/ServerProperty.h"
#include "PacketProcessor.h"
#include "../../IOCPNetLib/IOCPNetLib/PacketRaw.h"

namespace LogicLib
{
	class LogicMain
	{
	public:
		LogicMain(const MDServerNetLib::ServerProperty*  prop,
			MDUtillity::LoggerBase* logger,
			MDServerNetLib::PacketQueueConccurency* recvQue,
			MDServerNetLib::PacketQueueConccurency* sendQue);
		~LogicMain();

		void Run();

	private:

		PacketProcessor _packetProcessor;
		MDServerNetLib::ServerProperty _property;

		void logicThreadFunc();

		//TODO:�÷��̾� pool

		//TODO:�÷��̾�� Ÿ�������� ���´� Ÿ���� �谡 �ִ��� �������� ������ �ִ�.
		//�÷��̾�� ��ġ����(�ε���)�� ���´�. ��Ŷ�� ���� �������κ��� �÷��̾ �÷��̾�κ��� ��ġ(�ε���)�� �˾Ƴ���.

		//TODO:��ġ Ǯ pool �÷��̾� �ִ���� �ݸ�ŭ �����Ѵ�.

		//TODO:��ġ�� �÷��̾� ���� ���´�.
		//�������� �÷��̾�� �ƴ� �÷��̾ ����Ѵ�.
		//���� �ٲ�� �������� �÷��̾�� �ƴ��÷��̾ �ٲ۴�.

		//TODO:�÷��̾�� �����ð����� ��Ʈ��Ʈ�� ������ ������ �����Ų��.


	};

}


