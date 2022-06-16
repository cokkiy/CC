using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CC
{
    public partial class AppStartParameter
    {
        /// <summary>
        /// 返回表示启动参数的字符串,路径和参数之间用 （空格)&& （空格)分隔
        /// </summary>
        /// <returns>表示启动参数的字符串,路径和参数之间用 （空格)&& （空格)分隔</returns>
        public override string ToString()
        {
            return string.Format("{0} && {1}", this.AppPath, this.Arguments);
        }

        /// <summary>
        /// 转换为表示启动参数的字符串,路径和参数之间用 （空格)&& （空格)分隔
        /// </summary>
        /// <param name="param">要转换的启动参数</param>
        static public explicit  operator string (AppStartParameter param)
        {
            return param.ToString();
        }
    }
}
