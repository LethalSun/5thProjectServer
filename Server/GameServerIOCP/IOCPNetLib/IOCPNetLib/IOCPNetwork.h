#pragma once

#pragma comment(lib,"ws2_32")

#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <thread>
#include <concurrent_queue.h>
#include <vector>
#include "SeverProperty.h"
#include "ErrorCode.h"

namespace MDUtillity
{
	class LoggerBase;

}

namespace MDServerNetLib
{
	using ThreadPool = std::vector<std::thread>;
	class ServerProperty;
	class SessionPool;
	class Session;
	using PacketQueueConccurency = concurrency::concurrent_queue<PacketInfo>;
}

namespace MDServerNetLib
{
	
	class IOCPNetwork
	{
	public:
		IOCPNetwork(const ServerProperty*  config, 
			MDUtillity::LoggerBase* logger);
		~IOCPNetwork();

		NET_ERROR_CODE init();
		NET_ERROR_CODE SetPacketQueue(PacketQueueConccurency* recvQue, PacketQueueConccurency* sendQue);

		PacketInfo GetReceivedPacketTemp();

		void TempPushSendPacketQueue(const int pSessionIndex, const short pPacketId, const short pSize, const char * pMsg);

		int ClientSessionPoolSize();

		void Release();
	private:
		//서버 설정을 복사 한다.
		bool copyConfig(const ServerProperty* config);
		
		//기타 초기설정들
		bool makeAcceptSocket();
		bool makeCP();
		bool makeThreadPool(int threadNum = 0);
		bool makeSessionPool();
		bool bindAndListen();

		//새로운 클라이언트를 받아 들인다.
		bool acceptThreadFunc();

		//워커 스레드는 컴플리트 포트 처리와, 보내기 처리를 해준다.
		//컴플리트 포트는 보내기 완료시 보낸만큼 버퍼를 비워주고 버퍼가 남아 있다면
		//다시 센드 요청을 한다.
		//받기 완료시 받은 패킷을 다시 패킷으로 만들어서 _recvQueue에 넣어 준다
		//보내기 처리는 _sendQueue에 패킷이 있다면 그패킷의 세션버퍼에 패킷을 쓰고
		//센드 예약을 해준다. 하지만 센드 큐는 언제나 한개의 스레드가 전담한다.
		//왜냐면 스레드 하나가 큐에서 패킷을 뺏는데 다른쓰레드가 같은 세션의 패킷을뺐다면
		//같은 세션에 두개의 스레드가 접근할것이고 어떤 한세션에 동시에 접근하게 되면
		//순서보장을 위해서 락을 걸거나 다른 처리를 해야 한다. 그러므로 하나의 스레드에서
		//일단 현재있는 패킷을 다 센드예약을 걸어 놓으면 두개의 스레드가 동시에 접근 할수 없다.
		//만약 한번에 다 보내지지 않고 완료큐에 들어와서
		//워커 스레드에 들어 왔다면 뭔가 문제가 있는 세션이므로 종료한다.
		//만약 서버의 네트워크에 문제가 있다면 역시 종료해야 한다.
		unsigned int WINAPI workerThreadFunc(LPVOID lpParam);

		void doIOCPJob();
		void doSendJob();

		int receiveProcess(Session* sessionAddr, int dataSize);
		void addRecvPacketToQueue(const int sessionIndex, const short pktId, const short bodysize, char* datapos);

	private:
		//설정과 로그
		ServerProperty _property;
		MDUtillity::LoggerBase* _logger;

		//소켓과 컴플리트 포트 핸들
		SOCKET _acceptSocket;
		HANDLE _cpHandle;

		//스레드들
		///세션을 받는 스레드
		std::thread _acceptThread;
		///센드 큐에 있는 패킷을 각각의 세션의 버퍼에 쓰는 스레드.
		std::thread _sendThread;
		///센드 버퍼에 있는 내용을 입출력 예약하고 리시브 버퍼의 내용을 처리해서 큐에 넣는다.
		ThreadPool _threads;
		int _createdThreadNum = 0;

		//세션을 관리하는 객체.
		std::unique_ptr<SessionPool>_sessionPool;

		//외부와는 큐를 이용해서 로직과 패킷을 주고 받는다.
		PacketQueueConccurency* _recvQueue;
		PacketQueueConccurency* _sendQueue;
		std::atomic<bool> _isSendQueueAvailable{ true };
	};
}


