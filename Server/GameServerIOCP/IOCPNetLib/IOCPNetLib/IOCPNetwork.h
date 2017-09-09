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
		//���� ������ ���� �Ѵ�.
		bool copyConfig(const MDUtillity::ServerProperty* config);
		
		//��Ÿ �ʱ⼳����
		bool makeSocket();
		bool makeCP();
		bool makeThreadPool(int threadNum = 0);
		bool makeSessionPool();
		bool bindAndListen();

		//���ο� Ŭ���̾�Ʈ�� �޾� ���δ�.
		bool acceptThreadFunc();

		//������ ���鼭 ������ ������ ���忹���� �ɾ��ش�.
		//���ǿ��� �������� ������ �Ѵٰ� ���� ��Ŷ�� ����ִ� ť�� �����ִµ�
		//������ ���� ���°� �ƴ϶�� ť���� ��Ŷ�� ������ ���ۿ� ���� ������ ������ �Ѵ�.
		bool sendThreadFunc();
		bool tempSendFunc();

		//�����⳪ �ޱⰡ �Ϸ�Ǹ� �˸��� ó���� ���ش�.
		unsigned int WINAPI workerThreadFunc(LPVOID lpParam);
		int receiveProcess(Session* sessionAddr, int dataSize);
		void addRecvPacketToQueue(const int sessionIndex, const short pktId, const short bodysize, char* datapos);


	};
}


