using MySql.Data.Entity;
using StationLogModels;
using System.Data.Common;
using System.Data.Entity;

namespace LogClient
{
    [DbConfigurationType(typeof(MySqlEFConfiguration))]
    class StationLogContext : DbContext
    {
        public StationLogContext()
        {

        }
        public StationLogContext(string connectionString)
            : base(connectionString)
        {

        }

        public StationLogContext(DbConnection existDbConnection,bool conextOwnsConnection)
            :base(existDbConnection,conextOwnsConnection)
        {

        }

        protected override void OnModelCreating(DbModelBuilder modelBuilder)
        {
            base.OnModelCreating(modelBuilder);
            modelBuilder.Entity<RunningState>().HasIndex(r => r.RecordTime);
            modelBuilder.Entity<ProcessInfo>().HasIndex(p => p.StartTime);
            modelBuilder.Entity<ProcessInfo>().HasIndex(p => new { p.ComputerName, p.ProcessName });
            modelBuilder.Entity<NetStatistic>().HasIndex(n => n.RecordTime);
            modelBuilder.Entity<IFStatistic>().HasIndex(s => s.RecordTime);
            modelBuilder.Entity<PowerOnOffLog>().HasIndex(s => s.ComputerName);
            modelBuilder.Entity<PowerOnOffLog>().HasIndex(s => s.PowerOn);
            
        }

        public DbSet<Station> Stations { get; set; }

        public DbSet<StationState> StationStates { get; set; }

        public DbSet<RunningState> RunningStates { get; set; }

        public DbSet<ProcessInfo> ProcessInfos { get; set; }

        public DbSet<StationNI> StationNIs { get; set; }

        public DbSet<NetStatistic> NetStatistics { get; set; }

        public DbSet<IFStatistic> IFStatistics { get; set; }

        public DbSet<PowerOnOffLog> PowerOnOffLogs { get; set; }
        
    }
}
