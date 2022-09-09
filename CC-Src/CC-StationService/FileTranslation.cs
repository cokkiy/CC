using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CC_StationService
{
    /// <summary>
    /// 文件传输
    /// </summary>
    public class FileTranslation : Ice.Blobject
    {
        private readonly FileTranslationImp imp;
        public FileTranslation(Ice.Communicator ic)
        {
            imp = new FileTranslationImp(ic);
        }


        /// <summary>
        /// 实现ice_invoke方法，实现文件传输
        /// </summary>
        /// <param name="inParams">输入参数</param>
        /// <param name="outParams">输出参数</param>
        /// <param name="current"></param>
        /// <returns></returns>
        public override bool ice_invoke(byte[] inParams, out byte[] outParams, Ice.Current current)
        {
            Type fileTranslationImpType = typeof(FileTranslationImp);
            System.Reflection.MethodInfo method = fileTranslationImpType.GetMethod(current.operation,
                new Type[] { typeof(byte).MakeArrayType(), typeof(byte).MakeArrayType().MakeByRefType(), typeof(Ice.Current) });
            if (method != null)
            {
                object[] paras = { inParams, null, current };
                bool result = (bool)method.Invoke(imp, paras);
                outParams = (byte[])paras[1];
                return result;
            }
            else
            {
                Ice.Logger logger = PlatformMethodFactory.GetLogger();
                logger.warning(string.Format("Method: {0} not exists.", current.operation));
                Ice.OperationNotExistException ex = new Ice.OperationNotExistException();
                ex.id = current.id;
                ex.facet = current.facet;
                ex.operation = current.operation;
                throw ex;
            }
        }
    }
}
