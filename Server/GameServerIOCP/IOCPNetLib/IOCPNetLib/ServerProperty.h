#pragma once
namespace MDServerNetLib
{

	class ServerProperty
	{
	public:
		unsigned short PortNum;
		int BackLog = 0;
		int MaxClientCount = 0;
		int ExtraClientCount = 0;
		int MaxClientRecvBufferSize = 0;
		int MaxClientSendBufferSize = 0;
		int MAX_PACKET_BODY_SIZE = 1024;
	};

}
