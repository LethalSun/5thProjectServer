using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace DBServer
{
    //유저 생성 아이디 패스워드 레이팅 승수 패수 등을 DB에 만든다.
    #region DB_REQ_CREATE_USER
    #endregion

    //로그인시 계정의 아이디 패스워드를 보내면 나머지 정보를 받는다.
    #region DB_REQ_GET_USER_INFO
    #endregion

    //게임한판이 종료되었을때 승수와 패수 그리고 레이팅을 갱신한다.
    #region DB_REQ_SET_USER_DATA
    #endregion

    //매번 유저가 게임서버와 통신할때 토큰과 게임서버 아이디를 확인한다.
    #region RE_REQ_GET_USER_AUTH
    #endregion

    //유저가 로그아웃을 하거나 비정상일때 유저의 정보를 제거한다.
    #region RE_REQ_REMOVE_USER_AUTHKEY
    #endregion

    //게임 서버가 로그인을 받으면 자신에게 입장한 유저수를 갱신한다.
    #region RE_REQ_SET_GAMSERVER_USER_NUMBER
    #endregion

    //게임 서버가 죽었는지 안죽었는지 확인해서 레디스에 남아있는 정보를 지워주기 위한것
    #region RE_REQ_GAMESERVER_HEARTBEAT
    #endregion

    //게임 서버가 정상 종료 할경우 게임 데이터를 지운다. 근데 필요 할까?
    #region RE_REQ_REMOVE_GAMESERVER_DATA
    #endregion


}