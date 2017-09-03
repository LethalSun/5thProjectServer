using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Text;

using System.Web.Http;
using System.Security.Cryptography;

namespace DBServer
{
    public class TokenGenerator
    {
        public static string GetRandomString()
        {
            RandomNumberGenerator csprng = new RNGCryptoServiceProvider();
            byte[] rawByteArray = new byte[32];
            csprng.GetBytes(rawByteArray);


            return BitConverter.ToString(rawByteArray);
        }
    }
}