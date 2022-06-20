using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.NetworkInformation;
using System.Text;
using System.Threading;

namespace NIStatusDetectors
{
    /// <summary>
    /// 网卡状态检测器
    /// </summary>
    public class NIStatusDetector
    {
        private IPAddress address;
        private bool exit = false;
        private bool started = false;
        //current status
        private OperationalStatus currentStatus = OperationalStatus.Unknown;
        //检测间隔
        private int interval;

        /// <summary>
        /// 创建一个网卡状态检测器对象
        /// </summary>
        /// <param name="address">网卡IP地址</param>
        public NIStatusDetector(string address) : this(address, 1000)
        {
        }


        /// <summary>
        /// 创建一个网卡状态检测器对象
        /// </summary>
        /// <param name="address">网卡IP地址</param>
        /// <param name="millionSecondsInterval">以毫秒为单位的检测间隔</param>
        public NIStatusDetector(string address, int millionSecondsInterval)
        {
            this.address = IPAddress.Parse(address);
            interval = millionSecondsInterval;
        }

        /// <summary>
        /// 创建一个网卡状态检测器对象
        /// </summary>
        /// <param name="address">网卡IP地址</param>
        public NIStatusDetector(IPAddress address) : this(address, 1000)
        {

        }

        /// <summary>
        /// 创建一个网卡状态检测器对象
        /// </summary>
        /// <param name="address">网卡IP地址</param>
        /// <param name="millionSecondsInterval">以毫秒为单位的检测间隔</param>
        public NIStatusDetector(IPAddress address, int millionSecondsInterval)
        {
            this.address = address;
            interval = millionSecondsInterval;
        }

        private void StartCheck()
        {
            Thread checkThread = new Thread(this.CheckState);
            checkThread.Start(address);
        }

        /// <summary>
        /// 网卡状态变化事件,当网卡状态发生变化时触发
        /// </summary>
        public event EventHandler<NIStateChangedEventArgs> NIStateChanged;

        /// <summary>
        /// 获取网卡当前状态
        /// </summary>
        public OperationalStatus CurrentStatus => this.currentStatus;

        /// <summary>
        /// 开始网卡状态检测
        /// </summary>
        public void Start()
        {
            if(!started)
            {
                exit = false;
                started = true;
                StartCheck();
            }
        }

        /// <summary>
        /// 停止网卡状态检测
        /// </summary>
        public void Stop()
        {
            exit = true;
            started = false;
        }

        // do ni state check
        private void CheckState(object state)
        {
            IPAddress ip = state as IPAddress;

            while (!exit)
            {
                NetworkInterface[] cards = NetworkInterface.GetAllNetworkInterfaces();
                var specified = cards.FirstOrDefault(ni => ni.NetworkInterfaceType == NetworkInterfaceType.Ethernet &&
                ni.GetIPProperties().UnicastAddresses.Any(addr => addr.Address.Equals(ip)));

                if (specified != null)
                {
                    if (specified.OperationalStatus != currentStatus)
                    {
                        currentStatus = specified.OperationalStatus;
                        NIStateChanged?.Invoke(this, new NIStateChangedEventArgs(currentStatus));
                    }

                }
                else if (currentStatus != OperationalStatus.Unknown)
                {
                    currentStatus = OperationalStatus.Unknown;
                    NIStateChanged?.Invoke(this, new NIStateChangedEventArgs(currentStatus));
                }

                Thread.Sleep(interval);
            }

        }

    }



    /// <summary>
    /// 网卡状态变化事件参数
    /// </summary>
    public class NIStateChangedEventArgs : EventArgs
    {
        /// <summary>
        /// 获取网卡状态
        /// </summary>
        public OperationalStatus OperationalStatus { get; private set; }

        public NIStateChangedEventArgs(OperationalStatus status)
        {
            this.OperationalStatus = status;
        }

        /// <summary>
        /// 网卡是否可用
        /// </summary>
        public bool IsUp => OperationalStatus == OperationalStatus.Up;
    }


}
