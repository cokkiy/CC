using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Text;

namespace StationLogModels
{
    /// <summary>
    /// 程序运行时间统计
    /// </summary>
    public class AppRunningTime
    {
        [Key]
        public long Id { get; set; }

        /// <summary>
        /// 应用程序名称
        /// </summary>
        public string AppName { get; set; }

        /// <summary>
        /// 以分钟统计的运行时间
        /// </summary>
        public long RunningTime { get; set; }


        /// <summary>
        /// 组名
        /// </summary>
        public string GroupName { get; set; }

        /// <summary>
        /// 统计时间
        /// </summary>
        public DateTime UpdateTime { get; set; }

    }
}
