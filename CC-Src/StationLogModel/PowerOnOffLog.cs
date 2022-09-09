using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Text;

namespace StationLogModels
{
    /// <summary>
    /// 开关机记录
    /// </summary>
    public class PowerOnOffLog
    {
        [Key]
        public long Id { get; set; }
        [MaxLength(50)]
        public string ComputerName { get; set; }
        public DateTime PowerOn { get; set; }
        public DateTime? PowerOff { get; set; }
    }
}
