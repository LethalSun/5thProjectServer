using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Text;
using System.Threading.Tasks;

using System.Web.Http;
namespace DBServer.Request
{
    public class RERequestRemoveGameServerDataController : ApiController
    {
        [Route("Request/RERequestRemoveGameServerData")]
        [HttpPost]
        public async Task<RE_RES_REMOVE_GAMESERVER_DATA> Process(RE_REQ_REMOVE_GAMESERVER_DATA requestPacket)
        {
            RE_RES_REMOVE_GAMESERVER_DATA responsePacket = new RE_RES_REMOVE_GAMESERVER_DATA();
            return responsePacket;
        }
    }
}