using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Text;
using System.IO;

using MongoDB.Driver;

namespace DBServer.DB
{
    public class MongoDBLib
    {
        const string ConnectString = "mongodb://localhost:27017/?maxPoolSize=200";

        const string ACCOUNT_DATABASE = "TestAccountDB";
        const string USER_DATA_COLLECTION = "Users";

        const string GAME_DATABASE = "TestGameDB";
        const string USER_BASIC_GAME_DATA_COLLECTION = "UserBasicData";

        public static IMongoCollection<T> GetAccountDBUserCollection<T>()
        {
            return GetDBCollection<T>(ACCOUNT_DATABASE, USER_DATA_COLLECTION);
        }

        public static IMongoCollection<T> GetGameDBUserBasicDataCollection<T>()
        {
            return GetDBCollection<T>(GAME_DATABASE, USER_BASIC_GAME_DATA_COLLECTION);
        }

        static IMongoCollection<T> GetDBCollection<T>(string dbName, string collectionName)
        {
            var database = GetMongoDatabase(dbName);
            var collection = database.GetCollection<T>(collectionName);
            return collection;
        }

        static IMongoDatabase GetMongoDatabase(string dbName)
        {
            MongoClient client = new MongoClient(ConnectString);
            return client.GetDatabase(dbName);
        }

        public class DBUser
        {
            public string _id;// user ID
            public Int64 UID;//unique index
            public string PW;//pw
        }

        public class DBBasicGameData
        {
            public string _id;//user ID
            public float Rating;//Rating For Match making
            public int WinNumber;
            public int LoseNumber;
            //public int Ranking;
        }
            

    }
}