using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CC
{
    public partial class StationRunningState
    {
        /// <summary>
        /// 返回字符串表示的对象的值
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return string.Format("{0}......CPU:{1}  Memory:{2}  Processes Count:{3}", stationId, cpu, currentMemory, procCount);
        }
    }
}
