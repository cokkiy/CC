using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Text;

namespace StationLogModels
{
    public class ProcessInfo
    {
        public long Id { get; set; }

        [MaxLength(50)]
        public string ComputerName { get; set; }


        [MaxLength(200)]
        public string ProcessName { get; set; }

        /// <summary>
        /// 启动时间
        /// </summary>
        public DateTime StartTime { get; set; }


        /// <summary>
        /// 退出时间
        /// </summary>
        public DateTime? ExitTime { get; set; }
    }
}
