using System;
using System.Collections.Generic;
using System.Data.SqlClient;
using System.Linq;
using System.Threading.Tasks;
using LogViewerWeb.Models.ViewModels;
using Microsoft.EntityFrameworkCore;
using MySql.Data.MySqlClient;
using StationLogModels;

namespace LogViewerWeb.Data.DataAccesses.Providers
{
    public class ProcessDataProvider : IProcessDataProvider
    {
        protected StationLogContext _context;
        public ProcessDataProvider(StationLogContext context)
        {
            _context = context;
        }

        public Task<List<AppRunningStatistic>> GetAppRunningTimeStatistic(DateTime begin, DateTime end, int count)
        {
            var queryString = @"select TIMESTAMPDIFF(MINUTE,starttime,exittime) as time,
                                COUNT(processname) as count,
                                processname from processinfoes where starttime >= @begin
                                and starttime <= @end group by
                                processname order by time desc LIMIT @count; ";
            var beginParam = new MySqlParameter("begin", begin);
            var endParam = new MySqlParameter("end", end);
            var countParam = new MySqlParameter("count", count);

            var results = _context.RawSqlQueryAsync(queryString,
                x => new AppRunningStatistic
                {
                    AppName = (string)x["processname"],
                    RunningTimes = x["time"] == DBNull.Value ? 0 : (long)x["time"],
                    RunningCount = (long)x["count"]
                },
                beginParam, endParam, countParam);

            return results;
        }

        public Task<List<AppRunningStatistic>> GetAppRunningCountStatistics(DateTime begin, DateTime end, int count)
        {
            var queryString = @"select TIMESTAMPDIFF(MINUTE,starttime,exittime) as time,
                                COUNT(processname) as count,
                                processname from processinfoes where starttime >= @begin
                                and starttime <= @end  group by
                                processname order by count desc LIMIT @count; ";
            var beginParam = new MySqlParameter("begin", begin);
            var endParam = new MySqlParameter("end", end);
            var countParam = new MySqlParameter("count", count);

            var results = _context.RawSqlQueryAsync(queryString,
                x => new AppRunningStatistic
                {
                    AppName = (string)x["processname"],
                    RunningCount = (long)x["count"],
                    RunningTimes = x["time"] == DBNull.Value ? 0 : (long)x["time"]
                },
                beginParam, endParam, countParam);

            return results;
        }

        public Task<List<ProcessStartAndExitMessage>> GetProcessStartAndExitMessage(int count)
        {
            var startQuery = from start in _context.ProcessInfoes
                             where start.ExitTime == null
                             orderby start.StartTime descending
                             select new ProcessStartAndExitMessage
                             {
                                 ComputerName = start.ComputerName,
                                 IsExit = false,
                                 ActionTime = start.StartTime,
                                 ProcessName = start.ProcessName,
                             };
            var exitQuery = from exit in _context.ProcessInfoes
                            where exit.ExitTime != null
                            orderby exit.ExitTime descending
                            select new ProcessStartAndExitMessage
                            {
                                ComputerName = exit.ComputerName,
                                IsExit = true,
                                ActionTime = exit.ExitTime.Value,
                                ProcessName = exit.ProcessName,
                            };
            var result = (startQuery.Take(count)).Union(exitQuery.Take(count));
            return result.OrderByDescending(m => m.ActionTime).Take(count).ToListAsync();
        }

        public Task<int> GetRunningProcessesCount()
        {
            var startTime = DateTime.Now - TimeSpan.FromHours(48);
            return _context.ProcessInfoes.Where(p => p.StartTime > startTime && p.ExitTime == null)
                .CountAsync();
        }
    }
}