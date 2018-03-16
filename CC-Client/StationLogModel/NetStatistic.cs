using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Text;

namespace StationLogModels
{
    public class NetStatistic
    {
        [Key]
        public long Id { get; set; }

        [MaxLength(50)]
        public string ComputerName { get; set; }

        public long DatagramsReceived { get => datagramsReceived; set => datagramsReceived = value; }
        public long DatagramsSent { get => datagramsSent; set => datagramsSent = value; }
        public long DatagramsDiscarded { get => datagramsDiscarded; set => datagramsDiscarded = value; }
        public long DatagramsWithErrors { get => datagramsWithErrors; set => datagramsWithErrors = value; }
        public int UDPListeners { get => uDPListeners; set => uDPListeners = value; }
        public long SegmentsReceived { get => segmentsReceived; set => segmentsReceived = value; }
        public long SegmentsSent { get => segmentsSent; set => segmentsSent = value; }
        public long ErrorsReceived { get => errorsReceived; set => errorsReceived = value; }
        public long CurrentConnections { get => currentConnections; set => currentConnections = value; }
        public long ResetConnections { get => resetConnections; set => resetConnections = value; }

        public DateTime RecordTime { get; set; }


        #region Fields

        private long datagramsReceived;
        private long datagramsSent;
        private long datagramsDiscarded;
        private long datagramsWithErrors;
        private int uDPListeners;
        private long segmentsReceived;
        private long segmentsSent;
        private long errorsReceived;
        private long currentConnections;
        private long resetConnections;

        #endregion


        public NetStatistic()
        {

        }

        public NetStatistic(string computerName, string v1, string v2, string v3, string v4, string v5, string v6, string v7, string v8, string v9, string v10)
        {
            ComputerName = computerName;
            DatagramsReceived = long.Parse(v1);
            DatagramsSent = long.Parse(v2);
            DatagramsDiscarded = long.Parse(v3);
            DatagramsWithErrors = long.Parse(v4);
            UDPListeners = int.Parse(v5);
            SegmentsReceived = long.Parse(v6);
            SegmentsSent = long.Parse(v7);
            ErrorsReceived = long.Parse(v8);
            CurrentConnections = long.Parse(v9);
            ResetConnections = long.Parse(v10);
            RecordTime = DateTime.Now;
        }
    }
}
