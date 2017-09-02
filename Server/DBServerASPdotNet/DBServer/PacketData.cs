using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace DBServer
{
    //유저 생성 아이디 패스워드 레이팅 승수 패수 등을 DB에 만든다.
    #region DB_REQ_CREATE_USER

    public struct DB_REQ_CREATE_USER
    {
        public string userID;
        public string PW;
    }

    public struct DB_RES_CREATE_USER
    {
        public DB_RES_CREATE_USER Return(ERROR_CODE error)
        {
            Result = (short)error;
            return this;
        }

        public void SetResult(ERROR_CODE error)
        {
            Result = (short)error;
        }

        public short Result;
    }
    #endregion

    //로그인시 계정의 아이디 패스워드를 보내면 나머지 정보를 받는다.
    #region DB_REQ_GET_USER_INFO

    public struct DB_REQ_GET_USER_INFO
    {
        public string userID;
        public string PW;
    }
    //레이팅은 기본 값이 있고 승패에 따라서 서로가 서로의 점수를 빼았는다.
    //(총량은 변하지 않는다) 신규 유저의 경우 실력을 알수 없으므로일단 절대 변하지 않는 평균값을 기준으로 해서 레이팅의 평균은 계속 유지 된다.
    //보통은 레이팅에 따라서 변하는 최대의 값이 줄어들어서 보통 상한이 정해지게 되지만
    //일단은 일괄적으로 32를 적용하자.
    public struct DB_RES_GET_USER_INFO
    {
        public DB_RES_GET_USER_INFO Return(ERROR_CODE error)
        {
            Result = (short)error;
            return this;
        }

        public void SetResult(ERROR_CODE error)
        {
            Result = (short)error;
        }

        public short Result;
        public int NumWin;
        public int NumLose;
        public int Rating;
        public string GameServerAddress;
        public string AuthToken;
    }

    #endregion

    //게임한판이 종료되었을때 승수와 패수 그리고 레이팅을 갱신한다.
    #region DB_REQ_SET_USER_DATA

    public struct DB_REQ_SET_USER_DATA
    {
        public string userID;
        public int NumWin;
        public int NumLose;
        public int Rating;
    }

    public struct DB_RES_SET_USER_DATA
    {
        public DB_RES_SET_USER_DATA Return(ERROR_CODE error)
        {
            Result = (short)error;
            return this;
        }

        public void SetResult(ERROR_CODE error)
        {
            Result = (short)error;
        }

        public short Result;
    }

    #endregion

    //매번 유저가 게임서버와 통신할때 토큰과 게임서버 아이디를 확인한다.
    #region RE_REQ_GET_USER_AUTH

    public struct RE_REQ_GET_USER_AUTH
    {
        public string UserID;
        public string AuthToken;
    }

    public struct RE_RES_GET_USER_AUTH
    {
        public RE_RES_GET_USER_AUTH Return(ERROR_CODE error)
        {
            Result = (short)error;
            return this;
        }

        public void SetResult(ERROR_CODE error)
        {
            Result = (short)error;
        }

        public short Result;
    }


    #endregion

    //유저가 로그아웃을 하거나 비정상일때 유저의 정보를 제거한다.
    #region RE_REQ_REMOVE_USER_AUTHKEY
    public struct RE_REQ_REMOVE_USER_AUTHKEY
    {
        public string UserID;
        public string AuthToken;
    }

    public struct RE_RES_REMOVE_USER_AUTHKEY
    {
        public RE_RES_REMOVE_USER_AUTHKEY Return(ERROR_CODE error)
        {
            Result = (short)error;
            return this;
        }

        public void SetResult(ERROR_CODE error)
        {
            Result = (short)error;
        }

        public short Result;
    }
    #endregion

    //게임 서버가 로그인을 받으면 자신에게 입장한 유저수를 갱신한다.
    #region RE_REQ_SET_GAMSERVER_USER_NUMBER

    public struct RE_REQ_SET_GAMSERVER_USER_NUMBER
    {
        public int ServerID;
        public int UserNum;
    }

    public struct RE_RES_SET_GAMSERVER_USER_NUMBER
    {
        public RE_RES_SET_GAMSERVER_USER_NUMBER Return(ERROR_CODE error)
        {
            Result = (short)error;
            return this;
        }

        public void SetResult(ERROR_CODE error)
        {
            Result = (short)error;
        }

        public short Result;
    }

    #endregion

    //게임 서버가 죽었는지 안죽었는지 확인해서 레디스에 남아있는 정보를 지워주기 위한것
    #region RE_REQ_GAMESERVER_HEARTBEAT

    public struct RE_REQ_GAMESERVER_HEARTBEAT
    {
        public int ServerID;
    }

    public struct RE_RES_GAMESERVER_HEARTBEAT
    {
        public RE_RES_GAMESERVER_HEARTBEAT Return(ERROR_CODE error)
        {
            Result = (short)error;
            return this;
        }

        public void SetResult(ERROR_CODE error)
        {
            Result = (short)error;
        }

        public short Result;
    }
    #endregion

    //게임 서버가 정상 종료 할경우 게임 데이터를 지운다. 근데 필요 할까?
    #region RE_REQ_REMOVE_GAMESERVER_DATA

    public struct RE_REQ_REMOVE_GAMESERVER_DATA
    {
        public int ServerID;
    }

    public struct RE_RES_REMOVE_GAMESERVER_DATA
    {
        public RE_RES_REMOVE_GAMESERVER_DATA Return(ERROR_CODE error)
        {
            Result = (short)error;
            return this;
        }

        public void SetResult(ERROR_CODE error)
        {
            Result = (short)error;
        }

        public short Result;
    }
    #endregion


}