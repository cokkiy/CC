using Ice;
using StationLogModels;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;

namespace LogClient
{
    class Program
    {
        // 
        static Logger logger = new Logger();
        static Dictionary<string, ManageState> stations = new Dictionary<string, ManageState>();

        //启动时数据库中状态为运行的工作站信息
        static List<Station> runningStations;

        // 是否退出
        private static bool quit = false;

        private static Communicator communicator;
        private static ReaderWriterLockSlim readerWriterLockSlim = new ReaderWriterLockSlim();

        static void Main(string[] args)
        {
            Console.Title = "工作站状态记录程序";
            logger.print("Starting log client...");
            //设置工作目录为程序所在目录
            System.IO.Directory.SetCurrentDirectory(System.AppDomain.CurrentDomain.BaseDirectory);

            using (var context = new StationLogContext("stationLogContext"))
            {
                //初始化数据库
                if (!context.Database.Exists())
                {
                    logger.print("Create Database...");
                    context.Database.CreateIfNotExists();
                    logger.print("Database created.");
                }

                // 加载未关机信息
                runningStations = context.Stations.Where(s => s.IsRunning)
                    .ToList();
            }

            logger.print("Starting state receiving thread...");
            Thread receiveThread = new Thread(Receiver);
            receiveThread.Start();

            logger.print("Starting recycle thread...");
            Thread recyclingThread = new Thread(Recycle);
            recyclingThread.Start();

            logger.print("Starting process checker thread...");
            Thread processCheckerThread = new Thread(RunningProcessChecker);
            processCheckerThread.Start();

            logger.print("Starting already shutdown check thread...");
            Thread alreadyShutdownCheckerThread = new Thread(CheckIsAlradyShutdown);
            alreadyShutdownCheckerThread.Start();

            logger.print("Press q to quit.");
            while (Console.ReadKey().KeyChar != 'q')
            {
                Console.WriteLine();
                Console.WriteLine("Unknown input, Press q to quit.");
            }

            quit = true;
            communicator.destroy();

        }

        static void Receiver()
        {
            //初始化ICE 通信对象
            Ice.InitializationData initData = new Ice.InitializationData();
            initData.properties = Ice.Util.createProperties();
            initData.properties.load("Config.Client");
            initData.logger = logger;
            communicator = Ice.Util.initialize(initData);
            var adapter = communicator.createObjectAdapter("StateReceiver");
            var stateReceiver = new StationStateReceiver(communicator, stations,
                readerWriterLockSlim, runningStations);
            var identity = Ice.Util.stringToIdentity("stateReceiver");
            adapter.add(stateReceiver, identity);
            adapter.activate();
        }

        static void CheckIsAlradyShutdown()
        {
            Thread.Sleep(90000); //先等待90秒
            List<Station> notRunningStations = new List<Station>();
            readerWriterLockSlim.EnterReadLock();
            foreach (var station in runningStations)
            {
                if (!stations.Any(s => s.Value.ComputerName == station.ComputerName))
                {
                    notRunningStations.Add(station);
                    logger.warning(string.Format("{0}处于关机状态。上次开机时间是：{1}", station.ComputerName, station.PowerOnTime));
                }
            }
            runningStations.Clear();
            readerWriterLockSlim.ExitReadLock();
            if (notRunningStations.Count > 0)
            {
                DbReaderWriter.SetStationAlreadyShutDown(notRunningStations);
            }
        }

        /// <summary>
        /// 回收线程
        /// </summary>
        static void Recycle()
        {
            List<KeyValuePair<string, ManageState>> stopedMachines = new List<KeyValuePair<string, ManageState>>();
            while (!quit)
            {
                readerWriterLockSlim.EnterReadLock();
                foreach (var station in stations)
                {
                    if (DateTime.Now - station.Value.LastTick > TimeSpan.FromMinutes(2))
                    {
                        stopedMachines.Add(station);
                    }
                }
                readerWriterLockSlim.ExitReadLock();

                foreach (var stoped in stopedMachines)
                {
                    var stopedState = stoped.Value;
                    if (stopedState.RunningProcId.Count > 0)
                    {
                        DbReaderWriter.WriteStopProcessInfo(stopedState, stoped.Value.LastTick);
                    }

                    DbReaderWriter.SetMachineStoped(stopedState);

                    readerWriterLockSlim.EnterWriteLock();
                    stations.Remove(stoped.Key);
                    readerWriterLockSlim.ExitWriteLock();

                    logger.warning(string.Format("{0}于{1}关闭，StationID-{2}被回收,主机状态已设置为未运行。",
                        stoped.Value.ComputerName, stoped.Value.LastTick, stoped.Key));
                }

                stopedMachines.Clear();

                Thread.Sleep(1000);
            }
        }


        static void RunningProcessChecker()
        {
            while (!quit)
            {
                readerWriterLockSlim.EnterReadLock();
                foreach (var stationKV in stations)
                {
                    var station = stationKV.Value;
                    if (DateTime.Now - station.LastCheckProcessTime < TimeSpan.FromMinutes(1))
                        continue;
                    station.LastCheckProcessTime = DateTime.Now;
                    var controller = station.Controller;
                    controller.begin_getAllProcessInfo((AsyncResult ar) =>
                    {
                        if (ar.IsCompleted)
                        {
                            var processes = controller.end_getAllProcessInfo(ar);

                            var starts = processes.Distinct().Where(p =>
                            {
                                return !SystemProcessChecker.IsSysProcess(station.InitRunningProcesses, p.Name) &&
                                    !station.LoggedProcess.Any(r => r == p.Name);
                            })
                            .GroupBy(p => p.Name)
                            .Select(g => g.First().Name);

                            var exits = station.LoggedProcess.Where(r =>
                            {
                                return !processes.Any(p => p.Name == r);
                            }).Select(r => r);

                            if (exits.Count() > 0)
                            {
                                var count = exits.Count();
                                string exitProc = string.Empty;
                                exits.ToList().ForEach(s =>
                                {
                                    exitProc = string.Format("{0} {1}", exitProc, s);
                                });
                                DbReaderWriter.WriteProcessExitInfo(station, exits);
                                logger.print(string.Format("{0}的{1}个进程{2}退出。", station.ComputerName, count, exitProc));
                            }

                            if (starts.Count() > 0)
                            {
                                var count = starts.Count();
                                string startProc = string.Empty;
                                starts.ToList().ForEach(e =>
                                {
                                    startProc = string.Format("{0} {1}", startProc, e);
                                });
                                DbReaderWriter.WriteProcessStartInfo(station, starts);
                                logger.print(string.Format("{0}的{1}个进程{2}启动。", station.ComputerName, count, startProc));
                            }
                        }

                        station.LastCheckProcessTime = DateTime.Now;

                    }, null);
                }
                readerWriterLockSlim.ExitReadLock();

                Thread.Sleep(1000);
            }
        }
    }
}

