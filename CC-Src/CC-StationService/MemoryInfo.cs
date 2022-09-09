using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CC_StationService
{
    /// <summary>
    /// 内存信息
    /// </summary>
    struct MemoryInfo
    {
        /// <summary>
        /// 总内存（字节)
        /// </summary>
        public long Total { get; set; }
        /// <summary>
        /// 未分配内存（空闲内存)（字节)
        /// </summary>
        public long Free { get; set; }

        /// <summary>
        /// Buffers memory in Bytes
        /// </summary>
        public long Buffers { get; set; }

        /// <summary>
        /// Cached memory in Bytes
        /// </summary>
        public long Cached { get; set; }

        /// <summary>
        /// 获取被使用的内存总量（字节)
        /// </summary>
        public long Used
        {
            get
            {
                return Total - Free - Buffers - Cached;
            }
        }
    }
}
