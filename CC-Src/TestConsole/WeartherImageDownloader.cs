using CC;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace TestConsole
{
    /// <summary>
    /// 气象云图下载器
    /// </summary>
    public class WeartherImageDownloader
    {
        /// <summary>
        /// 获取或设置气象云图下载选项
        /// </summary>
        //public WeatherPictureDowlnloadOption Option { get; set; }

        public bool IsRunning { get; private set; }

        //是否停止运行
        private bool bStop = false;

        /// <summary>
        /// 停止下载线程运行
        /// </summary>
        public void Stop()
        {
            bStop = true;
        }

        /// <summary>
        /// 启动气象图片下载线程
        /// </summary>
        public void Start()
        {
            bStop = false;
            try
            {
                Console.WriteLine(bStop);
                Thread downloadThread = new Thread(DownloadProc);
                downloadThread.Start();
                IsRunning = true;
            }
            catch(Exception ex)
            {
                //PlatformMethodFactory.GetLogger().error(ex.Message);
                Console.WriteLine(ex.Message);
            }
        }

        //最后一次下载时间
        private DateTime lastDownloadTime = new DateTime();

        /// <summary>
        /// 下载图片函数
        /// </summary>
        private void DownloadProc()
        {
            while (true)
            {
                Console.WriteLine(".....................");
                if ((DateTime.Now - lastDownloadTime).TotalMinutes > 1)
                {
                    Console.WriteLine("List file...");
                    List<string> files = WeatherImageHelper.ListFile("ftp://192.168.0.100", "anonymous", "");

                    foreach (var file in files)
                    {
                        PictureType type = WeatherImageHelper.CheckType(file);
                        DateTime time = new DateTime();
                        switch (type)
                        {
                            case PictureType.FY2D:
                            case PictureType.FY2E:
                            case PictureType.FY2G:
                                time = WeatherImageHelper.GetFy2CloudPictureDateTime(file);
                                break;
                            case PictureType.DSLRadarPlainGraph:
                            case PictureType.DSLRadarVerticalSweep:
                            case PictureType.DSLRadarVelocitySpectrum:
                                time = WeatherImageHelper.GetDSLRadarPictureDateTime(file);
                                break;
                            case PictureType.WDRadarPlainGraph:
                            case PictureType.WDRadarVerticalSweep:
                                time = WeatherImageHelper.GetWDRadarPictureDateTime(file);
                                break;
                        }

                        if ((DateTime.Now - time).TotalHours < 2)
                        {
                            string url = "ftp://192.168.0.100" + "/" + file;
                            string localFile = "";
                            if (Environment.OSVersion.Platform == PlatformID.Unix
                                || Environment.OSVersion.Platform == PlatformID.MacOSX)
                            {
                                if (!Directory.Exists("/home/WeatherImages"))
                                {
                                    Directory.CreateDirectory("/home/WeatherImages");
                                }
                                localFile = "/home/WeatherImages" + "/" + file;
                            }
                            else
                            {
                                if (!Directory.Exists("D:\\WeatherImages"))
                                {
                                    Directory.CreateDirectory("D:\\WeatherImages");
                                }
                                localFile = "D:\\WeatherImages" + "/" + file;
                            }

                            Console.WriteLine("start download {0} ...", url);
                            WeatherImageHelper.DownloadFile(url, localFile, "anonymous", "");
                        }
                    }
                    //更新最后下载时间
                    lastDownloadTime = DateTime.Now;
                }

                Console.WriteLine("sleep");
                Thread.Sleep(1000);//休眠1秒
                Console.WriteLine("wake up");
                Console.WriteLine(bStop);
                if (bStop)
                {
                    Console.WriteLine("stopped");
                    IsRunning = false;
                    break;
                }
            }
        }

        //隐藏构造函数，避免从外部创建
        private WeartherImageDownloader()
        {

        }

        //唯一实例
        private static WeartherImageDownloader _Instance = null;
        //用于创建唯一实例的锁
        private static object lockObj = new object();

        /// <summary>
        /// 获取<seealso cref="WeartherImageDownloader"/>的唯一实例
        /// </summary>
        /// <returns></returns>
        public static WeartherImageDownloader GetInstance()
        {
            if (_Instance == null)
            {
                lock(lockObj)
                {
                    if (_Instance == null)
                        _Instance = new WeartherImageDownloader();
                }
            }

            return _Instance;
                
        }
    }
}
