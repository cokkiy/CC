using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LogClient
{
    /// <summary>
    /// 系统进程检查工具
    /// </summary>
    public static class SystemProcessChecker
    {
        static List<string>  processes =new List<string>();
        static SystemProcessChecker()
        {
            using (TextReader reader = new StreamReader("SysProcess.txt"))
            {
                var line = reader.ReadLine();
                while (line != null)
                {
                    if(!string.IsNullOrWhiteSpace(line))
                    {
                        processes.Add(line.TrimEnd(',', ' ', ';', '　'));
                    }

                    line = reader.ReadLine();
                }
            }
        }

        /// <summary>
        /// 检查是否是系统进程
        /// </summary>
        /// <param name="initRunningProcesses">系统初始化就运行的进程</param>
        /// <param name="name">进程名称</param>
        /// <returns></returns>
        public static bool IsSysProcess(List<string> initRunningProcesses, string name)
        {
            return processes.Contains(name);
        }
    }
}
