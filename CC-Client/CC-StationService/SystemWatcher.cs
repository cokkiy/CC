using CC;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace CC_StationService
{
    /// <summary>
    /// 系统状态监视类
    /// </summary>
    class SystemWatcher
    {
        /// <summary>
        /// 是否停止监视
        /// </summary>
        private bool stop=false;

        /// <summary>
        /// Ice Communicator
        /// </summary>
        private Ice.Communicator ic;

        /// <summary>
        /// 需要监视的应用进程名称或者Id:进程Id列表
        /// </summary>
        private List<string> procsToWatching;

        /// <summary>
        /// Logger system
        /// </summary>
        private Ice.Logger logger;

        /// <summary>
        /// 控制代理
        /// </summary>
        private IControllerPrx controlPrx;

        /// <summary>
        /// 文件传输代理
        /// </summary>
        private IFileTranslationPrx filePrx;

        /// <summary>
        /// 获取或设置以毫秒为单位的监视间隔
        /// </summary>
        /// <remarks>默认监视间隔为1000毫秒</remarks>
        public int Interval { get; set; }



        /// <summary>
        /// 创建一个系统监视对象,要监视的应用程序列表为空
        /// </summary>
        /// <param name="ic">Ice Communicator对象实例</param>
        /// <param name="interval">监视间隔,默认为1000毫秒</param>
        public SystemWatcher(Ice.Communicator ic, IControllerPrx controlPrx, IFileTranslationPrx filePrx, int interval = 1000)
        {
            Interval = interval;
            this.controlPrx = controlPrx;
            this.filePrx = filePrx;
            this.ic = ic;
            this.logger = ic.getLogger();
            this.procsToWatching = new List<string>();
        }


        /// <summary>
        /// Read write lock for app watching list
        /// </summary>
        ReaderWriterLockSlim watchingListLock = new ReaderWriterLockSlim();

        /// <summary>
        /// 添加需要监视的一组应用程序（进程)名称
        /// </summary>
        /// <param name="appsToWatching">要监视的一组应用程序（进程)名称</param>
        public void AddAppToWatching(IEnumerable<string> appsToWatching)
        {
            //开始编辑
            watchingListLock.EnterWriteLock();
            try
            {
                //先清除全部
                this.procsToWatching.Clear();
                //再添加新的监视列表
                this.procsToWatching.AddRange(appsToWatching);
            }
            finally
            {
                //编辑完毕
                watchingListLock.ExitWriteLock();
            }
        }

        /// <summary>
        /// 停止监视
        /// </summary>
        public void StopWatching()
        {
            stop = true;
        }

        /// <summary>
        /// 系统状态监视函数
        /// </summary>
        /// <param name="obj">Ice 通信对象</param>
        public void SystemWatching(object obj)
        {
            Ice.Communicator ic = obj as Ice.Communicator;
            if (ic != null)
            {
                Ice.ObjectPrx proxy;
                IStationStateReceiverPrx receiverProxy;
                try
                {
                    proxy = ic.propertyToProxy("StateReceiver.Proxy").ice_datagram();
                    receiverProxy = IStationStateReceiverPrxHelper.uncheckedCast(proxy);
                }
                catch(Exception ex)
                {
                    logger.error(ex.ToString());
                    return;
                }
                while (!stop)
                {
                    try
                    {
                        if (receiverProxy != null)
                        {
                            //收集工作站运行状态
                            var runningState = StateGatherer.GatherRunningState();
                            //汇报运行状态
                            receiverProxy.receiveStationRunningState(runningState, controlPrx, filePrx);

                            //收集工作站应用程序状态
                            List<AppRunningState> appsRunningState = new List<AppRunningState>();
                            //需要删除的监视进程
                            List<string> willRemovedProc = new List<string>();
                            //进入读阶段
                            watchingListLock.EnterReadLock();
                            try
                            {
                                foreach (var proc in procsToWatching)
                                {
                                    AppRunningState appState = StateGatherer.GatherAppRunningState(proc);
                                    appsRunningState.Add(appState);
                                    if (proc.StartsWith("Id:") && appState.process.Id <= 0)
                                    {
                                        //准备删除没有运行的进程
                                        willRemovedProc.Add(proc);
                                    }
                                }
                                //删除没用运行的进程
                                foreach (var proc in willRemovedProc)
                                {
                                    procsToWatching.Remove(proc);
                                }
                            }
                            finally
                            {
                                //离开读阶段
                                watchingListLock.ExitReadLock();
                            }

                            receiverProxy.receiveAppRunningState(appsRunningState);
                        }
                    }
                    catch (Ice.Exception ex)
                    {
                        logger.error(ex.ToString());
                        Console.Error.WriteLine(ex);
                    }
                    catch (Exception ex)
                    {
                        logger.error(ex.ToString());
                        Console.Error.WriteLine(ex);
                    }

                    Thread.Sleep(Interval); //汇报间隔
                }
            }
        }

        /// <summary>
        /// 开始监视
        /// </summary>
        internal void StartWatching()
        {
            stop = false;
            Thread monitorThread = new Thread(this.SystemWatching);
            monitorThread.Start(ic);
        }
    }
}
