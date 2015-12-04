using System;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using System.IO;
using System.Diagnostics;
using System.Threading;
using System.Collections.Generic;
using System.Net.NetworkInformation;
using System.Linq;

namespace TestConsole
{
    class Program
    {
        static void Main(string[] args)
        {
           var list = getAllNIInfo();

            Console.ReadKey();
        }

        private static Dictionary<string, List<string>> getAllNIInfo()
        {

            Dictionary<string, List<string>> niInfo = new Dictionary<string, List<string>>();
            NetworkInterface[] nis = NetworkInterface.GetAllNetworkInterfaces();
            foreach (var ni in nis)
            {
                if (ni.NetworkInterfaceType == NetworkInterfaceType.Ethernet)
                {
                    string mac = string.Join("-", ni.GetPhysicalAddress().GetAddressBytes()
                        .Select((byte b) => { return b.ToString("X2"); }));
                    IPInterfaceProperties ipProperties = ni.GetIPProperties();
                    List<string> ipList = new List<string>(); ;
                    foreach (var item in ipProperties.UnicastAddresses)
                    {
                        if(item.Address.AddressFamily== System.Net.Sockets.AddressFamily.InterNetwork)
                            ipList.Add(item.Address.ToString());
                    }
                    niInfo.Add(mac, ipList);
                }
            }

            return niInfo;
        }

        [DllImport("libc")]
        extern static long sysconf(int __name);
    }
}
