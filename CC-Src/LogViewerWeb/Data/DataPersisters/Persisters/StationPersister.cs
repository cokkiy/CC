using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace LogViewerWeb.Data.DataPersisters
{
    public class StationPersister: IStationPersister
    {
        private readonly StationLogContext _dbContext;

        public StationPersister(StationLogContext dbContext)
        {
            _dbContext=dbContext;
        }

        public Task<int> UpdateStationAsync(string computerName, string userName, string groupName, string networkName)
        {
            var station=_dbContext.Stations.FirstOrDefault(s=>s.ComputerName==computerName);
            if(station!=null)
            {
                station.UserName=userName;
                station.GroupName=groupName;
                station.NetworkName=networkName;
                return _dbContext.SaveChangesAsync();
            }

            return Task.FromResult(0);
        }
    }
}
