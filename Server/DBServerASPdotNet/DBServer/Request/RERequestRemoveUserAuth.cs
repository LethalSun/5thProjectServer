using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Text;
using System.Threading.Tasks;

using System.Web.Http;

namespace DBServer.Request
{
    public class RERequestRemoveUserAuthController : ApiController
    {

        [Route("Request/RERequestRemoveUserAuth")]
        [HttpPost]
        public async Task<RE_RES_REMOVE_USER_AUTH> Process(RE_REQ_REMOVE_USER_AUTH requestPacket)
        {
            RE_RES_REMOVE_USER_AUTH responsePacket = new RE_RES_REMOVE_USER_AUTH();

            return responsePacket;
        }
    }
}