using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Text;
using System.Threading.Tasks;

using System.Web.Http;
using System.Security.Cryptography;

namespace DBServer.Request
{
    public class DBRequestGetUserInfoController : ApiController
    {
        [Route("Request/DBRequestGetUserInfo")]
        [HttpPost]
        public async Task<DB_RES_GET_USER_INFO> Process(DB_REQ_GET_USER_INFO requestPacket)
        {
            var responseResult = new DB_RES_GET_USER_INFO();

            var userObject = await Data.UserRepository.GetUser(requestPacket.userID);

            if(userObject == null)
            {
                return responseResult.Return(ERROR_CODE.REQ_LOGIN_INVALID_USER); ;
            }

            if(userObject.PW != requestPacket.PW)
            {
                return responseResult.Return(ERROR_CODE.REQ_LOGIN_INVALID_PW);
            }

            var authToken = TokenGenerator.GetRandomString();

            await Data.AuthTokenRepository.AddAuth(requestPacket.userID, authToken);

            var serverId = await GameServerSelector.GetAvailableAddr();

            var userData = await Data.BasicGameDataRepository.GetGameData(requestPacket.userID);

            if (userData == null)
            {
                return responseResult.Return(ERROR_CODE.REQ_LOGIN_NO_DATA); ;
            }

            responseResult.SetResult(ERROR_CODE.NONE);
            responseResult.AuthToken = authToken;
            responseResult.NumWin = userData.WinNumber;
            responseResult.NumLose = userData.LoseNumber;
            responseResult.Rating = userData.Rating;
            responseResult.GameServerAddress = serverId;

            return responseResult;
        }
    }
}