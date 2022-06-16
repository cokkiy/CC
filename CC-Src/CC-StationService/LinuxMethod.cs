using System;
using System.Diagnostics;
using System.IO;
using System.Text.RegularExpressions;

namespace CC_StationService
{
    class LinuxMethod : IPlatformMethod
    {
        //总内存
        private static long totalMemory = 0;

        public long GetTotalMemory()
        {
            if (totalMemory != 0)
                return totalMemory;

            PerformanceCounter memoryCounter = new PerformanceCounter("Mono Memory", "Total Physical Memory");
            totalMemory =memoryCounter.RawValue;
            return totalMemory;
        }

        public void Reboot()
        {
            System.Diagnostics.Process.Start("shutdown", " -r 0");
        }

        public void Shutdown()
        {
            System.Diagnostics.Process.Start("shutdown", " -h 0");
        }

        /// <summary>
        /// 获取内存使用情况
        /// </summary>
        /// <returns></returns>
        public MemoryInfo GetMemoryInfo()
        {
            return MemoryUtil.GetMemoryInfo();
        }
    }

    /// <summary>
    /// Linux内存工具
    /// </summary>
    static class MemoryUtil
    {
        /// <summary>
        /// 获取Linux平台下的内存信息
        /// </summary>
        /// <returns></returns>
        public static MemoryInfo GetMemoryInfo()
        {
            MemoryInfo memInfo = new MemoryInfo();
            string[] memInfoLines = File.ReadAllLines(@"/proc/meminfo");
            MemoryInfoMatch[] infoMatches =
            {
                new MemoryInfoMatch(@"^Buffers:\s*(\d+)\s*(?i)(KB|MB)",(string value,int size)=>  memInfo.Buffers=Convert.ToInt64(value)*size ),
                new MemoryInfoMatch(@"^Cached:\s+(\d+)\s*(?i)(KB|MB)",(string value,int size)=>  memInfo.Cached=Convert.ToInt64(value)*size),
                new MemoryInfoMatch(@"^MemFree:\s+(\d+)\s*(?i)(KB|MB)",(string value,int size)=>  memInfo.Free=Convert.ToInt64(value)*size),
                new MemoryInfoMatch(@"^MemTotal:\s+(\d+)\s*(?i)(KB|MB)",(string value,int size)=>  memInfo.Total=Convert.ToInt64(value)*size),
            };

            foreach (string line in memInfoLines)
            {
                foreach (var infoMatch in infoMatches)
                {
                    Match match = infoMatch.Regex.Match(line);
                    if (match.Groups[1].Success)
                    {
                        string value = match.Groups[1].Value;
                        if (match.Groups[2].Success)
                        {
                            if (string.Compare(match.Groups[2].Value, "KB", true) == 0)
                            {
                                infoMatch.GetValue(value, 1024);
                            }
                            else
                            {
                                infoMatch.GetValue(value, 1024 * 1024);
                            }
                        }
                        else
                        {
                            infoMatch.GetValue(value, 1);
                        }

                    }
                }
            }

            return memInfo;
        }

        /// <summary>
        /// 内存文件信息匹配规则
        /// </summary>
        private class MemoryInfoMatch
        {

            public MemoryInfoMatch(string pattern, Func<string, int, long> action)
            {
                this.Regex = new Regex(pattern, RegexOptions.Compiled);
                this.GetValue = action;
            }

            /// <summary>
            /// 匹配模式
            /// </summary>
            public Regex Regex { get; private set; }
            /// <summary>
            /// 获取数值方法
            /// </summary>
            public Func<string, int, long> GetValue { get; private set; }

        }
    }

    /// <summary>
    /// 解决中标麒麟DNS bug，把IP地址加入到/etc/hosts文件中
    /// </summary>
    static class DnsFix
    {
        /// <summary>
        /// 把url字符串中的IP地址加入到/etc/hosts文件中
        /// </summary>
        /// <param name="url">ftp或http url</param>
        public static void AddHostToConfig(string url)
        {
            if (Uri.IsWellFormedUriString(url, UriKind.RelativeOrAbsolute))
            {
                Uri uri = new Uri(url);
                if (Environment.OSVersion.Platform == PlatformID.Unix
                     || Environment.OSVersion.Platform == PlatformID.MacOSX)
                {
                    bool alreadyAdded = false;
                    try
                    {
                        using (TextReader reader = new StreamReader("/etc/hosts"))
                        {
                            string entry = reader.ReadLine();
                            while (entry != null)
                            {
                                Console.WriteLine(entry);
                                int pos = entry.IndexOf(' ');
                                if (pos != -1)
                                {
                                    string host = entry.Substring(0, pos);
                                    if (uri.DnsSafeHost == host)
                                    {
                                        alreadyAdded = true;
                                        break;
                                    }
                                }
                                entry = reader.ReadLine();
                            }
                            reader.Close();
                        }
                    }
                    catch (System.Exception ex)
                    {
                        Console.WriteLine(ex.Message);
                    }

                    if (!alreadyAdded)
                    {
                        try
                        {
                            using (TextWriter writer = new StreamWriter("/etc/hosts", true))
                            {
                                writer.WriteLine("{0} {0}", uri.DnsSafeHost);
                                writer.Close();
                            }
                        }
                        catch (System.Exception ex)
                        {
                            Console.WriteLine(ex.Message);
                        }
                    }
                }
            }
        }

    }
}
