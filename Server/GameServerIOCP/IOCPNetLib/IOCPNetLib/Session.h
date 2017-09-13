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
		//��������� ����ü�� ������ �ִ�.
		SocketContext _socketContext;
		
		//send ���۸� ������ �ִ�.
		///�� ���۴� ������ post,flush �ϴ� �κп��� ���� �ɾ�� �Ѵ�.
		///��������� �ּ����� ������ ����ȭ �ϴ°��� �ϴ� �ϼ��ϰ� ����� ����.
		///��� ���ɶ��� ũ��Ƽ�� ������ ���յ� ����ȭ ��ü�� ����Ѵ�.
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
		//�񵿱� ����� �� �Ҷ����� ���� ��Ű�� �Ϸ� �ɶ� ���� ���� ���Ѽ� 0�϶���
		//������ Ǯ�� ���� ������.
		std::atomic<long> _refCount{ 0 };

		DisconnectReason _disconnectReason{ DisconnectReason::DR_NONE };
	};

}

