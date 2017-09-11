#pragma once
#include <string>
#include "happyhttp.h"
#include "ErrorCode.h"

namespace MDServerNetLib
{
	class HttpNetwork
	{
	public:
		HttpNetwork();
		~HttpNetwork();

		NET_ERROR_CODE PostReqGetUserAuth(std::string id, std::string token);
		NET_ERROR_CODE PostReqRemoveUserAuth(std::string id, std::string token);
		NET_ERROR_CODE PostReqGetUserData(std::string id, std::string token);
		NET_ERROR_CODE PostReqSetUserData(std::string id, std::string token, 
			int rating, int win, int lose);

	private:
		std::string PostReq(std::string);
		bool LoadProperty();
		
	};

}

