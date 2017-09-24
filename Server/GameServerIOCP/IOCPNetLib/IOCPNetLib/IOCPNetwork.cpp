
#include "IOCPNetwork.h"
#include "Logger.h"
#include "ServerProperty.h"
#include "SessionPool.h"
#include "Session.h"
namespace MDServerNetLib
{

	IOCPNetwork::IOCPNetwork(const ServerProperty*  config,
		MDUtillity::LoggerBase* logger, PacketQueueConccurency* recvQue, PacketQueueConccurency* sendQue)
		:_recvQueue{ recvQue },
		_sendQueue{ sendQue },
		_logger{ logger }
	{
		copyConfig(config);
	}


	IOCPNetwork::~IOCPNetwork()
	{
	}

	NET_ERROR_CODE IOCPNetwork::init()
	{
		WSADATA wsaData;

		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_STARTUP_FAIL;
			
		}
		_logger->Write(MDUtillity::LogType::INFO, "%s | WSAStartup Success", __FUNCTION__);

		if (makeAcceptSocket() == false)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_CREATE_FAIL;
		}
		_logger->Write(MDUtillity::LogType::INFO, "%s | makeAcceptSocket Success", __FUNCTION__);


		if (makeCP() == false)
		{
			return NET_ERROR_CODE::SERVER_CP_CREATE_FAIL;
		}

		_logger->Write(MDUtillity::LogType::INFO, "%s | makeCP Success", __FUNCTION__);


		//���⼭ �۾��� �����尡 �̹� ���� �Ǿ GQCS���� ��ٸ��� �ִ�.
		if (makeThreadPool() == false)
		{
			return NET_ERROR_CODE::WORK_THREAD_CREATE_FAIL;
		}

		_logger->Write(MDUtillity::LogType::INFO, "%s | makeThreadPool Success", __FUNCTION__);


		if (makeSessionPool() == false)
		{
			return NET_ERROR_CODE::SESSION_POOL_CREATE_FAIL;
		}
		_logger->Write(MDUtillity::LogType::INFO, "%s | makeSessionPool Success", __FUNCTION__);

		if (bindAndListen() == false)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_BIND_AND_LISTEN_FAIL;
		}
		_logger->Write(MDUtillity::LogType::INFO, "%s | bindAndListen start", __FUNCTION__);


		//Ŭ���̾�Ʈ ACCEPT������ ����
		_acceptThread = std::thread([this]() {
			_logger->Write(MDUtillity::LogType::INFO, "%s | Accept start", __FUNCTION__);

			acceptThreadFunc();
		});

		

		//�������� ������ ��Ŷ�� �����ϴ� ������
		_sendThread = std::thread([this]() {
			_logger->Write(MDUtillity::LogType::INFO, "%s | Send Thread start", __FUNCTION__);

			sendThreadFunc();
		});
		//TODO:���� ���н� �ͼ��� �߻� �ͼ��� ó�� �ڵ� �ʿ�.
		return NET_ERROR_CODE::NONE;
	}

	PacketRaw IOCPNetwork::GetReceivedPacketTemp()
	{
		PacketRaw pkt;
		if (_recvQueue->try_pop(pkt))
		{
			return pkt;
		}
		else
		{
			pkt.EmptyPacket();
			return pkt;
		}
	}

	//TODO: ��� �̰Ŵ� ��Ŷ ���μ����� �־�� �Ұ�.
	void IOCPNetwork::TempPushSendPacketQueue(const int sessionIndex, const short packetId, const short size, const char * msg)
	{
		PacketRaw pkt(sessionIndex,packetId, size, msg);

		_sendQueue->push(pkt);
	}

	int IOCPNetwork::ClientSessionPoolSize()
	{
		return _property.MaxClientCount + _property.ExtraClientCount;
	}

	bool IOCPNetwork::copyConfig(const ServerProperty * prop)
	{
		_property.PortNum = prop->PortNum;
		_property.BackLog = prop->BackLog;
		_property.MaxClientCount = prop->MaxClientCount;
		_property.ExtraClientCount = prop->ExtraClientCount;
		_property.MaxClientRecvBufferSize = prop->MaxClientRecvBufferSize;
		_property.MaxClientSendBufferSize = prop->MaxClientSendBufferSize;
		_property.MAX_PACKET_BODY_SIZE = prop->MAX_PACKET_BODY_SIZE;

		return true;
	}

#pragma region release
	void IOCPNetwork::Release()
	{
		//TODO: _threads ����
		//TODO:_acceptThread ����
		_acceptThread.join();

		for (auto& thread : _threads)
		{
			thread.join();
		}

		CloseHandle(_cpHandle);

		closesocket(_acceptSocket);

		WSACleanup();
	}
#pragma endregion

#pragma region makeAcceptSocket
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
#pragma endregion

#pragma region makeCP
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
#pragma endregion

#pragma region makeThreadPool
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
			//TODO:���� ���н� �ͼ��� �߻� �ͼ��� �߻��� �����Ų��..
			_threads.push_back(std::move(th));
		}

		return true;
	}
#pragma endregion

#pragma region makeSessionPool
	bool IOCPNetwork::makeSessionPool()
	{
		_sessionPool = std::unique_ptr<SessionPool>(
			new SessionPool(
				_property.MaxClientCount + _property.ExtraClientCount,
				_property.MaxClientRecvBufferSize,
				_property.MaxClientSendBufferSize)
			);
		return true;
	}
#pragma endregion

#pragma region bindAndListen
	bool IOCPNetwork::bindAndListen()
	{
		SOCKADDR_IN	serverAddr;

		ZeroMemory(&serverAddr, sizeof(SOCKADDR_IN));

		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddr.sin_port = htons(_property.PortNum);

		auto retVal = bind(_acceptSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

		if (retVal == SOCKET_ERROR)
		{
			//TODO: �Ʒ��� ���� ������ �α׷� �ٷ� ����ϸ� �����Ҽ� ���� ���Ͽ� ���� �ϱ�.
			_logger->Write(MDUtillity::LogType::ERR, "%s | Bind Port Faild", __FUNCTION__);
			return false;
		}

		retVal = listen(_acceptSocket, SOMAXCONN);

		if (retVal == SOCKET_ERROR)
		{
			_logger->Write(MDUtillity::LogType::ERR, "%s | Listen Port Faild", __FUNCTION__);
			return false;
		}

		return true;
	}
#pragma endregion

#pragma region acceptThreadFunc

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
				_logger->Write(MDUtillity::LogType::ERR, "%s | Accept Faild", __FUNCTION__);
				continue;
			}

			/* ������ �񵿱� ����� �Ұű� ������ ������ ���̴� ����̴� ����� ����.
			Ŀ���� ���۸� 0���� ����� ���� ������ ���򿡴� �ü���� �˾Ƽ� ���ش�. ����
			setsockopt(clientSocket, SOL_SOCKET, SO_RCVBUF, (const TCHAR*)&Zero, sizeof(Zero))
			*/

			Session* session = _sessionPool->AllocateSession();

			if (session == nullptr)
			{
				//TODO:�ִ� Ŭ���̾�Ʈ �� ���� extra session ���� ���� ��Ȳ. �������� �ȵȴ�.
				_logger->Write(MDUtillity::LogType::INFO, "%s | Session pool Full", __FUNCTION__);
				continue;
			}

			session->_clntSocket = clientSocket;
			session->SetConnected(true);
			session->SetSendable(true);

			if (CreateIoCompletionPort((HANDLE)clientSocket, _cpHandle, (ULONG_PTR)session, 0) != _cpHandle)
			{
				ret = WSAGetLastError();
				_logger->Write(MDUtillity::LogType::ERR, "%s | session add to CP Faild", __FUNCTION__);
				continue;
			}
			
			_logger->Write(MDUtillity::LogType::DEBUG, "%s | New Client Accepted. SessionID %d ", __FUNCTION__, session->index);

			if (session->PostRecv())
			{
				_logger->Write(MDUtillity::LogType::INFO, "%s | WSARecv Faild", __FUNCTION__);
			}
			_logger->Write(MDUtillity::LogType::DEBUG, "%s | Session %d Start Recive", __FUNCTION__, session->index);


		}
		return true;
	}

#pragma endregion

#pragma region send thread func
	bool IOCPNetwork::sendThreadFunc()
	{
		while (true)
		{
			doSendJob();
		}

		return true;
	}
#pragma endregion

#pragma region worker thread func
	DWORD IOCPNetwork::workerThreadFunc(LPVOID lpParam)
	{
		while (true)
		{
			doIOCPJob();
		}

		return 0;
	}
#pragma endregion

	void IOCPNetwork::doIOCPJob()
	{
		DWORD				dataSize = 0;
		DWORD				result = 0;
		Session*	sessionAddr = nullptr;
		IOContext*		ioContext = nullptr;

		result = GetQueuedCompletionStatus(_cpHandle, &dataSize, (PULONG_PTR)&sessionAddr,
			(LPOVERLAPPED*)&ioContext, INFINITE);

		auto context = reinterpret_cast<IOContext*>(ioContext);

#pragma region error handle
		if (result == 0)
		{
			if (WSAGetLastError() == WAIT_TIMEOUT && context == nullptr) {
				_logger->Write(MDUtillity::LogType::ERR, "%s | WAIT_TIMEOUT ", __FUNCTION__);
			}
			else if (WSAGetLastError() == ERROR_NETNAME_DELETED)
			{
				_logger->Write(MDUtillity::LogType::ERR, "%s | ERROR_NETNAME_DELETED ", __FUNCTION__);
				_sessionPool->SessionFree(sessionAddr);
				;
			}
			else
			{
				_logger->Write(MDUtillity::LogType::ERR, "%s | ERROR_GCQS ", __FUNCTION__);
			}

			if (ioContext != nullptr)
			{
				delete ioContext;
			}

			return;
		}

		if (dataSize == 0)
		{
			_logger->Write(MDUtillity::LogType::ERR, "%s |  client cut the session ", __FUNCTION__);
			//������ �ʱ�ȭ �ϰ� ������ �ݴ´�.
			_sessionPool->SessionFree(sessionAddr);

			if (ioContext != nullptr)
			{
				delete ioContext;
			}

			return;
		}

#pragma endregion

#pragma region recv completion handle

		if (ioContext->sessionIOtype == IOType::IO_RECV)
		{
			//��Ŷ�� ó�����ش�.
			receiveProcess(sessionAddr, dataSize);


		}
#pragma endregion

#pragma region send completion handle
		else if (ioContext->sessionIOtype == IOType::IO_SEND)
		{
			//���� ���۸� ó���� �ش�.
			sessionAddr->CompleteSend(dataSize);


		}

#pragma endregion

#pragma region else
		else
		{
			//���峪 ���ú갡 �ƴѰ�� 
			_logger->Write(MDUtillity::LogType::DEBUG, "%s | Unknown IOType", __FUNCTION__);
		}

#pragma endregion
		sessionAddr->ReleaseRef();
		delete ioContext;

	}

	void IOCPNetwork::doSendJob()
	{
		PacketRaw pkt;
		while (_sendQueue->try_pop(pkt))
		{
			auto session = _sessionPool.get()->GetSessionByIndex(pkt._sessionIdx);
			session->PostSendBuffer(pkt);
		}

		

		for (int i = 0; i < ClientSessionPoolSize(); ++i)
		{
			auto session = _sessionPool.get()->GetSessionByIndex(i);

			if (session == nullptr)continue;

			if (session->IsConnected() != true)
			{
				continue;
			}
			//�÷��� �ϴµ� ���� ���尡 �Ϸ������ ���� ��Ȳ�̸� �׳� ���´�.
			session->FlushSendBuffer();
		}
		//TODO:�ѹ� ��ó���ϰ� �纸�ϱ� ���� �̷��� �ϴ°� �ƴ϶� �������� �׳� �ٷ� ���忹���� �Ŵ°� ���� ��� �ұ�?
		//���࿡ �����ѹ��� �������� cp�� ���带 ���� ��Ŀ�����带 ����°� PostQueuedCompletionStatus�Լ��� ���ִ°�?
		std::this_thread::sleep_for(std::chrono::milliseconds(0));
	}

	int IOCPNetwork::receiveProcess(Session * sessionAddr, int dataSize)
	{
		//���� ���ۿ� ���� ���̹Ƿ� �ϴ� ���۱��̸� �߰��ϰ�.
		sessionAddr->CompleteRecv(dataSize);

		//������ ���� ���� ���̸� �޴´�.
		auto bufferStart = sessionAddr->_recvBuffer.GetbufferStartposition();
		auto bufferLenth = sessionAddr->_recvBuffer.GetContiguiousSize();

		auto headerSize = PacketHeaderSize;

		size_t readPos = 0;

		PacketHeader* pktHeader = nullptr;

		//����� �ϳ� ������ ���� �� ���� ������ ����.
		
		while (bufferLenth - readPos >= headerSize)
		{
			pktHeader = (PacketHeader*)(bufferStart + readPos);

			readPos += headerSize;

			if (pktHeader->_bodySize > 0)
			{
				if (pktHeader->_bodySize > (bufferLenth - readPos))
				{
					readPos -= headerSize;
				}

				if (pktHeader->_bodySize > _property.MAX_PACKET_BODY_SIZE)
				{
					_logger->Write(MDUtillity::LogType::ERR, "%s | Bad User Too Long Body", __FUNCTION__);
					return (int)NET_ERROR_CODE::RECV_CLIENT_MAX_PACKET;
				}
			}
			//TODO:�ٵ� ������� ��Ʈ�� ���� +1
			auto str = new char[pktHeader->_bodySize];

			memcpy_s(str, pktHeader->_bodySize, &bufferStart[readPos], pktHeader->_bodySize);

			addRecvPacketToQueue(sessionAddr->index,pktHeader->_id,pktHeader->_bodySize,str);

			delete[] str;

			readPos += pktHeader->_bodySize;
		}
		//TODO:�ٵ� �������۰� ���۵��� ����ų�� �� �����Ͱ���. ��Ʈ��ũ ��Ȳ�� �����ų�,���� �ð��� ������.
		//���������� �պ��� �ҵ� �ϴ� ���� ���ư����� ����°� �ʿ��ҵ�. �ƴϸ� 1�� �κа� 2�� �κ��� �����ִ°�����
		//�ѹ��� �ݺ��� �ְų�. �ٵ� ��Ŷ�� ũ�� �ʰ� ��Ʈ��ũ ��Ȳ�� ������ ������ �ϴ� ����ϴ°ɷ�.
		
		//ť�� ���� ��ŭ ���۸� �����ش�.
		if (readPos != 0)
		{
			sessionAddr->_recvBuffer.Remove(readPos);
		}

		return 0;
	}

	void IOCPNetwork::addRecvPacketToQueue(const int sessionIndex, const short pktId, const short bodysize, char * datapos)
	{
		auto in = static_cast<Packet::PacketId>(pktId);
		PacketRaw pkt(sessionIndex, pktId, bodysize, datapos);

		_recvQueue->push(pkt);

		_logger->Write(MDUtillity::LogType::DEBUG, "%s | Packet Recive, Session(%d), Packet ID(%hd),size(%hd)", __FUNCTION__, sessionIndex, pktId, bodysize);
	}


}
