namespace LogClient.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class AddProcessInfos : DbMigration
    {
        public override void Up()
        {
            CreateTable(
                "dbo.ProcessInfoes",
                c => new
                    {
                        Id = c.Long(nullable: false, identity: true),
                        ComputerName = c.String(maxLength: 50, storeType: "nvarchar"),
                        ProcessName = c.String(maxLength: 200, storeType: "nvarchar"),
                        StartTime = c.DateTime(nullable: false, precision: 0),
                        ExitTime = c.DateTime(precision: 0),
                    })
                .PrimaryKey(t => t.Id);
            
            AlterColumn("dbo.RunningStates", "ComputerName", c => c.String(maxLength: 50, storeType: "nvarchar"));
            AlterColumn("dbo.Stations", "ComputerName", c => c.String(maxLength: 50, storeType: "nvarchar"));
            AlterColumn("dbo.StationStates", "ComputerName", c => c.String(maxLength: 50, storeType: "nvarchar"));
        }
        
        public override void Down()
        {
            AlterColumn("dbo.StationStates", "ComputerName", c => c.String(unicode: false));
            AlterColumn("dbo.Stations", "ComputerName", c => c.String(unicode: false));
            AlterColumn("dbo.RunningStates", "ComputerName", c => c.String(unicode: false));
            DropTable("dbo.ProcessInfoes");
        }
    }
}
