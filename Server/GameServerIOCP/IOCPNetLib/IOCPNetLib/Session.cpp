
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

	//TODO: ���۴� ���ǳ��� ���� ���ۿ��� �Ź� ������ �����´�.
}