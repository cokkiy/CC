using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Text;

namespace StationLogModels
{
    public class IFStatistic
    {
        [Key]
        public long Id { get; set; }

        [MaxLength(50)]
        public string ComputerName { get; set; }

        [MaxLength(255)]
        public string IfName { get; set; }
        public float BytesReceivedPerSec { get => bytesReceivedPerSec; set => bytesReceivedPerSec = value; }
        public float BytesSentedPerSec { get => bytesSentedPerSec; set => bytesSentedPerSec = value; }
        public float TotalBytesPerSec { get => totalBytesPerSec; set => totalBytesPerSec = value; }
        public long BytesReceived { get => bytesReceived; set => bytesReceived = value; }
        public long BytesSented { get => bytesSented; set => bytesSented = value; }
        public long BytesTotal { get => bytesTotal; set => bytesTotal = value; }
        public long UnicastPacketReceived { get => unicastPacketReceived; set => unicastPacketReceived = value; }
        public long UnicastPacketSented { get => unicastPacketSented; set => unicastPacketSented = value; }
        public long MulticastPacketReceived { get => multicastPacketReceived; set => multicastPacketReceived = value; }
        public long MulticastPacketSented { get => multicastPacketSented; set => multicastPacketSented = value; }

        public DateTime RecordTime { get; set; }

        #region Fields
        private float bytesReceivedPerSec;
        private float bytesSentedPerSec;
        private float totalBytesPerSec;
        private long bytesReceived;
        private long bytesSented;
        private long bytesTotal;
        private long unicastPacketReceived;
        private long unicastPacketSented;
        private long multicastPacketReceived;
        private long multicastPacketSented;

        #endregion

        public IFStatistic(string computerName, string ifName, string v2, string v3, string v4, string v5, string v6, string v7, string v8, string v9, string v10, string v11)
        {
            ComputerName = computerName;
            IfName = ifName;
            this.bytesReceivedPerSec = float.Parse(v2);
            this.bytesSentedPerSec = float.Parse(v3);
            this.totalBytesPerSec = float.Parse(v4);
            this.bytesReceived = long.Parse(v5);
            this.bytesSented = long.Parse(v6);
            this.bytesTotal = long.Parse(v7);
            this.unicastPacketReceived = long.Parse(v8);
            this.unicastPacketSented = long.Parse(v9);
            this.multicastPacketReceived = long.Parse(v10);
            this.multicastPacketSented = long.Parse(v11);
            RecordTime = DateTime.Now;
        }
        
    }
}
