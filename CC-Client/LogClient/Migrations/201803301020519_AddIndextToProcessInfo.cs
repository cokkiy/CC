namespace LogClient.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class AddIndextToProcessInfo : DbMigration
    {
        public override void Up()
        {
            CreateIndex("dbo.ProcessInfoes", new[] { "ComputerName", "ProcessName" });
        }
        
        public override void Down()
        {
            DropIndex("dbo.ProcessInfoes", new[] { "ComputerName", "ProcessName" });
        }
    }
}
