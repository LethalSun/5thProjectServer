
#include "Session.h"

namespace MDServerNetLib
{
	Session::Session(int indexForPool, int recvBufLen, int sendBufLen)
		:_sendBuffer{sendBufLen},
		_recvBuffer{ recvBufLen },
		_maxRecvBufLen{ recvBufLen },
		_maxSendBufLen{ sendBufLen },
		_sessionPoolIndex{ indexForPool },
		index{ indexForPool }
	{

		ZeroMemory(&_socketContext.recvContext.overlapped, sizeof(WSAOVERLAPPED));
	}

	Session::~Session()
	{
	}

	//TODO: 버퍼는 세션내의 원형 버퍼에서 매번 보낼때 가져온다.
}