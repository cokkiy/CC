using CC;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;

namespace CC_StationService
{
    /// <summary>
    /// 气象云图下载器
    /// </summary>
    public class WeartherImageDownloader
    {
        /// <summary>
        /// 获取或设置气象云图下载选项
        /// </summary>
        public WeatherPictureDowlnloadOption Option { get; set; }

        /// <summary>
        /// 获取或设置日志记录器
        /// </summary>
        public Ice.Logger Logger { get; set; }

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
                Thread downloadThread = new Thread(DownloadProc);
                downloadThread.Start();
                IsRunning = true;
            }
            catch(Exception ex)
            {
                if (Logger != null)
                {
                    Logger.error(ex.Message);
                }
            }
        }

        //最后一次下载时间
        private DateTime lastDownloadTime = new DateTime();

        //列出并下载指定url下符合条件的所有文件
        private void ListAndDownloadFile(string url)
        {
            List<string> files = WeatherImageHelper.ListFile(url, Option.UserName, Option.Password);
            if(Logger!=null)
            {
                Logger.print(string.Format("文件总数: {0}", files.Count));
            }

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

                if ((DateTime.Now - time).TotalHours < Option.LastHours)
                {
                    string fileUrl = url + "/" + file;
                    string localFile = "";
                    string localName = WeatherImageHelper.GetLocalFileName(file, time, type);
                    if (Environment.OSVersion.Platform == PlatformID.Unix
                        || Environment.OSVersion.Platform == PlatformID.MacOSX)
                    {
                        if (!Directory.Exists(Option.SavePathForLinux))
                        {
                            Directory.CreateDirectory(Option.SavePathForLinux);
                        }
                        localFile = Option.SavePathForLinux + "/" + localName;
                    }
                    else
                    {
                        if (!Directory.Exists(Option.SavePathForWindows))
                        {
                            Directory.CreateDirectory(Option.SavePathForWindows);
                        }
                        localFile = Option.SavePathForWindows + "/" + localName;
                    }
                    if (!File.Exists(localFile))
                    {
                        WeatherImageHelper.DownloadFile(fileUrl, localFile, Option.UserName, Option.Password);
                    }
                }
            }
        }

        //删除旧文件
        private void DeleteOldFiles()
        {
            if (Option.DeletePreviousFiles)
            {
                Regex regex = new Regex("\\w{4,5}(?<year>\\d{4})(?<month>\\d{2})(?<day>\\d{2})-(?<hour>\\d{2})(?<min>\\d{2})(?<second>\\d{2})\\.(jpg|bmp)", RegexOptions.IgnoreCase);

                string localDir = Option.SavePathForWindows;
                if (Environment.OSVersion.Platform == PlatformID.Unix
                || Environment.OSVersion.Platform == PlatformID.MacOSX)
                {
                    localDir = Option.SavePathForLinux;
                }

                if (Directory.Exists(localDir))
                {
                    try
                    {
                        foreach (var file in Directory.EnumerateFiles(localDir))
                        {
                            string fileName = file.Substring(localDir.Length + 1);
                            Match match = regex.Match(fileName);
                            if (match.Success)
                            {
                                int year = int.Parse(match.Groups["year"].Value);
                                int month = int.Parse(match.Groups["month"].Value);
                                int day = int.Parse(match.Groups["day"].Value);
                                int hour = int.Parse(match.Groups["hour"].Value);
                                int min = int.Parse(match.Groups["min"].Value);
                                int second = int.Parse(match.Groups["second"].Value);
                                DateTime fileTime = new DateTime(year, month, day, hour, min, second);
                                if ((DateTime.Now - fileTime).TotalHours > Option.DeleteHowHoursAgo)
                                {
                                    try
                                    {
                                        File.Delete(file);
                                    }
                                    catch(System.Exception ex)
                                    {
                                        PlatformMethodFactory.GetLogger().error(ex.Message);
                                        PlatformMethodFactory.GetLogger().error(ex.ToString());
                                    }
                                }
                            }
                        }
                    }
                    catch
                    {

                    }
                }
            }
        }

        /// <summary>
        /// 下载图片函数
        /// </summary>
        private void DownloadProc()
        {
            while (true)
            {
                if ((DateTime.Now - lastDownloadTime).TotalMinutes > Option.Interval)
                {
                    if(Logger!=null)
                        Logger.print(string.Format("开始下载 {0} 下气象云图...",Option.Url));
                    //首先下载根目录下的所有文件
                    ListAndDownloadFile(Option.Url);

                    //下载子目录下的所有文件
                    string[] subDirs = Option.SubDirectory.Split(new char[] { ';', '；' }, StringSplitOptions.RemoveEmptyEntries);
                    foreach (var dir in subDirs)
                    {
                        string url = Option.Url;
                        if (url.EndsWith("/"))
                        {
                            url = url.TrimEnd('/');
                        }
                        url += "/" + dir;
                        if(!url.EndsWith("/"))
                        {
                            url += "/";
                        }
                        if (Logger != null)
                            Logger.print(string.Format("开始下载 {0} 下气象云图...", url));
                        ListAndDownloadFile(url);
                    }

                    //删除过期文件
                    DeleteOldFiles();


                    //更新最后下载时间
                    lastDownloadTime = DateTime.Now;
                }
                Thread.Sleep(1000);//休眠1秒
                if (bStop)
                {
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
