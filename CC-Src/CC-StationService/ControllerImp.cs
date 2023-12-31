﻿using System;
using System.Collections.Generic;
using CC;
using Ice;
using System.IO;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Net.Sockets;
using System.Net;
using System.Net.NetworkInformation;
using System.Linq;

namespace CC_StationService
{
    /// <summary>
    /// 系统控制类
    /// </summary>
    class ControllerImp : CC.IControllerDisp_
    {
        /// <summary>
        /// 平台相关函数
        /// </summary>
        private static IPlatformMethod platformMethod;

        /// <summary>
        /// 系统监视对象
        /// </summary>
        private SystemWatcher watcher;

        /// <summary>
        /// Ice Communicator 对象
        /// </summary>
        private Ice.Communicator ic;

        /// <summary>
        /// 静态构造函数
        /// </summary>
        static ControllerImp()
        {
            platformMethod = PlatformMethodFactory.GetPlatformMethod();
        }

        /// <summary>
        /// 创建一个系统控制类对象
        /// </summary>
        /// <param name="ic">Ice Communicator 对象</param>
        public ControllerImp(Ice.Communicator ic)
        {
            this.ic = ic;
        }

        
        /// <summary>
        /// 获取系统信息
        /// </summary>
        /// <param name="current__"></param>
        /// <returns>系统信息</returns>
        public override StationSystemState getSystemState(Current current__)
        {
            return StateGatherer.GatherSystemState();
        }

        /// <summary>
        /// 重启系统
        /// </summary>
        /// <param name="force">是否强制重启</param>
        /// <param name="current__"></param>
        public override void reboot(bool force, Current current__)
        {
            platformMethod.Reboot();
        }

        /// <summary>
        /// 设置系统监视对象
        /// </summary>
        public SystemWatcher SystemWatcher
        {
            set
            {
                this.watcher = value;
            }
        }        

        /// <summary>
        /// 设置状态监视间隔
        /// </summary>
        /// <param name="interval">状态监视间隔,单位秒</param>
        /// <param name="current__"></param>
        public override void setStateGatheringInterval(int interval, Current current__)
        {
            if (watcher != null)
            {
                watcher.Interval = interval * 1000;
            }
        }

        /// <summary>
        /// 设置监视应用程序列表
        /// </summary>
        /// <param name="procNames"></param>
        /// <param name="current__"></param>
        public override void setWatchingApp(List<string> procNames, Current current__)
        {
            watcher.AddAppToWatching(procNames);
        }

        /// <summary>
        /// 关闭系统
        /// </summary>
        /// <param name="current__"></param>
        public override void shutdown(Current current__)
        {
            platformMethod.Shutdown();
        }

        /// <summary>
        /// 启动指定名称的应用程序
        /// </summary>
        /// <param name="appParams">应用程序启动参数信息</param>
        /// <param name="current__"></param>
        /// <returns>程序启动结果</returns>
        public override List<AppStartingResult> startApp(List<AppStartParameter> appParams, Current current__)
        {
            ObjectPrx proxy = ic.propertyToProxy("AppLuncher.Proxy");
            try
            {
                AppController.ILuncherPrx luncherPrx = AppController.ILuncherPrxHelper.checkedCast(proxy);
                return luncherPrx.startApp(appParams);
            }
            catch (Ice.Exception ex)
            {
                if (ex.InnerException != null)
                    throw new AppControlError("应用程序启动代理接口错误:" + ex.InnerException.Message);
                else
                    throw new AppControlError("应用程序启动代理接口错误,没有运行或被阻塞.");
            }
        }

        /// <summary>
        /// 关闭指定进程Id的所有程序
        /// </summary>
        /// <param name="processIdList">进程Id列表</param>
        /// <param name="current__"></param>
        public override List<AppControlResult> closeApp(List<int> processIdList, Current current__)
        {
            ObjectPrx proxy = ic.propertyToProxy("AppLuncher.Proxy");            
            try
            {
                AppController.ILuncherPrx luncherPrx = AppController.ILuncherPrxHelper.checkedCast(proxy);
                return luncherPrx.closeApp(processIdList);
            }
            catch (Ice.Exception ex)
            {
                throw new AppControlError("应用程序启动代理接口错误:" + ex.InnerException.Message);
            }
        }

        /// <summary>
        /// 重启指定的应用程序
        /// </summary>
        /// <param name="appName">应用程序名称</param>
        /// <param name="current__"></param>
        public override List<AppStartingResult> restartApp(List<AppStartParameter> appParams, Current current__)
        {
            ObjectPrx proxy = ic.propertyToProxy("AppLuncher.Proxy");
            try
            {
                AppController.ILuncherPrx luncherPrx = AppController.ILuncherPrxHelper.checkedCast(proxy);
                return luncherPrx.restartApp(appParams);
            }
            catch (Ice.Exception ex)
            {
                throw new AppControlError("应用程序启动代理接口错误:" + ex.InnerException.Message);
            }
        }


        /// <summary>
        /// 捕获屏幕快照
        /// </summary>
        /// <param name="position">快照内容传送位置，如果为0，则捕获新快照，否则从该位置继续传送后续内容</param>
        /// <param name="length">传送数据长度</param>
        /// <param name="data">传送的数据</param>
        /// <param name="current__"></param>
        /// <returns>传送是否完毕</returns>
        public override bool captureScreen(long position, out int length, out byte[] data, Current current__)
        {
            ObjectPrx proxy = ic.propertyToProxy("AppLuncher.Proxy");
            try
            {
                AppController.ILuncherPrx luncherPrx = AppController.ILuncherPrxHelper.checkedCast(proxy);
                return luncherPrx.captureScreen(position, out length, out data);
            }
            catch (Ice.Exception ex)
            {
                PlatformMethodFactory.GetLogger().error("捕获屏幕快照失败：" + ex.ToString());
                throw new FileTransException("屏幕快照:应用程序启动代理接口错误", position, 0, 0, ex.ToString());
            }
            catch (System.Exception ex)
            {
                PlatformMethodFactory.GetLogger().error("捕获屏幕快照失败：" + ex.ToString());
                throw new FileTransException("屏幕快照", position, 0, 0, ex.ToString());
            }
        }

        public override List<CC.FileInfo> getFileInfo(CC.Path path, Current current__)
        {
            if((path.Name=="/" || path.Name=="/home") && Environment.OSVersion.Platform==PlatformID.Win32NT)
            {
                try
                {
                    //Windows 返回磁盘列表
                    string[] drivers = Directory.GetLogicalDrives();
                    List<CC.FileInfo> infos = new List<CC.FileInfo>();
                    foreach (var item in drivers)
                    {
                        CC.FileInfo info = new CC.FileInfo();
                        info.Parent = "/";
                        info.Path = item;
                        info.isDirectory = true;
                        info.CreationTime = "";
                        info.LastModify = "";
                        infos.Add(info);
                    }
                    return infos;
                }
                catch (System.Exception ex)
                {
                    throw new DirectoryAccessError("/", "访问被拒绝", ex);
                }
            }

            if(path.Name=="")
            {
                path.Name = "/home";
                if (Environment.OSVersion.Platform == PlatformID.Win32NT)
                {
                    path.Name = Environment.GetFolderPath(Environment.SpecialFolder.CommonDocuments);
                }
            }
            if (Directory.Exists(path.Name))
            {

                try
                {
                    List<CC.FileInfo> infos = new List<CC.FileInfo>();
                    DirectoryInfo dir = new DirectoryInfo(path.Name);
                    var dirs = dir.EnumerateDirectories();
                    foreach (var item in dirs)
                    {
                        CC.FileInfo info = new CC.FileInfo();
                        info.Parent = path.Name;
                        info.Path = item.Name;
                        info.isDirectory = true;
                        info.CreationTime = item.CreationTime.ToLongDateString();
                        info.LastModify = item.LastWriteTime.ToLongDateString();
                        infos.Add(info);
                    }
                    var files = dir.EnumerateFiles();
                    foreach (var item in files)
                    {
                        CC.FileInfo info = new CC.FileInfo();
                        info.Parent = path.Name;
                        info.Path = item.Name;
                        info.isDirectory = false;
                        info.CreationTime = item.CreationTime.ToLongDateString();
                        info.LastModify = item.LastWriteTime.ToLongDateString();
                        infos.Add(info);
                    }

                    return infos;
                }
                catch(System.Exception ex)
                {
                    throw new DirectoryAccessError(path.Name, ex.Message, ex);
                }
            }
            else
            {
                throw new DirectoryAccessError(path.Name, "目录不存在。");
            }
        }

        /// <summary>
        /// 获取当前运行的全部进程信息
        /// </summary>
        /// <param name="current__"></param>
        /// <returns>当前运行的全部进程信息</returns>
        public override List<ProcessInfo> getAllProcessInfo(Current current__)
        {
            List<ProcessInfo> infos = new List<ProcessInfo>();
            var processes = System.Diagnostics.Process.GetProcesses();
            foreach (var p in processes)
            {
                try
                {
                    if (p.Id != 0)
                    {
                        ProcessInfo info = new ProcessInfo();
                        info.Id = p.Id;
                        info.Name = p.ProcessName;
                        info.Time = p.TotalProcessorTime.ToString();
                        info.MemorySize = p.WorkingSet64;
                        infos.Add(info);
                    }
                }
                catch
                {
                }
            }

            return infos;
        }

        /// <summary>
        /// 获取服务端版本信息，包括服务程序版本和代理版本
        /// </summary>
        /// <param name="current__"></param>
        /// <returns></returns>
        public override ServerVersion getServerVersion(Current current__)
        {
            CC.Version serviceVersion = new CC.Version();
            CC.Version appVersion = new CC.Version();
            try
            {
                string path = System.AppDomain.CurrentDomain.BaseDirectory;
                serviceVersion = FileVersionInfo.GetVersionInfo(System.IO.Path.Combine(path, "CC-StationService.exe"));
                ObjectPrx proxy = ic.propertyToProxy("AppLuncher.Proxy");
                AppController.ILuncherPrx luncherPrx = AppController.ILuncherPrxHelper.checkedCast(proxy);
                appVersion = luncherPrx.getAppLuncherVersion();
            }
            catch
            {
            }
            return new ServerVersion(serviceVersion, appVersion);
        }

        /// <summary>
        /// 切换指显软件显示模式和页面切换
        /// </summary>
        /// <param name="cmd">显示命令</param>
        /// <param name="current__"></param>
        public override void switchDisplayPageAndMode(DisplayCommand cmd, Current current__)
        {
            UdpClient client = new UdpClient(AddressFamily.InterNetwork);
            System.Net.IPEndPoint receiver = new System.Net.IPEndPoint(IPAddress.Loopback, 9008);
            byte[] cmdData = new byte[512];        
            Array.Copy(BitConverter.GetBytes((int)cmd), cmdData, sizeof(int));
            try
            {
                client.Send(cmdData, 512, receiver);
                client.Close();
            }
            catch(System.Exception ex)
            {
                PlatformMethodFactory.GetLogger().error(string.Format("发送指显控制命令失败，命令{0},错误详细内容：{1}", cmd, ex.ToString()));
            }
        }

        /// <summary>
        /// 打开指定画面
        /// </summary>
        /// <param name="pageNum"></param>
        /// <param name="current__"></param>
        public override void openSpecPage(int pageNum, Current current__)
        {
            UdpClient client = new UdpClient(AddressFamily.InterNetwork);
            System.Net.IPEndPoint receiver = new System.Net.IPEndPoint(IPAddress.Loopback, 9008);
            byte[] cmdData = new byte[512];
            int cmd = 10; //open spec page
            Array.Copy(BitConverter.GetBytes((int)cmd), cmdData, sizeof(int));
            Array.Copy(BitConverter.GetBytes(pageNum), 0, cmdData, sizeof(int), sizeof(int));
            try
            {
                client.Send(cmdData, 512, receiver);
                client.Close();
            }
            catch (System.Exception ex)
            {
                PlatformMethodFactory.GetLogger().error(string.Format("发送指显控制命令失败，命令{0},错误详细内容：{1}", cmd, ex.ToString()));
            }
        }

        /// <summary>
        /// 获取中控服务程序所在绝对路径
        /// </summary>
        /// <param name="current__"></param>
        /// <returns></returns>
        public override string getServicePath(Current current__)
        {
            string path = System.AppDomain.CurrentDomain.BaseDirectory;
            return System.IO.Path.Combine(path, "CC-StationService.exe");
        }

        /// <summary>
        /// 获取应用程序启动代理程序所在绝对路径
        /// </summary>
        /// <param name="current__"></param>
        /// <returns></returns>
        public override string getLuncherProxyPath(Current current__)
        {
            ObjectPrx proxy = ic.propertyToProxy("AppLuncher.Proxy");
            try
            {
                AppController.ILuncherPrx luncherPrx = AppController.ILuncherPrxHelper.checkedCast(proxy);
                return luncherPrx.getAppLuncherPath();
            }
            catch(System.Exception ex)
            {
                PlatformMethodFactory.GetLogger().error(string.Format("获取应用程序代理绝对路径失败,发生错误：{0}", ex.ToString()));
            }
            return string.Empty;
        }

        /// <summary>
        /// 对指定的文件进行改名
        /// </summary>
        /// <param name="oldName">原文件名称</param>
        /// <param name="newName">新文件名</param>
        /// <param name="current__"></param>
        /// <returns>改名操作是否成功</returns>
        public override bool renameFile(string oldName, string newName, Current current__)
        {
            if (File.Exists(oldName))
            {
                try
                {
                    if (File.Exists(newName))
                    {
                        File.Delete(newName);
                    }
                    File.Move(oldName, newName);
                    return true;
                }
                catch (System.Exception ex)
                {
                    PlatformMethodFactory.GetLogger().error(string.Format("文件改名操作失败,发生错误：{0}", ex.ToString()));
                }
            }
            return false;
        }

        /// <summary>
        /// 设置气象云图下载选项，并启动下载线程
        /// </summary>
        /// <param name="option">气象云图下载选项</param>
        /// <param name="current__"></param>
        public override void setWeatherPictureDownloadOption(WeatherPictureDowlnloadOption option, Current current__)
        {
            if (string.IsNullOrWhiteSpace(option.Url) || option.Interval <= 0 || option.LastHours <= 0)
                return;             
        }


        public override List<NetworkInterfaceInfo> GetNetworkInterfaces(Current current__)
        {
            List<NetworkInterfaceInfo> infos = new List<NetworkInterfaceInfo>();
            NetworkInterface[] adapters = NetworkInterface.GetAllNetworkInterfaces();
            foreach (NetworkInterface adapter in adapters)
            {
                if (adapter.NetworkInterfaceType == NetworkInterfaceType.Ethernet
                    || adapter.NetworkInterfaceType == NetworkInterfaceType.FastEthernetT)
                {
                    var info = new NetworkInterfaceInfo();
                    info.Id = adapter.Id;
                    info.Name = adapter.Name;
                    info.Description = adapter.Description;
                    info.IsUp = adapter.OperationalStatus == OperationalStatus.Up;

                    IPInterfaceProperties properties = adapter.GetIPProperties();
                    info.DNSSuffix = properties.DnsSuffix;
                    info.DNSEnbled = properties.IsDnsEnabled;
                    info.DynamicDNS = properties.IsDynamicDnsEnabled;
                    info.DhcpServers = properties.DhcpServerAddresses.Select(s => s.ToString()).ToList();
                    info.IPAddresses = properties.UnicastAddresses.Select(a => a.Address.ToString()).ToList();
                    info.MulticastAddresses = properties.MulticastAddresses.Select(a => a.Address.ToString()).ToList();

                    infos.Add(info);
                }
            }

            return infos;
        }

        public override List<ConnectionInformation> GetConnectionInformations(Current current__)
        {
            IPGlobalProperties IPProperties = IPGlobalProperties.GetIPGlobalProperties();             
            TcpConnectionInformation[] connections = IPProperties.GetActiveTcpConnections();
            return connections.Select(c => new ConnectionInformation {
                LocalAddress =c.LocalEndPoint.Address.ToString(),
                LocalPort=c.LocalEndPoint.Port,
                Remote=c.RemoteEndPoint.Address.ToString(),
                RemotePort=c.RemoteEndPoint.Port,
                State=c.State.ToString()
            }).ToList();       
           
        }

        public override List<ListenerInfo> GetTCPListenerInfos(Current current__)
        {
            IPGlobalProperties IPProperties = IPGlobalProperties.GetIPGlobalProperties();
            IPEndPoint[] endPoints = IPProperties.GetActiveTcpListeners();
            return endPoints.Select(e => new ListenerInfo {
                Address=e.Address.ToString(),
                Port=e.Port
            }).ToList();            
        }

        public override List<ListenerInfo> GetUDPListenerInfos(Current current__)
        {
            IPGlobalProperties IPProperties = IPGlobalProperties.GetIPGlobalProperties();            
            IPEndPoint[] endPoints = IPProperties.GetActiveUdpListeners();
            return endPoints.Select(e => new ListenerInfo
            {
                Address = e.Address.ToString(),
                Port = e.Port
            }).ToList();
        }

        public override void sendRemoteCtrlCommand(int cmdCode, byte[] ctrlParams, Current current__)
        {
            UdpClient client = new(AddressFamily.InterNetwork);
            IPEndPoint receiver = new(IPAddress.Loopback, 9008);
            byte[] cmdData = new byte[512];
            Array.Copy(BitConverter.GetBytes(cmdCode), cmdData, sizeof(int));
            int remain = cmdData.Length - sizeof(int);
            Array.Copy(ctrlParams, 0, cmdData, 4, ctrlParams.Length <= remain ? ctrlParams.Length : remain);
            try
            {
                client.Send(cmdData, 512, receiver);
                client.Close();
            }
            catch (System.Exception ex)
            {
                PlatformMethodFactory.GetLogger().error(string.Format("发送指显控制命令失败，命令{0},错误详细内容：{1}", cmdCode, ex.ToString()));
            }
        }
    }
}
