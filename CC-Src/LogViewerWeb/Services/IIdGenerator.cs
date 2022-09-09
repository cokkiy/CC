using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace LogViewerWeb.Services
{
    /// <summary>
    /// Id产生器接口
    /// </summary>
    public interface IIdGenerator
    {
        /// <summary>
        /// 获取长度为<paramref name="length"/>字符串Id
        /// </summary>
        /// <param name="length">Id长度</param>
        /// <returns>字符串Id,其长度为<paramref name="length"/></returns>
        string GetStringId(int length);

        /// <summary>
        /// 获取长度为<c><paramref name="strLength"/>+<paramref name="numLength"/></c>
        /// 的字符串和数字组成的Id
        /// </summary>
        /// <param name="strLength">字符串长度</param>
        /// <param name="numLength">数字长度</param>
        /// <returns>字符串和数字组成的Id，其长度为
        /// <c><paramref name="strLength"/>+<paramref name="numLength"/></c></returns>
        string GetStringNumberId(int strLength, int numLength);

        /// <summary>
        /// 获取一个由随机5个字符和5位数字组成的Key
        /// </summary>
        /// <returns>一个由随机5个字符和5位数字组成的key</returns>
        string GetSessionKey();
    }
}
