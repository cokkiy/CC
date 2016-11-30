using CC;
using System;
using System.ServiceProcess;

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
                //设置工作目录为程序所在目录
                System.IO.Directory.SetCurrentDirectory(System.AppDomain.CurrentDomain.BaseDirectory);
                //初始化ICE 通信对象
                Ice.InitializationData initData = new Ice.InitializationData();
                initData.properties = Ice.Util.createProperties();
                initData.properties.load("Config.Client");
                initData.logger = logger;
                ic = Ice.Util.initialize(initData);

                //创建控制适配器和对象
                Ice.ObjectAdapter controlAdapter = ic.createObjectAdapter("Controller");
                ControllerImp controllerServant = new ControllerImp(ic);
                //将控制服务加入适配器并创建控制服务的本地代理
                Ice.ObjectPrx objectPrx = controlAdapter.add(controllerServant, ic.stringToIdentity("StationController"));
                IControllerPrx controlPrx = IControllerPrxHelper.uncheckedCast(objectPrx);

                //创建文件传输服务
                FileTranslation fileTrans = new FileTranslation();
                objectPrx = controlAdapter.add(fileTrans, ic.stringToIdentity("FileTranslation"));
                IFileTranslationPrx filePrx = IFileTranslationPrxHelper.uncheckedCast(objectPrx);

                //初始化监视应用程序列表为空
                watcher = new SystemWatcher(ic, controlPrx, filePrx);
                //为控制代理设置系统监视对象
                controllerServant.SystemWatcher = watcher;

                //启动控制通道
                controlAdapter.activate();
                logger.print("开始监听控制消息...");

                //启动监视线程
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
            //停止下载气象云图
            WeartherImageDownloader downloader = WeartherImageDownloader.GetInstance();
            downloader.Stop();
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
