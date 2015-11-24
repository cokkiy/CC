using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Ice;
using CCService;

namespace CC_StationService
{
    class MonitorImp : IMonitorDisp_
    {
        /// <summary>
        /// 平台相关函数
        /// </summary>
        static IPlatformMethod platformMethod;
        static MonitorImp()
        {
            if (Environment.OSVersion.Platform == PlatformID.Unix)
            {
                // linux,unix
                platformMethod = new LinuxMethod();
            }
            else
            {
                // windows 
                platformMethod = new WindowsMethod();
            }
        }

        public override long getAppCurrentMemory(string appName, Current current__)
        {
            throw new NotImplementedException();
        }

        public override long getCurrentMemory(Current current__)
        {
            throw new NotImplementedException();
        }

        public override long getTotalMemory(Current current__)
        {
            return (long)platformMethod.GetTotalMemory();
        }
    }
}
