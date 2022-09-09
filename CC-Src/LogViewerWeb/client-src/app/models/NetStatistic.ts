
module Models {
    
    export class NetStatistic {
        
            public id: number;
            public computerName: string;
            public datagramsReceived: number;
            public datagramsSent: number;
            public datagramsDiscarded: number;
            public datagramsWithErrors: number;
            public udpListeners: number;
            public segmentsReceived: number;
            public segmentsSent: number;
            public errorsReceived: number;
            public currentConnections: number;
            public resetConnections: number;
            public recordTime: Date;
    }
    
    
    
}