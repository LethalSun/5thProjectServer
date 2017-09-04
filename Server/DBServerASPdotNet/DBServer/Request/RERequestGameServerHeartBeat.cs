using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Text;
using System.Threading.Tasks;

using System.Web.Http;
namespace DBServer.Request
{
    public class RERequestGameServerHeartBeatController : ApiController
    {
        [Route("Request/RERequestGameServerHeartBeat")]
        [HttpPost]
        public async Task<RE_RES_GAMESERVER_HEARTBEAT> Process(RE_REQ_GAMESERVER_HEARTBEAT requestPacket)
        {
            RE_RES_GAMESERVER_HEARTBEAT responsePacket = new RE_RES_GAMESERVER_HEARTBEAT();
            return responsePacket;
        }
    }
}