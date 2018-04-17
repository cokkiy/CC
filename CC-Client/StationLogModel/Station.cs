using System;
using System.ComponentModel.DataAnnotations;

namespace StationLogModels
{
    public class Station
    {
        [Key]
        public int Id { get; set; }

        [MaxLength(50)]
        public string ComputerName { get; set; }
        public string UserName { get; set; }
        public string GroupName { get; set; }

        /// <summary>
        /// 所在网络名，比如办公网、开发网、数据网等
        /// </summary>
        public string NetworkName { get; set; }
        public bool IsRunning { get; set; }

        /// <summary>
        /// 开机时间
        /// </summary>
        public DateTime? PowerOnTime { get; set; }

        /// <summary>
        /// 关机时间
        /// </summary>
        public DateTime? ShutdownTime { get; set; }
    }
}
