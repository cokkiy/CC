using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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

        /// <summary>
        /// 创建文件
        /// </summary>
        /// <param name="inParams"></param>
        /// <param name="outParams"></param>
        /// <param name="current"></param>
        /// <returns>返回是否抛出异常</returns>
        public bool createFile(byte[] inParams, out byte[] outParams, Ice.Current current)
        {
            bool result = false;
            //Create file
            Ice.Communicator communicator = current.adapter.getCommunicator();
            Ice.InputStream inStream = Ice.Util.createInputStream(communicator, inParams);
            inStream.startEncapsulation();
            string fileName = inStream.readString();
            inStream.endEncapsulation();
            inStream.destroy();
            Ice.Logger logger = PlatformMethodFactory.GetLogger();
            Ice.OutputStream outStream = Ice.Util.createOutputStream(communicator);
            try
            {
                string dir = fileName;
                int pos = fileName.LastIndexOf(Path.DirectorySeparatorChar);
                if (pos != -1)
                {
                    dir = fileName.Substring(0, pos + 1);
                }
                if (!Directory.Exists(dir))
                {
                    Directory.CreateDirectory(dir);
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
            finally
            {
                outStream.destroy();
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
            Ice.InputStream inStream = Ice.Util.createInputStream(communicator, inParams);
            inStream.startEncapsulation();
            long offset = inStream.readLong();
            int length = inStream.readInt();
            byte[] data = inStream.readByteSeq();
            inStream.endEncapsulation();
            inStream.destroy();
            Ice.OutputStream outStream = Ice.Util.createOutputStream(communicator);
            Ice.Logger logger = PlatformMethodFactory.GetLogger();
            if (length != data.Length)
            {
                CC.FileTransException ex = new CC.FileTransException("", offset, length, data.Length, "接收到的数据长度与发送数据长度不一致。");
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
            finally
            {
                outStream.destroy();
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
            Ice.OutputStream outStream = Ice.Util.createOutputStream(communicator);
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
                writeStream.Close();
                logger.print("文件已成功关闭。");
                outStream.startEncapsulation();
                outStream.writeBool(true);
                outStream.endEncapsulation();
                outParams = outStream.finished();
                result = true;             
            }
            outStream.destroy();
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
            Ice.InputStream inStream = Ice.Util.createInputStream(communicator, inParams);
            inStream.startEncapsulation();
            string fileName = inStream.readString();
            long offset = inStream.readLong();
            inStream.endEncapsulation();
            inStream.destroy();
            Ice.OutputStream outStream = Ice.Util.createOutputStream(communicator);
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
            finally
            {
                outStream.destroy();
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
            Ice.InputStream inStream = Ice.Util.createInputStream(communicator, inParams);
            inStream.startEncapsulation();
            string fileName = inStream.readString();
            inStream.endEncapsulation();
            inStream.destroy();

            Ice.OutputStream outStream = Ice.Util.createOutputStream(communicator);
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

            outStream.destroy();

            return result;
        }
    }
}
