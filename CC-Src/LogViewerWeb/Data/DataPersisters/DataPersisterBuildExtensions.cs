using Microsoft.Extensions.DependencyInjection;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace LogViewerWeb.Data.DataPersisters
{
    public static class DataPersisterBuildExtensions
    {
        /// <summary>
        /// 添加数据持久化程序到<paramref name="services"/>指定的服务集合中
        /// </summary>
        /// <param name="services"></param>
        public static void AddDataPersisters(this IServiceCollection services)
        {
            services.AddTransient<IStationPersister, StationPersister>();
        }
    }
}
