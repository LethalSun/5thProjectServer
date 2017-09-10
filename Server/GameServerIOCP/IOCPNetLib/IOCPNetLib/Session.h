#pragma once
#include "pch.h"
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

	struct IOContext
	{
		WSAOVERLAPPED	overlapped;
		//TODO: �̳��� �̿��ؼ� ���ú����� �������� ���Ʈ ���� ���� �Ҽ� �ִ�.
		char*			Buffer = nullptr;
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
	private:

	public:
		//��������� ����ü�� ������ �ִ�.
		SocketContext _socketContext;
		
		//send ���۸� ������ �ִ�.
		MDUtillity::CircularBuffer _sendBuffer;
		MDUtillity::CircularBuffer _recvBuffer;
	private:
		const int _maxRecvBufLen;
		const int _maxSendBufLen;
		const int _sessionPoolIndex;


		int _sendBufRemainLenth = 0;
		int _sendBufStartPos = 0;

		int _recvBufRemainLenth = 0;
		int _recvBUfStartPos = 0;

		std::atomic<bool> _isSendAvailable{ false };
		std::atomic<bool> _isConnected{ false };
		std::atomic<long> _refCount{ 0 };
	};

}

