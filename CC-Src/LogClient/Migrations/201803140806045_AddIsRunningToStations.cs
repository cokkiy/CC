namespace LogClient.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class AddIsRunningToStations : DbMigration
    {
        public override void Up()
        {
            AddColumn("dbo.Stations", "IsRunning", c => c.Boolean(nullable: false));
        }
        
        public override void Down()
        {
            DropColumn("dbo.Stations", "IsRunning");
        }
    }
}
