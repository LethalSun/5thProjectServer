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
		//���� ������ ���� �Ѵ�.
		bool copyConfig(const ServerProperty* config);
		
		//��Ÿ �ʱ⼳����
		bool makeAcceptSocket();
		bool makeCP();
		bool makeThreadPool(int threadNum = 0);
		bool makeSessionPool();
		bool bindAndListen();

		//���ο� Ŭ���̾�Ʈ�� �޾� ���δ�.
		bool acceptThreadFunc();

		//��Ŀ ������� ���ø�Ʈ ��Ʈ ó����, ������ ó���� ���ش�.
		//���ø�Ʈ ��Ʈ�� ������ �Ϸ�� ������ŭ ���۸� ����ְ� ���۰� ���� �ִٸ�
		//�ٽ� ���� ��û�� �Ѵ�.
		//�ޱ� �Ϸ�� ���� ��Ŷ�� �ٽ� ��Ŷ���� ���� _recvQueue�� �־� �ش�
		//������ ó���� _sendQueue�� ��Ŷ�� �ִٸ� ����Ŷ�� ���ǹ��ۿ� ��Ŷ�� ����
		//���� ������ ���ش�. ������ ���� ť�� ������ �Ѱ��� �����尡 �����Ѵ�.
		//�ֳĸ� ������ �ϳ��� ť���� ��Ŷ�� ���µ� �ٸ������尡 ���� ������ ��Ŷ�����ٸ�
		//���� ���ǿ� �ΰ��� �����尡 �����Ұ��̰� � �Ѽ��ǿ� ���ÿ� �����ϰ� �Ǹ�
		//���������� ���ؼ� ���� �ɰų� �ٸ� ó���� �ؾ� �Ѵ�. �׷��Ƿ� �ϳ��� �����忡��
		//�ϴ� �����ִ� ��Ŷ�� �� ���忹���� �ɾ� ������ �ΰ��� �����尡 ���ÿ� ���� �Ҽ� ����.
		//���� �ѹ��� �� �������� �ʰ� �Ϸ�ť�� ���ͼ�
		//��Ŀ �����忡 ��� �Դٸ� ���� ������ �ִ� �����̹Ƿ� �����Ѵ�.
		//���� ������ ��Ʈ��ũ�� ������ �ִٸ� ���� �����ؾ� �Ѵ�.
		unsigned int WINAPI workerThreadFunc(LPVOID lpParam);

		void doIOCPJob();
		void doSendJob();

		int receiveProcess(Session* sessionAddr, int dataSize);
		void addRecvPacketToQueue(const int sessionIndex, const short pktId, const short bodysize, char* datapos);

	private:
		//������ �α�
		ServerProperty _property;
		MDUtillity::LoggerBase* _logger;

		//���ϰ� ���ø�Ʈ ��Ʈ �ڵ�
		SOCKET _acceptSocket;
		HANDLE _cpHandle;

		//�������
		///������ �޴� ������
		std::thread _acceptThread;
		///���� ť�� �ִ� ��Ŷ�� ������ ������ ���ۿ� ���� ������.
		std::thread _sendThread;
		///���� ���ۿ� �ִ� ������ ����� �����ϰ� ���ú� ������ ������ ó���ؼ� ť�� �ִ´�.
		ThreadPool _threads;
		int _createdThreadNum = 0;

		//������ �����ϴ� ��ü.
		std::unique_ptr<SessionPool>_sessionPool;

		//�ܺοʹ� ť�� �̿��ؼ� ������ ��Ŷ�� �ְ� �޴´�.
		PacketQueueConccurency* _recvQueue;
		PacketQueueConccurency* _sendQueue;
		std::atomic<bool> _isSendQueueAvailable{ true };
	};
}


