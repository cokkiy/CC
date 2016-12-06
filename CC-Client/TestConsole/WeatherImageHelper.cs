using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace TestConsole
{
    /// <summary>
    /// 气象云图辅助类
    /// </summary>
    public static class WeatherImageHelper
    {
        /// <summary>
        /// 列出ftp服务器上的所有文件
        /// </summary>  
        /// <param name="url">服务器地址</param>
        /// <param name="username">ftp登录用户名</param>
        /// <param name="password">ftp登录密码</param>
        /// <returns>ftp服务器上的所有文件列表</returns>
        public static List<string> ListFile(string url, string username = "anonymous", string password = "")
        {
            try
            {
                return ListFile(new Uri(url), username, password);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                Console.WriteLine(ex.ToString());
                return new List<string>();
            }
        }

        /// <summary>
        /// 列出ftp服务器上的所有文件
        /// </summary>
        /// <param name="uri">ftp服务器地址</param>
        /// <param name="userName">ftp登录用户名</param>
        /// <param name="password">ftp登录密码</param>
        /// <returns>ftp服务器上的所有文件列表</returns>
        public static List<string> ListFile(Uri uri, string userName = "anonymous", string password = "")
        {
            List<string> files = new List<string>();
            if (uri.Scheme == Uri.UriSchemeFtp)
            {
                FtpWebRequest request = (FtpWebRequest)WebRequest.Create(uri);
                request.Method = WebRequestMethods.Ftp.ListDirectory;
                request.Credentials = new NetworkCredential(userName, password);

                // The following streams are used to read the data returned from the server.
                FtpWebResponse response = null;
                Stream responseStream = null;
                StreamReader readStream = null;

                try
                {
                    response = (FtpWebResponse)request.GetResponse();
                    responseStream = response.GetResponseStream();
                    readStream = new StreamReader(responseStream, System.Text.Encoding.UTF8);

                    if (readStream != null)
                    {
                        files.AddRange(readStream.ReadToEnd().Split(new char[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries));
                    }
                    //Console.WriteLine("List status: {0}", response.StatusDescription);
                }
                catch(Exception ex)
                {
                    Console.WriteLine(ex.Message);
                    Console.WriteLine(ex.ToString());
                }
                finally
                {
                    if (readStream != null)
                    {
                        readStream.Close();
                    }
                    if (response != null)
                    {
                        response.Close();
                    }
                }
            }

            return files;
        }

        /// <summary>
        /// 检测指定文件的类型（气象图类型）
        /// </summary>
        /// <param name="filename">文件名</param>
        /// <returns>气象图类型</returns>
        public static PictureType CheckType(string filename)
        {
            string fy2DPattern = @"FY2D_(?<year>20\d{2})_(?<month>\d{2})_(?<day>\d{2})_(?<hour>\d{2})_(?<min>\d{2})(_.+)*\.JPG";
            string fy2EPattern = @"FY2E_(?<year>20\d{2})_(?<month>\d{2})_(?<day>\d{2})_(?<hour>\d{2})_(?<min>\d{2})(_.+)*\.JPG";
            string fy2GPattern = @"FY2G_(?<year>20\d{2})_(?<month>\d{2})_(?<day>\d{2})_(?<hour>\d{2})_(?<min>\d{2})(_.+)*\.JPG";
            string wdPlainGraphPattern = @"\d{8}AP\d+\.bmp"; ;
            string wdVerticalSweepPattern = @"\d{8}AR\d+\.bmp";
            string dslPlainGraphPattern = @"P\d{12}\.bmp"; ;
            string dslVerticalSweepPattern = @"R\d{12}\.bmp";
            string dslVelocitySpectrumPattern = @"S\d{12}\.bmp";
            if (Regex.IsMatch(filename, fy2DPattern, RegexOptions.IgnoreCase))
                return PictureType.FY2D;
            if (Regex.IsMatch(filename, fy2EPattern, RegexOptions.IgnoreCase))
                return PictureType.FY2E;
            if (Regex.IsMatch(filename, fy2GPattern, RegexOptions.IgnoreCase))
                return PictureType.FY2G;
            if (Regex.IsMatch(filename, wdPlainGraphPattern, RegexOptions.IgnoreCase))
                return PictureType.WDRadarPlainGraph;
            if (Regex.IsMatch(filename, wdVerticalSweepPattern, RegexOptions.IgnoreCase))
                return PictureType.WDRadarVerticalSweep;
            if (Regex.IsMatch(filename, dslPlainGraphPattern, RegexOptions.IgnoreCase))
                return PictureType.DSLRadarPlainGraph;
            if (Regex.IsMatch(filename, dslVerticalSweepPattern, RegexOptions.IgnoreCase))
                return PictureType.DSLRadarVerticalSweep;
            if (Regex.IsMatch(filename, dslVelocitySpectrumPattern, RegexOptions.IgnoreCase))
                return PictureType.DSLRadarVelocitySpectrum;

            return PictureType.Unknown;

        }

        /// <summary>
        /// 获取风云2卫星图时间
        /// </summary>
        /// <param name="filename">风云2卫星图文件名</param>
        /// <returns>风云2卫星图时间</returns>
        public static DateTime GetFy2CloudPictureDateTime(string filename)
        {
            string pattern = @"FY2[DEG]_(?<year>20\d{2})_(?<month>\d{2})_(?<day>\d{2})_(?<hour>\d{2})_(?<min>\d{2})(_.+)*\.JPG";

            Match match = Regex.Match(filename, pattern, RegexOptions.IgnoreCase);
            if (match.Success)
            {
                int year = int.Parse(match.Groups["year"].Value);
                int month = int.Parse(match.Groups["month"].Value);
                int day = int.Parse(match.Groups["day"].Value);
                int hour = int.Parse(match.Groups["hour"].Value);
                int min = int.Parse(match.Groups["min"].Value);
                return new DateTime(year, month, day, hour, min, 0);
            }

            return new DateTime();
        }


        /// <summary>
        /// 获取气象室（Weather Department）雷达图像时间
        /// </summary>
        /// <param name="filename">气象室雷达图像文件名</param>
        /// <returns>气象室雷达图像时间</returns>
        public static DateTime GetWDRadarPictureDateTime(string filename)
        {
            // 07021555AP6.bmp
            string pattern = @"(?<month>\d{2})(?<day>\d{2})(?<hour>\d{2})(?<min>\d{2})(.)*\.bmp";
            Match match = Regex.Match(filename, pattern, RegexOptions.IgnoreCase);
            if (match.Success)
            {
                int month = int.Parse(match.Groups["month"].Value);
                int day = int.Parse(match.Groups["day"].Value);
                int hour = int.Parse(match.Groups["hour"].Value);
                int min = int.Parse(match.Groups["min"].Value);
                int year = DateTime.Now.Year;
                if (month > DateTime.Now.Month) //解决跨年问题，图片是12月份的，当前是1月份
                {
                    year = DateTime.Now.Year - 1;
                }
                return new DateTime(year, month, day, hour, min, 0);
            }

            return new DateTime();
        }

        /// <summary>
        /// 获取大树里气象雷达图片时间信息
        /// </summary>
        /// <param name="filename">大树里气象雷达图片文件名</param>
        /// <returns>大树里气象雷达图片时间信息</returns>
        public static DateTime GetDSLRadarPictureDateTime(string filename)
        {
            // R160624162132.bmp P160624162132.bmp S160624162132.bmp
            string pattern = @"[RPS](?<year>\d{2})(?<month>\d{2})(?<day>\d{2})(?<hour>\d{2})(?<min>\d{2})(?<second>\d{2})\.bmp";
            Match match = Regex.Match(filename, pattern, RegexOptions.IgnoreCase);
            if (match.Success)
            {
                int year = 2000 + int.Parse(match.Groups["year"].Value);
                int month = int.Parse(match.Groups["month"].Value);
                int day = int.Parse(match.Groups["day"].Value);
                int hour = int.Parse(match.Groups["hour"].Value);
                int min = int.Parse(match.Groups["min"].Value);
                int second = int.Parse(match.Groups["second"].Value);
                return new DateTime(year, month, day, hour, min, second);
            }

            return new DateTime();
        }


        /// <summary>
        /// 下载<paramref name="url"/>指定文件，并保存到本地文件<paramref name="localFileName"/>中
        /// </summary>
        /// <param name="url">要下载的文件地址</param>
        /// <param name="localFileName">本地文件名</param>
        /// <returns>下载文件是否成功</returns>
        public static bool DownloadFile(string url, string localFileName, string userName = "anonymous", string password = "")
        {
            bool result = false;
            FtpWebRequest request = (FtpWebRequest)WebRequest.Create(url);
            request.Method = WebRequestMethods.Ftp.DownloadFile;
            request.Credentials = new NetworkCredential(userName, password);
            FtpWebResponse response = (FtpWebResponse)request.GetResponse();

            Stream responseStream = null;
            BinaryReader readStream = null;
            BinaryWriter writeStream = null;
            try
            {
                responseStream = response.GetResponseStream();
                readStream = new BinaryReader(responseStream);
                // Display information about the data received from the server.
                Console.WriteLine("Bytes received: {0}", response.ContentLength);

                Console.WriteLine("Message from server: {0}", response.StatusDescription);
                Console.WriteLine("Resource: {0}", response.ResponseUri);

                // Write the bytes received from the server to the local file. 
                if (readStream != null)
                {
                    writeStream = new BinaryWriter(File.Open(localFileName, FileMode.Create));
                    byte[] buf = readStream.ReadBytes(1024);
                    while (buf.Length > 0)
                    {
                        writeStream.Write(buf);
                        if (buf.Length == 1024)
                        {
                            buf = readStream.ReadBytes(1024);
                        }
                        else
                        {
                            break;
                        }
                    }

                    result = true;
                }
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
                Console.WriteLine(ex.ToString());
            }
            finally
            {
                if (readStream != null)
                {
                    readStream.Close();
                }
                if (response != null)
                {
                    response.Close();
                }
                if (writeStream != null)
                {
                    writeStream.Close();
                }
            }

            return result;
        }
    }
}
