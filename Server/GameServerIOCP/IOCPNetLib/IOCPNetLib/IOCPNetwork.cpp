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

		//여기서 작업자 스레드가 이미 시작 되어서 GQCS에서 기다리고 있다.
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

		//클라이언트 ACCEPT스레드 실행
		_acceptThread = std::thread([this]() {
			acceptThreadFunc();
		});
		//TODO:생성 실패시 익셉션 발생 익셉션 처리 코드 필요.
		//SEND스레드 실행

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
		//TODO: _threads 조인
		//TODO:_acceptThread 조인
		//TODO:_sendThread 조인
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
		//0 을 입력 받으면 코어수 *2
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
			//TODO:생성 실패시 익셉션 발생 익셉션 처리 코드 필요.
			_threads.push_back(std::move(th));
		}

		return true;
	}

	bool IOCPNetwork::makeSessionPool()
	{
		//TODO:유니크 ptr을 사용할수 있다. 여기서 세션풀을 동적 할당했으므로  딜리트 해야 하는데

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
			//TODO: 아래와 위의 오류를 로그로 바로 기록하면 구별할수 있음 
			//지금은 안됨
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
				//TODO: 억셉트 실패 로그
				continue;
			}

			/* 어차피 비동기 통신을 할거기 때문에 소켓이 블럭이던 논블럭이던 상관이 없다.
			커널쪽 버퍼를 0으로 만드는 과정 하지만 요즘에는 운영체제가 알아서 해준다. 생략
			int	Zero = 0;
			if (SOCKET_ERROR == setsockopt(clientSocket, SOL_SOCKET, SO_RCVBUF, (const TCHAR*)&Zero, sizeof(Zero)))
			continue;

			if (SOCKET_ERROR == setsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, (const TCHAR*)&Zero, sizeof(Zero)))
			continue;
			*/

			Session* session = _sessionPool->AllockSession();

			if (session == nullptr)
			{
				//TODO:최대 클라이언트 수 도달
				//어떤 처리
				continue;//하지만 서버가 종료 되어서는 안되게.
			}

			session->_socketContext.clntSocket = clientSocket;


			if (CreateIoCompletionPort((HANDLE)clientSocket, _cpHandle, (ULONG_PTR)session, 0) != _cpHandle)
			{
				ret = WSAGetLastError();
				//TODO:컴플리션포트와 세션 연결실패 로그
				//이유는?
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

				if (dataSize == 0)	// 클라이언트가 접속을 끊었다
				{
					//m_Log.EventLog(1, szTemp);
					_sessionPool->SessionFree(sessionAddr);
					continue;
				}

				if (ioContext == &sessionAddr->_socketContext.recvContext)
				{
					// 보낸 패킷의 사이즈만큼 받아서 처리할 것
					//PacketProcess(pSession);
					//TODO: 패킷을 처리 하는건 여러 스레드에서 하는데 
					//실제 패킷이 논리스레드 에서 돌아가는건 상관이 없나?
					receiveProcess(sessionAddr, dataSize);

					DWORD flags = 0;	// out parameter, and I/O 처리가 끝나면 꼭 해당 소켓에 recv 를 걸어둔다.

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
			//TODO: 패킷을 받아서 세션에 써준다.
			
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