using System;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using System.IO;
using System.Diagnostics;
using System.Threading;
using System.Collections.Generic;
using System.Net.NetworkInformation;
using System.Linq;
using Ice;
using CC;
using System.Net;
using System.Text;
using System.Reflection;
using System.Security.Principal;
using System.Security.AccessControl;
using System.Xml.Serialization;
using System.Xml.Linq;
using System.Xml;
using System.Management;

namespace TestConsole
{
    
    class Program
    {
        private static Dictionary<string, PerformanceCounter> bytesReceivedPerSecondCounters = new Dictionary<string, PerformanceCounter>();
        private static Dictionary<string, PerformanceCounter> bytesSentPerSecondCounters = new Dictionary<string, PerformanceCounter>();
        private static Dictionary<string, PerformanceCounter> bytesTotalPerSecondCounters = new Dictionary<string, PerformanceCounter>();


        static void Main(string[] args)
        {
            GetPerInstance("Network Interface");
            NetworkInterface[] adapters = NetworkInterface.GetAllNetworkInterfaces();
            //foreach (var adapter in adapters)
            //{
            //    //Console.WriteLine("ID:....{0}",adapter.Id);
            //    //Console.WriteLine("Name:....{0}", adapter.Name);
            //    Console.WriteLine("Description:....{0}", adapter.Description);
            //}
            foreach (NetworkInterface adapter in adapters)
            {
                if ((adapter.NetworkInterfaceType == NetworkInterfaceType.Ethernet
                    || adapter.NetworkInterfaceType == NetworkInterfaceType.FastEthernetT)
                    && adapter.OperationalStatus == OperationalStatus.Up)
                {
                    try
                    {
                        Console.WriteLine("Description:....{0}", adapter.Description);
                        string description = adapter.Description.Replace('(', '[').Replace(')', ']').Replace('/','_');
                        if (!bytesReceivedPerSecondCounters.ContainsKey(description))
                        {
                            PerformanceCounter bytesReceivedPerSecondCounter = new PerformanceCounter("Network Interface", "Bytes Received/sec", description);
                            bytesReceivedPerSecondCounters.Add(description, bytesReceivedPerSecondCounter);
                        }
                        if (!bytesSentPerSecondCounters.ContainsKey(description))
                        {
                            PerformanceCounter bytesSentPerSecondCounter = new PerformanceCounter("Network Interface", "Bytes Sent/sec", description);
                            bytesSentPerSecondCounters.Add(description, bytesSentPerSecondCounter);
                        }

                        if (!bytesTotalPerSecondCounters.ContainsKey(description))
                        {
                            PerformanceCounter totalPerSecondCounter = new PerformanceCounter("Network Interface", "Bytes Total/sec", description);
                            bytesTotalPerSecondCounters.Add(description, totalPerSecondCounter);
                        }

                        var counter1 = bytesReceivedPerSecondCounters[description];
                        var counter2 = bytesSentPerSecondCounters[description];
                        var counter3 = bytesTotalPerSecondCounters[description];

                        Console.WriteLine("", counter1.NextValue());
                        Console.WriteLine("", counter1.NextValue());
                        Console.WriteLine("", counter1.NextValue());

                        //ifStatistics.BytesReceivedPerSec = counter1.NextValue();
                        //ifStatistics.BytesSentedPerSec = counter2.NextValue();
                        //ifStatistics.TotalBytesPerSec = counter3.NextValue();
                        //ifStatistics.BytesReceived = counter1.RawValue;
                        //ifStatistics.BytesSented = counter2.RawValue;
                        //ifStatistics.BytesTotal = counter3.RawValue;

                        var ipv4Statistics = adapter.GetIPv4Statistics();

                        //ifStatistics.UnicastPacketReceived = ipv4Statistics.UnicastPacketsReceived;
                        //ifStatistics.UnicastPacketSented = ipv4Statistics.UnicastPacketsSent;
                        //ifStatistics.MulticastPacketReceived = ipv4Statistics.NonUnicastPacketsReceived;
                        //ifStatistics.MulticastPacketSented = ipv4Statistics.NonUnicastPacketsSent;
                    }
                    catch (System.Exception ex)
                    {
                        Console.WriteLine(ex.ToString());
                    }
                }
            }


            Console.WriteLine("Press any key to exits");
            Console.ReadKey();
        }

        


        /// <summary>
        /// 获取本机所有网卡配置信息
        /// </summary>
        /// <returns>Mac Address->IP Address 列表</returns>
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
                        //只添加IPV4地址
                        if (item.Address.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork)
                            ipList.Add(item.Address.ToString());
                    }
                    niInfo.Add(ni.Name, ipList);
                }
            }

            return niInfo;
        }

        private static void chown(string fileName, string user)
        {
            if (System.Environment.OSVersion.Platform == PlatformID.Unix)
            {
                try
                {
                    System.Diagnostics.Process chownProcess = new System.Diagnostics.Process();
                    chownProcess.StartInfo.FileName = "chown";
                    chownProcess.StartInfo.UseShellExecute = true;
                    chownProcess.StartInfo.Arguments = string.Format("{0} {1}", user, fileName);
                    chownProcess.Start();
                }
                catch (System.Exception ex)
                {
                    //Ice.Logger logger = PlatformMethodFactory.GetLogger();
                    Console.WriteLine(string.Format("chown Error. fileName:{0}, new owner:{1}. {2}", fileName, user, ex.ToString()));
                }
            }
        }

        private static void GetPerInstance(string categoryName)
        {
            PerformanceCounterCategory performanceCounterCategory = new PerformanceCounterCategory(categoryName);
            var instances = performanceCounterCategory.GetInstanceNames();
            foreach (var instance in instances)
            {
                Console.WriteLine(instance);
            }
        }

    }
}
