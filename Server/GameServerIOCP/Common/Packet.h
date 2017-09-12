#pragma once

#include <string>
#include <vector>
#include <json\json\json.h>

namespace Packet
{
	class IJsonSerializable
	{
	public:
		virtual ~IJsonSerializable( void ) {};
		virtual void Serialize( Json::Value& root ) =0;
		virtual void Deserialize( Json::Value& root ) =0;
	};

	class CJsonSerializer
	{
	public:
		static bool Serialize( IJsonSerializable* pObj, std::string& output );
		static bool Deserialize( IJsonSerializable* pObj, std::string& input );

	private:
		CJsonSerializer( void ) {};
	};

	// C++ IOCP������ Unity C#�� ����ϱ� ���� ��Ŷ ����
	//���Ӽ��� ���ӿ�û
	class GAMESEVER_REQ_GAMESERVER_ENTER : public IJsonSerializable
	{
	public:
		GAMESEVER_REQ_GAMESERVER_ENTER( void ) {}
		virtual ~GAMESEVER_REQ_GAMESERVER_ENTER( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		std::string					ID;
		std::string					AuthToken;
		int							GameServerID;
	};

	//���Ӽ��� ���Ӵ亯
	class GAMESEVER_RES_GAMESERVER_ENTER : public IJsonSerializable
	{
	public:
		GAMESEVER_RES_GAMESERVER_ENTER( void ) {}
		virtual ~GAMESEVER_RES_GAMESERVER_ENTER( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							Result;
	};

	//���Ӽ��� ���� ����
	class GAMESEVER_NTF_NEW_USER : public IJsonSerializable
	{
	public:
		GAMESEVER_NTF_NEW_USER( void ) {}
		virtual ~GAMESEVER_NTF_NEW_USER( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		std::string					NewUserID;
		std::string					NewUserIndex;
	};

	//���Ӽ��� ���� ��û
	class GAMESEVER_REQ_GAMESERVER_INFO : public IJsonSerializable
	{
	public:
		GAMESEVER_REQ_GAMESERVER_INFO( void ) {}
		virtual ~GAMESEVER_REQ_GAMESERVER_INFO( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		std::string					ID;
		std::string					AuthToken;
	};

	//���Ӽ��� ���� �亯
	class GAMESEVER_RES_GAMESERVER_INFO : public IJsonSerializable
	{
	public:
		GAMESEVER_RES_GAMESERVER_INFO( void ) {}
		virtual ~GAMESEVER_RES_GAMESERVER_INFO( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							Result;
		int							ServerUserNum;
		int							ServerMatchNum;
	};

	//���Ӽ��� �� ��ġ ���� �Է� ��û
	class GAMESEVER_REQ_SHIP_DEPLOY_INFO : public IJsonSerializable
	{
	public:
		GAMESEVER_REQ_SHIP_DEPLOY_INFO( void ) {}
		virtual ~GAMESEVER_REQ_SHIP_DEPLOY_INFO( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		std::string					ID;
		std::string					AuthToken;
		unsigned					NumOfTile;
		std::string					ShipOnTileInfo;
	};

	//���Ӽ��� �� ��ġ ���� �Է� �亯
	class GAMESEVER_RES_SHIP_DEPLOY_INFO : public IJsonSerializable
	{
	public:
		GAMESEVER_RES_SHIP_DEPLOY_INFO( void ) {}
		virtual ~GAMESEVER_RES_SHIP_DEPLOY_INFO( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							Result;
	};

	//���Ӽ��� ���ӽ��� ��û
	class GAMESEVER_REQ_GAME_READY : public IJsonSerializable
	{
	public:
		GAMESEVER_REQ_GAME_READY( void ) {}
		virtual ~GAMESEVER_REQ_GAME_READY( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		std::string					ID;
		std::string					AuthToken;
	};

	//���Ӽ��� ���ӽ��� �亯
	class GAMESEVER_RES_GAME_READY : public IJsonSerializable
	{
	public:
		GAMESEVER_RES_GAME_READY( void ) {}
		virtual ~GAMESEVER_RES_GAME_READY( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							Result;
	};

	//���Ӽ��� ���ӽ��� ����ڵ鿡�� ���� ������ ������ ����Ʈ�� �˼� �ִ�.
	class GAMESEVER_NTF_GAME_START : public IJsonSerializable
	{
	public:
		GAMESEVER_NTF_GAME_START( void ) {}
		virtual ~GAMESEVER_NTF_GAME_START( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							Result;
	};

	//���Ӽ��� ��ź ��ġ ��û
	class GAMESEVER_REQ_BOMB : public IJsonSerializable
	{
	public:
		GAMESEVER_REQ_BOMB( void ) {}
		virtual ~GAMESEVER_REQ_BOMB( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		std::string					ID;
		std::string					AuthToken;
		int							BombedTile;
	};

	//���Ӽ��� ��ź ��ġ �亯
	class GAMESEVER_RES_BOMB : public IJsonSerializable
	{
	public:
		GAMESEVER_RES_BOMB( void ) {}
		virtual ~GAMESEVER_RES_BOMB( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							Result;
		int							BombedTile;
	};

	//���Ӽ��� ��ź ��ġ ���� Result�� �¾Ҵ��� �ƴ��� �׸��� ���� �˼� �ִ�.
	class GAMESEVER_NTF_BOMB : public IJsonSerializable
	{
	public:
		GAMESEVER_NTF_BOMB( void ) {}
		virtual ~GAMESEVER_NTF_BOMB( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							Result;
		int							BombedTile;
	};

	//���Ӽ��� ���� ����
	class GAMESEVER_NTF_GAMEND : public IJsonSerializable
	{
	public:
		GAMESEVER_NTF_GAMEND( void ) {}
		virtual ~GAMESEVER_NTF_GAMEND( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							Result;
	};

	//���Ӽ��� ���� ��Ʈ��Ʈ ��û
	class GAMSERVER_REQ_USER_HEARTBEAT : public IJsonSerializable
	{
	public:
		GAMSERVER_REQ_USER_HEARTBEAT( void ) {}
		virtual ~GAMSERVER_REQ_USER_HEARTBEAT( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		std::string					ID;
		std::string					AuthToken;
	};

	//���Ӽ��� ���� ��Ʈ��Ʈ �亯
	class GAMSERVER_RES_USER_HEARTBEAT : public IJsonSerializable
	{
	public:
		GAMSERVER_RES_USER_HEARTBEAT( void ) {}
		virtual ~GAMSERVER_RES_USER_HEARTBEAT( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							Result;
	};

	//���Ӽ��� �α׾ƿ� ��û
	class GAMSERVER_REQ_USER_LOGOUT : public IJsonSerializable
	{
	public:
		GAMSERVER_REQ_USER_LOGOUT( void ) {}
		virtual ~GAMSERVER_REQ_USER_LOGOUT( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		std::string					ID;
		std::string					AuthToken;
	};

	//���Ӽ��� �α׾ƿ� �亯 �亯�� ���� ���´�.
	class GAMSERVER_RES_USER_LOGIN : public IJsonSerializable
	{
	public:
		GAMSERVER_RES_USER_LOGIN( void ) {}
		virtual ~GAMSERVER_RES_USER_LOGIN( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							Result;
	};

	//���Ӽ��� �α׾ƿ� ����
	class GAMSERVER_NTF_USER_LOGOUT : public IJsonSerializable
	{
	public:
		GAMSERVER_NTF_USER_LOGOUT( void ) {}
		virtual ~GAMSERVER_NTF_USER_LOGOUT( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							Result;
	};

	enum PacketId
	{
		ID_GAMESEVER_REQ_GAMESERVER_ENTER			= 101,
		ID_GAMESEVER_RES_GAMESERVER_ENTER			= 102,
		ID_GAMESEVER_NTF_NEW_USER			= 103,
		ID_GAMESEVER_REQ_GAMESERVER_INFO			= 104,
		ID_GAMESEVER_RES_GAMESERVER_INFO			= 105,
		ID_GAMESEVER_REQ_SHIP_DEPLOY_INFO			= 106,
		ID_GAMESEVER_RES_SHIP_DEPLOY_INFO			= 107,
		ID_GAMESEVER_REQ_GAME_READY			= 108,
		ID_GAMESEVER_RES_GAME_READY			= 109,
		ID_GAMESEVER_NTF_GAME_START			= 110,
		ID_GAMESEVER_REQ_BOMB			= 111,
		ID_GAMESEVER_RES_BOMB			= 112,
		ID_GAMESEVER_NTF_BOMB			= 113,
		ID_GAMESEVER_NTF_GAMEND			= 114,
		ID_GAMSERVER_REQ_USER_HEARTBEAT			= 115,
		ID_GAMSERVER_RES_USER_HEARTBEAT			= 116,
		ID_GAMSERVER_REQ_USER_LOGOUT			= 117,
		ID_GAMSERVER_RES_USER_LOGIN			= 118,
		ID_GAMSERVER_NTF_USER_LOGOUT			= 119,
	};
}