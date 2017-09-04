using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Text;
using System.Threading.Tasks;

using System.Web.Http;

namespace DBServer.Request
{
    public class RERequestSetGameServerUserNumberController : ApiController
    {
        [Route("Request/ERequestSetGameServerUserNumber")]
        [HttpPost]
        public async Task<RE_RES_SET_GAMSERVER_USER_NUMBER> Process(RE_REQ_SET_GAMSERVER_USER_NUMBER requestPacket)
        {
            RE_RES_SET_GAMSERVER_USER_NUMBER responsePacket = new RE_RES_SET_GAMSERVER_USER_NUMBER();

            return responsePacket;
        }
    }
}