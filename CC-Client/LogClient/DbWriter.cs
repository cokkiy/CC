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

        internal static void WriteProcessInfo(ManageState manageState,List<string> startOrExitsProcNames, bool start)
        {
            using (StationLogContext logContext = new StationLogContext("stationLogContext"))
            {

                if (start)
                {
                    List<StationLogModels.ProcessInfo> infos = new List<StationLogModels.ProcessInfo>();
                    foreach (var proc in startOrExitsProcNames)
                    {
                        var info = new StationLogModels.ProcessInfo
                        {
                            ComputerName = manageState.ComputerName,
                            ProcessName = proc,
                            StartTime = DateTime.Now,
                        };
                        infos.Add(info);
                    }
                    logContext.ProcessInfos.AddRange(infos);
                    logContext.SaveChanges();

                    infos.ForEach(p => manageState.RunningProcId.Add(p.ProcessName, p.Id));
                }
                else
                {
                    List<StationLogModels.ProcessInfo> infos = new List<StationLogModels.ProcessInfo>();

                    foreach (var proc in startOrExitsProcNames)
                    {
                        if (manageState.RunningProcId.ContainsKey(proc))
                        {
                            var id = manageState.RunningProcId[proc];
                            var info = logContext.ProcessInfos.FirstOrDefault(p => p.Id == id);
                            info.ExitTime = DateTime.Now;
                            manageState.RunningProcId.Remove(proc);
                        }
                        else
                        {
                            var info = new StationLogModels.ProcessInfo
                            {
                                ComputerName = manageState.ComputerName,
                                ProcessName = proc,
                                StartTime = manageState.StartTime,
                                ExitTime = DateTime.Now
                            };
                            infos.Add(info);
                        }
                    }

                    if (infos.Count > 0)
                    {
                        logContext.ProcessInfos.AddRange(infos);
                    }

                    logContext.SaveChanges();
                }
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
                        PowerOn = DateTime.Now,
                    };
                    logContext.PowerOnOffLogs.Add(powerOnLog);
                }

                logContext.SaveChanges();

                return powerOnLog.Id;
            }
        }

        internal static void WriteStopProcessInfo(ManageState stopedState)
        {
            using (StationLogContext logContext = new StationLogContext("stationLogContext"))
            {
                foreach (var proc in stopedState.RunningProcId)
                {
                    var info = logContext.ProcessInfos.FirstOrDefault(p => p.Id == proc.Value);
                    info.ExitTime = DateTime.Now;                    
                }

                logContext.SaveChanges();
            }
        }
    }
}
