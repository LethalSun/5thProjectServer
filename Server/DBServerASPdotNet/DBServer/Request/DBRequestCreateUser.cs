using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Text;
using System.Threading.Tasks;

using System.Web.Http;

namespace DBServer.Request
{
    public class DBRequestCreateUserController :ApiController
    {
        [Route("Request/DBRequestCreateUser")]
        [HttpPost]
        public async Task<DB_RES_CREATE_USER> Process(DB_REQ_CREATE_USER requestPacket)
        {
            var responseResult = new DB_RES_CREATE_USER();

            if(string.IsNullOrEmpty(requestPacket.userID))
            {
                return responseResult.Return(ERROR_CODE.REQ_CREATE_USER_INVALID_ID);
            }

            var uid = await Data.UserRepository.AddUser(
                requestPacket.userID, 
                requestPacket.PW);

            if(uid == 0)
            {
                return responseResult.Return(ERROR_CODE.REQ_CREATE_USER_DUPLICATE_USER_ID);
            }

            //TODO: 만약에 아이디는 없는데 게임이 데이터가 남아 있다면? 오류 검사를해야 하지 않을까?
            await Data.BasicGameDataRepository.AddGameData(requestPacket.userID);

            responseResult.SetResult(ERROR_CODE.NONE);

            return responseResult;
        }
    }
}