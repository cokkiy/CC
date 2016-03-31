﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CC;
using Ice;
using System.IO;

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
        /// Ice Communicator 对象
        /// </summary>
        private Ice.Communicator ic;

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
        /// <param name="ic">Ice Communicator 对象</param>
        public ControllerImp(Ice.Communicator ic)
        {
            this.ic = ic;
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
        /// 设置系统监视对象
        /// </summary>
        public SystemWatcher SystemWatcher
        {
            set
            {
                this.watcher = value;
            }
        }        

        /// <summary>
        /// 设置状态监视间隔
        /// </summary>
        /// <param name="interval">状态监视间隔,单位秒</param>
        /// <param name="current__"></param>
        public override void setStateGatheringInterval(int interval, Current current__)
        {
            if (watcher != null)
            {
                watcher.Interval = interval * 1000;
            }
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
        /// <param name="appParams">应用程序启动参数信息</param>
        /// <param name="current__"></param>
        /// <returns>程序启动结果</returns>
        public override List<AppStartingResult> startApp(List<AppStartParameter> appParams, Current current__)
        {
            ObjectPrx proxy = ic.propertyToProxy("AppLuncher.Proxy");
            try
            {
                AppController.ILuncherPrx luncherPrx = AppController.ILuncherPrxHelper.checkedCast(proxy);
                return luncherPrx.startApp(appParams);
            }
            catch (Ice.Exception ex)
            {
                if (ex.InnerException != null)
                    throw new AppControlError("应用程序启动代理接口错误:" + ex.InnerException.Message);
                else
                    throw new AppControlError("应用程序启动代理接口错误,没有运行或被阻塞.");
            }
        }

        /// <summary>
        /// 关闭指定进程Id的所有程序
        /// </summary>
        /// <param name="processIdList">进程Id列表</param>
        /// <param name="current__"></param>
        public override List<AppControlResult> closeApp(List<int> processIdList, Current current__)
        {
            ObjectPrx proxy = ic.propertyToProxy("AppLuncher.Proxy");            
            try
            {
                AppController.ILuncherPrx luncherPrx = AppController.ILuncherPrxHelper.checkedCast(proxy);
                return luncherPrx.closeApp(processIdList);
            }
            catch (Ice.Exception ex)
            {
                throw new AppControlError("应用程序启动代理接口错误:" + ex.InnerException.Message);
            }
        }

        /// <summary>
        /// 重启指定的应用程序
        /// </summary>
        /// <param name="appName">应用程序名称</param>
        /// <param name="current__"></param>
        public override List<AppStartingResult> restartApp(List<AppStartParameter> appParams, Current current__)
        {
            ObjectPrx proxy = ic.propertyToProxy("AppLuncher.Proxy");
            try
            {
                AppController.ILuncherPrx luncherPrx = AppController.ILuncherPrxHelper.checkedCast(proxy);
                return luncherPrx.restartApp(appParams);
            }
            catch (Ice.Exception ex)
            {
                throw new AppControlError("应用程序启动代理接口错误:" + ex.InnerException.Message);
            }
        }
        
    }
}
