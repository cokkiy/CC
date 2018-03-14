using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CC;
using Ice;

namespace LogClient
{
    class StationStateReceiver : CC.IStationStateReceiverDisp_
    {
        private Communicator communicator;
        private Ice.Logger logger;
        //记录StationId和工作站基本信息
        private Dictionary<string, ManageState> stations = new Dictionary<string, ManageState>();
   

        public StationStateReceiver(Communicator communicator)
        {
            this.communicator = communicator;
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
        }
        private object lockObj=new object();
        private List<string> getingSystemStateStations = new List<string>();
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

            if(stations.ContainsKey(stationRunningState.stationId))
            {
                var manageState = stations[stationRunningState.stationId];
                if(DateTime.Now-manageState.LastWriteTime>TimeSpan.FromMinutes(1))
                {
                    // 1分钟记录一次
                    DbWriter.WriteRunningState(manageState.ComputerName, stationRunningState);
                }
                
            }

            // 显示日志
            if (logger != null)
            {
                logger.print(stationRunningState.ToString());
            }

            //记录数据库
            //if(lastWriteToDb.ContainsKey(stationRunningState.))

        }

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
                            stations.Add(state.stationId, new ManageState { ComputerName=state.computerName });
                            DbWriter.WriteStation(state);
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
