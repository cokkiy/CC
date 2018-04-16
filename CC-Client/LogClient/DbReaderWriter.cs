using CC;
using System;
using System.Collections.Generic;
using System.Linq;
using StationLogModels;
using System.Data.Entity;

namespace LogClient
{
    static class DbReaderWriter
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

        internal static void SetStationAlreadyShutDown(List<Station> notRunningStations)
        {
            using (StationLogContext logContext = new StationLogContext("stationLogContext"))
            {
                foreach (var station in notRunningStations)
                {
                    station.IsRunning = false;
                    //只设置开机时间小于4小时的,大于4小时的不设置
                    if (DateTime.Now - station.PowerOnTime < TimeSpan.FromHours(4))
                    {
                        station.ShutdownTime = DateTime.Now - TimeSpan.FromMinutes(1.5);
                    }

                    logContext.Stations.Attach(station);
                    logContext.Entry(station).State = EntityState.Modified;

                    var log = logContext.PowerOnOffLogs
                        .OrderByDescending(s => s.PowerOn).FirstOrDefault(l => l.ComputerName == station.ComputerName && l.PowerOff == null);
                    if (log != null && station.PowerOnTime - log.PowerOn < TimeSpan.FromMinutes(5))
                    {
                        log.PowerOff = station.ShutdownTime;
                    }

                    var startTime = DateTime.Now - TimeSpan.FromHours(4);
                    var process = logContext.ProcessInfos.Where(p =>p.StartTime>=startTime 
                    && p.ComputerName == station.ComputerName
                    && p.ExitTime == null).ToList();
                    process.ForEach(p => p.ExitTime = DateTime.Now - TimeSpan.FromMinutes(2));

                    var shouldDelete = logContext.ProcessInfos.Where(p => p.StartTime < startTime && p.ExitTime == null
                    && p.ComputerName == station.ComputerName).ToList();
                    logContext.ProcessInfos.RemoveRange(shouldDelete);
                }

                logContext.SaveChanges();
            }
        }

        internal static void WriteRunningState(ManageState manageState, StationRunningState stationRunningState)
        {
            using (StationLogContext logContext = new StationLogContext("stationLogContext"))
            {
                var id = logContext.Stations.FirstOrDefault(s => s.ComputerName == manageState.ComputerName)?.Id;
                if(id.HasValue)
                {
                    logContext.RunningStates.Add(new RunningState {
                        ComputerName = manageState.ComputerName,
                        CPU = stationRunningState.cpu,
                        CurrentMemory = stationRunningState.currentMemory,
                        MemoryPercent = ((float)stationRunningState.currentMemory) / manageState.TotalMemory,
                        ProcessCount=stationRunningState.procCount,
                        RecordTime=DateTime.Now,
                    });
                    logContext.SaveChanges();
                }
            }
        }

        internal static void SaveNetStatistic(NetStatistic statistic, List<IFStatistic> ifStatistics)
        {
            using (StationLogContext logContext = new StationLogContext("stationLogContext"))
            {
                logContext.NetStatistics.Add(statistic);
                logContext.IFStatistics.AddRange(ifStatistics);
                logContext.SaveChangesAsync().Wait();
            }
        }


        internal static void WriteProcessExitInfo(ManageState station, IEnumerable<string> exits)
        {
            using (StationLogContext logContext = new StationLogContext("stationLogContext"))
            {
                var exitlist = exits.ToList();
                foreach (var exit in exitlist)
                {
                    if (station.RunningProcId.ContainsKey(exit))
                    {
                        var id = station.RunningProcId[exit];
                        var process = logContext.ProcessInfos.SingleOrDefault(p => p.Id == id);
                        process.ExitTime = DateTime.Now;
                        station.RunningProcId.Remove(exit);
                        station.LoggedProcess.Remove(exit);
                    }
                }
                logContext.SaveChanges();
                
            }
        }

        internal static void SetMachineStoped(ManageState manageState)
        {
            using (StationLogContext logContext = new StationLogContext("stationLogContext"))
            {
                var station = logContext.Stations.FirstOrDefault(s => s.ComputerName == manageState.ComputerName);
                if (station != null)
                {
                    station.IsRunning = false;
                    station.ShutdownTime = manageState.LastTick;
                }

                if (manageState.PowerOnLogId != -1)
                {
                    var powerOnLog = logContext.PowerOnOffLogs.SingleOrDefault(l => l.Id == manageState.PowerOnLogId);
                    if (powerOnLog != null)
                    {
                        powerOnLog.PowerOff = manageState.LastTick;
                    }
                }

                logContext.SaveChanges();
            }
        }

        internal static void WriteProcessStartInfo(ManageState station, IEnumerable<string> starts)
        {
            List<StationLogModels.ProcessInfo> infos = new List<StationLogModels.ProcessInfo>();
            using (StationLogContext logContext = new StationLogContext("stationLogContext"))
            {
                foreach (var start in starts)
                {
                    var startTime = DateTime.Now - TimeSpan.FromHours(4);
                    var info = logContext.ProcessInfos.Where(p => p.ComputerName == station.ComputerName
                      && p.ProcessName == start && p.ExitTime == null && p.StartTime >= startTime)
                    .FirstOrDefault();
                    if (info == null)
                    {
                        info = new StationLogModels.ProcessInfo
                        {
                            ComputerName = station.ComputerName,
                            ProcessName = start,
                            StartTime = DateTime.Now,
                        };
                    }
                    infos.Add(info);
                }
                logContext.ProcessInfos.AddRange(infos);
                logContext.SaveChanges();
                station.LoggedProcess.AddRange(starts);
                infos.ForEach(i => station.RunningProcId.Add(i.ProcessName, i.Id));
            }
        }

        /// <summary>
        /// 更新网卡信息
        /// </summary>
        /// <param name="state"></param>
        internal static void UpdateNI(StationSystemState state)
        {
            using (StationLogContext logContext = new StationLogContext("stationLogContext"))
            {
                List<(string IfName, string Mac, string IP)> noSaved = new List<(string, string, string)>();
                var allNIs = logContext.StationNIs.Where(n => n.ComputerName == state.computerName).ToList();
                foreach (var item in state.NetworkInterfaces)
                {
                    var IfNameAndMac = item.Key.Split(':');
                    if (IfNameAndMac.Length < 2)
                        continue;
                    var IfName = IfNameAndMac[0];
                    var MAC = IfNameAndMac[1];
                    var IPs = item.Value;
                    var noSavedIPs = IPs.FindAll(ip => { return !allNIs.Any(ni => ni.IfName == IfName && ni.MAC == MAC && ni.IP == ip); });
                    noSavedIPs.ForEach(ip => { noSaved.Add((IfName, MAC, ip)); });
                }

                if(noSaved.Count>0)
                {
                    foreach (var item in noSaved)
                    {
                        StationNI ni = new StationNI
                        {
                            ComputerName=state.computerName,
                            IfName=item.IfName,
                            MAC=item.Mac,
                            IP=item.IP,
                            Update=DateTime.Now,
                        };

                        logContext.StationNIs.Add(ni);                        
                    }

                    logContext.SaveChanges();
                }
            }
        }

        /// <summary>
        /// 获取已经运行主机的运行记录Id
        /// </summary>
        /// <param name="computerName">主机名称</param>
        /// <returns></returns>
        internal static long GetPowerOnLogId(string computerName, DateTime startTime)
        {
            using (StationLogContext logContext = new StationLogContext("stationLogContext"))
            {
                var log = logContext.PowerOnOffLogs.OrderByDescending(s => s.PowerOn)
                    .FirstOrDefault(s => s.ComputerName == computerName && s.PowerOff == null);
                if (log == null || startTime - log.PowerOn < TimeSpan.FromMinutes(1))
                {
                    log = new PowerOnOffLog
                    {
                        ComputerName = computerName,
                        PowerOn = startTime,
                    };
                    logContext.PowerOnOffLogs.Add(log);
                    logContext.SaveChanges();
                }

                return log.Id;
            }
        }

        internal static long SetMachineRunning(string computerName)
        {
            using (StationLogContext logContext = new StationLogContext("stationLogContext"))
            {
                var station = logContext.Stations.FirstOrDefault(s => s.ComputerName == computerName);
                if (station != null)
                {
                    station.IsRunning = true;
                    station.PowerOnTime = DateTime.Now;
                    station.ShutdownTime = null;
                }

                var powerOnLog = logContext.PowerOnOffLogs.FirstOrDefault(s => s.ComputerName == computerName
                  && s.PowerOff == null);

                if (powerOnLog == null || (DateTime.Now - powerOnLog.PowerOn) > TimeSpan.FromHours(4))
                {
                    powerOnLog = new PowerOnOffLog
                    {
                        ComputerName = computerName,
                        PowerOn = station.PowerOnTime.Value,
                    };
                    logContext.PowerOnOffLogs.Add(powerOnLog);
                }

                logContext.SaveChanges();

                return powerOnLog.Id;
            }
        }

        internal static void WriteStopProcessInfo(ManageState stopedState, DateTime stopTime)
        {
            using (StationLogContext logContext = new StationLogContext("stationLogContext"))
            {
                foreach (var proc in stopedState.RunningProcId)
                {
                    var info = logContext.ProcessInfos.FirstOrDefault(p => p.Id == proc.Value);
                    info.ExitTime = stopTime;
                }

                logContext.SaveChanges();
            }
        }
    }
}
