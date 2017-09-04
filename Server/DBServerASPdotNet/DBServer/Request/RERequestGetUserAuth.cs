using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Text;
using System.Threading.Tasks;

using System.Web.Http;

namespace DBServer.Request
{

    public class RERequestGetUserAuthController : ApiController
    {
        [Route("Request/RERequestGetUserAuth")]
        [HttpPost]
        public async Task<RE_RES_GET_USER_AUTH> Process(RE_REQ_GET_USER_AUTH requestPacket)
        {
            RE_RES_GET_USER_AUTH responsePacket = new RE_RES_GET_USER_AUTH();

            responsePacket.SetResult(ERROR_CODE.NONE);

            var isOk = await Data.AuthTokenRepository.CheckAuth(requestPacket.UserID, requestPacket.AuthToken, requestPacket.GameServerAddress);
            if (isOk == false)
            {
                responsePacket.Return(ERROR_CODE.REQ_AUTH_FAIL);
            }

            return responsePacket;
        }
    }
}