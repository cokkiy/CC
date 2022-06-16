using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Ice;
using System.IO;

namespace CC_StationService
{
    /// <summary>
    /// 实现Linux下日志功能
    /// </summary>
    class LinuxLogger : Ice.Logger
    {
        /// <summary>
        /// 日志文件名称
        /// </summary>
        string logFileName = string.Empty;

        //temp obj for lock
        object lockObj = new object();

        /// <summary>
        /// Create a logger object by specified log file name
        /// </summary>
        public LinuxLogger()
        {
            string path = "/var/log/CCService";
            if (!Directory.Exists(path))
            {
                Directory.CreateDirectory(path);
            }
            logFileName = string.Format("{0}/{1}.log", path, DateTime.Now.ToString("yyyy-MM-dd.HHmmss.fff"));
        }
        public Ice.Logger cloneWithPrefix(string prefix)
        {
            return this;
        }

        public void error(string message)
        {
            try
            {
                lock(lockObj)
                {
                    using (var writer = new StreamWriter(logFileName, true, Encoding.UTF8))
                    {
                        writer.WriteLine("{0} Error:{1}", DateTime.Now, message);
                        writer.Close();
                    }
                }
            }
            catch (System.Exception ex)
            {
                Console.Error.WriteLine(ex);
            }

            Console.WriteLine(message);

        }

        public string getPrefix()
        {
            return "";
        }

        public void print(string message)
        {
            try
            {
                lock(lockObj)
                {
                    using (var writer = new StreamWriter(logFileName, true, Encoding.UTF8))
                    {
                        writer.WriteLine("{0} {1}", DateTime.Now, message);
                        writer.Close();
                    }
                }
            }
            catch (System.Exception ex)
            {
                Console.Error.WriteLine(ex);
            }

            Console.WriteLine(message);
        }

        public void trace(string category, string message)
        {
            try
            {
                lock(lockObj)
                {
                    using (var writer = new StreamWriter(logFileName, true, Encoding.UTF8))
                    {
                        writer.WriteLine("{0} Trace Category:{1} Message:{2}", DateTime.Now, category, message);
                        writer.Close();
                    }
                }
            }
            catch (System.Exception ex)
            {
                Console.Error.WriteLine(ex);
            }
        }

        public void warning(string message)
        {
            try
            {
                lock(lockObj)
                {
                    using (var writer = new StreamWriter(logFileName, true, Encoding.UTF8))
                    {
                        writer.WriteLine("{0} Warning:{1}", DateTime.Now, message);
                        writer.Close();
                    }
                }
            }
            catch(System.Exception ex)
            {
                Console.Error.WriteLine(ex);
            }
            Console.WriteLine(message);
        }
    }
}
