#pragma once
#include <WinSock2.h>
#include <atomic>
#include <Windows.h>
#include "CircularBuffer.h"
#include "SeverProperty.h"
#include "PacketRaw.h"

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

	class Session
	{
	public:
		Session(int indexForPool, int recvBufLen, int sendBufLen);
		~Session();

		bool IsConnected()const { return _isConnected.load(); }
		void SetConnected(bool boolean) { _isConnected.store(boolean); }
		void SetSendable(bool boolean){_isSendAvailable.store(boolean); }

		bool PostSendBuffer(PacketRaw& pkt);
		bool FlushSendBuffer();

		bool PostRecv();

		void CompleteSend(DWORD transferred);
		void CompleteRecv(DWORD transferred);

		void AddRef();
		void ReleaseRef();

		bool Reset();
		
	private:
		bool release();

	public:
		const int index;
		SOCKET	_clntSocket = INVALID_SOCKET;
		
		//send ���۸� ������ �ִ�.
		///�� ���۴� ������ post,flush �ϴ� �κп��� ���� �ɾ�� �Ѵ�.
		///��������� �ּ����� ������ ����ȭ �ϴ°��� �ϴ� �ϼ��ϰ� ����� ����.
		///��� ���ɶ��� ũ��Ƽ�� ������ ���յ� ����ȭ ��ü�� ����Ѵ�.

		MDUtillity::CircularBuffer _sendBuffer;
		MDUtillity::CircularBuffer _recvBuffer;

	private:
		const int _maxRecvBufLen;
		const int _maxSendBufLen;

		CRITICAL_SECTION _spinlockMutex;

		std::atomic<bool> _isSendAvailable{ false };
		std::atomic<bool> _isConnected{ false };
		std::atomic<long> _refCount{ 0 };//�񵿱� ����� �� �Ҷ����� ���� ��Ű�� �Ϸ� �ɶ� ���� ���� ���Ѽ� 0�϶��� ������ Ǯ�� ���� ������.
	};

	struct IOContext
	{
		WSAOVERLAPPED	overlapped;
		IOType			sessionIOtype;
		Session*		session;
		WSABUF			wsaBuf;
	};

}

