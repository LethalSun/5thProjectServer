#include "pch.h"
#include "PacketProcessor.h"

namespace LogicLib
{
	PacketProcessor::PacketProcessor(const MDServerNetLib::ServerProperty*  prop,
		MDUtillity::LoggerBase* logger,
		MDServerNetLib::PacketQueueConccurency* recvQue,
		MDServerNetLib::PacketQueueConccurency* sendQue)
		:_recvQue{ recvQue }, _sendQue{ sendQue }, _playerPool{ prop->MaxClientCount},
		_matchPool{ prop->MaxClientCount /2 }
	{

	}


	PacketProcessor::~PacketProcessor()
	{
	}

}
