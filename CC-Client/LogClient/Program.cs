using Ice;
using MySql.Data.Entity;
using System;
using System.Collections.Generic;
using System.Data.Entity;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace LogClient
{
    class Program
    {
        // 
        static Logger logger = new Logger();
        static Dictionary<string, ManageState> stations = new Dictionary<string, ManageState>();
        // 是否退出
        private static bool quit = false;
        private static Communicator communicator;

        static void Main(string[] args)
        {
            Console.Title = "工作站状态记录程序";
            logger.print("Starting log client...");
            //设置工作目录为程序所在目录
            System.IO.Directory.SetCurrentDirectory(System.AppDomain.CurrentDomain.BaseDirectory);

            //初始化数据库
            using (StationLogContext conext = new StationLogContext("stationLogContext"))
            {
                if (!conext.Database.Exists())
                {
                    logger.print("Create Database...");
                    conext.Database.CreateIfNotExists();
                    logger.print("Database created.");
                }
            }

            

            logger.print("Starting state receiving thread...");
            Thread receiveThread = new Thread(Receiver);
            receiveThread.Start();

            logger.print("Starting recycle thread...");
            Thread recyclingThread = new Thread(Recycle);
            recyclingThread.Start();

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
            var stateReceiver = new StationStateReceiver(communicator, stations);
            var identity = Ice.Util.stringToIdentity("stateReceiver");
            adapter.add(stateReceiver, identity);
            adapter.activate();
        }

        /// <summary>
        /// 回收线程
        /// </summary>
        static void Recycle()
        {
            List<KeyValuePair<string, ManageState>> stopedMachines = new List<KeyValuePair<string, ManageState>>();
            while (!quit)
            {
                foreach (var station in stations)
                {
                    if (DateTime.Now - station.Value.LastTick > TimeSpan.FromMinutes(1))
                    {
                        stopedMachines.Add(station);
                    }
                }

                foreach (var stoped in stopedMachines)
                {
                    var stopedState = stoped.Value;
                    if(stopedState.RunningProcId.Count>0)
                    {
                        DbWriter.WriteStopProcessInfo(stopedState);
                    }

                    DbWriter.SetMachineStoped(stopedState.ComputerName);
                }

                Thread.Sleep(1000);
            }
        }
    }
}

