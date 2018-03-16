using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CC;
using Ice;
using StationLogModels;

namespace LogClient
{
    class StationStateReceiver : CC.IStationStateReceiverDisp_
    {
        private Communicator communicator;
        private Ice.Logger logger;
        //记录StationId和工作站基本信息
        private Dictionary<string, ManageState> stations;


        public StationStateReceiver(Communicator communicator, Dictionary<string, ManageState> stations)
        {
            this.communicator = communicator;
            this.stations = stations;
            logger = this.communicator.getLogger();
        }

        public override void receiveAppRunningState(List<AppRunningState> appRunningState, Current current__)
        {
            // do nothing
        }

        public override void receiveNetStatistics(string data, Current current__)
        {
            if (logger != null)
            {
                logger.print(data);
            }

            var datas = data.Split(':');
            if(datas.Length<11)
            {
                logger.warning("网络数据格式不对.");
                return;
            }

            if (stations.ContainsKey(datas[0]))
            {
                var manageState = stations[datas[0]];
                if (DateTime.Now - manageState.NetStatisticLastWriteTime > TimeSpan.FromMinutes(1))
                {
                    manageState.NetStatisticLastWriteTime = DateTime.Now;
                    var computerName = manageState.ComputerName;
                    NetStatistic statistic = new NetStatistic(computerName, datas[1], datas[2], datas[3],
                        datas[4], datas[5], datas[6], datas[7], datas[8], datas[9], datas[10]);

                    List<IFStatistic> ifStatistics = new List<IFStatistic>();

                    var IfCount = (datas.Length - 11) / 11;
                    for (int i = 1; i <= IfCount; i++)
                    {
                        IFStatistic ifStatistic = new IFStatistic(computerName, datas[i * 11 + 0],
                            datas[i * 11 + 1], datas[i * 11 + 2], datas[i * 11 + 3], datas[i * 11 + 4],
                            datas[i * 11 + 5], datas[i * 11 + 6], datas[i * 11 + 7], datas[i * 11 + 8],
                            datas[i * 11 + 9], datas[i * 11 + 10]);

                        ifStatistics.Add(ifStatistic);
                    }

                    DbWriter.SaveNetStatistic(statistic, ifStatistics);
                }
            }

        }


        private object lockObj = new object();
        private List<string> getingSystemStateStations = new List<string>();
        private List<string> getingProcessInfoStations = new List<string>();
        public override void receiveStationRunningState(StationRunningState stationRunningState, IControllerPrx controller, IFileTranslationPrx fileProxy, Current current__)
        {
            if (!stations.ContainsKey(stationRunningState.stationId))
            {
                if (getingSystemStateStations.Contains(stationRunningState.stationId))
                {
                    return;
                }

                getingSystemStateStations.Add(stationRunningState.stationId);

                GetSystemState(stationRunningState, controller);
            }

            if (stations.ContainsKey(stationRunningState.stationId))
            {
                var manageState = stations[stationRunningState.stationId];
                manageState.LastTick = DateTime.Now;

                if (DateTime.Now - manageState.LastWriteTime > TimeSpan.FromMinutes(1))
                {
                    // 1分钟记录一次
                    DbWriter.WriteRunningState(manageState, stationRunningState);
                    manageState.LastWriteTime = DateTime.Now;
                }

                // 判断进程信息是否发生了变化
                if (manageState.ProcessCount != stationRunningState.procCount)
                {
                    if (getingProcessInfoStations.Contains(stationRunningState.stationId))
                    {
                        return;
                    }

                    getingProcessInfoStations.Add(stationRunningState.stationId);
                    controller.begin_getAllProcessInfo((AsyncResult ar) => {
                        if(ar.IsCompleted)
                        {
                            SaveProcessInfo(controller, ar, manageState);
                        }

                        getingProcessInfoStations.Remove(stationRunningState.stationId);

                    }, null);
                }

            }

            // 显示日志
            if (logger != null)
            {
                logger.print(stationRunningState.ToString());
            }

        }

        private static void SaveProcessInfo(IControllerPrx controller, AsyncResult ar, ManageState manageState)
        {
            var processes = controller.end_getAllProcessInfo(ar);

            var exits = manageState.RunningProcesses.FindAll((p) =>
            {
                return !processes.Any(proc => proc.Name == p);
            }).Distinct().ToList();

            var starts = processes.FindAll((p) =>
            {
                return !manageState.RunningProcesses.Any(run => run == p.Name);
            }).Select(p => p.Name).Distinct().ToList();

            manageState.RunningProcesses = processes.Select(p => p.Name).ToList();


            if (exits.Count > 0)
            {
                DbWriter.WriteProcessInfo(manageState, exits, false);
            }

            if (starts.Count > 0)
            {
                DbWriter.WriteProcessInfo(manageState, starts, true);
            }
        }

        /// <summary>
        /// 获取系统状态
        /// </summary>
        /// <param name="stationRunningState"></param>
        /// <param name="controller"></param>

        private void GetSystemState(StationRunningState stationRunningState, IControllerPrx controller)
        {
            controller.begin_getSystemState((AsyncResult result) =>
            {
                if (result.IsCompleted)
                {
                    var state = controller.end_getSystemState(result);
                    if (logger != null)
                    {
                        logger.print(state.ToString());
                    }
                    lock (lockObj)
                    {
                        if (!stations.ContainsKey(stationRunningState.stationId))
                        {
                            var manageState = new ManageState
                            {
                                ComputerName = state.computerName,
                                TotalMemory = state.totalMemory,
                                StartTime = DateTime.Now,
                                LastTick=DateTime.Now
                            };

                            stations.Add(state.stationId, manageState);
                            DbWriter.WriteStation(state);
                            DbWriter.UpdateNI(state);
                            DbWriter.SetMachineRunning(state.computerName);

                            //获取初始进程信息
                            controller.begin_getAllProcessInfo((AsyncResult aResult) =>
                            {
                                if (aResult.IsCompleted)
                                {
                                    var processes = controller.end_getAllProcessInfo(aResult);
                                    manageState.RunningProcesses = processes.Select(p => p.Name).ToList();
                                }

                            }, null);
                        }
                    }
                }
                getingSystemStateStations.Remove(stationRunningState.stationId);
            }, null);
        }

        public override void receiveSystemState(StationSystemState systemState, Current current__)
        {
            if (logger != null)
            {
                logger.print(systemState.ToString());
            }
        }
    }
}
