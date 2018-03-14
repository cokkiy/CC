using CC;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using StationLogModels;

namespace LogClient
{
    static class DbWriter
    {
        public static void WriteStation(StationSystemState state)
        {
            using (StationLogContext logContext = new StationLogContext("stationLogContext"))
            {
                if (!logContext.Stations.Any(s => s.ComputerName == state.computerName))
                {
                    logContext.Stations.Add(new Station { ComputerName = state.computerName });
                    logContext.SaveChanges();
                }

                int? stationId = logContext.Stations.FirstOrDefault(s => s.ComputerName == state.computerName)?.Id;
                if (stationId.HasValue)
                {
                    var stationState = logContext.StationStates.OrderByDescending(s => s.UpdateTime).FirstOrDefault(s => s.StationId == stationId.Value);
                    if (stationState == null || stationState.OSName != state.osName || stationState.OSVersion != state.osVersion
                            || stationState.TotalMemory != state.totalMemory)
                    {

                        stationState = new StationState
                        {
                            StationId = stationId.Value,
                            ComputerName = state.computerName,
                            OSName = state.osName,
                            OSVersion = state.osVersion,
                            TotalMemory = state.totalMemory,
                            UpdateTime = DateTime.Now
                        };

                        logContext.StationStates.Add(stationState);
                        logContext.SaveChanges();
                    }
                }
            }
        }

        internal static void WriteRunningState(string computerName, StationRunningState stationRunningState)
        {
            throw new NotImplementedException();
        }
    }
}
