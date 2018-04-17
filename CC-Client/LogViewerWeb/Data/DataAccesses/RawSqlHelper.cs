using Microsoft.EntityFrameworkCore;
using System;
using System.Collections.Generic;
using System.Data.Common;
using System.Linq;
using System.Threading.Tasks;

namespace LogViewerWeb.Data
{
    public static class RawSqlHelper
    {
        public static async Task<List<T>> RawSqlQueryAsync<T>(this DbContext dbContext, string query, Func<DbDataReader, T> map, params DbParameter[] parameters)
        {
            using (var command = dbContext.Database.GetDbConnection().CreateCommand())
            {
                command.CommandText = query;
                command.CommandType = System.Data.CommandType.Text;
                if (parameters.Length > 0)
                    command.Parameters.AddRange(parameters);
                dbContext.Database.OpenConnection();
                using (var result = await command.ExecuteReaderAsync())
                {
                    var entities = new List<T>();

                    while (result.Read())
                    {
                        entities.Add(map(result));
                    }

                    return entities;
                }
            }
        }
    }
}
