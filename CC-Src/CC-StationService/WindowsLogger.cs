using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Ice;
using System.Diagnostics;

namespace CC_StationService
{
    /// <summary>
    /// Windows 系统下日志实现
    /// </summary>
    class WindowsLogger : Ice.Logger
    {
        /// <summary>
        /// 创建一个Windows系统下的日志记录器
        /// </summary>
        public WindowsLogger()
        {
            if (!EventLog.SourceExists("CCService"))
            {
                EventLog.CreateEventSource("CCService", "Application");
            }            
        }
        public Logger cloneWithPrefix(string prefix)
        {
            return this;
        }

        public void error(string message)
        {
            var log = new EventLog();
            log.Source = "CCService";
            log.WriteEntry(message, EventLogEntryType.Error);
            log.Close();
        }

        public string getPrefix()
        {
            return "";
        }

        public void print(string message)
        {
            var log = new EventLog();
            log.Source = "CCService";
            log.WriteEntry(message, EventLogEntryType.Information);
            log.Close();
        }

        public void trace(string category, string message)
        {
            var log = new EventLog();
            log.Source = "CCService";
            log.WriteEntry(message, EventLogEntryType.Information);
            log.Close();
        }

        public void warning(string message)
        {
            var log = new EventLog();
            log.Source = "CCService";
            log.WriteEntry(message, EventLogEntryType.Warning);
            log.Close();
        }
    }
}
