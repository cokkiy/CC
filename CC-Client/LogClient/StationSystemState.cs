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
            StringBuilder builder = new StringBuilder();
            builder.AppendFormat("{0} {1} {2} {3} TotalMemory:{4}",
                computerName, stationId, osName, osVersion, totalMemory);
            builder.AppendLine();
            foreach (var ni in NetworkInterfaces)
            {
                builder.AppendFormat("{0}:", ni.Key);
                builder.AppendLine();
                ni.Value.ForEach(ip =>
                {
                    builder.AppendFormat("{0}", ip);
                    builder.AppendLine();
                });    
            }
            return builder.ToString(); ;
        }
    }
}
