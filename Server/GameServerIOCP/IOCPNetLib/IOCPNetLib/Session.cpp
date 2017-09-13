
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
		//TODO: 초기 wasbuf 등록 해주기. 초기화 도 마찬가지.
		ZeroMemory(&_socketContext.recvContext.overlapped, sizeof(WSAOVERLAPPED));
		InitializeCriticalSectionAndSpinCount(&_spinlockMutex, 4000);//4000 is recomended
	}

	Session::~Session()
	{
		DeleteCriticalSection(&_spinlockMutex);
	}

	//TODO: 버퍼는 세션내의 원형 버퍼에서 매번 보낼때 가져온다.
}