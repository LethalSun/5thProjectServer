#pragma once

#include <fstream>
#include <string>
#include "ServerProperty.h"
namespace MDUtillity
{
	class MyJsonFileReader
	{
	public:
		MyJsonFileReader(std::string filePath)
			:_filePath{ filePath }
		{

		}

		~MyJsonFileReader() = default;

		void LoadProperty()
		{

		}

		MDServerNetLib::ServerProperty* GetProperty()
		{
			 
		}

	private:

		std::string _filePath;
		MDServerNetLib::ServerProperty _property;
	};
}