using System.Threading.Tasks;
using CC;
using NIStatusDetectors;
using System;
using System.ServiceProcess;
using System.Text.RegularExpressions;

namespace CC_StationService
{
    /// <summary>
    /// 工作站信息监视服务
    /// </summary>
    public class StationMonitorService : BackgroundService
    {
        //ice communicator
        private Ice.Communicator ic = null;
        private Ice.ObjectAdapter controlAdapter;

        /// <summary>
        /// 系统监视
        /// </summary>
        private SystemWatcher watcher = null;

        //是否已经启动
        private bool isStarted = false;

        /// <summary>
        /// 日志记录器
        /// </summary>
        Ice.Logger logger = PlatformMethodFactory.GetLogger();
        private NIStatusDetector detector;
        private Ice.InitializationData initData;

        public StationMonitorService()
        {
        }

        protected override Task ExecuteAsync(CancellationToken token)
        {
            return Task.CompletedTask;
        }

        protected void OnStart(string[] args)
        {
            try
            {
                //设置工作目录为程序所在目录
                System.IO.Directory.SetCurrentDirectory(System.AppDomain.CurrentDomain.BaseDirectory);
                //开始监控
                Start();

            }
            catch (System.Exception ex)
            {
                logger.error(ex.ToString());
            }
        }

        internal void Start()
        {
            //初始化ICE 通信对象
            initData = new Ice.InitializationData();
            initData.properties = Ice.Util.createProperties();
            initData.properties.load("Config.Client");
            initData.logger = logger;

            string value = initData.properties.getProperty("StateReceiver.Proxy");
            var match = Regex.Match(value, @"--sourceAddress\s(?<IP>\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})");
            if (match.Success)
            {
                var ip = match.Groups["IP"].Value;
                detector = new NIStatusDetector(ip);
                detector.NIStateChanged += Detector_NIStateChanged;
                // 开始状态检测
                detector.Start();

                logger.print("监控服务启动...");
            }
        }

        private void Detector_NIStateChanged(object sender, NIStateChangedEventArgs e)
        {
            if (e.IsUp && !isStarted)
            {
                ic = Ice.Util.initialize(initData);
                //创建控制适配器和对象
                controlAdapter = ic.createObjectAdapter("Controller");
                ControllerImp controllerServant = new ControllerImp(ic);
                //将控制服务加入适配器并创建控制服务的本地代理
                Ice.ObjectPrx objectPrx = controlAdapter.add(controllerServant, Ice.Util.stringToIdentity("StationController"));
                IControllerPrx controlPrx = IControllerPrxHelper.uncheckedCast(objectPrx);

                //创建文件传输服务
                FileTranslation fileTrans = new FileTranslation(ic);
                objectPrx = controlAdapter.add(fileTrans, Ice.Util.stringToIdentity("FileTranslation"));
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

                isStarted = true;
            }
            else
            {
                Console.WriteLine("检测到网卡停止工作,监控将停止。");
                StopMC();
            }
        }

        //停止监控
        private void StopMC()
        {
            if (isStarted)
            {
                controlAdapter.destroy();
                //停止监视系统
                watcher.StopWatching();

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

                isStarted = false;

                logger.print("监控已停止。");
            }
        }

        protected void OnPause()
        {
            detector.NIStateChanged -= Detector_NIStateChanged;
            detector.Stop();
            StopMC();
            logger.print("服务已暂停.");
        }

        protected void OnContinue()
        {
            Start();
            logger.print("服务继续...");
        }

        protected void OnStop()
        {
            detector.NIStateChanged -= Detector_NIStateChanged;
            detector.Stop();
            StopMC();
            logger.print("服务已停止.");
        }

    }
}
