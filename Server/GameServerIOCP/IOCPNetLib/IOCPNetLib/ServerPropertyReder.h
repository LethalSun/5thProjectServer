#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "ServerProperty.h"
#include "../../../ThirdPartylib/nlohmann_Json/json.hpp"
namespace MDUtillity
{
	class ServerPropertyReader
	{
	public:
		ServerPropertyReader(std::string filePath)
			:_filePath{ filePath }
		{
			loadProperty();
		}

		~ServerPropertyReader() = default;

		MDServerNetLib::ServerProperty* GetProperty()
		{
			return &_property;
		}
	private:
		void loadProperty()
		{
			std::ifstream propertyFStream(_filePath);

			if (propertyFStream.fail())
			{
				std::cout << "configfilefailed\n";
				return;
			}

			nlohmann::json propertyJson;
			propertyFStream >> propertyJson;
			_property.PortNum = propertyJson["portNum"].get<int>();
			_property.BackLog = propertyJson["backlog"].get<int>();

			_property.MaxClientCount = propertyJson["maxClientCount"].get<int>();
			_property.ExtraClientCount = propertyJson["extraClientCount"].get<int>();
			_property.MaxClientRecvBufferSize = propertyJson["maxClientRecvSize"].get<int>(); 
			_property.MaxClientSendBufferSize = propertyJson["maxClientSendSize"].get<int>();
		}



	private:

		std::string _filePath;
		MDServerNetLib::ServerProperty _property;
	};
}