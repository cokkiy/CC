using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Ice;

namespace CC_StationService
{
    class ControllerImp : CCService.IControllerDisp_
    {
        /// <summary>
        /// 平台相关函数
        /// </summary>
        private static IPlatformMethod platformMethod;
        static ControllerImp()
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
        public override void reboot(bool force, Current current__)
        {
            platformMethod.Reboot();
        }

        public override void shutdown(Current current__)
        {
            platformMethod.Shutdown();
        }

        public override void startApp(string appName, Current current__)
        {
            System.Diagnostics.Process.Start(appName);
        }
    }
}
