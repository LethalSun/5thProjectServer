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
    public class LoginController : ApiController
    {
        [Route("Request/Login")]
        [HttpPost]
        public async Task<RES_LOGIN> Process(REQ_LOGIN requestPacket)
        {
            var responseResult = new RES_LOGIN();

            var userObject = await UserRepository.GetUser(requestPacket.UserID);

            if (userObject == null)
            {
                return responseResult.Return(ERROR_CODE.REQ_LOGIN_INVALID_USER);
            }

            if (userObject.PW != requestPacket.PW)
            {
                return responseResult.Return(ERROR_CODE.REQ_LOGIN_PW);
            }
            //시간은 좋지 않다. https://paragonie.com/blog/2016/05/how-generate-secure-random-numbers-in-various-programming-languages

            var authToken = System.DateTime.Now.Ticks.ToString();
            //레디스에 등록하기 전에 확인을 해보거나 
            await AuthTokenRepository.Add(requestPacket.UserID, authToken);

            responseResult.SetResult(ERROR_CODE.NONE);
            responseResult.AuthToken = authToken;
            return responseResult;
        }

    }
}