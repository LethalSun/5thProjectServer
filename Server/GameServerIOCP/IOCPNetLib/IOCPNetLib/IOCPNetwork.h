#pragma once

namespace MDUtillity
{
	class LoggerBase;
	class ServerProperty;
}

namespace MDServerNetLib
{
	
	class IOCPNetwork
	{
	public:
		IOCPNetwork(const MDUtillity::ServerProperty*  config, MDUtillity::LoggerBase* logger);
		~IOCPNetwork();

	private:
		//서버 설정을 복사 한다.
		bool copyConfig(const MDUtillity::ServerProperty* config);
		
		//기타 초기설정들
		bool makeSocket();
		bool makeCP();
		bool makeThreadPool(int threadNum = 0);
		bool makeSessionPool();
		bool bindAndListen();

		//새로운 클라이언트를 받아 들인다.
		bool acceptThreadFunc();

		//세션을 돌면서 각각의 세션의 센드예약을 걸어준다.
		//세션에는 로직에서 보내야 한다고 말한 패킷을 담고있는 큐를 갖고있는데
		//보내기 예약 상태가 아니라면 큐에서 패킷을 꺼내서 버퍼에 쓰고 보내기 예약을 한다.
		bool sendThreadFunc();
		bool tempSendFunc();

		//보내기나 받기가 완료되면 알맞은 처리를 해준다.
		unsigned int WINAPI workerThreadFunc(LPVOID lpParam);
		int receiveProcess(Session* sessionAddr, int dataSize);
		void addRecvPacketToQueue(const int sessionIndex, const short pktId, const short bodysize, char* datapos);


	};
}


