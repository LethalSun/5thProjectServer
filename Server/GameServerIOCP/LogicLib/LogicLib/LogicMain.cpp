#include "pch.h"
#include "LogicMain.h"


namespace LogicLib
{
	LogicMain::LogicMain(const MDServerNetLib::ServerProperty*  prop,
		MDUtillity::LoggerBase* logger,
		MDServerNetLib::PacketQueueConccurency* recvQue,
		MDServerNetLib::PacketQueueConccurency* sendQue)
		:_packetProcessor(prop, logger, sendQue)
	{
	}


	LogicMain::~LogicMain()
	{
		_logicThread.join();
	}

	void LogicMain::StartLogicThreadFunc()
	{
		_logicThread = std::thread([&]() 
		{
			logicThreadFunc();
		});
	}

	void LogicMain::logicThreadFunc()
	{
		while (true)
		{
			MDServerNetLib::PacketRaw pkt;
			
			if (_recvQue->try_pop(pkt))
			{
				_packetProcessor.ProcessPacket(pkt);
			}

		}
	}

}

