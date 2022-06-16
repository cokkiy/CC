using System;
using System.IO;
using Ice;

namespace CC_StationService
{
    /// <summary>
    /// 文件传输类具体方法实现
    /// </summary>
    class FileTranslationImp
    {
        //写入的文件流
        private FileStream writeStream = null;
        //读缓冲区大小
        private const int readBufferSize = 1024 * 10;

        private readonly Ice.Communicator ic;

        public FileTranslationImp(Ice.Communicator ic)
        {
            this.ic = ic;
        }

        /// <summary>
        /// 创建文件
        /// </summary>
        /// <param name="inParams"></param>
        /// <param name="outParams"></param>
        /// <param name="current"></param>
        /// <returns>返回是否抛出异常</returns>
        public bool CreateFile(byte[] inParams, out byte[] outParams, Ice.Current current)
        {
            bool result = false;
            //Create file
            Ice.Communicator communicator = current.adapter.getCommunicator();
            var inStream = new InputStream(communicator, inParams);
            inStream.startEncapsulation();
            string fileName = inStream.readString();            
            inStream.endEncapsulation();          
            Ice.Logger logger = PlatformMethodFactory.GetLogger();
            OutputStream outStream = new(communicator);
            try
            {
                string dir = fileName;
                int pos = fileName.LastIndexOf(Path.DirectorySeparatorChar);
                if (pos == -1)
                    pos = fileName.LastIndexOf(Path.AltDirectorySeparatorChar);
                if (pos != -1)
                {
                    dir = fileName[..(pos + 1)];
                }
                if (!Directory.Exists(dir))
                {
                    Directory.CreateDirectory(dir);
                    chown(dir);
                    chmod2Exe(dir);
                }
                writeStream = File.Create(fileName);
                logger.print("成功创建文件:"+fileName);
                outStream.startEncapsulation();
                outStream.writeBool(true);
                outStream.endEncapsulation();
                outParams = outStream.finished();
                result = true;
            }
            catch (System.Exception ex)
            {
                writeStream = null;                
                logger.error(ex.ToString());
                CC.FileTransException exception = new CC.FileTransException(fileName, 0, 0, 0, ex.Message);
                outStream.startEncapsulation();
                outStream.writeBool(false);
                outStream.writeException(exception);
                outStream.endEncapsulation();
                outParams = outStream.finished();
                result = false;
            }

            return result;
        }

        /// <summary>
        /// 传输文件数据
        /// </summary>
        /// <param name="inParams"></param>
        /// <param name="outParams"></param>
        /// <param name="current"></param>
        public bool transData(byte[] inParams, out byte[] outParams, Ice.Current current)
        {
            bool result = false;
            // trans data
            Ice.Communicator communicator = current.adapter.getCommunicator();
            Ice.InputStream inStream = new(communicator, inParams);
            inStream.startEncapsulation();
            long offset = inStream.readLong();
            int length = inStream.readInt();
            byte[] data = inStream.readByteSeq();
            inStream.endEncapsulation();
            Ice.OutputStream outStream = new(communicator);
            Ice.Logger logger = PlatformMethodFactory.GetLogger();
            if (length != data.Length)
            {
                CC.FileTransException ex = new("", offset, length, data.Length, "接收到的数据长度与发送数据长度不一致。");
                logger.error(string.Format("接收到的数据长度与发送数据长度不一致。 Parameters offset ={0} length={1}, Real data length={2}", offset, length, data.Length));
                outStream.startEncapsulation();
                outStream.writeBool(false);
                outStream.writeException(ex);
                outStream.endEncapsulation();
                outParams = outStream.finished();
                result = false;
            }
            else if (writeStream == null)
            {
                CC.FileTransException ex = new CC.FileTransException("", offset, length, data.Length, "文件没有打开或已经关闭。");
                logger.error("文件没有打开或已经关闭。");
                outStream.startEncapsulation();
                outStream.writeBool(false);
                outStream.writeException(ex);
                outStream.endEncapsulation();
                outParams = outStream.finished();
                result = false;
            }
            try
            {
                writeStream.Seek(offset, SeekOrigin.Begin);
                writeStream.Write(data, 0, length);
                outStream.startEncapsulation();
                outStream.writeBool(true);
                outStream.endEncapsulation();
                outParams = outStream.finished();
                result = true;
            }
            catch (System.Exception ex)
            {
                logger.warning(string.Format("Parameters offset={0} length={1}, Real data length={2}", offset, length, data.Length));
                logger.error(ex.ToString());
                CC.FileTransException exception = new CC.FileTransException("", offset, length, data.Length, ex.Message);

                outStream.startEncapsulation();
                outStream.writeBool(false);
                outStream.writeException(exception);
                outStream.endEncapsulation();
                outParams = outStream.finished();
                result = false;
            }

            return result;
        }

        /// <summary>
        /// 关闭文件
        /// </summary>
        public bool closeFile(byte[] inParams, out byte[] outParams, Ice.Current current)
        {
            bool result = false;
            // closeFile
            Ice.Communicator communicator = current.adapter.getCommunicator();           
            Ice.OutputStream outStream = new(communicator);
            Ice.Logger logger = PlatformMethodFactory.GetLogger();
            if (writeStream == null)
            {
                logger.error("文件没有打开或已经关闭。");
                CC.FileTransException ex = new CC.FileTransException("", 0, 0, 0, "文件没有打开或已经关闭。");
                outStream.startEncapsulation();
                outStream.writeBool(false);
                outStream.writeException(ex);
                outStream.endEncapsulation();
                outParams = outStream.finished();
                result = false;
            }
            else
            {
                string fileName = writeStream.Name;
                writeStream.Close();
                chmod2LowPriority(fileName);
                if(isExecutable(fileName))
                {
                    chmod2Exe(fileName);
                }
                chown(fileName);
                logger.print("文件已成功关闭。");
                outStream.startEncapsulation();
                outStream.writeBool(true);
                outStream.endEncapsulation();
                outParams = outStream.finished();
                result = true;             
            }
            return result;
        }

        /// <summary>
        /// 获取指定文件指定位置的数据
        /// </summary>
        /// <param name="inParams"></param>
        /// <param name="outParams"></param>
        /// <param name="current"></param>
        public bool getData(byte[] inParams, out byte[] outParams, Ice.Current current)
        {
            bool result = false;
            Ice.Communicator communicator = current.adapter.getCommunicator();
            Ice.InputStream inStream = new(communicator, inParams);
            inStream.startEncapsulation();
            string fileName = inStream.readString();
            long offset = inStream.readLong();
            inStream.endEncapsulation();
            Ice.OutputStream outStream = new(communicator);
            Ice.Logger logger = PlatformMethodFactory.GetLogger();
            try
            {
                using (FileStream reader = new FileStream(fileName,FileMode.Open,FileAccess.Read))
                {
                    reader.Seek(offset, SeekOrigin.Begin);
                    byte[] data = new byte[readBufferSize];
                    int readedLength = reader.Read(data, 0, readBufferSize);
                    outStream.startEncapsulation();
                    outStream.writeInt(readedLength);
                    outStream.writeByteSeq(data);
                    outStream.writeBool(readedLength < readBufferSize); //是否还有字节未读
                    outStream.endEncapsulation();
                    outParams = outStream.finished();
                    reader.Close();
                }
                result = true;
            }
            catch (System.Exception ex)
            {
                logger.error(ex.ToString());
                CC.FileTransException exception = new CC.FileTransException(fileName, offset, 0, 0, ex.Message);
                outStream.startEncapsulation();
                outStream.writeException(exception);
                outStream.endEncapsulation();
                outParams = outStream.finished();
                result = false;
            }
           
            return result;
        }

        /// <summary>
        /// 获取文件大小
        /// </summary>
        /// <param name="inParams"></param>
        /// <param name="outParams"></param>
        /// <param name="current"></param>
        /// <returns></returns>
        public bool getSize(byte[] inParams, out byte[] outParams, Ice.Current current)
        {
            bool result = false;
            // closeFile
            Ice.Communicator communicator = current.adapter.getCommunicator();
            Ice.InputStream inStream = new(communicator, inParams);
            inStream.startEncapsulation();
            string fileName = inStream.readString();
            inStream.endEncapsulation();

            Ice.OutputStream outStream = new(communicator);
            Ice.Logger logger = PlatformMethodFactory.GetLogger();

            if (File.Exists(fileName))
            {
                long length = 0;
                try
                {
                    using (var stream = File.OpenRead(fileName))
                    {
                        length = stream.Length;
                        stream.Close();
                    }
                    outStream.startEncapsulation();
                    outStream.writeLong(length);
                    outStream.endEncapsulation();
                    outParams = outStream.finished();
                    result = true;
                }
                catch (System.Exception ex)
                {
                    CC.FileTransException exception = new CC.FileTransException(fileName, 0, 0, 0, ex.Message);
                    outStream.startEncapsulation();
                    outStream.writeException(exception);
                    outStream.endEncapsulation();
                    outParams = outStream.finished();
                    result = false;
                }

                
            }
            else
            {
                //文件不存在
                CC.FileTransException exception = new CC.FileTransException(fileName, 0, 0, 0, "文件不存在");
                outStream.startEncapsulation();
                outStream.writeException(exception);
                outStream.endEncapsulation();
                outParams = outStream.finished();
                result = false;
            }

            return result;
        }

        /// <summary>
        /// 检查指定的文件是否是Linux可执行文件
        /// Linux可执行文件包括：可执行程序，sh脚本和Python脚本等
        /// </summary>
        /// <param name="fileName">检查的文件名</param>
        /// <returns>如果是可执行文件，返回true,否则返回false</returns>
        private bool isExecutable(string fileName)
        {
            if (System.Environment.OSVersion.Platform == PlatformID.Unix)
            {
                try
                {
                    System.Diagnostics.Process fileProcess = new System.Diagnostics.Process();
                    fileProcess.StartInfo.FileName = "/usr/bin/file";
                    fileProcess.StartInfo.UseShellExecute = false;
                    fileProcess.StartInfo.Arguments = fileName;
                    fileProcess.StartInfo.RedirectStandardOutput = true;
                    fileProcess.Start();
                    string output = fileProcess.StandardOutput.ReadToEnd();
                    fileProcess.WaitForExit();
                    if (output.Contains("ELF 64-bit LSB executable")
                        || output.Contains("ELF 32-bit LSB executable")
                        || output.Contains("PE32 executable")
                        || output.Contains("PE32+ executable")
                        || output.Contains("shell script")
                        || output.Contains("python script")
                        || output.Contains("Python script"))
                        return true;
                }
                catch(System.Exception ex)
                {
                    Ice.Logger logger = PlatformMethodFactory.GetLogger();
                    logger.error(string.Format("isExecutable Error. fileName:{0} {1}", fileName, ex.ToString()));
                }
            }
            return false;
        }

        //设置文件权限，是任何人都可以读取删除
        private void chmod2LowPriority(string fileName)
        {
            if (System.Environment.OSVersion.Platform == PlatformID.Unix)
            {
                try
                {
                    System.Diagnostics.Process chmodProcess = new System.Diagnostics.Process();
                    chmodProcess.StartInfo.FileName = "chmod";
                    chmodProcess.StartInfo.UseShellExecute = true;
                    chmodProcess.StartInfo.Arguments = "666 " + fileName;
                    chmodProcess.Start();
                    Ice.Logger logger = PlatformMethodFactory.GetLogger();
                }
                catch (System.Exception ex)
                {
                    Ice.Logger logger = PlatformMethodFactory.GetLogger();
                    logger.error(string.Format("chmod2LowPriority Error. fileName:{0} {1}", fileName, ex.ToString()));
                }
            }
        }

        //设置文件为可执行模式
        private void chmod2Exe(string fileName)
        {
            if (System.Environment.OSVersion.Platform == PlatformID.Unix)
            {
                try {
                    System.Diagnostics.Process chmodProcess = new System.Diagnostics.Process();
                    chmodProcess.StartInfo.FileName = "chmod";
                    chmodProcess.StartInfo.UseShellExecute = true;
                    chmodProcess.StartInfo.Arguments = "777 " + fileName;
                    chmodProcess.Start();
                }
                catch(System.Exception ex)
                {
                    Ice.Logger logger = PlatformMethodFactory.GetLogger();
                    logger.error(string.Format("chmod2Exe Error. fileName:{0} {1}", fileName, ex.ToString()));
                }
            }
        }

        /// <summary>
        /// 修改指定文件、文件夹的所有者
        /// </summary>
        /// <param name="fileName">文件名或文件夹名称</param>
        /// <param name="user">新的所有者</param>
        private void chown(string fileName)
        {
            if (System.Environment.OSVersion.Platform == PlatformID.Unix)
            {
                string userName = GetCurrentUserName();
                Ice.Logger logger = PlatformMethodFactory.GetLogger();
                try
                {
                    //change user
                    System.Diagnostics.Process chownProcess = new System.Diagnostics.Process();
                    chownProcess.StartInfo.FileName = "/bin/chown";
                    chownProcess.StartInfo.UseShellExecute = true;
                    chownProcess.StartInfo.Arguments = string.Format("{0} {1}", userName, fileName);
                    if (!chownProcess.Start())
                    {
                        logger.warning(string.Format("Chown fail. fileName:{0}, new owner:{1}.", fileName, userName));
                    }

                    // change group
                    System.Diagnostics.Process chgrpProcess = new System.Diagnostics.Process();
                    chgrpProcess.StartInfo.FileName = "/bin/chgrp";
                    chgrpProcess.StartInfo.UseShellExecute = true;
                    chgrpProcess.StartInfo.Arguments = string.Format("{0} {1}", userName, fileName);
                    if (!chgrpProcess.Start())
                    {
                        logger.warning(string.Format("Chgrp fail. fileName:{0}, new owner:{1}.", fileName, userName));
                    }
                }
                catch (System.Exception ex)
                {
                    //Ice.Logger logger = PlatformMethodFactory.GetLogger();
                    logger.error(string.Format("chown Error. fileName:{0}, new owner:{1}. {2}", fileName, userName, ex.ToString()));
                }
            }
        }


        private string currentUserName;
        private bool userNameGetted = false;
        private string GetCurrentUserName()
        {
            if (userNameGetted)
            {
                return currentUserName;
            }
            else
            {
                currentUserName = "root";
                ObjectPrx proxy = ic.propertyToProxy("AppLuncher.Proxy");
                try
                {
                    AppController.ILuncherPrx luncherPrx = AppController.ILuncherPrxHelper.checkedCast(proxy);
                    currentUserName = luncherPrx.getCurrentUser();
                    userNameGetted = true;
                }
                catch
                {
                }

                return currentUserName;
            }
        }
    }
}
