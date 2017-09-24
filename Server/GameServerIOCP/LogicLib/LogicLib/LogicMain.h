#pragma once

#include "../../IOCPNetLib/IOCPNetLib/Logger.h"
#include "../../IOCPNetLib/IOCPNetLib/ServerProperty.h"
#include "PacketProcessor.h"
#include "../../IOCPNetLib/IOCPNetLib/PacketRaw.h"
#include <thread>
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

		void StartLogicThreadFunc();

	private:

		PacketProcessor _packetProcessor;

		MDServerNetLib::ServerProperty _property;

		void logicThreadFunc();

		std::thread _logicThread;

		MDServerNetLib::PacketQueueConccurency* _recvQue;
	};

}


