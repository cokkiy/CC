using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace LogViewerWeb.Models.ViewModels
{
    public class ProcessStartAndExitMessage
    {
        public string ComputerName { get; set; }
        public string ProcessName { get; set; }
        public bool IsExit { get; set; }
        public DateTime ActionTime { get; set; }
    }
}
