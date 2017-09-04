using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Text;
using System.Threading.Tasks;

namespace DBServer.Data
{
    public class AuthTokenRepository
    {
        public static async Task AddAuth(string userID, string authToken)
        {
            //TODO: 게임 서버 아이디 받아와서 넣는 동작 필요,
           // DB.DBBasicGameData

            await DB.Redis.SetStringAsync<DBUserSession>(userID, new DBUserSession()
                { AuthToken = authToken,
                GameServerAddress = "",
                  CV = 1,
                  CDV = 1, });
        }

        public static async Task<bool> CheckAuth(string userID,string authToken, string gameServerAddress)
        {
            var sessionInfo = await DB.Redis.GetStringAsync<DBUserSession>(userID);

            if(sessionInfo.Item1 == false|| 
                sessionInfo.Item2.AuthToken != authToken ||
                sessionInfo.Item2.GameServerAddress != gameServerAddress)

            {
                return false;
            }

            return true;
        }

        public static async Task<bool> Delete(string userID)
        {
            var isComplete = await DB.Redis.DeleteStringAsync<DBUserSession>(userID);

            return isComplete;
        }

    }

    public struct DBUserSession
    {
        public string AuthToken;
        public string GameServerAddress;
        public short CV;// ClientVersion
        public short CDV;// ClientDataVersion
    }
}