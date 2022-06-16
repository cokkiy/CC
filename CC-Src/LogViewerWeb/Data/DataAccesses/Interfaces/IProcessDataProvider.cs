using LogViewerWeb.Models.ViewModels;
using StationLogModels;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace LogViewerWeb.Data
{
    public interface IProcessDataProvider
    {
        /// <summary>
        /// 获取运行进程数量
        /// </summary>
        /// <returns></returns>
        Task<int> GetRunningProcessesCount();

        /// <summary>
        /// 获取进程启动退出消息
        /// </summary>
        /// <param name="count">消息数量</param>
        /// <returns></returns>
        Task<List<ProcessStartAndExitMessage>> GetProcessStartAndExitMessage(int count);


        /// <summary>
        /// 获取应用程序运行时间统计
        /// </summary>
        /// <param name="begin">开始时间</param>
        /// <param name="end">截止时间</param>
        /// <param name="count">获取数量</param>
        /// <returns></returns>
        Task<List<AppRunningStatistic>> GetAppRunningTimeStatistic(DateTime begin, DateTime end, int count);

        /// <summary>
        /// 获取应用程序运行次数统计
        /// </summary>
        /// <param name="begin">开始时间</param>
        /// <param name="end">截止时间</param>
        /// <param name="count">获取数量</param>
        /// <returns></returns>
        Task<List<AppRunningStatistic>> GetAppRunningCountStatistics(DateTime begin, DateTime end, int count);
    }
}
