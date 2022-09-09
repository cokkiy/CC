using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.ServiceProcess;
using System.Text;
using System.Threading;

namespace CCMonitor
{
    public partial class MonitorService : ServiceBase
    {
        //是否停止
        private bool stop = false;
        //最后一次心跳时间
        private DateTime lastTick = DateTime.Now;

        public MonitorService()
        {
            InitializeComponent();
        }

        protected override void OnStart(string[] args)
        {
            Start();
        }

        protected override void OnStop()
        {
            stop = true;
        }

        public void Start()
        {
            Logger.Message("Start...");            
            stop = false;

            Logger.Message("心跳检测线程启动...");
            Thread heartbeatThread = new Thread(HeartbeatMonitor);
            heartbeatThread.Start();

            Logger.Message("结果检测线程启动...");
            Thread checkResultAndHandleThread = new Thread(CheckResultAndHandle);
            checkResultAndHandleThread.Start();

            Logger.Message("启动完毕。");
        }


        /// <summary>
        /// 心跳检测
        /// </summary>
        /// <returns></returns>
        private void HeartbeatMonitor()
        {
            UdpClient client = new UdpClient(54321, AddressFamily.InterNetwork);
            client.BeginReceive(ReceiveCallback, client);
        }

        public void ReceiveCallback(IAsyncResult ar)
        {
            var client = ar.AsyncState as UdpClient;
            IPEndPoint endPoint = new IPEndPoint(IPAddress.Loopback, 0);
            try
            {
                Byte[] receiveBytes = client.EndReceive(ar, ref endPoint);
                if (receiveBytes.Length > 0)
                {
                    lastTick = DateTime.Now;
                }

                if (!stop)
                {
                    client.BeginReceive(ReceiveCallback, client);
                }
            }
            catch(Exception ex)
            {
                Logger.Error(ex);
            }
        }

        //检测结果并处理
        private void CheckResultAndHandle()
        {
            while (!stop)
            {
                if (DateTime.Now - lastTick > TimeSpan.FromSeconds(10))
                {
                    Logger.Message("CC Service Not Running, I Will try to start it. LastTick:{0}", lastTick);
                    //Restart Server
                    if (Environment.OSVersion.Platform == PlatformID.Unix)
                    {
                        try
                        {
                            Process.Start("service", "ccservice stop");
                            Logger.Message("Exec:service ccservice stop ");
                            Thread.Sleep(2000);
                            Process.Start("service", "ccservice start");
                            Logger.Message("Exec:service ccservice start ");
                        }
                        catch(Exception ex)
                        {
                            Logger.Error(ex);
                        }
                    }
                    else
                    {
                        Process.Start("net", " start \"Station Monitor Service\"");
                    }
                }
                else
                {
                    //Console.WriteLine("上次心跳时间：{0}", lastTick);
                }

                Thread.Sleep(1000);
            }
        }
    }
}
