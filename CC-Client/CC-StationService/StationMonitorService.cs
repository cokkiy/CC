using CC;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.ServiceProcess;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace CC_StationService
{
    /// <summary>
    /// 工作站信息监视服务
    /// </summary>
    public partial class StationMonitorService : ServiceBase
    {
        //ice communicator
        private Ice.Communicator ic = null;
        /// <summary>
        /// 系统监视
        /// </summary>
        private SystemWatcher watcher = null;

        /// <summary>
        /// 日志记录器
        /// </summary>
        Ice.Logger logger = PlatformMethodFactory.GetLogger();


        public StationMonitorService()
        {
            InitializeComponent();
        }

        protected override void OnStart(string[] args)
        {
            try
            {
                //初始化ICE 通信对象
                Ice.InitializationData initData = new Ice.InitializationData();
                initData.properties = Ice.Util.createProperties();
                initData.properties.load("Config.Client");
                initData.logger = logger;
                ic = Ice.Util.initialize(initData);

                //创建控制适配器和对象
                Ice.ObjectAdapter controlAdapter = ic.createObjectAdapter("Controller");
                Ice.Object obj = new ControllerImp(watcher);
                IControllerPrx controlPrx = IControllerPrxHelper.uncheckedCast(controlAdapter.add(obj, ic.stringToIdentity("StationController")));
                //启动控制通道
                controlAdapter.activate();
                logger.print("开始监听控制消息...");

                //启动监视线程
                //初始化监视应用程序列表为空
                watcher = new SystemWatcher(ic, controlPrx);
                watcher.StartWatching();
                logger.print("开始系统资源监视...");               
            }
            catch (Exception ex)
            {
                logger.error(ex.ToString());
            }
        }

        
        protected override void OnPause()
        {
            //停止监视系统
            watcher.StopWatching();
            base.OnPause();
            logger.print("服务已暂停.");
        }

        protected override void OnContinue()
        {
            base.OnContinue();
            //开始监视
            watcher.StartWatching();
            logger.print("服务继续...");
        }

        protected override void OnStop()
        {
            //停止监视系统
            watcher.StopWatching();
            //释放通信资源
            if (ic != null)
            {
                //clean up
                try
                {
                    ic.destroy();
                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                }
            }

            logger.print("服务已停止.");
        }
    }
}
