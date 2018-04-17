using System.Threading.Tasks;
using StationLogModels;

namespace LogViewerWeb.Data.DataPersisters
{
    /// <summary>
    /// 工作站信息持久化接口
    /// </summary>
    public interface IStationPersister
    {
        Task<int> UpdateStationAsync(string computerName,string userName,string groupName,string networkName);
    }
}
