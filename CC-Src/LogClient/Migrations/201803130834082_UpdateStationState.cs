namespace LogClient.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class UpdateStationState : DbMigration
    {
        public override void Up()
        {
            CreateTable(
                "dbo.Stations",
                c => new
                    {
                        Id = c.Int(nullable: false, identity: true),
                        ComputerName = c.String(unicode: false),
                        UserName = c.String(unicode: false),
                        GroupName = c.String(unicode: false),
                        NetworkName = c.String(unicode: false),
                    })
                .PrimaryKey(t => t.Id);
            
            CreateTable(
                "dbo.StationStates",
                c => new
                    {
                        Id = c.Int(nullable: false, identity: true),
                        StationId = c.Int(nullable: false),
                        OSName = c.String(unicode: false),
                        OSVersion = c.String(unicode: false),
                        TotalMemory = c.Long(nullable: false),
                        ComputerName = c.String(unicode: false),
                        UpdateTime = c.DateTime(nullable: false, precision: 0),
                    })
                .PrimaryKey(t => t.Id);
            
        }
        
        public override void Down()
        {
            DropTable("dbo.StationStates");
            DropTable("dbo.Stations");
        }
    }
}
