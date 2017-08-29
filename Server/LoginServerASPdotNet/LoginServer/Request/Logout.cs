using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Web.Http;
using LoginServer.Data;

namespace LoginServer.Request
{
    public class LogoutController : ApiController
    {
        [Route("Request/Logout")]
        [HttpPost]
        public async Task<RES_LOGOUT> Process(REQ_LOGOUT requestPacket)
        {
            var responseResult = new RES_LOGOUT();

            var userObject = await AuthTokenRepository.Check(requestPacket.UserID, requestPacket.AuthToken);

            if (userObject == false)
            {
                return responseResult.Return(ERROR_CODE.REQ_LOGOUT_FAIL_CHECK_ID_OR_AUTHTOKEN);
            }

            await AuthTokenRepository.Delete(requestPacket.UserID);

            responseResult.SetResult(ERROR_CODE.NONE);
            return responseResult;
        }
    }
}