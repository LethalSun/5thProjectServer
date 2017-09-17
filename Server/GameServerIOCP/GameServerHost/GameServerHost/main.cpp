
#include <iostream>
#include "../../IOCPNetLib/IOCPNetLib/IOCPNetwork.h"
#include "../../LogicLib/LogicLib/LogicMain.h"
#include "../../IOCPNetLib/IOCPNetLib/ServerProperty.h"
#include "../../IOCPNetLib/IOCPNetLib/Logger.h"
#include "../../IOCPNetLib/IOCPNetLib/ServerPropertyReder.h"
//"../../Common/SeverProperty.json"
int main()
{

	//TODO: 디비 아이오 처리는 IOCP 워커 스레드 에서 처리한다.

	MDUtillity::Logger logger;

	MDUtillity::ServerPropertyReader propertyReader("../../Common/SeverProperty.json");

	MDServerNetLib::PacketQueueConccurency recvQueue;

	MDServerNetLib::PacketQueueConccurency sendQueue;

	auto aa = propertyReader.GetProperty();

	MDServerNetLib::IOCPNetwork iocpManger(propertyReader.GetProperty(), &logger, &recvQueue, &sendQueue);

	iocpManger.init();

	int endTrigger;

	std::cin >> endTrigger;

	//TODO: recvQueue.push(종료작업 요청 패킷);

	//TODO: iocpManger.Release();//정리.

}