using System;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using System.IO;
using System.Diagnostics;
using System.Threading;
using System.Collections.Generic;
using System.Net.NetworkInformation;
using System.Linq;
using Ice;
using CC;

namespace TestConsole
{
    class Program
    {
        static void Main(string[] args)
        {
            //             try
            //             {
            // 
            //                 Ice.InitializationData initData = new InitializationData();
            //                 initData.properties = Ice.Util.createProperties();
            //                 initData.properties.load("Config.Server");
            //                 Ice.Communicator ic = Ice.Util.initialize(initData);
            //                 ObjectAdapter adapter = ic.createObjectAdapter("StateReceiver");
            //                 receiver r = new receiver();
            //                 adapter.add(r, ic.stringToIdentity("stateReceiver"));
            //                 adapter.activate();
            //                 Thread sap = new Thread(startApp);
            //                 sap.Start(r);
            //                 ic.waitForShutdown();
            //                 Console.WriteLine("Press any key to start app...");
            //                 Console.ReadKey();
            // 
            //             }
            //             catch
            //             {
            // 
            //             }
            Type myType = typeof(my);
            my m = new my();
            var method = myType.GetMethod("doSth",
                new Type[] { typeof(int).MakeArrayType(), typeof(int).MakeArrayType().MakeByRefType(), typeof(int) });
            //_MethodInfo method = methods.FirstOrDefault(mt => mt.Name == "doSth");
            if (method != null)
            {
                object[] paras = { 2, null, 3 };
                method.Invoke(m, paras);
                Console.WriteLine(paras[1]);
            }

            Console.ReadKey();
        }

        static void startApp(object r)
        {
            receiver recv = r as receiver;
            while (recv.prx == null)
            {
                Thread.Sleep(100);
            }
            List<AppStartParameter> param = new List<AppStartParameter>();
            param.Add(new AppStartParameter { AppPath = "/home/cokkiy/Projects/build-qttools-gcc-5.2/bin/designer", Arguments = "" });
            for (int i = 0; i < 100; i++)
            {
                Console.WriteLine("Starting {0} times", i);
                if (recv.prx != null)
                {
                    var result = recv.prx.startApp(param);
                    foreach (var item in result)
                    {
                        Console.WriteLine("{0}", item.Result);
                    }

                }
                Thread.Sleep(1000);
            }
        }

        class my
        {
            public void doSth(int[] i, out int[] p, int d)
            {
                Console.WriteLine(i);
                p = new int[1];
                p[0] = 100;
            }
        }


        class receiver : CC.IStationStateReceiverDisp_
        {
            public IControllerPrx prx = null;

            public override void receiveAppRunningState(List<AppRunningState> appRunningState, Current current__)
            {
            }

            public override void receiveStationRunningState(StationRunningState stationRunningState, IControllerPrx controller, Current current__)
            {
                Console.WriteLine("cpu:{0}", stationRunningState.cpu);
                prx = controller;
            }

            public override void receiveSystemState(StationSystemState systemState, Current current__)
            {
            }
        }
    }
}
