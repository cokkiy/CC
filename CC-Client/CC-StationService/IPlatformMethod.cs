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

        /// <summary>
        /// 获取内存使用情况
        /// </summary>
        /// <returns>内存使用情况</returns>
        MemoryInfo GetMemoryInfo();
    }

    /// <summary>
    /// Factory for Platfrom specified method and log
    /// </summary>
    static class PlatformMethodFactory
    {
        private static IPlatformMethod platformMethod = null;
        private static Ice.Logger logger = null;

        /// <summary>
        /// 获取平台相关函数的唯一实现
        /// </summary>
        /// <returns>平台相关函数的唯一实现</returns>
        public static IPlatformMethod GetPlatformMethod()
        {
            if (platformMethod == null)
            {
                if (Environment.OSVersion.Platform == PlatformID.Unix
                    || Environment.OSVersion.Platform == PlatformID.MacOSX)
                {
                    // linux,unix,macos
                    platformMethod = new LinuxMethod();
                }
                else
                {
                    // windows 
                    platformMethod = new WindowsMethod();
                }
            }
            return platformMethod;
        }

        /// <summary>
        /// 获取日志记录工具的唯一对象
        /// </summary>
        /// <returns>日志记录工具的唯一对象</returns>
        public static Ice.Logger GetLogger()
        {
            if (logger == null)
            {
                if (Environment.OSVersion.Platform == PlatformID.Unix
                    || Environment.OSVersion.Platform == PlatformID.MacOSX)
                {
                    // linux,unix,macos
                    logger = new LinuxLogger();
                }
                else
                {
                    // windows 
                    logger = new WindowsLogger();
                }
            }
            return logger;
        }
    }
}
