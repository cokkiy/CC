namespace LogClient.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class AddNetStatisticAndIndex : DbMigration
    {
        public override void Up()
        {
            CreateTable(
                "dbo.IFStatistics",
                c => new
                    {
                        Id = c.Long(nullable: false, identity: true),
                        ComputerName = c.String(maxLength: 50, storeType: "nvarchar"),
                        IfName = c.String(maxLength: 255, storeType: "nvarchar"),
                        BytesReceivedPerSec = c.Single(nullable: false),
                        BytesSentedPerSec = c.Single(nullable: false),
                        TotalBytesPerSec = c.Single(nullable: false),
                        BytesReceived = c.Long(nullable: false),
                        BytesSented = c.Long(nullable: false),
                        BytesTotal = c.Long(nullable: false),
                        UnicastPacketReceived = c.Long(nullable: false),
                        UnicastPacketSented = c.Long(nullable: false),
                        MulticastPacketReceived = c.Long(nullable: false),
                        MulticastPacketSented = c.Long(nullable: false),
                        RecordTime = c.DateTime(nullable: false, precision: 0),
                    })
                .PrimaryKey(t => t.Id)
                .Index(t => t.RecordTime);
            
            CreateTable(
                "dbo.NetStatistics",
                c => new
                    {
                        Id = c.Long(nullable: false, identity: true),
                        ComputerName = c.String(maxLength: 50, storeType: "nvarchar"),
                        DatagramsReceived = c.Long(nullable: false),
                        DatagramsSent = c.Long(nullable: false),
                        DatagramsDiscarded = c.Long(nullable: false),
                        DatagramsWithErrors = c.Long(nullable: false),
                        UDPListeners = c.Int(nullable: false),
                        SegmentsReceived = c.Long(nullable: false),
                        SegmentsSent = c.Long(nullable: false),
                        ErrorsReceived = c.Long(nullable: false),
                        CurrentConnections = c.Long(nullable: false),
                        ResetConnections = c.Long(nullable: false),
                        RecordTime = c.DateTime(nullable: false, precision: 0),
                    })
                .PrimaryKey(t => t.Id)
                .Index(t => t.RecordTime);
            
            CreateTable(
                "dbo.StationNIs",
                c => new
                    {
                        Id = c.Int(nullable: false, identity: true),
                        ComputerName = c.String(maxLength: 50, storeType: "nvarchar"),
                        IfName = c.String(maxLength: 255, storeType: "nvarchar"),
                        IP = c.String(maxLength: 36, storeType: "nvarchar"),
                        MAC = c.String(maxLength: 23, storeType: "nvarchar"),
                    })
                .PrimaryKey(t => t.Id);
            
            CreateIndex("dbo.ProcessInfoes", "StartTime");
            CreateIndex("dbo.RunningStates", "RecordTime");
        }
        
        public override void Down()
        {
            DropIndex("dbo.RunningStates", new[] { "RecordTime" });
            DropIndex("dbo.ProcessInfoes", new[] { "StartTime" });
            DropIndex("dbo.NetStatistics", new[] { "RecordTime" });
            DropIndex("dbo.IFStatistics", new[] { "RecordTime" });
            DropTable("dbo.StationNIs");
            DropTable("dbo.NetStatistics");
            DropTable("dbo.IFStatistics");
        }
    }
}
