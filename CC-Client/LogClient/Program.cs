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
        static Logger logger = new Logger();
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

            logger.print("Starting state receiving thread.");
            Thread receiveThread = new Thread(Receiver);
            receiveThread.Start();

            logger.print("Press q to quit.");
            while (Console.ReadKey().KeyChar != 'q')
            {
                Console.WriteLine();
                Console.WriteLine("Unknown input, Press q to quit.");
            }
        }

        static void Receiver()
        {
            //初始化ICE 通信对象
            Ice.InitializationData initData = new Ice.InitializationData();
            initData.properties = Ice.Util.createProperties();
            initData.properties.load("Config.Client");
            initData.logger = logger;
            Ice.Communicator communicator = Ice.Util.initialize(initData);
            var adapter = communicator.createObjectAdapter("StateReceiver");
            var stateReceiver = new StationStateReceiver(communicator);
            var identity = Ice.Util.stringToIdentity("stateReceiver");
            adapter.add(stateReceiver, identity);
            adapter.activate();
        }
    }
}

