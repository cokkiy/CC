using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.ServiceProcess;

namespace CC_StationService
{
    static class Program
    {
        /// <summary>
        /// 应用程序的主入口点。
        /// </summary>
        static void Main(string[] args)
        {
            AppDomain.CurrentDomain.UnhandledException += CurrentDomain_UnhandledException;
            if (System.Environment.UserInteractive)
            {
                string parameter = string.Concat(args);
                switch (parameter)
                {
                    case "-v":
                        {
                            string path = System.AppDomain.CurrentDomain.BaseDirectory;
                            var serviceVersion = FileVersionInfo.GetVersionInfo(System.IO.Path.Combine(path, "CC-StationService.exe"));
                            Console.WriteLine(serviceVersion.FileVersion.ToString());
                        }
                        break;
                    case "--version":
                        {
                            string path = System.AppDomain.CurrentDomain.BaseDirectory;
                            var serviceVersion = FileVersionInfo.GetVersionInfo(System.IO.Path.Combine(path, "CC-StationService.exe"));
                            Console.WriteLine(serviceVersion.ToString());
                        }
                        break;
                    case "-d":
                    case "--debug":
                        {
                            var debugService = new StationMonitorService();
                            debugService.Start();
                        }
                        break;
                    default:
                        Console.WriteLine("Usage: CC-StationService.exe -i or --install to install this service.");
                        Console.WriteLine("            CC-StationService.exe -u or -uninstall to uninstall this service.");
                        break;
                }
            }
            else
            {
                ServiceBase[] ServicesToRun = new ServiceBase[]
                {
                    new StationMonitorService()
                };
                ServiceBase.Run(ServicesToRun);
            }

        }

        private static void CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            string path = Path.Combine(Path.GetTempPath(), @"ccservice-install-error.txt");
            Exception ex = e.ExceptionObject as Exception;
            File.AppendAllText(path, ex.Message + ex.InnerException.Message);
        }
    }
}
