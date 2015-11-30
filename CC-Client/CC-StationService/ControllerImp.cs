using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CC;
using Ice;

namespace CC_StationService
{
    /// <summary>
    /// 系统控制类
    /// </summary>
    class ControllerImp : CC.IControllerDisp_
    {
        /// <summary>
        /// 平台相关函数
        /// </summary>
        private static IPlatformMethod platformMethod;

        /// <summary>
        /// 系统监视对象
        /// </summary>
        private SystemWatcher watcher;

        /// <summary>
        /// 静态构造函数
        /// </summary>
        static ControllerImp()
        {
            platformMethod = PlatformMethodFactory.GetPlatformMethod();
        }

        /// <summary>
        /// 创建一个系统控制类对象
        /// </summary>
        /// <param name="watcher">系统监视实例</param>
        public ControllerImp(SystemWatcher watcher)
        {
            this.watcher = watcher;
        }

        /// <summary>
        /// 关闭指定的程序
        /// </summary>
        /// <param name="appName">程序名称</param>
        /// <param name="current__"></param>
        public override void closeApp(string appName, Current current__)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// 获取系统信息
        /// </summary>
        /// <param name="current__"></param>
        /// <returns>系统信息</returns>
        public override StationSystemState getSystemState(Current current__)
        {
            return StateGatherer.GatherSystemState();
        }

        /// <summary>
        /// 重启系统
        /// </summary>
        /// <param name="force">是否强制重启</param>
        /// <param name="current__"></param>
        public override void reboot(bool force, Current current__)
        {
            platformMethod.Reboot();
        }

        /// <summary>
        /// 重启指定的应用程序
        /// </summary>
        /// <param name="appName">应用程序名称</param>
        /// <param name="current__"></param>
        public override void restartApp(string appName, Current current__)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// 设置状态监视间隔
        /// </summary>
        /// <param name="interval">状态监视间隔</param>
        /// <param name="current__"></param>
        public override void setStateGatheringInterval(int interval, Current current__)
        {
            watcher.Interval = interval;
        }

        /// <summary>
        /// 设置监视应用程序列表
        /// </summary>
        /// <param name="procNames"></param>
        /// <param name="current__"></param>
        public override void setWatchingApp(List<string> procNames, Current current__)
        {
            watcher.AddAppToWatching(procNames);
        }

        /// <summary>
        /// 关闭系统
        /// </summary>
        /// <param name="current__"></param>
        public override void shutdown(Current current__)
        {
            platformMethod.Shutdown();
        }

        /// <summary>
        /// 启动指定名称的应用程序
        /// </summary>
        /// <param name="appName">应用程序名称（必须是完成路径)</param>
        /// <param name="current__"></param>
        public override void startApp(string appName, Current current__)
        {
            System.Diagnostics.Process.Start(appName);
        }
    }
}
