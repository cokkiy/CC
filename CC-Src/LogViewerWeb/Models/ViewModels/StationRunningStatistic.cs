using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Threading.Tasks;

namespace LogViewerWeb.Models.ViewModels
{
    public class StationRunningStatistic
    {
        public long Count { get; set; }
        public int Year { get; set; }
        public int Month { get; set; }
        public int Week { get; set; }
        public int Day { get; set; }
        public int Hour { get; set; }
        public int Quarter { get; set; }
        public TimeType TimeType { get; set; }

        /// <summary>
        /// 字符串表示的时间,可用来直接显示
        /// </summary>
        public string Time
        {
            get
            {
                string result = string.Empty;
                var today = DateTime.Today;
                var yesterday = today - TimeSpan.FromDays(1);

                switch (TimeType)
                {
                    case TimeType.Hourly:
                        result = string.Format("{0}月{1}日{2}点", Month, Day, Hour);
                        break;
                    case TimeType.Daily:
                        if (today.Year == Year && today.Month == Month && today.Day == Day)
                        {
                            result = "今天";
                        }
                        else if (yesterday.Year == Year && yesterday.Month == Month && yesterday.Day == Day)
                        {
                            result = "昨天";
                        }
                        else
                        {
                            result = string.Format("{0}.{1:D2}.{2:D2}", Year, Month, Day);
                        }
                        break;
                    case TimeType.Weekly:
                        GregorianCalendar cal = new GregorianCalendar(GregorianCalendarTypes.Localized);
                        int week = cal.GetWeekOfYear(today, CalendarWeekRule.FirstFullWeek, DayOfWeek.Monday);
                        int lastWeek = cal.GetWeekOfYear(today - TimeSpan.FromDays(7), CalendarWeekRule.FirstFullWeek, DayOfWeek.Monday);
                        if (today.Year == Year && week == Week)
                        {
                            result = "本周";
                        }
                        else if ((today - TimeSpan.FromDays(7)).Year == Year && lastWeek == Week)
                        {
                            result = "上周";
                        }
                        else
                        {
                            result = string.Format("{0}年第{1}周", Year, Week);
                        }
                        break;
                    case TimeType.Monthly:
                        result = string.Format("{0}年{1}月", Year, Month);
                        break;
                    case TimeType.Quarterly:
                        result = string.Format("{0}年{1}季度",Year,Quarter);
                        break;
                    case TimeType.Yearly:
                        result = string.Format("{0}", Year);
                        break;
                }

                return result;
            }
        }
    }

    /// <summary>
    /// 统计分组时间类型
    /// </summary>
    public enum TimeType
    {
        /// <summary>
        /// 小时
        /// </summary>
        Hourly,
        /// <summary>
        /// 天
        /// </summary>
        Daily,

        /// <summary>
        /// 周
        /// </summary>
        Weekly,

        /// <summary>
        /// 月
        /// </summary>
        Monthly,

        /// <summary>
        /// 季度
        /// </summary>
        Quarterly,

        /// <summary>
        /// 年
        /// </summary>
        Yearly
    }
}
