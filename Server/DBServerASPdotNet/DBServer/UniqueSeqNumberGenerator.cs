using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Text;
using System.IO;

namespace DBServer
{
    public class UniqueSeqNumberGenerator
    {
        static Snowflake.IdWorker worker;

        public static void Init(Int64 workerId, Int64 dataCenterId)
        {
            worker = new Snowflake.IdWorker(workerId, dataCenterId);
        }

        public static Int64 GetUniqueNumber()
        {
            return worker.NextId();
        }
    }
}