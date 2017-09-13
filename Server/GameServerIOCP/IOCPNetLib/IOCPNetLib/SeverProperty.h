#pragma once

namespace MDServerNetLib
{
	class ServerProperty
	{
	public:
		int MaxClientCount = 0;
		int ExtraClientCount = 0;
		int MaxClientRecvBufferSize = 0;
		int MaxClientSendBufferSize = 0;
		int MAX_PACKET_BODY_SIZE = 1024;
		unsigned short Port;
	};
}




