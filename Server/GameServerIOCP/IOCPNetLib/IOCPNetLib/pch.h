#pragma once
#pragma comment(lib,"ws2_32")

#include <iostream>

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <array>
#include <memory>
#include <concurrent_queue.h>
#include "Define.h"
#include "ServerNetErrorCode.h"

using PacketQueueConccurency = concurrency::concurrent_queue<MDServerNetLib::PacketInfo>;
