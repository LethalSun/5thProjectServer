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

	// C++ IOCP서버와 Unity C#이 통신하기 위한 패킷 정의
	//게임서버 접속요청
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

	//게임서버 접속답변
	class GAMESEVER_RES_GAMESERVER_ENTER : public IJsonSerializable
	{
	public:
		GAMESEVER_RES_GAMESERVER_ENTER( void ) {}
		virtual ~GAMESEVER_RES_GAMESERVER_ENTER( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							Result;
	};

	//게임서버 접속 통지
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

	//게임서버 정보 요청
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

	//게임서버 정보 답변
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

	//게임서버 배 배치 정보 입력 요청
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

	//게임서버 배 배치 정보 입력 답변
	class GAMESEVER_RES_SHIP_DEPLOY_INFO : public IJsonSerializable
	{
	public:
		GAMESEVER_RES_SHIP_DEPLOY_INFO( void ) {}
		virtual ~GAMESEVER_RES_SHIP_DEPLOY_INFO( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							Result;
	};

	//게임서버 게임시작 요청
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

	//게임서버 게임시작 답변
	class GAMESEVER_RES_GAME_READY : public IJsonSerializable
	{
	public:
		GAMESEVER_RES_GAME_READY( void ) {}
		virtual ~GAMESEVER_RES_GAME_READY( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							Result;
	};

	//게임서버 게임시작 당사자들에게 통지 누구의 턴인지 리절트로 알수 있다.
	class GAMESEVER_NTF_GAME_START : public IJsonSerializable
	{
	public:
		GAMESEVER_NTF_GAME_START( void ) {}
		virtual ~GAMESEVER_NTF_GAME_START( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							Result;
	};

	//게임서버 폭탄 배치 요청
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

	//게임서버 폭탄 배치 답변
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

	//게임서버 폭탄 배치 통지 Result로 맞았는지 아닌지 그리고 턴을 알수 있다.
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

	//게임서버 종료 통지
	class GAMESEVER_NTF_GAMEND : public IJsonSerializable
	{
	public:
		GAMESEVER_NTF_GAMEND( void ) {}
		virtual ~GAMESEVER_NTF_GAMEND( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							Result;
	};

	//게임서버 유저 하트비트 요청
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

	//게임서버 유저 하트비트 답변
	class GAMSERVER_RES_USER_HEARTBEAT : public IJsonSerializable
	{
	public:
		GAMSERVER_RES_USER_HEARTBEAT( void ) {}
		virtual ~GAMSERVER_RES_USER_HEARTBEAT( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							Result;
	};

	//게임서버 로그아웃 요청
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

	//게임서버 로그아웃 답변 답변이 오면 끊는다.
	class GAMSERVER_RES_USER_LOGIN : public IJsonSerializable
	{
	public:
		GAMSERVER_RES_USER_LOGIN( void ) {}
		virtual ~GAMSERVER_RES_USER_LOGIN( void ) {}
		virtual void Serialize ( Json::Value& root );
		virtual void Deserialize( Json::Value& root );

		int							Result;
	};

	//게임서버 로그아웃 통지
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