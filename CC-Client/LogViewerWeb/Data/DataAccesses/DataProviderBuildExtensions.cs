using Microsoft.Extensions.DependencyInjection;
using LogViewerWeb.Data.DataAccesses.Providers;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace LogViewerWeb.Data
{
    public static class DataProviderBuildExtensions
    {
        /// <summary>
        /// 添加数据提供程序到<paramref name="services"/>指定的服务集合中
        /// </summary>
        /// <param name="services">服务集合</param>
        public static void AddDataProviders(this IServiceCollection services)
        {
            services.AddScoped<IStationDataProvider, StationDataProvider>();
            services.AddScoped<IProcessDataProvider, ProcessDataProvider>();
        }
    }
}
