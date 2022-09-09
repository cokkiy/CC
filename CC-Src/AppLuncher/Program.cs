using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AppLuncher
{
    class Program
    {
        static void Main(string[] args)
        {
            string parameter = string.Concat(args);
            switch (parameter)
            {
                case "-v":
                    {
                        string path = System.AppDomain.CurrentDomain.BaseDirectory;
                        var serviceVersion = FileVersionInfo.GetVersionInfo(System.IO.Path.Combine(path, "AppLuncher.exe"));
                        Console.WriteLine(serviceVersion.FileVersion.ToString());
                    }
                    return;
                case "-version":
                    {
                        string path = System.AppDomain.CurrentDomain.BaseDirectory;
                        var serviceVersion = FileVersionInfo.GetVersionInfo(System.IO.Path.Combine(path, "AppLuncher.exe"));
                        Console.WriteLine(serviceVersion.ToString());
                    }
                    return;
            }
            Console.WriteLine("Starting...");
            Ice.Communicator ic = null;
            try
            {
                //初始化ICE 通信对象
                Ice.InitializationData initData = new Ice.InitializationData();
                initData.properties = Ice.Util.createProperties();
                initData.properties.load("Config.Server");
                ic = Ice.Util.initialize(initData);
                Ice.ObjectAdapter adapter = ic.createObjectAdapter("AppLuncher");
                Ice.Object obj = new Luncher();
                adapter.add(obj, ic.stringToIdentity("Luncher"));
                adapter.activate();
                Console.WriteLine("Started.");
                if (Environment.OSVersion.Platform == PlatformID.Win32NT && parameter != "-debug")
                {
                    var hWnd = HideMe.GetConsoleWindow();
                    HideMe.ShowWindow(hWnd, HideMe.SW_HIDE);
                }
                ic.waitForShutdown();
                Console.ReadKey();
            }
            catch (Exception e)
            {
                Console.Error.WriteLine(e);
            }

            if (ic != null)
            {
                //clean up
                try
                {
                    ic.destroy();
                }
                catch (Exception e)
                {
                    Console.Error.WriteLine(e);
                }
            }
        }
    } 
}
