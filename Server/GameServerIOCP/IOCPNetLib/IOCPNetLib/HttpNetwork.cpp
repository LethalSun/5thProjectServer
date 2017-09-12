
#include "HttpNetwork.h"

namespace MDServerNetLib
{
	void onBegin(const happyhttp::Response* res, void* udata)
	{

	}
	
	void onData(const happyhttp::Response* res, void* udata, const unsigned char* data, int n)
	{

	}

	void onComplete()
	{

	}

	HttpNetwork::HttpNetwork()
	{
	}


	HttpNetwork::~HttpNetwork()
	{
	}

	NET_ERROR_CODE HttpNetwork::PostReqGetUserAuth(std::string id, std::string token)
	{
		return NET_ERROR_CODE();
	}

	NET_ERROR_CODE HttpNetwork::PostReqRemoveUserAuth(std::string id, std::string token)
	{
		return NET_ERROR_CODE();
	}

	NET_ERROR_CODE HttpNetwork::PostReqGetUserData(std::string id, std::string token)
	{
		return NET_ERROR_CODE();
	}

	NET_ERROR_CODE HttpNetwork::PostReqSetUserData(std::string id, std::string token, int rating, int win, int lose)
	{
		return NET_ERROR_CODE();
	}

	std::string HttpNetwork::PostReq(std::string)
	{
		return std::string();
	}

	bool HttpNetwork::LoadProperty()
	{
		return false;
	}

}
