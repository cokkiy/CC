//******************************************************
//C3IFrame:定义了C3I帧格式处理类
// cokkiy 2015-09-17
// 根据原指显代码做适应性修改,移植到linux系统下
//*******************************************************
#pragma once
#include "abstractparam.h"
#include "selfshare/src/config/channelselectconfig.h"
#include "C3I/CParamInfoRT.h"
#include "FrameHeader.h"

class C3IFrame
{
public:
    //初始化C3IFrame类,传递网络配置文件指针和是否在主用通道处理线程内
    C3IFrame(ChannelSelectConfig *channelConfig, bool inPrimaryChannelThread=true);
    ~C3IFrame();

    //设置C3I参数信息
    void setParamInfo(CParamInfoRT* paramInfo);

    //分析输入数据,生产指显参数
    bool parse(char* buf, unsigned short frameLen);

    //设置是否在主用通道中
    void setChannel(bool isInPrimaryChannel=true);

private:
    //网络配置文件
    ChannelSelectConfig *channelConfig;
    //是否在主用通道处理线程内
    bool inPrimaryChannelThread;

    //C3I参数约定表
    CParamInfoRT* m_pParamInfoRT;

   
    ///************************************
    /// 方法:    ProcChildFrame_WZ
    /// 可访问性:    private 
    /// <param name="pBuffer">数据缓存区</param>
    /// <param name="pFHead"> 当前帧帧头</param>
    /// <param name="procFramePos">当前位置</param>
    /// <returns>char* 指向剩余未处理的数据的指针</returns>
    /// <remarks>多表完整信息帧处理函数</remarks>
    /// 作者：cokkiy（张立民)
    /// 创建时间：2015/09/21 21:21:56
    ///************************************
    char* ProcChildFrame_WZ(char *pBuffer,
        C3IFrameHeader* pFHead,
        unsigned short& usProcFrameLen);

    ///************************************
    /// 方法:    ProcChildFrame_TD
    /// 可访问性:    private 
    /// <param name="pBuffer">数据缓冲区</param>
    /// <param name="pFHead">帧头</param>
    /// <param name="procFramePos">当前处理位置</param>
    /// <returns>指向尚未处理的数据的指针</returns>
    /// <remarks>C3I 多单表挑点数据帧</remarks>
    /// 作者：cokkiy（张立民)
    /// 创建时间：2015/09/21 21:35:19
    ///************************************
    char* ProcChildFrame_TD(char *pBuffer,
        C3IFrameHeader* pFHead,
        unsigned short& usProcFrameLen);

    ///************************************
    /// 方法:    ProcChildFrame_TD
    /// 可访问性:    private 
    /// <param name="pBuffer">数据缓冲区</param>
    /// <param name="pFHead">帧头</param>
    /// <param name="procFramePos">当前处理位置</param>
    /// <returns>指向尚未处理的数据的指针</returns>
    /// <remarks>处理指挥子帧</remarks>
    /// 作者：cokkiy（张立民)
    /// 创建时间：2015/09/21 21:35:19
    ///************************************
    char* ProcChildFrame_ZH(char *pBuffer,
        C3IFrameHeader* pFHead,
        unsigned short& usProcFrameLen,
        unsigned long ulFrameTime);

    //处理带数长挑点帧
    char* ProcChildFrame_AddLength_TD(char *pBuffer,
        C3IFrameHeader* pFHead, unsigned short& usProcFrameLen);

    
    //************************************
    // 方法:    checkChannelToContinue
    // 可访问性:    private 
    // 返回值:   bool
    // 参数: uint tableNo
    // 说明：检查指定表号的主备配置,看是否需要继续处理
    // 返回值说明：如果符合主备配置表设置,返回true,否则返回false
    // 创建时间：2015/09/18
    //************************************
    bool checkChannelToContinue(uint tableNo);

    //检查参数是否正确
    bool CheckParam(AbstractParam* param);
};

