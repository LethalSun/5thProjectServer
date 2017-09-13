#pragma once
#include <WinSock2.h>
#include <atomic>
#include <Windows.h>
#include "CircularBuffer.h"
#include "SeverProperty.h"

namespace MDServerNetLib
{
	enum class DisconnectReason
	{
		DR_NONE,
		DR_ACTIVE,
		DR_ONCONNECT_ERROR,
		DR_IO_REQUEST_ERROR,
		DR_COMPLETION_ERROR,
		DR_SENDFLUSH_ERROR
	};
	enum class IOType
	{
		IO_NONE,
		IO_SEND,
		IO_RECV,
	};

	struct IOContext
	{
		WSAOVERLAPPED	overlapped;
		IOType			sessionIOtype;
		WSABUF			wsaBuf;
	};

	struct SocketContext
	{
		SOCKET			clntSocket = INVALID_SOCKET;
		IOContext	recvContext;
		IOContext	sendContext;
	};

	class Session
	{
	public:
		Session(int indexForPool, int recvBufLen, int sendBufLen);
		~Session();

		bool IsConnected()const { return _isConnected.load(); }
		void SetConnected(bool boolean) { _isConnected.store(boolean); }

		bool PostSendBuffer(PacketRaw& pkt);
		bool FlushSendBuffer();

		bool PostRecv();

		void CompleteSend(DWORD transferred);
		void CompleteRecv(DWORD transferred);

		void AddRef();
		void ReleaseRef();

		void DisconnectSync();

		DisconnectReason GetDisconnectReason() { return _disconnectReason; }
		bool SetDisconnectReason(DisconnectReason dr);

		bool Reset();
		
	private:
		void release();

	public:
		const int index;
		//오버랩디드 구조체를 가지고 있다.
		SocketContext _socketContext;
		
		//send 버퍼를 가지고 있다.
		///각 버퍼는 세션의 post,flush 하는 부분에서 락을 걸어야 한다.
		///아토믹으로 최소한의 락으로 동기화 하는것은 일단 완성하고 고민해 보자.
		///대신 스핀락과 크리티컬 섹션이 결합된 동기화 객체를 사용한다.
		MDUtillity::CircularBuffer _sendBuffer;
		MDUtillity::CircularBuffer _recvBuffer;
	private:
		const int _maxRecvBufLen;
		const int _maxSendBufLen;
		const int _sessionPoolIndex;

		CRITICAL_SECTION _spinlockMutex;

		int _sendBufRemainLenth = 0;
		int _sendBufStartPos = 0;

		int _recvBufRemainLenth = 0;
		int _recvBUfStartPos = 0;

		std::atomic<bool> _isSendAvailable{ false };
		std::atomic<bool> _isConnected{ false };
		//비동기 입출력 을 할때마다 증가 시키고 완료 될때 마다 감소 시켜서 0일때만
		//세션을 풀로 돌려 보낸다.
		std::atomic<long> _refCount{ 0 };

		DisconnectReason _disconnectReason{ DisconnectReason::DR_NONE };
	};

}

