using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CC
{
    public partial class StationSystemState
    {
        public override string ToString()
        {
            return String.Format("{0} {1} {2} {3} TotalMemory:{4}", computerName,stationId, osName, osVersion, totalMemory); ;
        }
    }
}
