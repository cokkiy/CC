using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using LogViewerWeb.Models.ViewModels;
using StationLogModels;

namespace LogViewerWeb.Data {
    public interface IStationDataProvider
    {
        /// <summary>
        /// 获取全部工作站信息
        /// </summary>
        /// <returns>全部工作站信息</returns>
        Task<List<Station>> GetStations();

        /// <summary>
        /// 获取全部开机的工作站信息
        /// </summary>
        /// <returns></returns>
        Task<List<Station>> GetPowerOnStations();

        /// <summary>
        /// 获取指定工作组内的工作站信息
        /// </summary>
        /// <param name="groupName"></param>
        /// <returns></returns>
        Task<List<Station>> GetStationsInGroup(string groupName);

        /// <summary>
        /// 获取全部工作站数量
        /// </summary>
        /// <returns></returns>
        Task<int> GetTotalStationsCount();

        /// <summary>
        /// 获取制定工作组内全部工作站数量
        /// </summary>
        /// <returns></returns>
        Task<int> GetStationsCountInGroup(string groupName);

        /// <summary>
        /// 获取开机工作站数量和总工作站数量
        /// </summary>
        /// <returns></returns>
        (int running, int total) GetRunningAndTotalCount();

        /// <summary>
        /// 获取工作站信息
        /// </summary>
        /// <param name="powerOnOnly">是否只获取开机的工作站信息</param>
        /// <returns></returns>
        Task<List<StationViewModel>> GetStationsInfo(bool powerOnOnly);

        /// <summary>
        /// 获取统计信息
        /// </summary>
        /// <param name="begin">开始时间</param>
        /// <param name="end">结束时间</param>
        /// <param name="type">统计分组类型</param>
        /// <param name="distinct">是否不统计重复开机情况</param>
        /// <returns>统计数据</returns>
        Task<List<StationRunningStatistic>> GetStationRunningStatistics(DateTime begin, DateTime end, TimeType type, bool distinct);

        /// <summary>
        /// 获取工作站报警信息
        /// </summary>
        /// <param name="index">开始位置</param>
        /// <param name="count">数量</param>
        /// <returns>工作站报警信息</returns>
        Task<List<RunningState>> GetStationAlerts(int index, int count);
    }

}