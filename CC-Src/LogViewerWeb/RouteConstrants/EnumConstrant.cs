using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Routing;
using Microsoft.AspNetCore.Routing.Constraints;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace LogViewerWeb.RouteConstrants
{
    public class EnumConstrant<T> : IRouteConstraint where T: struct
    {
        public bool Match(HttpContext httpContext, IRouter route, string routeKey, RouteValueDictionary values, RouteDirection routeDirection)
        {
            if (values.TryGetValue(routeKey, out object value) && value != null)
            {
                if (!(value is T))
                {
                    string v = Convert.ToString(value);
                    if (Enum.TryParse<T>(v, true, out T tv))
                        return true;
                    return false;
                }
                return true;
            }

            return false;
        }
    }
}
