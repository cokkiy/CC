using System;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using System.IO;
using System.Diagnostics;
using System.Threading;

namespace TestConsole
{
    class Program
    {
        static void Main(string[] args)
        {

            //             PerformanceCounter pCounter = new PerformanceCounter("Process", "% Processor Time", "4085/designer");
            //             for (int i = 0; i < 10; i++)
            //             {
            //                 Console.WriteLine(pCounter.NextValue()/System.Environment.ProcessorCount);
            //                 Thread.Sleep(500);
            //             }
            // 
            // 
            //             Process[] processes = Process.GetProcessesByName("designer");
            //             foreach (var process in processes)
            //             {
            // 
            //                 Console.WriteLine("{0}/{1}:Thread Count:{2} Memory:{3} StartTime:{4} CPU:{5}",
            //                     process.Id, process.ProcessName, process.Threads.Count,
            //                     process.WorkingSet64, process.StartTime, process.TotalProcessorTime);
            //             }

            if (!EventLog.SourceExists("CCService"))
            {
                EventLog.CreateEventSource("CCService", "Application");
            }

            Console.ReadKey();
        }

       

        [DllImport("libc")]
        extern static long sysconf(int __name);
    }
}
