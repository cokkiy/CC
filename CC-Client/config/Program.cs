using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.NetworkInformation;
using System.Text;
using System.Threading.Tasks;

namespace config
{
    class Program
    {
        static string ClientConfig = @"/opt/CC-Client/Config.Server";
        static string ServerConfig = @"/usr/sbin/ccservice/Config.Client";

        private static bool doConfig(int index, List<string> ips, string fileName, bool client)
        {
            string number = "0";
            if (ips.Count > 1)
            {
                number = Console.ReadLine();
            }
            else
            {
                Console.WriteLine("0");
            }          

            int ipIndex = 0;
            if (int.TryParse(number, out ipIndex))
            {
                if (ipIndex < index)
                {
                    //配置
                    string ip = ips[ipIndex];
                    List<string> text = new List<string>();
                    using (FileStream f = new FileStream(fileName, FileMode.Open, FileAccess.Read))
                    {
                        using (TextReader reader = new StreamReader(f))
                        {
                            string line = reader.ReadLine();
                            while (line != null)
                            {
                                if (client)
                                {
                                    if (line.StartsWith("StateReceiver.Endpoints"))
                                    {
                                        line = string.Format("StateReceiver.Endpoints=udp -h 239.100.1.1 -p 10000 --interface {0} ", ip);
                                    }
                                }
                                else
                                {
                                    if (line.StartsWith("StateReceiver.Proxy"))
                                    {
                                        line = string.Format("StateReceiver.Proxy=stateReceiver:udp -h 239.100.1.1 -p 10000 --sourceAddress {0} --interface {0} ", ip);
                                    }
                                }
                                text.Add(line);
                                line = reader.ReadLine();
                            }
                        }
                        f.Close();
                    }


                    using (FileStream f = new FileStream(fileName, FileMode.Truncate, FileAccess.Write))
                    {
                        using (TextWriter writer = new StreamWriter(f))
                        {
                            foreach (var line in text)
                            {
                                writer.WriteLine(line);
                            }
                        }
                        f.Close();
                    }

                    return true;
                }
                else
                {
                    Console.WriteLine(string.Format("输入错误，请输入有效的序号(0-{0})。", index - 1));
                }
            }
            else
            {
                Console.WriteLine(string.Format("输入错误，请输入有效的序号(0-{0})。", index - 1));
            }

            return false;
        }

        static void Main(string[] args)
        {
            Dictionary<string, List<string>> niInfos = getAllNIInfo();

            if(args.Length!=1)
            {
                Console.WriteLine("语法错误：必须指定配置服务端还是客户端。");
                Console.WriteLine("用法：config.exe s 或 config.exe c。");
                return;
            }

            List<string> ips = new List<string>();
            int index = 0;
            foreach (var ni in niInfos)
            {
                foreach (var ip in ni.Value)
                {
                    ips.Add(ip);
                    Console.WriteLine(string.Format("{0} --> IP地址：{1} MAC： {2}", index, ip, ni.Key));
                    index++;
                }
            }            

            if (args[0]=="c")
            {
                //客户端
                bool result = false;
                while (!result)
                {
                    Console.WriteLine("选择一个客户端要绑定的IP地址（输入序号）：");
                    result = doConfig(index, ips, ClientConfig, true);
                }
            }
            else if(args[0]=="s")
            {
                //服务端
                bool result = false;
                while (!result)
                {
                    Console.WriteLine("选择一个服务端要绑定的IP地址（输入序号）：");
                    result = doConfig(index, ips, ServerConfig, false);
                }
            }

            Console.WriteLine("配置完毕。");
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
                    niInfo.Add(mac, ipList);
                }
            }

            return niInfo;
        }
    }
}
