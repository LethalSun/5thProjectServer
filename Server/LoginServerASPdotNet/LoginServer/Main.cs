using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace LoginServer
{
    class Main
    {
        public static bool EnableRequestHeathCheck { get; private set; }


        public static ERROR_CODE Init()
        {
            var error = ERROR_CODE.NONE;

            UniqueSeqNumberGenerator.Init(1, 1);

            error = InitDB();
            if (error != ERROR_CODE.NONE)
            {
                //6.0에서 새로 나온것
                Console.WriteLine($"Starting. Fail DB:{error}");
                return error;
            }

            EnableRequestHeathCheck = true;
            return ERROR_CODE.NONE;
        }

        static ERROR_CODE InitDB()
        {
            var redisList = "localhost:6380"; //"17.10.60.21:6379,17.10.60.21:6380"
            var error = DB.Redis.Init(redisList);
            if (error != ERROR_CODE.NONE)
            {
                return error;
            }

            return error;
        }
    }
}