using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StationLogModels
{
    /// <summary>
    /// 系统状态信息
    /// </summary>
    public class StationState
    {
        public int Id { get; set; }

        public int StationId { get; set; }

        public string OSName { get; set; }

        public string OSVersion { get; set; }

        public long TotalMemory { get; set; }

        [MaxLength(50)]
        public string ComputerName { get; set; }

        public DateTime UpdateTime { get; set; }        
    }
}
