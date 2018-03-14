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
        }

        public DbSet<Station> Stations { get; set; }

        public DbSet<StationState> StationStates { get; set; }
    }
}
