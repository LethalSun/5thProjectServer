#include "pch.h"
#include "LogicMain.h"


namespace LogicLib
{
	LogicMain::LogicMain(const MDServerNetLib::ServerProperty*  prop,
		MDUtillity::LoggerBase* logger,
		MDServerNetLib::PacketQueueConccurency* recvQue,
		MDServerNetLib::PacketQueueConccurency* sendQue)
		:_packetProcessor(prop, logger, recvQue, sendQue)
	{
	}


	LogicMain::~LogicMain()
	{
	}

}

