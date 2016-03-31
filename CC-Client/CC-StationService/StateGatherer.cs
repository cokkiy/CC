using CC;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net.NetworkInformation;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace CC_StationService
{
    /// <summary>
    /// 系统状态收集器
    /// </summary>
    static class StateGatherer
    {
        /// <summary>
        /// 平台相关函数
        /// </summary>
        private static IPlatformMethod platformMethod = null;

        /// <summary>
        /// 工作站ID
        /// </summary>
        private static string stationId = null;

        /// <summary>
        /// CPU性能计数器
        /// </summary>
        private static PerformanceCounter cpuCounter = null;

        /// <summary>
        /// 进程性能计数器
        /// </summary>
        private static Dictionary<int, PerformanceCounter> processCounters = new Dictionary<int, PerformanceCounter>();

        static StateGatherer()
        {
            platformMethod = PlatformMethodFactory.GetPlatformMethod();
            //初始化性能计数器
            cpuCounter = new PerformanceCounter("Processor", "% Processor Time", "_Total", true); 

            stationId = string.Format("{0}-{1}", System.Environment.OSVersion, Guid.NewGuid());
        }

        /// <summary>
        /// 收集系统状态
        /// </summary>
        /// <returns>系统状态</returns>
        public static StationSystemState GatherSystemState()
        {
            StationSystemState state = new StationSystemState();
            state.totalMemory = (long)platformMethod.GetTotalMemory();
            state.NetworkInterfaces = getAllNIInfo();
            state.computerName = System.Environment.MachineName;
            state.osName = System.Environment.OSVersion.Platform.ToString();
            state.osVersion = System.Environment.OSVersion.Version.ToString();
            state.stationId = stationId;
            return state;
        }


        /// <summary>
        /// 收集系统运行状态
        /// </summary>
        /// <returns>系统运行状态</returns>
        public static StationRunningState GatherRunningState()
        {
            StationRunningState runningState = new StationRunningState();
            runningState.stationId = stationId;
            runningState.procCount = GetTotalProcessCount();
            runningState.currentMemory = platformMethod.GetMemoryInfo().Used;
            runningState.cpu = cpuCounter.NextValue();
            return runningState;
        }


        /// <summary>
        /// 收集应用程序运行状态
        /// </summary>
        /// <param name="procName">应用程序名称/进程名称</param>
        /// <returns>应用程序运行状态<see cref="appName"/></returns>
        public static AppRunningState GatherAppRunningState(string procName)
        {
            if (procName.StartsWith("Id:"))
            {
                int id = -1;
                if (!int.TryParse(procName.Substring(3), out id))
                {
                    id = -1;
                }
                return GatherAppRunningState(id);
            }
            else
            {
                AppRunningState runningState = new AppRunningState();
                runningState.stationId = stationId;
                runningState.isRunning = false;
                runningState.process.ProcessMonitorName = procName;
                runningState.process.Id = -1;
                try
                {
                    System.Diagnostics.Process[] processes = System.Diagnostics.Process.GetProcessesByName(procName);
                    foreach (var process in processes)
                    {
                        runningState.process.Id = process.Id;
                        runningState.process.ProcessName = process.ProcessName;
                        runningState.currentMemory += process.WorkingSet64;
                        runningState.procCount++;
                        runningState.threadCount += process.Threads.Count;
                        //runningState.startTime = process.StartTime.ToBinary();
                        //runningState.appVersion = process.MainModule.FileVersionInfo.ProductVersion; CPU占用太高
                        runningState.isRunning = true;
                        runningState.cpu += GetProcessCPU(process);
                    }
                }
                catch(Exception ex)
                {
                    PlatformMethodFactory.GetLogger().error(ex.Message);
                }
                return runningState;
            }
        }

        /// <summary>
        /// 收集指定进程Id的应用程序运行状态
        /// </summary>
        /// <param name="Id">进程Id</param>
        /// <returns>应用程序运行状态</returns>
        public static AppRunningState GatherAppRunningState(int Id)
        {
            AppRunningState runningState = new AppRunningState();
            runningState.stationId = stationId;
            runningState.isRunning = false;
            runningState.process.Id = -1; //0在Windows下代表Idle进程
            //设置进程监控Id为原来的监控Id
            runningState.process.ProcessMonitorName = string.Format("Id:{0}", Id);
            if (Id > 0)
            {
                try
                {
                    System.Diagnostics.Process process = System.Diagnostics.Process.GetProcessById(Id);
                    runningState.isRunning = true;
                    runningState.currentMemory = process.WorkingSet64;
                    runningState.procCount = 1;
                    runningState.process.Id = Id;
                    runningState.threadCount = process.Threads.Count;
                    //runningState.startTime = process.StartTime.ToBinary();
                    //runningState.appVersion = process.MainModule.FileVersionInfo.ProductVersion; CPU占用太高
                    runningState.cpu = GetProcessCPU(process);
                }
                catch (ArgumentException ex)
                {
                    //进程没有运行
                    PlatformMethodFactory.GetLogger().error(ex.Message);
                }
                catch (Exception ex)
                {
                    PlatformMethodFactory.GetLogger().error(ex.Message);
                }
            }
             return runningState;
        }

        /// <summary>
        /// 获取指定进程的CPU信息
        /// </summary>
        /// <param name="process">进程</param>
        /// <returns>CPU信息</returns>
        private static float GetProcessCPU(System.Diagnostics.Process process)
        {
            try
            {
                if (processCounters.ContainsKey(process.Id))
                {
                    return processCounters[process.Id].NextValue() / System.Environment.ProcessorCount;
                }
                else
                {
                    string instanceName = process.ProcessName;
                    if (Environment.OSVersion.Platform == PlatformID.Unix)
                    {
                        instanceName = string.Format("{0}/{1}", process.Id, process.ProcessName);
                    }
                    PerformanceCounter counter = new PerformanceCounter("Process", "% Processor Time", instanceName);
                    processCounters.Add(process.Id, counter);
                    return counter.NextValue() / System.Environment.ProcessorCount;
                }
            }
            catch(Exception ex)
            {
                PlatformMethodFactory.GetLogger().error(ex.Message);
            }
            return 0; //不能计算
        }

        /// <summary>
        /// 获取总进程数
        /// </summary>
        /// <returns>系统中运行的总进程数</returns>
        private static int GetTotalProcessCount()
        {
            int count = 0;
            try
            {
                System.Diagnostics.Process[] processes = System.Diagnostics.Process.GetProcesses();
                count = processes.Count();
            }
            catch(Exception ex)
            {
                PlatformMethodFactory.GetLogger().error(ex.Message);
            }
            return count;
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
