using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
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
        private ReaderWriterLockSlim readerWriterLockSlim;


        public StationStateReceiver(Communicator communicator, 
            Dictionary<string, ManageState> stations, ReaderWriterLockSlim readerWriterLockSlim)
        {
            this.communicator = communicator;
            this.stations = stations;
            logger = this.communicator.getLogger();
            this.readerWriterLockSlim = readerWriterLockSlim;
        }

        public override void receiveAppRunningState(List<AppRunningState> appRunningState, Current current__)
        {
            // do nothing
        }

        public override void receiveNetStatistics(string data, Current current__)
        {
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
                    logger.print(string.Format("{0}网络数据已记录。", computerName));
                }
            }
            else
            {
                logger.warning(string.Format("收到网络数据，但主机不能确认：{0}.", datas[0]));
            }

        }


        private object lockObj = new object();
        private List<string> getingSystemStateStations = new List<string>();
        private List<string> getingProcessInfoStations = new List<string>();
        public override void receiveStationRunningState(StationRunningState stationRunningState, IControllerPrx controller, IFileTranslationPrx fileProxy, Current current__)
        {            
            if (!stations.ContainsKey(stationRunningState.stationId))
            {
                logger.warning(string.Format("主机{0}未在已知主机词典中，准备查询...", stationRunningState.stationId));
                if (getingSystemStateStations.Contains(stationRunningState.stationId))
                {
                    logger.warning(string.Format("主机{0}未在已知主机词典中，并且已经在查询中，本次数据被忽略。", stationRunningState.stationId));

                    return;
                }

                getingSystemStateStations.Add(stationRunningState.stationId);
                logger.warning(string.Format("{0}进入主机基本状态查询锁，并开始查询。", stationRunningState.stationId));

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

                    logger.print(string.Format("{0}运行状态被记录。", manageState.ComputerName));
                }

                // 判断进程信息是否发生了变化
                if (manageState.ProcessCount != stationRunningState.procCount)
                {
                    if(DateTime.Now-manageState.LastCheckProcessTime<TimeSpan.FromSeconds(30))
                    {
                        logger.warning(string.Format("{0}进程数量发生变化，但与上次查询间隔不足30秒，本次变化被忽略。", manageState.ComputerName));
                        return;
                    }

                    if (getingProcessInfoStations.Contains(stationRunningState.stationId))
                    {
                        logger.warning(string.Format("{0}进程数量发生变化，但上次查询尚未结束，本次数据被忽略。", manageState.ComputerName));
                        return;
                    }

                    manageState.LastCheckProcessTime = DateTime.Now;

                    getingProcessInfoStations.Add(stationRunningState.stationId);
                    logger.warning(string.Format("{0}进程数量发生变化{1}->{2}，进入进程查询锁并开始查询。", manageState.ComputerName, manageState.ProcessCount, stationRunningState.procCount));

                    controller.begin_getAllProcessInfo((AsyncResult ar) => {
                        if(ar.IsCompleted)
                        {
                            SaveProcessInfo(controller, ar, manageState);
                        }

                        getingProcessInfoStations.Remove(stationRunningState.stationId);
                        logger.warning(string.Format("{0}进程查询结束，退出进程查询锁。", manageState.ComputerName));

                    }, null);
                }

            }
        }

        private void SaveProcessInfo(IControllerPrx controller, AsyncResult ar, ManageState manageState)
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
                string exitProc=string.Empty;
                exits.ForEach((e) => string.Format("{0} {1}", exitProc, e));
                logger.print(string.Format("{0}的{1}个进程{2}退出。", manageState.ComputerName, exits.Count, exitProc));
            }

            if (starts.Count > 0)
            {
                DbWriter.WriteProcessInfo(manageState, starts, true);
                string startProc = string.Empty;
                starts.ForEach((e) => string.Format("{0} {1}", startProc, e));
                logger.print(string.Format("{0}的{1}个进程{2}启动。", manageState.ComputerName, starts.Count, startProc));
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
                        logger.warning(string.Format("主机查询结果：{0}", state));
                    }
                    lock (lockObj)
                    {
                        if (!stations.ContainsKey(stationRunningState.stationId))
                        {
                            var manageState = new ManageState
                            {
                                ComputerName = state.computerName,
                                TotalMemory = state.totalMemory,
                                IsLinux = state.osName == "Unix",
                                StartTime = DateTime.Now,
                                LastTick = DateTime.Now
                            };

                            readerWriterLockSlim.EnterReadLock();
                            stations.Add(state.stationId, manageState);
                            readerWriterLockSlim.ExitReadLock();

                            logger.warning(string.Format("添加主机Id:{0},名称{1}到已知主机词典中。", stationRunningState.stationId, state.computerName));
                            DbWriter.WriteStation(state);
                            DbWriter.UpdateNI(state);
                            manageState.PowerOnLogId = DbWriter.SetMachineRunning(state.computerName);
                            logger.warning(string.Format("主机{0}状态已设置为运行。", state.computerName));

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
                logger.warning(string.Format("{0}主机基本状态查询锁已移除。", stationRunningState.stationId));

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
