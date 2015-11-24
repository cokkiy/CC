using System;
using System.Collections.Generic;
using System.Linq;
using System.Management;
using System.Text;
using System.Threading.Tasks;

namespace CC_StationService
{
    class WindowsMethod : IPlatformMethod
    {
        public long GetTotalMemory()
        {
            ObjectQuery query = new ObjectQuery("SELECT * FROM Win32_ComputerSystem");
            var searcher = new ManagementObjectSearcher(query);
            var collection = searcher.Get();
            foreach (var item in collection)
            {
                return (long)item["TotalPhysicalMemory"];
            }
            return 0;
        }

        public void Reboot()
        {
            System.Diagnostics.Process.Start("shutdown -r -f");
        }

        public void Shutdown()
        {
            System.Diagnostics.Process.Start("shutdown -s -f");
        }
    }
}
