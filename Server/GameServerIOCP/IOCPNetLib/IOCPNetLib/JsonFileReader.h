#pragma once
#include <fstream>
#include <string>
#include "ServerProperty.h"
namespace MDUtillity
{
	class PropertyFileReader
	{
	public:
		PropertyFileReader(std::string filePath);
		~PropertyFileReader();
	private:
		std::string _filePath;

		MDServerNetLib::ServerProperty _property;
	};

}