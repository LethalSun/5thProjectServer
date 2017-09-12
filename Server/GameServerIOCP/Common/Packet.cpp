#include "Packet.h"

namespace Packet
{
	bool CJsonSerializer::Serialize( IJsonSerializable* pObj, std::string& output )
	{
		if (pObj == NULL)
			return false;

		Json::Value serializeRoot;
		pObj->Serialize(serializeRoot);

		Json::StyledWriter writer;
		output = writer.write( serializeRoot );

		return true;
	}

	bool CJsonSerializer::Deserialize( IJsonSerializable* pObj, std::string& input )
	{
		if (pObj == NULL)
			return false;

		Json::Value deserializeRoot;
		Json::Reader reader;

		if ( !reader.parse(input, deserializeRoot) )
			return false;

		pObj->Deserialize(deserializeRoot);

		return true;
	}

	void GAMESEVER_REQ_GAMESERVER_ENTER::Serialize( Json::Value& root )
	{
		root["ID"] = ID;
		root["AuthToken"] = AuthToken;
		root["GameServerID"] = GameServerID;
	}

	void GAMESEVER_REQ_GAMESERVER_ENTER::Deserialize( Json::Value& root )
	{
		ID = root.get("ID", "").asString();
		AuthToken = root.get("AuthToken", "").asString();
		GameServerID = root.get("GameServerID", 0).asInt();
	}

	void GAMESEVER_RES_GAMESERVER_ENTER::Serialize( Json::Value& root )
	{
		root["Result"] = Result;
	}

	void GAMESEVER_RES_GAMESERVER_ENTER::Deserialize( Json::Value& root )
	{
		Result = root.get("Result", 0).asInt();
	}

	void GAMESEVER_NTF_NEW_USER::Serialize( Json::Value& root )
	{
		root["NewUserID"] = NewUserID;
		root["NewUserIndex"] = NewUserIndex;
	}

	void GAMESEVER_NTF_NEW_USER::Deserialize( Json::Value& root )
	{
		NewUserID = root.get("NewUserID", "").asString();
		NewUserIndex = root.get("NewUserIndex", "").asString();
	}

	void GAMESEVER_REQ_GAMESERVER_INFO::Serialize( Json::Value& root )
	{
		root["ID"] = ID;
		root["AuthToken"] = AuthToken;
	}

	void GAMESEVER_REQ_GAMESERVER_INFO::Deserialize( Json::Value& root )
	{
		ID = root.get("ID", "").asString();
		AuthToken = root.get("AuthToken", "").asString();
	}

	void GAMESEVER_RES_GAMESERVER_INFO::Serialize( Json::Value& root )
	{
		root["Result"] = Result;
		root["ServerUserNum"] = ServerUserNum;
		root["ServerMatchNum"] = ServerMatchNum;
	}

	void GAMESEVER_RES_GAMESERVER_INFO::Deserialize( Json::Value& root )
	{
		Result = root.get("Result", 0).asInt();
		ServerUserNum = root.get("ServerUserNum", 0).asInt();
		ServerMatchNum = root.get("ServerMatchNum", 0).asInt();
	}

	void GAMESEVER_REQ_SHIP_DEPLOY_INFO::Serialize( Json::Value& root )
	{
		root["ID"] = ID;
		root["AuthToken"] = AuthToken;
		root["NumOfTile"] = NumOfTile;
		root["ShipOnTileInfo"] = ShipOnTileInfo;
	}

	void GAMESEVER_REQ_SHIP_DEPLOY_INFO::Deserialize( Json::Value& root )
	{
		ID = root.get("ID", "").asString();
		AuthToken = root.get("AuthToken", "").asString();
		NumOfTile = root.get("NumOfTile", 0).asInt();
		ShipOnTileInfo = root.get("ShipOnTileInfo", "").asString();
	}

	void GAMESEVER_RES_SHIP_DEPLOY_INFO::Serialize( Json::Value& root )
	{
		root["Result"] = Result;
	}

	void GAMESEVER_RES_SHIP_DEPLOY_INFO::Deserialize( Json::Value& root )
	{
		Result = root.get("Result", 0).asInt();
	}

	void GAMESEVER_REQ_GAME_READY::Serialize( Json::Value& root )
	{
		root["ID"] = ID;
		root["AuthToken"] = AuthToken;
	}

	void GAMESEVER_REQ_GAME_READY::Deserialize( Json::Value& root )
	{
		ID = root.get("ID", "").asString();
		AuthToken = root.get("AuthToken", "").asString();
	}

	void GAMESEVER_RES_GAME_READY::Serialize( Json::Value& root )
	{
		root["Result"] = Result;
	}

	void GAMESEVER_RES_GAME_READY::Deserialize( Json::Value& root )
	{
		Result = root.get("Result", 0).asInt();
	}

	void GAMESEVER_NTF_GAME_START::Serialize( Json::Value& root )
	{
		root["Result"] = Result;
	}

	void GAMESEVER_NTF_GAME_START::Deserialize( Json::Value& root )
	{
		Result = root.get("Result", 0).asInt();
	}

	void GAMESEVER_REQ_BOMB::Serialize( Json::Value& root )
	{
		root["ID"] = ID;
		root["AuthToken"] = AuthToken;
		root["BombedTile"] = BombedTile;
	}

	void GAMESEVER_REQ_BOMB::Deserialize( Json::Value& root )
	{
		ID = root.get("ID", "").asString();
		AuthToken = root.get("AuthToken", "").asString();
		BombedTile = root.get("BombedTile", 0).asInt();
	}

	void GAMESEVER_RES_BOMB::Serialize( Json::Value& root )
	{
		root["Result"] = Result;
		root["BombedTile"] = BombedTile;
	}

	void GAMESEVER_RES_BOMB::Deserialize( Json::Value& root )
	{
		Result = root.get("Result", 0).asInt();
		BombedTile = root.get("BombedTile", 0).asInt();
	}

	void GAMESEVER_NTF_BOMB::Serialize( Json::Value& root )
	{
		root["Result"] = Result;
		root["BombedTile"] = BombedTile;
	}

	void GAMESEVER_NTF_BOMB::Deserialize( Json::Value& root )
	{
		Result = root.get("Result", 0).asInt();
		BombedTile = root.get("BombedTile", 0).asInt();
	}

	void GAMESEVER_NTF_GAMEND::Serialize( Json::Value& root )
	{
		root["Result"] = Result;
	}

	void GAMESEVER_NTF_GAMEND::Deserialize( Json::Value& root )
	{
		Result = root.get("Result", 0).asInt();
	}

	void GAMSERVER_REQ_USER_HEARTBEAT::Serialize( Json::Value& root )
	{
		root["ID"] = ID;
		root["AuthToken"] = AuthToken;
	}

	void GAMSERVER_REQ_USER_HEARTBEAT::Deserialize( Json::Value& root )
	{
		ID = root.get("ID", "").asString();
		AuthToken = root.get("AuthToken", "").asString();
	}

	void GAMSERVER_RES_USER_HEARTBEAT::Serialize( Json::Value& root )
	{
		root["Result"] = Result;
	}

	void GAMSERVER_RES_USER_HEARTBEAT::Deserialize( Json::Value& root )
	{
		Result = root.get("Result", 0).asInt();
	}

	void GAMSERVER_REQ_USER_LOGOUT::Serialize( Json::Value& root )
	{
		root["ID"] = ID;
		root["AuthToken"] = AuthToken;
	}

	void GAMSERVER_REQ_USER_LOGOUT::Deserialize( Json::Value& root )
	{
		ID = root.get("ID", "").asString();
		AuthToken = root.get("AuthToken", "").asString();
	}

	void GAMSERVER_RES_USER_LOGIN::Serialize( Json::Value& root )
	{
		root["Result"] = Result;
	}

	void GAMSERVER_RES_USER_LOGIN::Deserialize( Json::Value& root )
	{
		Result = root.get("Result", 0).asInt();
	}

	void GAMSERVER_NTF_USER_LOGOUT::Serialize( Json::Value& root )
	{
		root["Result"] = Result;
	}

	void GAMSERVER_NTF_USER_LOGOUT::Deserialize( Json::Value& root )
	{
		Result = root.get("Result", 0).asInt();
	}

}