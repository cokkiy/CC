using Microsoft.EntityFrameworkCore;
using StationLogModels;

namespace LogViewerWeb.Data
{    
    public class StationLogContext : DbContext
    {
        public StationLogContext(DbContextOptions<StationLogContext> options)
            : base(options)
        {
        }

        public DbSet<Station> Stations { get; set; }

        public DbSet<StationState> StationStates { get; set; }

        public DbSet<RunningState> RunningStates { get; set; }

        public DbSet<ProcessInfo> ProcessInfoes { get; set; }

        public DbSet<StationNI> StationNIs { get; set; }

        public DbSet<NetStatistic> NetStatistics { get; set; }

        public DbSet<IFStatistic> IFStatistics { get; set; }

        public DbSet<PowerOnOffLog> PowerOnOffLogs { get; set; }


    }
}
