using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;

using System.Web.Http;

namespace LoginServer.Request
{
    public class HeathCheckController : ApiController
    {
        [Route("Request/HeathCheck")]
        [HttpGet]
        public HttpResponseMessage Process()
        {
            //아마존 로드 밸런스 같은곳에서 서버에 분산을 하게 될때 보통은 인덱스.html에 요청을 하지만 우리는 웹페이지가 아니므로
            //설정으로 이곳에 요청을 보내게 해서 살아있음을 알린다.
            //일부러 403 코드를 보내는건 서버가 사용불가능 하다는 걸 알려서 여러개의 서버를 돌릴때 다른 서버로 유저들을 유도하고
            //유저가 없을때 서버를 제거하고 특정 작업을 해줄수 있다.
            if (Main.EnableRequestHeathCheck)
            {
                //Logger.Info("RequestHeathCheck");
                return Request.CreateResponse(System.Net.HttpStatusCode.OK);
            }
            else
            {
                return Request.CreateResponse(System.Net.HttpStatusCode.Forbidden);
            }
        }

    }
}