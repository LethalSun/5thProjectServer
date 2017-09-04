using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Text;
using System.Threading.Tasks;

namespace DBServer
{
    public class GameServerSelector
    {
        public static async Task<string> GetAvailableAddr()
        {
            await Data.AuthTokenRepository.CheckAuth("id", "sdafasdf", 3);

            var gameServerID = "";

            return gameServerID;
        }
    }
}