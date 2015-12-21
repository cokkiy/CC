﻿using System;
using System.Configuration.Install;
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
                    case "-i":
                    case "-install":
                        ManagedInstallerClass.InstallHelper(new string[] { Assembly.GetExecutingAssembly().Location });
                        break;
                    case "-u":
                    case "-uninstall":
                        ManagedInstallerClass.InstallHelper(new string[] { "/u", Assembly.GetExecutingAssembly().Location });
                        break;
                    default:
                        Console.WriteLine("Usage: CC-StationService.exe -i or -install to install this service.");
                        Console.WriteLine("            CC-StationService.exe -u or -uninstall to uninstall this service.");
                        break;
                }
            }
            else
            {
                ServiceBase[] ServicesToRun;
                ServicesToRun = new ServiceBase[]
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
