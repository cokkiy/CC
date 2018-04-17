using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Text;

namespace StationLogModels
{
    public class RunningState
    {
        [Key]
        public long Id { get; set; }

        [MaxLength(50)]
        public string ComputerName { get; set; }
        public long CurrentMemory { get; set; }
        public float MemoryPercent { get; set; }
        public int ProcessCount { get; set; }
        public float CPU { get; set; }
        public DateTime RecordTime { get; set; }
    }
}
