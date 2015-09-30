//******************************************************************
//文件名:PDXPFrame.h 
// 功能：提供解析PDXP包的所有方法
// 作者：cokkiy 张立民
// 日期:2015-09-28
// Version:1.0
//*****************************************************************
#pragma once

#include "FrameHeader.h"
#include "C3I/stru.h"
class CParamInfoRT;
class ChannelSelectConfig;
//PDXP包解析类，提供解析PDXP包的所有方法
class PDXPFrame
{
public:
    //初始化C3IFrame类,传递网络配置文件指针和是否在主用通道处理线程内
    PDXPFrame(ChannelSelectConfig *channelConfig, bool inPrimaryChannelThread = true);
    ~PDXPFrame();

    //设置C3I参数信息
    void setParamInfo(CParamInfoRT* paramInfo);

    //分析输入数据,生产指显参数
    bool parse(char* buf, unsigned short frameLen);

private:
    //网络配置文件
    ChannelSelectConfig *channelConfig;
    //是否在主用通道处理线程内
    bool inPrimaryChannelThread;

    //C3I参数约定表
    CParamInfoRT* m_pParamInfoRT;

    //表示本帧是多单表挑点数据帧
    char* ProcFrame_MultiSTable_TD(char *pBuffer,
        PDXPFrameHeader *pFHead,
        unsigned short &procFramePos);

    //表示本帧是多表完整数据帧
    char* ProcFrame_MultiTable_WZ(char *pBuffer,
        PDXPFrameHeader *pFHead,
        unsigned short &procFramePos);

    //表示本帧是多表挑点数据帧
    void ProcFrame_MultiTable_TD(char *pBuffer,
        PDXPFrameHeader *pFHead,
        unsigned short &procFramePos);

    //表示本帧是单表挑点数据帧
    void ProcFrame_STable_TD(char *pBuffer,
        PDXPFrameHeader *pFHead,
        unsigned short &procFramePos);

    //表示本帧是单表完整信息帧
    void ProcFrame_STable_WZ(char *pBuffer,
        PDXPFrameHeader* pFHead,
        unsigned short& procFramePos);

    //************************************
    // 方法:    checkChannelToContinue
    // 可访问性:    private 
    // 返回值:   bool
    // 参数: uint tableNo
    // 说明：检查指定表号的主备配置,看是否需要继续处理
    // 返回值说明：如果符合主备配置表设置,返回true,否则返回false
    // 创建时间：2015/09/18
    //************************************
    bool checkChannelToContinue(unsigned int tableNo);

    //检查参数是否正确
    bool CheckParam(stru_Param* param);

    //将日期转换为BCD码格式
    unsigned int ChangeDateToBCD(unsigned int uldate);
};

