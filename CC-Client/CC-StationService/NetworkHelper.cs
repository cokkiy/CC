using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CC_StationService
{
    public static class NetworkHelper
    {
        public static string GetString(this CC.Statistics statistics)
        {
            int count = statistics.IfStatistics.Count;
            StringBuilder builder = new StringBuilder(
                string.Format("{0}:{1}:{2}:{3}:{4}:{5}:{6}:{7}:{8}:{9}:{10}",
                statistics.StationId,
                statistics.DatagramsReceived,
                statistics.DatagramsSent,
                statistics.DatagramsDiscarded,
                statistics.DatagramsWithErrors,
                statistics.UDPListeners,
                statistics.SegmentsReceived,
                statistics.SegmentsSent,
                statistics.ErrorsReceived,
                statistics.CurrentConnections,
                statistics.ResetConnections
                ));
            foreach (var ifData in statistics.IfStatistics)
            {
                var ifDtatString = GetString(ifData);
                builder.AppendFormat(":{0}", ifDtatString);
            }

            return builder.ToString();
        }

        private static string GetString(CC.InterfaceStatistics ifStatistics)
        {
            return string.Format("{0}:{1}:{2}:{3}:{4}:{5}:{6}:{7}:{8}:{9}:{10}",
                ifStatistics.IfName,
                ifStatistics.BytesReceivedPerSec,
                ifStatistics.BytesSentedPerSec,
                ifStatistics.TotalBytesPerSec,
                ifStatistics.BytesReceived,
                ifStatistics.BytesSented,
                ifStatistics.BytesTotal,
                ifStatistics.UnicastPacketReceived,
                ifStatistics.UnicastPacketSented,
                ifStatistics.MulticastPacketReceived,
                ifStatistics.MulticastPacketSented);
        }
    }
}
