using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CC_StationService
{
    /// <summary>
    /// 气象图类型
    /// </summary>
    public enum PictureType
    {
        /// <summary>
        /// 未知
        /// </summary>
        Unknown,
        /// <summary>
        /// 风云2D
        /// </summary>
        FY2D,
        /// <summary>
        /// 风云2E
        /// </summary>
        FY2E,
        /// <summary>
        /// 风云2G
        /// </summary>
        FY2G,
        /// <summary>
        /// 气象室雷达平扫图
        /// </summary>
        WDRadarPlainGraph,
        /// <summary>
        /// 气象室雷达垂扫图
        /// </summary>
        WDRadarVerticalSweep,
        /// <summary>
        /// 大树里雷达平扫图
        /// </summary>
        DSLRadarPlainGraph,
        /// <summary>
        /// 大树里雷达垂扫图
        /// </summary>
        DSLRadarVerticalSweep,
        /// <summary>
        /// 大树里雷达速度谱宽
        /// </summary>
        DSLRadarVelocitySpectrum

    }
}
