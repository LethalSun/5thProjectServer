using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Text;
using System.Threading.Tasks;

using System.Web.Http;

namespace DBServer.Request
{
    public class DBRequestSetUserDataController : ApiController
    {
        [Route("Request/DBRequestSetUserData")]
        [HttpPost]
        public async Task<DB_RES_SET_USER_DATA> Process(DB_REQ_SET_USER_DATA requestPacket)
        {
            DB_RES_SET_USER_DATA responsePacket = new DB_RES_SET_USER_DATA();
            return responsePacket;
        }
    }
}