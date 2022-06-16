namespace LogClient.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class AddRunningStates : DbMigration
    {
        public override void Up()
        {
            CreateTable(
                "dbo.RunningStates",
                c => new
                    {
                        Id = c.Long(nullable: false, identity: true),
                        ComputerName = c.String(unicode: false),
                        CurrentMemory = c.Long(nullable: false),
                        ProcessCount = c.Int(nullable: false),
                        CPU = c.Single(nullable: false),
                        RecordTime = c.DateTime(nullable: false, precision: 0),
                    })
                .PrimaryKey(t => t.Id);
            
        }
        
        public override void Down()
        {
            DropTable("dbo.RunningStates");
        }
    }
}
