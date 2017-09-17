
#include <iostream>
#include "../../IOCPNetLib/IOCPNetLib/IOCPNetwork.h"
#include "../../LogicLib/LogicLib/LogicMain.h"
#include "../../IOCPNetLib/IOCPNetLib/ServerProperty.h"
#include "../../IOCPNetLib/IOCPNetLib/Logger.h"
#include "../../IOCPNetLib/IOCPNetLib/ServerPropertyReder.h"
//"../../Common/SeverProperty.json"
int main()
{

	//TODO: ��� ���̿� ó���� IOCP ��Ŀ ������ ���� ó���Ѵ�.

	MDUtillity::Logger logger;

	MDUtillity::ServerPropertyReader propertyReader("../../Common/SeverProperty.json");

	MDServerNetLib::PacketQueueConccurency recvQueue;

	MDServerNetLib::PacketQueueConccurency sendQueue;

	auto aa = propertyReader.GetProperty();

	MDServerNetLib::IOCPNetwork iocpManger(propertyReader.GetProperty(), &logger, &recvQueue, &sendQueue);

	iocpManger.init();

	int endTrigger;

	std::cin >> endTrigger;

	//TODO: recvQueue.push(�����۾� ��û ��Ŷ);

	//TODO: iocpManger.Release();//����.

}