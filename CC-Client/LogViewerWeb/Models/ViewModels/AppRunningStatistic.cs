using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace LogViewerWeb.Models.ViewModels
{
    public class AppRunningStatistic
    {
        /// <summary>
        /// 程序名称
        /// </summary>
        public string AppName { get; set; }

        /// <summary>
        /// 运行时间（秒）
        /// </summary>
        public long RunningTimes { get; set; }

        /// <summary>
        /// 运行次数
        /// </summary>
        public long RunningCount { get; set; }
    }
}
