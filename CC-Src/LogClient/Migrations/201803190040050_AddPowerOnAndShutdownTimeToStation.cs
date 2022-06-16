namespace LogClient.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class AddPowerOnAndShutdownTimeToStation : DbMigration
    {
        public override void Up()
        {
            AddColumn("dbo.Stations", "PowerOnTime", c => c.DateTime(precision: 0));
            AddColumn("dbo.Stations", "ShutdownTime", c => c.DateTime(precision: 0));
        }
        
        public override void Down()
        {
            DropColumn("dbo.Stations", "ShutdownTime");
            DropColumn("dbo.Stations", "PowerOnTime");
        }
    }
}
