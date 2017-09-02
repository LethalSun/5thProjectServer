using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.IO;
using System.Text;
using System.Threading.Tasks;

using MongoDB.Driver;

namespace DBServer.Data
{
    public class UserRepository
    {
        public static async Task<long> AddUser(string userID, string pw)
        {
            try
            {
                //유니크 번호 받아오기
                var uid = UniqueSeqNumberGenerator.GetUniqueNumber();

                //초기화
                var user = new DB.DBUser()
                {
                    UID = uid,
                    _id = userID,
                    PW = pw,
                };

                //추가 추가가 안되면 예외
                var collection = DB.MongoDBLib.GetAccountDBUserCollection<DB.DBUser>();
                var option = new InsertOneOptions();
                option.BypassDocumentValidation = true;
                await collection.InsertOneAsync(user, option);

                return uid;
            }
            catch(Exception)
            {
                //즉 0이면 이미 존재하는 유저
                return 0;
            }
        }

        public static async Task<DB.DBUser>GetUser(string userID)
        {
            var collection = DB.MongoDBLib.GetAccountDBUserCollection<DB.DBUser>();
            var data = await collection.Find(x => x._id == userID).FirstOrDefaultAsync();

            return data;
        }


    }
}