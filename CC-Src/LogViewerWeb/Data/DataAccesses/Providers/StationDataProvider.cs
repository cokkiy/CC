using LogViewerWeb.Models.ViewModels;
using Microsoft.EntityFrameworkCore;
using MySql.Data.MySqlClient;
using StationLogModels;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace LogViewerWeb.Data.DataAccesses.Providers
{
    public class StationDataProvider : IStationDataProvider
    {
        protected StationLogContext _context;
        public StationDataProvider(StationLogContext context)
        {
            _context = context;
        }

        public Task<List<Station>> GetPowerOnStations()
        {
            return _context.Stations.Where(s => s.IsRunning).ToListAsync();
        }

        public (int running, int total) GetRunningAndTotalCount()
        {
            return (_context.Stations.Where(s => s.IsRunning).Count(),
                _context.Stations.Count());
        }

        public Task<List<StationRunningStatistic>> GetStationRunningStatistics(DateTime begin, DateTime end, TimeType type, bool distinct)
        {
            var queryString = GetQueryString(type, distinct);

            var beginParam = new MySqlParameter("begin", begin);
            var endParam = new MySqlParameter("end", end);

            Task<List<StationRunningStatistic>> statistics = null;

            switch (type)
            {
                case TimeType.Hourly:
                    statistics = _context.RawSqlQueryAsync(queryString,
                        x => new StationRunningStatistic
                        {
                            Count = (long)x["count"],
                            Year = Convert.ToInt32(x["Year"]),
                            Month = Convert.ToInt32(x["Month"]),
                            Day = Convert.ToInt32(x["Day"]),
                            Hour=Convert.ToInt32(x["Hour"]),
                            TimeType = type
                        }, beginParam, endParam);
                    break;
                case TimeType.Daily:
                    statistics = _context.RawSqlQueryAsync(queryString,
                        x => new StationRunningStatistic
                        {
                            Count = (long)x["count"],
                            Year = Convert.ToInt32(x["Year"]),
                            Month = Convert.ToInt32(x["Month"]),
                            Day = Convert.ToInt32(x["Day"]),
                            TimeType = type
                        }, beginParam, endParam);
                    break;
                case TimeType.Weekly:
                    statistics = _context.RawSqlQueryAsync(queryString,
                        x => new StationRunningStatistic
                        {
                            Count = (long)x["count"],
                            Year = Convert.ToInt32(x["Year"]),
                            Week = Convert.ToInt32(x["Week"]),
                            TimeType = type
                        }, beginParam, endParam);
                    break;
                case TimeType.Monthly:
                    statistics = _context.RawSqlQueryAsync(queryString,
                        x => new StationRunningStatistic
                        {
                            Count = (long)x["count"],
                            Year = Convert.ToInt32(x["Year"]),
                            Month = Convert.ToInt32(x["Month"]),
                            TimeType = type
                        }, beginParam, endParam);
                    break;
                case TimeType.Quarterly:
                    statistics = _context.RawSqlQueryAsync(queryString,
                        x => new StationRunningStatistic
                        {
                            Count = (long)x["count"],
                            Year = Convert.ToInt32(x["Year"]),
                            Quarter = Convert.ToInt32(x["Quarter"]),
                            TimeType = type
                        }, beginParam, endParam);
                    break;
                case TimeType.Yearly:
                    statistics = _context.RawSqlQueryAsync(queryString,
                        x => new StationRunningStatistic
                        {
                            Count = (long)x["count"],
                            Year = Convert.ToInt32(x["Year"]),
                            TimeType = type
                        }, beginParam, endParam);
                    break;
            }

            return statistics;
        }

        private static string GetQueryString(TimeType type, bool distinct)
        {
            string queryStringGroupByHourly = @"select {0} as count, YEAR(PowerOn) as Year,
                                Month(PowerOn) as Month,Day(PowerOn) as Day,Hour(PowerOn) as Hour
                                from poweronofflogs 
                                where PowerOn>=@begin and PowerOn<=@end 
                                group by Year,Month,Day,Hour order by Year,Month,Day,Hour;";
            string queryStringGroupByDaily = @"select {0} as count, YEAR(PowerOn) as Year,
                                Month(PowerOn) as Month,Day(PowerOn) as Day
                                from poweronofflogs 
                                where PowerOn>=@begin and PowerOn<=@end 
                                group by Year,Month,Day order by Year,Month,Day;";
            string queryStringGroupByWeekly = @"select {0} as count, YEAR(PowerOn) as Year,
                                 Week(PowerOn,7) as Week
                                 from poweronofflogs 
                                 where PowerOn>=@begin and PowerOn<=@end
                                 group by Year,Week order by Year,Week;";
            string queryStringGroupByMonthly = @"select {0} as count, YEAR(PowerOn) as Year,
                                 Month(PowerOn) as Month
                                 from poweronofflogs 
                                 where PowerOn>=@begin and PowerOn<=@end
                                 group by Year,Month order by Year,Month;";
            string queryStringGroupByQuarterly = @"select {0} as count, YEAR(PowerOn) as Year,
                                Quarter(PowerOn) as Quarter
                                from poweronofflogs 
                                where PowerOn>=@begin and PowerOn<=@end 
                                group by Year,Quarter order by Year,Quarter;";
            string queryStringGroupByYearly = @"select {0} as count, YEAR(PowerOn) as Year
                                 from poweronofflogs
                                 where PowerOn>=@begin and PowerOn<=@end
                                 group by Year order by Year;";

            string query = string.Empty;
            switch (type)
            {
                case TimeType.Hourly:
                    query = queryStringGroupByHourly;
                    break;
                case TimeType.Daily:
                    query = queryStringGroupByDaily;
                    break;
                case TimeType.Weekly:
                    query = queryStringGroupByWeekly;
                    break;
                case TimeType.Monthly:
                    query = queryStringGroupByMonthly;
                    break;
                case TimeType.Quarterly:
                    query = queryStringGroupByQuarterly;
                    break;
                case TimeType.Yearly:
                    query = queryStringGroupByYearly;
                    break;
                default:
                    query = queryStringGroupByDaily;
                    break;
            }

            return distinct ? string.Format(query, "COUNT(DISTINCT(ComputerName))") : string.Format(query, "COUNT(ComputerName)");
        }

        public Task<List<Station>> GetStations()
        {
            return _context.Stations.ToListAsync();
        }

        public Task<int> GetStationsCountInGroup(string groupName)
        {
            return _context.Stations.Where(s => s.GroupName == groupName).CountAsync();
        }

        public Task<List<StationViewModel>> GetStationsInfo(bool powerOnOnly)
        {
            var query = from s in _context.Stations
                        join l in _context.StationStates.OrderByDescending(st => st.UpdateTime)
                        on s.Id equals l.StationId
                        into ss
                        where ((powerOnOnly && s.IsRunning == powerOnOnly) || !powerOnOnly)
                        select new StationViewModel
                        {
                            ComputerName = s.ComputerName,
                            GroupName = s.GroupName,
                            UserName = s.UserName,
                            OsName = ss.First().OSName + " " + ss.First().OSVersion,
                            PowerOn = s.PowerOnTime,
                            TotalMemory = (int)(ss.First().TotalMemory / 1024),
                            IsRunning = s.IsRunning
                        };

            return query.ToListAsync();
        }

        public Task<List<Station>> GetStationsInGroup(string groupName)
        {
            return _context.Stations.Where(s => s.GroupName == groupName).ToListAsync();
        }

        public Task<int> GetTotalStationsCount()
        {
            return _context.Stations.CountAsync();
        }

        public Task<List<RunningState>> GetStationAlerts(int index, int count)
        {
            return _context.RunningStates.Where(r => r.CPU > 70 || r.MemoryPercent > 0.7)
                .OrderByDescending(r => r.RecordTime)
                .Skip(index).Take(count).Select(r => r).ToListAsync();
        }
    }
}
