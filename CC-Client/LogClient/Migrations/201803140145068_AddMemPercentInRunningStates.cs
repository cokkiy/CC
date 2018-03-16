namespace LogClient.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class AddMemPercentInRunningStates : DbMigration
    {
        public override void Up()
        {
            AddColumn("dbo.RunningStates", "MemoryPercent", c => c.Single(nullable: false));
        }
        
        public override void Down()
        {
            DropColumn("dbo.RunningStates", "MemoryPercent");
        }
    }
}
