using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Management;
using System.Text;
using System.Threading.Tasks;

namespace CC_StationService
{
#pragma warning disable CA1416
    class WindowsMethod : IPlatformMethod
    {
        //总内存
        private static long totalMemory = 0;

        public long GetTotalMemory()
        {
            if (totalMemory != 0)
                return totalMemory;
            ObjectQuery query = new ObjectQuery("SELECT * FROM Win32_ComputerSystem");
            var searcher = new ManagementObjectSearcher(query);
            var collection = searcher.Get();
            foreach (var item in collection)
            {
                totalMemory = (long)(ulong)item["TotalPhysicalMemory"];
                return totalMemory;
            }
            return 0;
        }

        public void Reboot()
        {
            System.Diagnostics.Process.Start("shutdown", " -r -f -t 0");
        }

        public void Shutdown()
        {
            System.Diagnostics.Process.Start("shutdown", " -s -f -t 0");
        }

        /// <summary>
        /// 获取内存使用情况
        /// </summary>
        /// <returns></returns>
        public MemoryInfo GetMemoryInfo()
        {
            PerformanceCounter memoryCounter = new PerformanceCounter("Memory", "Available Bytes");
            MemoryInfo info = new MemoryInfo();
            if (totalMemory == 0)
            {
                info.Total = GetTotalMemory();
            }
            info.Total = totalMemory;
            info.Free = memoryCounter.RawValue;
            info.Cached = 0;
            info.Buffers = 0;
            memoryCounter.Close();
            return info;
        }

    }
#pragma warning restore CA1416   
}
