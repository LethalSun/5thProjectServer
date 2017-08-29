﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LoginServer.Data
{
    public static class AuthTokenRepository
    {
        //static Dictionary<string, string> AuthTokenDic = new Dictionary<string, string>();


        public static async Task Add(string userID, string authToken)
        {
            await DB.Redis.SetStringAsync<DBUserSession>(userID, new DBUserSession() { AuthToken = authToken, CV = 1, CDV = 1 });
        }

        public static async Task<bool> Check(string userID, string authToken)
        {
            var sessionInfo = await DB.Redis.GetStringAsync<DBUserSession>(userID);
            if (sessionInfo.Item1 == false || sessionInfo.Item2.AuthToken != authToken)
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
        public short CV;            // ClientVersion
        public short CDV;           // ClientDataVersion
    }
}