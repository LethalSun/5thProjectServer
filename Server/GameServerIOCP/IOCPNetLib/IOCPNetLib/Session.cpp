
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
		//TODO: �ʱ� wasbuf ��� ���ֱ�. �ʱ�ȭ �� ��������.
		ZeroMemory(&_socketContext.recvContext.overlapped, sizeof(WSAOVERLAPPED));
		InitializeCriticalSectionAndSpinCount(&_spinlockMutex, 4000);//4000 is recomended
	}

	Session::~Session()
	{
		DeleteCriticalSection(&_spinlockMutex);
	}

	//TODO: ���۴� ���ǳ��� ���� ���ۿ��� �Ź� ������ �����´�.
}