#include "pch.h"
#include "IOCPNetwork.h"
#include "Logger.h"
#include "SeverProperty.h"
#include "SessionPool.h"
#include "Session.h"
namespace MDServerNetLib
{

	IOCPNetwork::IOCPNetwork(const ServerProperty*  config,
		MDUtillity::LoggerBase* logger)
	{
		copyConfig(config);
		_logger = logger;
	}


	IOCPNetwork::~IOCPNetwork()
	{
	}

	NET_ERROR_CODE IOCPNetwork::init()
	{	
		WSADATA wsaData;

		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			_logger->Write(MDUtillity::LogType::DEBUG, "%s | WSAstartUp Faild", __FUNCTION__);
			return NET_ERROR_CODE::SERVER_SOCKET_STARTUP_FAIL;
		}

		if (makeAcceptSocket() == false)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_CREATE_FAIL;
		}

		if (makeCP() == false)
		{
			return NET_ERROR_CODE::SERVER_CP_CREATE_FAIL;
		}

		//���⼭ �۾��� �����尡 �̹� ���� �Ǿ GQCS���� ��ٸ��� �ִ�.
		if (makeThreadPool() == false)
		{
			return NET_ERROR_CODE::WORK_THREAD_CREATE_FAIL;
		}

		if (makeSessionPool() == false)
		{
			return NET_ERROR_CODE::SESSION_POOL_CREATE_FAIL;
		}

		if (bindAndListen() == false)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_BIND_AND_LISTEN_FAIL;
		}

		//Ŭ���̾�Ʈ ACCEPT������ ����
		_acceptThread = std::thread([this]() {
			acceptThreadFunc();
		});
		//TODO:���� ���н� �ͼ��� �߻� �ͼ��� ó�� �ڵ� �ʿ�.
		//SEND������ ����

		_sendThread = std::thread([this]() {
			tempSendFunc();
		});

		return NET_ERROR_CODE::NONE;
	}

	NET_ERROR_CODE IOCPNetwork::SetPacketQueue(PacketQueueConccurency * recvQue, PacketQueueConccurency * sendQue)
	{
		_recvQueue = recvQue;
		_sendQueue = sendQue;
		return NET_ERROR_CODE::NONE;
	}

	PacketInfo IOCPNetwork::GetReceivedPacketTemp()
	{
		PacketInfo pkt;
		if (_recvQueue->try_pop(pkt))
		{
			return pkt;
		}
		else
		{
			pkt.PacketId = 0;
			return pkt;
		}
		return PacketInfo();
	}

	void IOCPNetwork::TempPushSendPacketQueue(const int sessionIndex, const short packetId, const short size, const char * msg)
	{
		PacketInfo pkt;
		pkt.SessionIndex = sessionIndex;
		pkt.PacketId = packetId;
		pkt.PacketBodySize = size;
		memcpy(pkt.Data, msg, size);

		_sendQueue->push(pkt);
	}

	int IOCPNetwork::ClientSessionPoolSize()
	{
		return _property.MaxClientCount + _property.ExtraClientCount;
	}

	void IOCPNetwork::Release()
	{
		//TODO: _threads ����
		//TODO:_acceptThread ����
		//TODO:_sendThread ����
		_acceptThread.join();
		_sendThread.join();

		for (auto& thread : _threads)
		{
			thread.join();
		}

		CloseHandle(_cpHandle);

		WSACleanup();
	}

	bool IOCPNetwork::copyConfig(const ServerProperty * prop)
	{
		_property.ExtraClientCount = prop->ExtraClientCount;
		_property.MaxClientCount = prop->MaxClientCount;

		return true;
	}

	bool IOCPNetwork::makeAcceptSocket()
	{
		_acceptSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

		if (_acceptSocket == INVALID_SOCKET)
		{
			_logger->Write(MDUtillity::LogType::DEBUG, "%s | Make AcceptSocket Faild", __FUNCTION__);
			return false;
		}

		return true;
	}

	bool IOCPNetwork::makeCP()
	{
		_cpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		if (_cpHandle == INVALID_HANDLE_VALUE)
		{
			_logger->Write(MDUtillity::LogType::DEBUG, "%s | Make Completion Port Faild", __FUNCTION__);
			return false;
		}

		return true;
	}

	bool IOCPNetwork::makeThreadPool(int threadNum)
	{
		//0 �� �Է� ������ �ھ�� *2
		if (threadNum == 0)
		{
			SYSTEM_INFO info;
			GetSystemInfo(&info);

			_createdThreadNum = info.dwNumberOfProcessors * 2;
		}
		else if (threadNum < 0)
		{
			_logger->Write(MDUtillity::LogType::DEBUG, "%s | Make Completion Port Faild", __FUNCTION__);
			return false;
		}
		else
		{
			_createdThreadNum = threadNum;
		}

		for (int i = 0; i < _createdThreadNum; ++i)
		{
			std::thread th = std::thread([this]() {workerThreadFunc(this->_cpHandle); });
			//TODO:���� ���н� �ͼ��� �߻� �ͼ��� ó�� �ڵ� �ʿ�.
			_threads.push_back(std::move(th));
		}

		return true;
	}

	bool IOCPNetwork::makeSessionPool()
	{
		//TODO:����ũ ptr�� ����Ҽ� �ִ�. ���⼭ ����Ǯ�� ���� �Ҵ������Ƿ�  ����Ʈ �ؾ� �ϴµ�

		auto sessionNum = _property.MaxClientCount + _property.ExtraClientCount;
		auto recvBufSize = _property.MaxClientRecvBufferSize;
		auto sendBufSize = _property.MaxClientSendBufferSize;

		_sessionPool = std::unique_ptr<SessionPool>(
			new SessionPool(
				sessionNum,
				recvBufSize,
				sendBufSize)
			);
		return true;
	}

	bool IOCPNetwork::bindAndListen()
	{
		SOCKADDR_IN	serverAddr;

		ZeroMemory(&serverAddr, sizeof(SOCKADDR_IN));

		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddr.sin_port = htons(_property.Port);

		auto retVal = bind(_acceptSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

		if (retVal == SOCKET_ERROR)
		{
			//TODO: �Ʒ��� ���� ������ �α׷� �ٷ� ����ϸ� �����Ҽ� ���� 
			//������ �ȵ�
			_logger->Write(MDUtillity::LogType::DEBUG, "%s | Bind Port Faild", __FUNCTION__);
			return false;
		}

		retVal = listen(_acceptSocket, SOMAXCONN);

		if (retVal == SOCKET_ERROR)
		{
			_logger->Write(MDUtillity::LogType::DEBUG, "%s | Listen Port Faild", __FUNCTION__);
			return false;
		}

		return true;
	}

	bool IOCPNetwork::acceptThreadFunc()
	{
		SOCKET clientSocket = INVALID_SOCKET;

		SOCKADDR_IN clientAddr;

		int addrlen = sizeof(clientAddr);

		DWORD recvByte = 0;

		DWORD flag = 0;

		int ret = 0;

		while (true)
		{
			clientSocket = accept(_acceptSocket, (SOCKADDR*)&clientAddr, &addrlen);

			if (clientSocket == INVALID_SOCKET)
			{
				//TODO: ���Ʈ ���� �α�
				continue;
			}

			/* ������ �񵿱� ����� �Ұű� ������ ������ ���̴� ����̴� ����� ����.
			Ŀ���� ���۸� 0���� ����� ���� ������ ���򿡴� �ü���� �˾Ƽ� ���ش�. ����
			int	Zero = 0;
			if (SOCKET_ERROR == setsockopt(clientSocket, SOL_SOCKET, SO_RCVBUF, (const TCHAR*)&Zero, sizeof(Zero)))
			continue;

			if (SOCKET_ERROR == setsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, (const TCHAR*)&Zero, sizeof(Zero)))
			continue;
			*/

			Session* session = _sessionPool->AllockSession();

			if (session == nullptr)
			{
				//TODO:�ִ� Ŭ���̾�Ʈ �� ����
				//� ó��
				continue;//������ ������ ���� �Ǿ�� �ȵǰ�.
			}

			session->_socketContext.clntSocket = clientSocket;


			if (CreateIoCompletionPort((HANDLE)clientSocket, _cpHandle, (ULONG_PTR)session, 0) != _cpHandle)
			{
				ret = WSAGetLastError();
				//TODO:���ø�����Ʈ�� ���� ������� �α�
				//������?
				continue;
			}

			ret = WSARecv(clientSocket,
				&(session->_socketContext.recvContext.wsaBuf),
				1,
				&recvByte,
				&flag,
				&session->_socketContext.recvContext.overlapped,
				NULL);

			if (SOCKET_ERROR == ret)
			{
				ret = WSAGetLastError();
				if (ret != WSA_IO_PENDING)
				{
					return FALSE;
				}
			}
		}
		return false;
	}

	unsigned int IOCPNetwork::workerThreadFunc(LPVOID lpParam)
	{
		


		return 0;
	}

	void IOCPNetwork::doIOCPJob()
	{
		DWORD				dataSize = 0;
		DWORD				result = 0;
		Session*	sessionAddr = nullptr;
		IOContext*		ioContext = nullptr;

		while (true)
		{
			if (GetQueuedCompletionStatus(_cpHandle,
				&dataSize,
				(PULONG_PTR)&sessionAddr,
				(LPOVERLAPPED*)&ioContext,
				INFINITE))
			{
				result = WSAGetLastError();
			}

			if (result == 0)
			{
				DWORD err = GetLastError();

				if (sessionAddr == nullptr)
					continue;

				if (dataSize == 0)	// Ŭ���̾�Ʈ�� ������ ������
				{
					//m_Log.EventLog(1, szTemp);
					_sessionPool->SessionFree(sessionAddr);
					continue;
				}

				if (ioContext == &sessionAddr->_socketContext.recvContext)
				{
					// ���� ��Ŷ�� �����ŭ �޾Ƽ� ó���� ��
					//PacketProcess(pSession);
					//TODO: ��Ŷ�� ó�� �ϴ°� ���� �����忡�� �ϴµ� 
					//���� ��Ŷ�� �������� ���� ���ư��°� ����� ����?
					receiveProcess(sessionAddr, dataSize);

					DWORD flags = 0;	// out parameter, and I/O ó���� ������ �� �ش� ���Ͽ� recv �� �ɾ�д�.

					ZeroMemory(&sessionAddr->_socketContext.recvContext.overlapped, sizeof(WSAOVERLAPPED));

					sessionAddr->_socketContext.recvContext.wsaBuf.buf =
						&sessionAddr->_socketContext.recvContext.Buffer[sessionAddr->RecvBufferStartPos()];

					sessionAddr->_socketContext.recvContext.wsaBuf.len = sessionAddr->GetMaxRecvBufLen() - sessionAddr->RemainReceiveDataSize();

					WSARecv(sessionAddr->_socketContext.clntSocket,
						&(sessionAddr->_socketContext.recvContext.wsaBuf),
						1,
						NULL,
						&flags,
						&(sessionAddr->_socketContext.recvContext.overlapped),
						NULL);

				}
				else if (ioContext == &sessionAddr->_socketContext.sendContext)
				{
					sessionAddr->CompleteSend();
				}
			}
			else
			{
				if (result == WAIT_TIMEOUT)
				{
				}
				else if (result == ERROR_NETNAME_DELETED)
				{
					//m_Log.EventLog(2, "WorkerThread", "ERROR CODE : ERROR_NETNAME_DELETED");
					_sessionPool->SessionFree(sessionAddr);
				}
				else
				{
					//m_Log.EventLog(2, "GetQueuedCompletionStatus unknown Error [%d]", dwResult);
					_sessionPool->SessionFree(sessionAddr);
				}

				continue;
			}
		}
	}

	void IOCPNetwork::doSendJob()
	{
		PacketInfo pkt;
		while (_sendQueue->try_pop(pkt))
		{
			//TODO: ��Ŷ�� �޾Ƽ� ���ǿ� ���ش�.
			
		}
	}

	int IOCPNetwork::receiveProcess(Session * sessionAddr, int dataSize)
	{
		return 0;
	}

	void IOCPNetwork::addRecvPacketToQueue(const int sessionIndex, const short pktId, const short bodysize, char * datapos)
	{
	}
	

}