using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace CCMonitor
{
    static class Logger
    {
        /// <summary>
        /// 日志文件名称
        /// </summary>
        static string logFileName = string.Empty;

        //temp obj for lock
        static object lockObj = new object();

        static Logger()
        {
            string path = "/var/log/CCService";
            if (!Directory.Exists(path))
            {
                Directory.CreateDirectory(path);
            }
            logFileName = string.Format("{0}/monitor_{1}.log", path, DateTime.Now.ToString("yyyy-MM-dd.HHmmss.fff"));
        }
        public static void  Error(string message)
        {
            Console.Error.WriteLine(message);
            try
            {
                lock (lockObj)
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
        }

        public static void Message(string message)
        {
            Console.WriteLine(message);
            try
            {
                lock (lockObj)
                {
                    using (var writer = new StreamWriter(logFileName, true, Encoding.UTF8))
                    {
                        writer.WriteLine("{0} Info:{1}", DateTime.Now, message);
                        writer.Close();
                    }
                }
            }
            catch (System.Exception ex)
            {
                Console.Error.WriteLine(ex);
            }

        }

        internal static void Error(Exception ex)
        {
            string message = ex.ToString();
            Error(message);
        }

        internal static void Message(string format, params object[] args)
        {
            string message = string.Format(format, args);
            Message(message);
        }
    }
}
