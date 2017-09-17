#pragma once
#include <memory>
#include <string>
#include <concurrent_queue.h>
#include "../../Common/Packet.h"
#include "ServerProperty.h"

namespace MDServerNetLib
{
#pragma pack(push, 1)

	struct PacketHeader
	{
		int _id;
		int _bodySize;
	};

#pragma pack(pop)
	constexpr short PacketHeaderSize = sizeof(PacketHeader);

	constexpr int MAX_PACKET_BODY_SIZE = 1024;

	struct PacketRaw
	{
		PacketRaw(int sessionIdx, short id, short bodySize, const char * body)
			: _packetId{ id },
			_sessionIdx{sessionIdx},
			_bodySize{ bodySize },
			_body{ body }
		{}

		PacketRaw() = default;

		void EmptyPacket()
		{
			_packetId = -1;
			_sessionIdx = -1;
			_bodySize = -1;
		}

		int _sessionIdx = 0;
		short _packetId = 0;
		short _bodySize = 0;
		std::string  _body;
	};


	using PacketQueueConccurency = concurrency::concurrent_queue<PacketRaw>;
}