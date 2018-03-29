using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LogClient
{
    /// <summary>
    /// 工作站管理状态
    /// </summary>
    class ManageState
    {
        public string ComputerName { get; set; }

        /// <summary>
        /// 内存总量
        /// </summary>
        public long TotalMemory { get; set; }

        /// <summary>
        /// 最后心跳时间
        /// </summary>
        public DateTime LastTick { get; set; }

        /// <summary>
        /// 最后记录状态时间
        /// </summary>
        public DateTime LastWriteTime { get; set; }


        /// <summary>
        /// 网络数据最后记录时间
        /// </summary>
        public DateTime NetStatisticLastWriteTime { get; set; }


        /// <summary>
        /// 启动时间
        /// </summary>
        public DateTime StartTime { get; set; }

        /// <summary>
        /// 运行的进程
        /// </summary>
        public List<string> RunningProcesses { get; set; }

        /// <summary>
        /// 最后一次检查进程信息时间
        /// </summary>
        public DateTime LastCheckProcessTime { get; set; }


        public int ProcessCount
        {
            get
            {
                return IsLinux ? RunningProcesses.Count : RunningProcesses.Count + 1;
            }
        }

        /// <summary>
        /// 开关机记录Id
        /// </summary>
        public long PowerOnLogId { get; set; }

        /// <summary>
        /// 是否是Linux/unix系统
        /// </summary>
        public bool IsLinux { get; internal set; }

        //运行进程数据库Id
        public Dictionary<string, long> RunningProcId = new Dictionary<string, long>();

        public ManageState()
        {
            PowerOnLogId = -1;
        }
    }
}
