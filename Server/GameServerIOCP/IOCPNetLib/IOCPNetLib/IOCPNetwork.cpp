
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

		//서버에서 보내는 패킷을 예약하는 스레드
		_sendThread = std::thread([this]() {
			sendThreadFunc();
		});
		//TODO:생성 실패시 익셉션 발생 익셉션 처리 코드 필요.
		return NET_ERROR_CODE::NONE;
	}

	NET_ERROR_CODE IOCPNetwork::SetPacketQueue(PacketQueueConccurency * recvQue, PacketQueueConccurency * sendQue)
	{
		_recvQueue = recvQue;
		_sendQueue = sendQue;
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

	//TODO: 사실 이거는 패킷 프로세스에 있어야 할것.
	void IOCPNetwork::TempPushSendPacketQueue(const int sessionIndex, const short packetId, const short size, const char * msg)
	{
		PacketRaw pkt(sessionIndex,(Packet::PacketId)packetId, size, msg);

		_sendQueue->push(pkt);
	}

	int IOCPNetwork::ClientSessionPoolSize()
	{
		return _property.MaxClientCount + _property.ExtraClientCount;
	}

	bool IOCPNetwork::copyConfig(const ServerProperty * prop)
	{
		_property.ExtraClientCount = prop->ExtraClientCount;
		_property.MaxClientCount = prop->MaxClientCount;

		return true;
	}

#pragma region release
	void IOCPNetwork::Release()
	{
		//TODO: _threads 조인
		//TODO:_acceptThread 조인
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
		serverAddr.sin_port = htons(_property.Port);

		auto retVal = bind(_acceptSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

		if (retVal == SOCKET_ERROR)
		{
			//TODO: 아래와 위의 오류를 로그로 바로 기록하면 구별할수 있음 파일에 저장 하기.
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

			/* 어차피 비동기 통신을 할거기 때문에 소켓이 블럭이던 논블럭이던 상관이 없다.
			커널쪽 버퍼를 0으로 만드는 과정 하지만 요즘에는 운영체제가 알아서 해준다. 생략
			setsockopt(clientSocket, SOL_SOCKET, SO_RCVBUF, (const TCHAR*)&Zero, sizeof(Zero))
			*/

			Session* session = _sessionPool->AllocateSession();

			if (session == nullptr)
			{
				//TODO:최대 클라이언트 수 도달 extra session 마저 없는 상황. 벌어지면 안된다.
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
			
			if (session->PostRecv())
			{
				_logger->Write(MDUtillity::LogType::INFO, "%s | WSARecv Faild", __FUNCTION__);
			}
			
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
				return;
			}
			else if (WSAGetLastError() == ERROR_NETNAME_DELETED)
			{
				_logger->Write(MDUtillity::LogType::ERR, "%s | ERROR_NETNAME_DELETED ", __FUNCTION__);
				_sessionPool->SessionFree(sessionAddr);
				return;
			}
			else
			{
				_logger->Write(MDUtillity::LogType::ERR, "%s | ERROR_GCQS ", __FUNCTION__);
				return;
			}
		}

		if (dataSize == 0)
		{
			_logger->Write(MDUtillity::LogType::ERR, "%s |  client cut the session ", __FUNCTION__);
			//세션을 초기화 하고 소켓을 닫는다.
			_sessionPool->SessionFree(sessionAddr);

			return;
		}

#pragma endregion

#pragma region recv completion handle

		if (ioContext->sessionIOtype == IOType::IO_RECV)
		{
			//패킷을 처리해준다.
			receiveProcess(sessionAddr, dataSize);
		}
#pragma endregion

#pragma region send completion handle
		else if (ioContext->sessionIOtype == IOType::IO_SEND)
		{
			//센드 버퍼를 처리해 준다.
			sessionAddr->CompleteSend(dataSize);
		}

#pragma endregion

#pragma region else
		else
		{
			//센드나 리시브가 아닌경우 
			_logger->Write(MDUtillity::LogType::DEBUG, "%s | Unknown IOType", __FUNCTION__);
		}

#pragma endregion

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

			if (session->IsConnected() != true)
			{
				continue;
			}
			//플러쉬 하는데 아직 센드가 완료되있지 않은 상황이면 그냥 나온다.
			session->FlushSendBuffer();
		}
		//TODO:한번 다처리하고 양보하기 원래 이렇게 하는게 아니라 로직에서 그냥 바로 센드예약을 거는게 가장 깔끔 할까?
		//만약에 로직한바퀴 돌때마다 cp에 센드를 위한 워커스레드를 깨우는걸 PostQueuedCompletionStatus함수로 해주는건?
		std::this_thread::sleep_for(std::chrono::milliseconds(0));
	}

	int IOCPNetwork::receiveProcess(Session * sessionAddr, int dataSize)
	{
		//원형 버퍼에 써진 것이므로 일단 버퍼길이를 추가하고.
		sessionAddr->CompleteRecv(dataSize);

		//버퍼의 시작 점과 길이를 받는다.
		auto bufferStart = sessionAddr->_recvBuffer.GetbufferStartposition();
		auto bufferLenth = sessionAddr->_recvBuffer.GetContiguiousSize();

		auto headerSize = PacketHeaderSize;

		size_t readPos = 0;

		PacketHeader* pktHeader = nullptr;

		//헤더가 하나 온전히 있을 때 까지 다음을 수행.
		
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

			auto str = new char[pktHeader->_bodySize];

			memcpy_s(str, pktHeader->_bodySize, &bufferStart[readPos], pktHeader->_bodySize);

			addRecvPacketToQueue(sessionAddr->index,pktHeader->_id,pktHeader->_bodySize,str);

			delete[] str;

			readPos += pktHeader->_bodySize;
		}

		//TODO:근데 원형버퍼가 오작동을 일으킬수 도 있을것같다. 네트워크 상황이 않좋거나,오래 시간이 지나면.
		//원형버퍼좀 손봐야 할듯 일단 먼저 돌아가도록 만드는게 필요할듯. 아니면 1번 부분과 2번 부분을 합쳐주는과정을
		//한번씩 반복해 주거나. 근데 패킷이 크지 않고 네트워크 상황이 좋을것 같으니 일단 사용하는걸로.

		return 0;
	}

	void IOCPNetwork::addRecvPacketToQueue(const int sessionIndex, const short pktId, const short bodysize, char * datapos)
	{
		PacketRaw pkt(sessionIndex, (Packet::PacketId)pktId, bodysize, datapos);

		_recvQueue->push(pkt);

	}


}
