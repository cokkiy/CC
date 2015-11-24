using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Management;
using System.Runtime.InteropServices;

namespace TestConsole
{
    class Program
    {
        static void Main(string[] args)
        {
            //Console.WriteLine(System.GC.GetTotalMemory(false));
            long result = sysconf((int)SysConfName._SC_PHYS_PAGES) * (long)sysconf((int)SysConfName._SC_PAGESIZE);

            Console.WriteLine(result);
            Console.ReadKey();
        }

        [DllImport("libc")]
        extern static long sysconf(int __name);
    }
}
