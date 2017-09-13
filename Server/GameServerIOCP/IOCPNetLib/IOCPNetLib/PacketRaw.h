#pragma once
#include <memory>
#include "../../Common/Packet.h"
#include "SeverProperty.h"
namespace MDServerNetLib
{
#pragma pack(push, 1)

	struct PacketHeader
	{
		int _id;
		int _bodySize;
	};

	constexpr short PacketHeaderSize = sizeof(PacketHeader);

	constexpr int MAX_PACKET_BODY_SIZE = 1024;
	struct PacketRaw
	{
		void SetPacketRaw(Packet::PacketId id, int sessionIdx, int bodySize, const char * body)
		{
			_packetId = id;
			_sessionIdx = sessionIdx;
			_bodySize = bodySize;
			strcpy_s(_body, _bodySize, body);
		}

		void EmptyPacket()
		{
			_packetId = -1;
			_sessionIdx = -1;
			_bodySize = -1;
			memset(_body, 0, MAX_PACKET_BODY_SIZE);
		}

		int _sessionIdx = 0;
		short _packetId = 0;
		short _bodySize = 0;
		char  _body[MAX_PACKET_BODY_SIZE];
	};

#pragma pack(pop)
}