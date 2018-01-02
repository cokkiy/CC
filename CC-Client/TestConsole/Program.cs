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

        static void Main(string[] args)
        {
            Console.WriteLine(Environment.OSVersion.VersionString);
            Console.WriteLine(Environment.Version);
            Console.WriteLine(Environment.MachineName);

            Console.WriteLine(Environment.GetEnvironmentVariable("PROCESSOR_IDENTIFIER"));

            DriveInfo[] drivers = DriveInfo.GetDrives();
            foreach (var driver in drivers)
            {
                Console.WriteLine(driver.Name);
                Console.WriteLine(driver.DriveType);
                if (driver.IsReady)
                {
                    Console.WriteLine(driver.DriveFormat);
                    Console.WriteLine(driver.TotalFreeSpace);
                    Console.WriteLine(driver.TotalSize);
                    Console.WriteLine(driver.VolumeLabel);
                }
            }

            ManagementClass mc = new ManagementClass("win32_processor");
            ManagementObjectCollection moc = mc.GetInstances();
            string id = string.Empty;
            foreach (ManagementObject mo in moc)
            {
                id = mo.Properties["processorID"].Value.ToString();
                break;
            }

            Console.WriteLine(id);

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

    }
}
