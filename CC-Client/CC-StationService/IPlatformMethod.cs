using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CC_StationService
{
    /// <summary>
    /// IPlatformMethod：定义了平台相关方法
    /// </summary>
    interface IPlatformMethod
    {
        /// <summary>
        /// 获取系统总内存大小（字节)
        /// </summary>
        /// <returns>系统总内存大小（字节)</returns>
        long GetTotalMemory();
        /// <summary>
        /// 重启系统
        /// </summary>
        void Reboot();

        /// <summary>
        /// 关闭系统
        /// </summary>
        void Shutdown();
    }
}
