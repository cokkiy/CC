using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Text;

namespace StationLogModels
{
    public class StationNI
    {
        [Key]
        public int Id { get; set; }

        [MaxLength(50)]
        public string ComputerName { get; set; }

        /// <summary>
        /// 网卡名称
        /// </summary>
        [MaxLength(255)]
        public string IfName { get; set; }

        /// <summary>
        /// IPv4地址
        /// </summary>
        [MaxLength(36)]
        public string IP { get; set; }


        /// <summary>
        /// MAC地址
        /// </summary>
        [StringLength(23)]
        public string MAC { get; set; }
        public DateTime Update { get; set; }
    }
}
