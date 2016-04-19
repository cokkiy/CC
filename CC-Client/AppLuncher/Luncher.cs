using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Ice;
using AppController;
using CC;
using SysProcess = System.Diagnostics.Process;
using System.IO;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Imaging;
using System.Diagnostics;

namespace AppLuncher
{
    class Luncher : ILuncherDisp_
    {

        public override List<AppControlResult> closeApp(List<int> processIdList, Current current__)
        {
            List<AppControlResult> controlResults = new List<AppControlResult>();
            foreach (var Id in processIdList)
            {
                try
                {
                    SysProcess process = SysProcess.GetProcessById(Id);
                    if (!process.HasExited)
                        process.Kill();
                    System.Threading.Thread.Sleep(2000);
                    try
                    {
                        if (process.HasExited)
                        {
                            controlResults.Add(AppControlResult.Closed);
                        }
                        else
                        {
                            controlResults.Add(AppControlResult.FailToClose);
                        }
                    }
                    catch (System.ComponentModel.Win32Exception)
                    {
                        controlResults.Add(AppControlResult.Closed);
                    }
                }
                catch(InvalidOperationException)
                {
                    controlResults.Add(AppControlResult.NotRunnning);
                }
                catch (ArgumentException)
                {
                    controlResults.Add(AppControlResult.NotRunnning);
                }
                catch(System.ComponentModel.Win32Exception)
                {
                    controlResults.Add(AppControlResult.FailToClose);
                }
            }
            return controlResults;
        }

        /// <summary>
        /// 重新启动应用程序
        /// </summary>
        /// <param name="appParams">启动参数</param>
        /// <param name="current__"></param>
        /// <returns></returns>
        public override List<CC.AppStartingResult> restartApp(List<CC.AppStartParameter> appParams, Current current__)
        {
            List<int> processIdList = new List<int>();
            foreach (var param in appParams)
            {
                var processes = SysProcess.GetProcessesByName(param.ProcessName);
                foreach (var process in processes)
                {
                    processIdList.Add(process.Id);
                }                
            }
            closeApp(processIdList);
            return startApp(appParams);
        }

        /// <summary>
        /// 启动程序
        /// </summary>
        /// <param name="appParams">程序启动参数</param>
        /// <param name="current__"></param>
        /// <returns></returns>
        public override List<CC.AppStartingResult> startApp(List<CC.AppStartParameter> appParams, Current current__)
        {
            List<CC.AppStartingResult> results = new List<CC.AppStartingResult>();
            foreach (var param in appParams)
            {
                if(param.AllowMultiInstance)
                {
                    //允许多个实例
                    try
                    {
                        //获取已经运行的进程信息
                        SysProcess[] running = SysProcess.GetProcessesByName(param.ProcessName);
                        //启动新的进程
                        SysProcess process = SysProcess.Start(param.AppPath, param.Arguments);
                        System.Threading.Thread.Sleep(10);
                        //获取新的全部运行的进程信息
                        SysProcess[] newProcesses = SysProcess.GetProcessesByName(param.ProcessName);
                        //找到刚刚启动的进程信息
                        var newer = newProcesses.FirstOrDefault(p => running.FirstOrDefault(r => r.Id == p.Id) == null);
                        if (newer != null)
                        {
                            AppStartingResult result = new AppStartingResult
                            {
                                ProcessId = newer.Id,
                                ParamId = param.ParamId,
                                ProcessName = newer.ProcessName,
                                CtrlResult = AppControlResult.Started,
                                Result = "Success"
                            };
                            results.Add(result);
                        }
                        else
                        {
                            AppStartingResult result = new AppStartingResult
                            {
                                ProcessId = -1,
                                ParamId = param.ParamId,
                                ProcessName = param.ProcessName,
                                CtrlResult = AppControlResult.Error,
                                Result = "进程名称不正确"
                            };
                            results.Add(result);
                        }
                    }
                    catch(InvalidOperationException ex)
                    {
                        AppStartingResult result = new AppStartingResult
                        {
                            ProcessId = -1,
                            ParamId = param.ParamId,
                            ProcessName = "",
                            CtrlResult = AppControlResult.Error,
                            Result = ex.Message
                        };
                        results.Add(result);
                    }
                    catch(Win32Exception ex)
                    {
                        AppStartingResult result = new AppStartingResult
                        {
                            ProcessId = -1,
                            ParamId = param.ParamId,
                            ProcessName = "",
                            CtrlResult = AppControlResult.Error,
                            Result = ex.Message
                        };
                        results.Add(result);
                    }
                    catch(FileNotFoundException ex)
                    {
                        AppStartingResult result = new AppStartingResult
                        {
                            ProcessId = -1,
                            ParamId = param.ParamId,
                            ProcessName = "",
                            CtrlResult = AppControlResult.Error,
                            Result = ex.Message
                        };
                        results.Add(result);
                    }
                    catch(System.Exception ex)
                    {
                        AppStartingResult result = new AppStartingResult
                        {
                            ProcessId = -1,
                            ParamId = param.ParamId,
                            ProcessName = "",
                            CtrlResult = AppControlResult.Error,
                            Result = ex.Message
                        };
                        results.Add(result);
                    }
                }
                else
                {
                    //不允许多个实例
                    var processes = SysProcess.GetProcessesByName(param.ProcessName);
                    if(processes.Length==0)
                    {
                        try
                        {
                            //如果没有运行,则启动一个新的
                            SysProcess process = SysProcess.Start(param.AppPath, param.Arguments);
                            System.Threading.Thread.Sleep(10);
                            SysProcess[] newProcesses = SysProcess.GetProcessesByName(param.ProcessName);
                            if (newProcesses.Length > 0)
                            {
                                AppStartingResult result = new AppStartingResult
                                {
                                    ProcessId =newProcesses[0].Id,
                                    ParamId = param.ParamId,
                                    ProcessName = newProcesses[0].ProcessName,
                                    CtrlResult = AppControlResult.Started,
                                    Result = "Success"
                                };
                                results.Add(result);
                            }
                            else
                            {
                                AppStartingResult result = new AppStartingResult
                                {
                                    ProcessId = -1,
                                    ParamId = param.ParamId,
                                    ProcessName = param.ProcessName,
                                    CtrlResult = AppControlResult.Error,
                                    Result = "进程名称不正确"
                                };
                                results.Add(result);
                            }

                        }
                        catch (InvalidOperationException ex)
                        {
                            AppStartingResult result = new AppStartingResult
                            {
                                ParamId = param.ParamId,
                                ProcessName = "",
                                CtrlResult = AppControlResult.Error,
                                Result = ex.Message
                            };
                            results.Add(result);
                        }
                        catch (Win32Exception ex)
                        {
                            AppStartingResult result = new AppStartingResult
                            {
                                ParamId = param.ParamId,
                                ProcessName = "",
                                CtrlResult = AppControlResult.Error,
                                Result = ex.Message
                            };
                            results.Add(result);
                        }
                        catch (FileNotFoundException ex)
                        {
                            AppStartingResult result = new AppStartingResult
                            {
                                ParamId = param.ParamId,
                                ProcessName = "",
                                CtrlResult = AppControlResult.Error,
                                Result = ex.Message
                            };
                            results.Add(result);
                        }
                        catch (System.Exception ex)
                        {
                            AppStartingResult result = new AppStartingResult
                            {
                                ParamId = param.ParamId,
                                ProcessName = "",
                                CtrlResult = AppControlResult.Error,
                                Result = ex.Message
                            };
                            results.Add(result);
                        }
                    }
                    else
                    {
                        //如果已经启动,则不启动另一个
                        AppStartingResult result = new AppStartingResult
                        {
                            ProcessId=processes[0].Id,
                            ParamId = param.ParamId,
                            ProcessName = param.ProcessName,
                            CtrlResult = AppControlResult.AlreadyRunning,
                            Result = "Already Running"
                        };
                        results.Add(result);
                    }
                }
            }                
            return results;
        }

        //保存屏幕快照的内存流
        MemoryStream stream = null;
        //传送缓冲区大小
        const int bufSize = 4096; //4K Bytes

        /// <summary>
        /// 实现截屏功能
        /// </summary>
        /// <param name="position">快照内容传送位置，如果为0，则捕获新快照，否则从该位置继续传送后续内容</param>
        /// <param name="length">传送数据长度</param>
        /// <param name="data">传送的数据</param>
        /// <param name="current__"></param>
        /// <returns>传送是否完毕</returns>
        public override bool captureScreen(long position, out int length, out byte[] data, Current current__)
        {
            if (position == 0)
            {
                try
                {
                    //捕获新快照
                    Rectangle rect = System.Windows.Forms.Screen.PrimaryScreen.Bounds;
                    Bitmap image = new Bitmap(rect.Width, rect.Height);
                    Graphics g = Graphics.FromImage(image);
                    g.CopyFromScreen(new Point(0, 0), new Point(0, 0), new Size(rect.Width, rect.Height));
                    stream = new MemoryStream();
                    image.Save(stream, ImageFormat.Png);
                }
                catch (System.Exception ex)
                {
                    throw new FileTransException("屏幕快照", position, 0, 0, ex.ToString());
                }
            }

            if (stream == null)
            {
                throw new FileTransException("屏幕快照", position, 0, 0, "没有捕获快照");
            }
            else
            {
                try
                {
                    data = new byte[bufSize];
                    stream.Seek(position, SeekOrigin.Begin);
                    length = stream.Read(data, 0, bufSize);
                    if (length < bufSize)
                    {
                        stream.Close();
                    }
                    return length < bufSize;
                }
                catch (System.Exception ex)
                {
                    throw new FileTransException("屏幕快照", position, 0, 0, ex.ToString());
                }
            }
        }

        public override CC.Version getAppLuncherVersion(Current current__)
        {
            CC.Version v = new CC.Version();
            try
            {
                string path = System.AppDomain.CurrentDomain.BaseDirectory;
                v = FileVersionInfo.GetVersionInfo(System.IO.Path.Combine(path, "AppLuncher.exe"));
            }
            catch
            {

            }
            return v;
        }
    }
}
