using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Text;
using System.Threading.Tasks;

using CloudStructures;

namespace DBServer.DB
{
    public class Redis
    {
        static RedisGroup redisGroupBasic = null;

        public static ERROR_CODE Init(string address)
        {
            try
            {
                var basicRedisConnectString = address.Split(",").ToList();
                var redisSetting = new RedisSettings[basicRedisConnectString.Count];

            }
            catch
            {

            }
        }

    }
}