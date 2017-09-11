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
		//TODO: enum�� �̿��ؼ� ���ú����� �������� ���Ʈ ���� ���� �Ҽ� �ִ�.
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

		bool OnRecvComplete();

		bool CommitSendBuffer(const char* data, size_t len);
		bool FlushSendBuffer();

		void SendCompeletion(DWORD transferred);
		void RecvCompeletion(DWORD transferred);

	private:

	public:
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


		int _sendBufRemainLenth = 0;
		int _sendBufStartPos = 0;

		int _recvBufRemainLenth = 0;
		int _recvBUfStartPos = 0;

		std::atomic<bool> _isSendAvailable{ false };
		std::atomic<bool> _isConnected{ false };
		//�񵿱� ����� �� �Ҷ����� ���� ��Ű�� �Ϸ� �ɶ� ���� ���� ���Ѽ� 0�϶���
		//������ Ǯ�� ���� ������.
		std::atomic<long> _refCount{ 0 };
	};

}

