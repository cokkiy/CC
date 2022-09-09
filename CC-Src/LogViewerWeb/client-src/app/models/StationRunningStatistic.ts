
module Models {
    
    
    export class StationRunningStatistic {
        
            public count: number;
            public year: number;
            public month: number;
            public week: number;
            public day: number;
            public hour: number;
            public quarter: number;
            public timeType: TimeType;
            public time: string;
    }
    
    export enum TimeType {
        
            Hourly,
            Daily,
            Weekly,
            Monthly,
            Quarterly,
            Yearly,
    }
    
}