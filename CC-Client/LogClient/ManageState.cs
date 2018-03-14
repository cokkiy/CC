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
        /// 最后心跳时间
        /// </summary>
        public DateTime LastTick { get; set; }

        /// <summary>
        /// 最后记录状态时间
        /// </summary>
        public DateTime LastWriteTime { get; set; }


    }
}
