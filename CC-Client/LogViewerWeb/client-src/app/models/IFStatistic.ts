
module Models {
    
    export class IFStatistic {
        
            public id: number;
            public computerName: string;
            public ifName: string;
            public bytesReceivedPerSec: number;
            public bytesSentedPerSec: number;
            public totalBytesPerSec: number;
            public bytesReceived: number;
            public bytesSented: number;
            public bytesTotal: number;
            public unicastPacketReceived: number;
            public unicastPacketSented: number;
            public multicastPacketReceived: number;
            public multicastPacketSented: number;
            public recordTime: Date;
    }
    
    
    
}