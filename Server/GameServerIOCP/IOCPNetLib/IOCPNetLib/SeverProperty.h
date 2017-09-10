#pragma once
//#include "pch.h"

namespace MDServerNetLib
{
	class ServerProperty
	{
	public:
		int MaxClientCount = 0;
		int ExtraClientCount = 0;
		int MaxClientRecvBufferSize = 0;
		int MaxClientSendBufferSize = 0;
		unsigned short Port;
	};

	const int MAX_PACKET_BODY_SIZE = 1024; // 최대 패킷 보디 크기
	const int MaxPacketSize = 4096;
	
	struct PacketInfo
	{
		int SessionIndex = 0;
		short PacketId = 0;
		short PacketBodySize = 0;
		char Data[MAX_PACKET_BODY_SIZE];

	};

}




