using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Text;

namespace StationLogModels
{
    /// <summary>
    /// 按小时统计的开机数量
    /// </summary>
    public class HourlyPowerOnStatistic
    {
        [Key]
        public long Id { get; set; }
        /// <summary>
        /// 统计时间-天
        /// </summary>
        public DateTime Day { get; set; }

        /// <summary>
        /// 统计时间-小时（0-23）
        /// </summary>
        [Range(0,23)]
        public int Hour { get; set; }

        /// <summary>
        /// 专业组名称
        /// </summary>
        [MaxLength(20)]
        public string GroupName { get; set; }

        public int Amount { get; set; }
    }
}
