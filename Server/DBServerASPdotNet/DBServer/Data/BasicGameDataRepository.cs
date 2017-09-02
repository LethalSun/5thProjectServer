using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Text;
using System.Threading.Tasks;
using MongoDB.Driver;

namespace DBServer.Data
{
    public class BasicGameDataRepository
    {
        public static async Task AddGameData(string userID)
        {
            var basicRating = 1000;

            var gameData = new DB.DBBasicGameData()
            {
                _id = userID,
                Rating = basicRating,
                WinNumber = 0,
                LoseNumber = 0,
            };

            var collection = DB.MongoDBLib.GetGameDBUserBasicDataCollection<DB.DBBasicGameData>();
            await collection.InsertOneAsync(gameData);
        }

        public static async Task<DB.DBBasicGameData> GetGameData(string userID)
        {
            var collection = DB.MongoDBLib.GetGameDBUserBasicDataCollection<DB.DBBasicGameData>();
            var data = await collection.Find(x => x._id == userID).FirstOrDefaultAsync();
            return data;
        }

        //TODO:이게 잘 돌아 갈지는 모르겠다 일단 중요한것은 아니니 나중에 수정
        public static async Task<DB.DBBasicGameData> UpdateGameData(string userID,int rating,int winNum, int loseNum)
        {
            try
            {
                var collection = DB.MongoDBLib.GetGameDBUserBasicDataCollection<DB.DBBasicGameData>();

                var result = await collection.FindOneAndUpdateAsync<DB.DBBasicGameData>
                    (
                    u => u._id == userID,
                    Builders<DB.DBBasicGameData>.Update.Set("Rating", rating).Set("WinNumber", winNum).Set("LoseNumber", loseNum),
                    new FindOneAndUpdateOptions<DB.DBBasicGameData, DB.DBBasicGameData>
                    {
                        ReturnDocument = ReturnDocument.After
                    }
                    );

                return result;
            }
            catch (Exception ex)
            {
                return null;
            }
        }
    }
}