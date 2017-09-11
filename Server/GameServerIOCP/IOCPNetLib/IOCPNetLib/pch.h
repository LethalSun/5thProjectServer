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
#include "../../../ThirdPartylib/json/json/json.h"
#include "../../../ThirdPartylib//nlohmann_Json/json.hpp"