using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Text;

using Owin;
using System.Web.Http;


namespace DBServer
{
    public class Startup
    {
        public void Configuration(IAppBuilder appBuilder)
        {
            var config = new HttpConfiguration();

            config.MapHttpAttributeRoutes();

            config.Routes.MapHttpRoute(
                name: "DefaultApi", 
                routeTemplate: "api/{controller}/{id}", 
                defaults: new { id = RouteParameter.Optional }
                );

            appBuilder.UseWebApi(config);

        }
    }
}