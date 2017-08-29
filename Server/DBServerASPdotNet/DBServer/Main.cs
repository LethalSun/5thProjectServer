using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.IO;

namespace DBServer
{
    public class Main
    {
        public static ERROR_CODE Init()
        {
            var error = ERROR_CODE.NONE;

            UniqueSeqNumberGenerator.Init(1, 1);

            error = InitDB();
        }

        public ERROR_CODE initDB()
        {
            var redisList = "localhost:6380";
            var error = 
        }

    }
}