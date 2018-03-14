using System;
using System.ComponentModel.DataAnnotations;

namespace StationLogModels
{
    public class Station
    {
        [Key]
        public int Id { get; set; }
        public string ComputerName { get; set; }
        public string UserName { get; set; }
        public string GroupName { get; set; }

        /// <summary>
        /// 所在网络名，比如办公网、开发网、数据网等
        /// </summary>
        public string NetworkName { get; set; }
    }
}
