using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace CC_StationService
{
    class LinuxMethod : IPlatformMethod
    {
        public long GetTotalMemory()
        {
            return sysconf((int)LinuxSysConfigName._SC_PHYS_PAGES) * sysconf((int)LinuxSysConfigName._SC_PAGESIZE);
        }

        public void Reboot()
        {
            System.Diagnostics.Process.Start("shutdown -r 0");
        }

        public void Shutdown()
        {
            System.Diagnostics.Process.Start("shutdown -h 0");
        }

        [DllImport("libc")]
        extern static long sysconf(int __name);
    }
}
