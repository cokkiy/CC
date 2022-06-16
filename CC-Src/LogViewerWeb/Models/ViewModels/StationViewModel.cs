using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace LogViewerWeb.Models.ViewModels
{
    public class StationViewModel
    {
        public string ComputerName { get; set; }
        public string UserName { get; set; }
        public string GroupName { get; set; }
        public DateTime? PowerOn { get; set; }
        public string OsName { get; set; }
        public int TotalMemory { get; set; }

        public bool IsRunning { get; set; }
    }
}
