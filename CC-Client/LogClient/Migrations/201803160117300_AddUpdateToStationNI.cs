namespace LogClient.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class AddUpdateToStationNI : DbMigration
    {
        public override void Up()
        {
            AddColumn("dbo.StationNIs", "Update", c => c.DateTime(nullable: false, precision: 0));
        }
        
        public override void Down()
        {
            DropColumn("dbo.StationNIs", "Update");
        }
    }
}
