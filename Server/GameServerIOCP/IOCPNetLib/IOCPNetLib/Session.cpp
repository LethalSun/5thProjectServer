#include "pch.h"
#include "Session.h"

namespace MDServerNetLib
{
	Session::Session(int indexForPool, int recvBufLen, int sendBufLen)
		:_sendBuffer{sendBufLen},
		_recvBuffer{ recvBufLen },
		_maxRecvBufLen{ recvBufLen },
		_maxSendBufLen{ sendBufLen },
		_sessionPoolIndex{ indexForPool }
	{
		ZeroMemory(&_socketContext.recvContext.overlapped, sizeof(WSAOVERLAPPED));
		_socketContext.recvContext.wsaBuf.buf = _socketContext.recvContext.Buffer = new char[_maxRecvBufLen];
		_socketContext.recvContext.wsaBuf.len = _maxRecvBufLen;
		_socketContext.sendContext.wsaBuf.buf = _socketContext.sendContext.Buffer = new char[_maxSendBufLen];
		_socketContext.sendContext.wsaBuf.len = _maxSendBufLen;
	}

	Session::~Session()
	{
		delete[] _socketContext.recvContext.Buffer;
		delete[] _socketContext.sendContext.Buffer;
	}
}