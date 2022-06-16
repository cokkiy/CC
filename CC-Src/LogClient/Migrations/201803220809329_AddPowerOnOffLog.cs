namespace LogClient.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class AddPowerOnOffLog : DbMigration
    {
        public override void Up()
        {
            CreateTable(
                "dbo.PowerOnOffLogs",
                c => new
                    {
                        Id = c.Long(nullable: false, identity: true),
                        ComputerName = c.String(maxLength: 50, storeType: "nvarchar"),
                        PowerOn = c.DateTime(nullable: false, precision: 0),
                        PowerOff = c.DateTime(precision: 0),
                    })
                .PrimaryKey(t => t.Id)
                .Index(t => t.ComputerName)
                .Index(t => t.PowerOn);
            
        }
        
        public override void Down()
        {
            DropIndex("dbo.PowerOnOffLogs", new[] { "PowerOn" });
            DropIndex("dbo.PowerOnOffLogs", new[] { "ComputerName" });
            DropTable("dbo.PowerOnOffLogs");
        }
    }
}
