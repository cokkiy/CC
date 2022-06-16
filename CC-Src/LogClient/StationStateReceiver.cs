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
        private List<Station> alreadyRunningStations;


        public StationStateReceiver(Communicator communicator, 
            Dictionary<string, ManageState> stations, ReaderWriterLockSlim readerWriterLockSlim,
            List<Station> alreadyRunningStations)
        {
            this.communicator = communicator;
            this.stations = stations;
            logger = this.communicator.getLogger();
            this.readerWriterLockSlim = readerWriterLockSlim;
            this.alreadyRunningStations = alreadyRunningStations;
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

                    DbReaderWriter.SaveNetStatistic(statistic, ifStatistics);
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
                    DbReaderWriter.WriteRunningState(manageState, stationRunningState);
                    manageState.LastWriteTime = DateTime.Now;

                    logger.print(string.Format("{0}运行状态被记录。", manageState.ComputerName));
                }                
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
                                LastTick = DateTime.Now,
                                Controller = controller,
                            };

                            bool isAlreadyRunning = false;

                            readerWriterLockSlim.EnterWriteLock();
                            var alreadRunning = alreadyRunningStations.Find(s => s.ComputerName == state.computerName
                            && DateTime.Now - s.PowerOnTime < TimeSpan.FromHours(4));
                            if(alreadRunning!=null)
                            {
                                isAlreadyRunning = true;
                                manageState.StartTime = alreadRunning.PowerOnTime.Value;
                            }
                            stations.Add(state.stationId, manageState);
                            readerWriterLockSlim.ExitWriteLock();

                            logger.warning(string.Format("添加主机Id:{0},名称{1}到已知主机词典中。", stationRunningState.stationId, state.computerName));
                            if (!isAlreadyRunning)
                            {
                                DbReaderWriter.WriteStation(state);
                                DbReaderWriter.UpdateNI(state);
                                manageState.PowerOnLogId = DbReaderWriter.SetMachineRunning(state.computerName);
                                logger.warning(string.Format("主机{0}状态已设置为运行。", state.computerName));
                            }
                            else
                            {
                                manageState.PowerOnLogId = DbReaderWriter.GetPowerOnLogId(manageState.ComputerName, manageState.StartTime);
                            }

                            //获取初始进程信息
                            //controller.begin_getAllProcessInfo((AsyncResult aResult) =>
                            //{
                            //    if (aResult.IsCompleted)
                            //    {
                            //        var processes = controller.end_getAllProcessInfo(aResult);
                            //        manageState.InitRunningProcesses = processes.GroupBy(p => p.Name).Select(g => g.First().Name).ToList();
                            //    }

                            //}, null);
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
