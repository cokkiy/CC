//******************************************************
//C3IFrame: 本文件实现了C3I帧格式处理方法
// cokkiy 2015-09-17
#include "C3IFrame.h"
#include "FrameHeader.h"
#include <QDebug>


///************************************
/// 方法:    C3IFrame
/// 可访问性:    public 
/// <param name="channelConfig">主备切换配置表</param>
/// <param name="inPrimaryChannelThread">是否在主用通道线程中,
/// <c>true</c>表示在主用通道线程中,<c>false</c>表示在备用通道线程中</param>
/// <returns></returns>
/// <remarks>构造函数,初始化C3IFrame处理函数</remarks>
/// 作者：cokkiy（张立民)
/// 创建时间：2015/09/22 15:57:31
///************************************
C3IFrame::C3IFrame(ChannelSelectConfig *channelConfig, bool inPrimaryChannelThread/* =true */)
{
    this->channelConfig = channelConfig;
    this->inPrimaryChannelThread = inPrimaryChannelThread;
}


C3IFrame::~C3IFrame()
{
}

///************************************
/// 方法:    setParamInfo
/// 可访问性:    public 
/// <param name="paramInfo">C3I参数约定表参数信息</param>
/// <returns>void</returns>
/// <remarks>设置处理需要的从xml配置文件中装载的参数信息</remarks>
/// 作者：cokkiy（张立民)
/// 创建时间：2015/09/22 16:01:46
///************************************
void C3IFrame::setParamInfo(CParamInfoRT* paramInfo)
{
    this->m_pParamInfoRT = paramInfo;
}


///************************************
/// 方法:    parse
/// 可访问性:    public 
/// <param name="buf">数据缓冲区</param>
/// <param name="frameLen">处理帧总长度</param>
/// <returns>bool</returns>
/// <remarks>分析输入数据,生成指显参数</remarks>
/// 作者：cokkiy（张立民)
/// 创建时间：2015/09/22 16:14:09
///************************************
bool C3IFrame::parse(char* buf, unsigned short frameLen)
{
    //首先获取 921 c3i帧头
    C3IFrameHeader* pfhead = (C3IFrameHeader*)buf;

    if (pfhead->datalen + sizeof(C3IFrameHeader) > frameLen)
    {
        //帧错误,帧比数据长
        qWarning() << QObject::tr("Frame error, Frame length larger than data length.");
        return false;
    }

    //指向未处理数据域的指针
    char* prevbuffer = buf + 24;

    //处理的（子)帧位置
    unsigned short procFramePos = 0;

    switch (pfhead->frame_type)//帧类别
    {
    case 0://00h：表示本帧是多表完整信息帧；				
        while (procFramePos < pfhead->datalen && prevbuffer != NULL)
        {
            //处理位置未到整个帧尾部并且还有数据需要处理
            //处理完整帧中的一个子帧（参数)
            prevbuffer = this->ProcChildFrame_WZ(prevbuffer, pfhead, procFramePos);
            if (prevbuffer == 0)
            {
                //信源%1完整帧%2第%3字节处前后异常
                QString str("SourceID:%1, Full Frame:%2, Error occured at:%3");
                qWarning() << str.arg(pfhead->sourse).arg(pfhead->framecounter).arg(procFramePos + 24);
                break;
            }
        }
        break;
    case 1://01h：表示本帧是多表挑点信息帧；
        while (procFramePos < pfhead->datalen && prevbuffer)
        {
            prevbuffer = this->ProcChildFrame_TD(prevbuffer, pfhead, procFramePos);
            if (prevbuffer == 0)
            {
                //信源%1挑点帧%2第%3字节处前后异常
                QString str("SourceID:%1, Point Frame:%2, Error occured at:%3");
                qWarning() << str.arg(pfhead->sourse).arg(pfhead->framecounter).arg(procFramePos + 24);
                break;
            }
        }
        break;
    case 4://04h：表示本帧是指挥信息帧；
        while (procFramePos < pfhead->datalen && prevbuffer)
        {
            prevbuffer = this->ProcChildFrame_ZH(prevbuffer, pfhead, procFramePos, pfhead->time);
            if (prevbuffer == 0)
            {
                //信源%1指挥帧%2第%3字节处前后异常
                QString str("SourceID:%1, Command Frame:%2, Error occured at:%3");
                qWarning() << str.arg(pfhead->sourse).arg(pfhead->framecounter).arg(procFramePos + 24);
                break;
            }
        }
        break;
    case 0x11://11h：带数长挑点信息帧
        while (procFramePos < pfhead->datalen && prevbuffer)
        {
            prevbuffer = this->ProcChildFrame_AddLength_TD(prevbuffer, pfhead, procFramePos);
            if (prevbuffer == 0)
            {
                //信源%1挑点帧%2第%3字节处前后异常
                QString str("SourceID:%1, Point with Data Length Frame:%2, Error occured at:%3");
                qWarning() << str.arg(pfhead->sourse).arg(pfhead->framecounter).arg(procFramePos + 24);

                break;
            }
        }
        break;
    case 0xf3://f3h：指显历史曲线数据应答帧
    {
        //先取到表号 参数号
        //将整块拷贝
        //判结束尾
        //得到结束尾直接显示
        //没有得到结束尾就等待10秒，由等待状态转为绘制状态
        //先取到表号 参数号
        //将整块拷贝
        //判结束尾
        //得到结束尾直接显示
        //没有得到结束尾就等待10秒，由等待状态转为绘制状态                   

    }
    break;
    default:
        break;
    }
    return true;
}


///************************************
/// 方法:    setChannel
/// 可访问性:    public 
/// <param name="isInPrimaryChannel">是否在主用通道中</param>
/// <returns>void</returns>
/// <remarks>设置是否在主用通道线程中调用</remarks>
/// 作者：cokkiy（张立民)
/// 创建时间：2015/09/21 22:02:48
///************************************
void C3IFrame::setChannel(bool isInPrimaryChannel/*=true*/)
{
    this->inPrimaryChannelThread = isInPrimaryChannel;
}

///************************************
/// 方法:    ProcChildFrame_WZ
/// 可访问性:    private 
/// <param name="pBuffer">数据缓存区</param>
/// <param name="pFHead"> 当前帧帧头</param>
/// <param name="procFramePos">当前处理位置</param>
/// <returns>char* 指向剩余未处理的数据的指针</returns>
/// <remarks>多表完整信息帧处理函数</remarks>
/// 作者：cokkiy（张立民)
/// 创建时间：2015/09/21 21:21:56
///************************************
char* C3IFrame::ProcChildFrame_WZ(char *pBuffer, C3IFrameHeader* pFHead, 
    unsigned short& procFramePos)
{
    long childFrameLen = 0;//子帧的数据区长
    unsigned long childFrameTime = 0;//子帧时间
    unsigned short paramTable = 0;//表号
    unsigned short paramCode = 0;//参数号
    unsigned short paramLen = 0;//参数长
    unsigned char transType = 0;//参数传输类型
    unsigned long paramDate = *(unsigned int*)&pFHead->date;

    //子帧的数据区长
    childFrameLen = *(unsigned short*)pBuffer;
    pBuffer += 2;
    procFramePos += 2;
    //子帧时间
    childFrameTime = *(unsigned int*)pBuffer;
    pBuffer += 4;
    procFramePos += 4;
    //表号
    paramTable = *(unsigned short*)pBuffer;
    pBuffer += 2;
    procFramePos += 2;

    if (childFrameLen + procFramePos > pFHead->datalen)
        return 0;

    if (*(unsigned short*)(pBuffer + childFrameLen) != 0xffff)//子帧尾不对
        return 0;
    //当前为主用线程，表号使用主用信道，则处理
    //当前为备用线程，表号使用备用信道，则处理
    if (checkChannelToContinue(paramTable))
    {
        //开始处理子帧的数据
        while (childFrameLen > 0)
        {
            paramCode++;//完整帧参数号自动累加
            AbstractParam* t_param = m_pParamInfoRT->GetParam(paramTable, paramCode);
            if(t_param==NULL)
                break;
            transType = t_param->GetParamTranType();

            if (transType != 10 &&
                transType != 11)//非 字符串、原码
            {
                paramLen = t_param->GetParamDataLen();
                if (paramLen == 0)//没有这个参数
                {
                    //信源%1完整帧%2第%3字节处前后异常%4号表%5参数不存在
                    QString str("SourceID:%1, Full Frame:%2, Error occured at:%3. Can't find TableNo:%4 or ParaNo:%5");
                    qWarning() << str.arg(pFHead->sourse).arg(pFHead->framecounter).arg(procFramePos + 24).arg(paramTable).arg(paramCode);

                   
                    procFramePos = procFramePos + childFrameLen + 2;
                    return pBuffer + childFrameLen + 2;
                }
                if (childFrameLen >= paramLen)
                {
                    char* pBuffer_temp = m_pParamInfoRT->SetParamValue(
                        t_param, pBuffer, childFrameTime, paramDate);
                    if (pBuffer_temp == 0)
                    {
                        //信源%1完整帧%2第%3字节处前后异常%4号表%5参数设置失败
                        QString str("SourceID:%1, Full Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 SetParamValue failure");
                        qWarning() << str.arg(pFHead->sourse).arg(pFHead->framecounter).arg(procFramePos + 24).arg(paramTable).arg(paramCode);

                        procFramePos = procFramePos + childFrameLen + 2;
                        return pBuffer + childFrameLen + 2;
                    }
                    procFramePos += paramLen;
                    childFrameLen -= paramLen;
                    pBuffer = pBuffer_temp;
                }
                else
                {
                    //信源%1完整帧%2第%3字节处前后异常%4号表%5参数
                    QString str("SourceID:%1, Full Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 Unknown Error");
                    qWarning() << str.arg(pFHead->sourse).arg(pFHead->framecounter).arg(procFramePos + 24).arg(paramTable).arg(paramCode);

                    
                    procFramePos = procFramePos + childFrameLen + 2;
                    return pBuffer + childFrameLen + 2;
                }
            }//非 字符串、原码
            else//字符串、原码
            {
                if (childFrameLen >= 3)
                {
                    char* pBuffer_temp = m_pParamInfoRT->SetParamValue(
                        t_param, pBuffer, childFrameTime, paramDate);
                    if (pBuffer_temp == 0)
                    {
                        //信源%1完整帧%2第%3字节处前后异常%4号表%5参数
                        QString str("SourceID:%1, Full Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 SetParamValue failure");
                        qWarning() << str.arg(pFHead->sourse).arg(pFHead->framecounter).arg(procFramePos + 24)
                            .arg(paramTable).arg(paramCode);
                        
                        procFramePos = procFramePos + childFrameLen + 2;
                        return pBuffer + childFrameLen + 2;
                    }
                    procFramePos += pBuffer_temp - pBuffer;
                    childFrameLen -= pBuffer_temp - pBuffer;
                    pBuffer = pBuffer_temp;
                }
                else
                {
                    //信源%1完整帧%2第%3字节处前后异常%4号表%5参数
                    QString str("SourceID:%1, Full Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 Unknown Error");
                    qWarning() << str.arg(pFHead->sourse).arg(pFHead->framecounter).arg(procFramePos + 24)
                        .arg(paramTable).arg(paramCode);
                   
                    procFramePos = procFramePos + childFrameLen + 2;
                    return pBuffer + childFrameLen + 2;
                }
            }

        }//end while(usChildFrameLen > 0)
        procFramePos += 2;
        pBuffer += 2;
        return pBuffer;
    }
    //否则不处理，直接跳到下一子帧
    else
    {
        pBuffer = pBuffer + childFrameLen + 2;
        procFramePos = procFramePos + childFrameLen + 2;
        return pBuffer;
    }

}

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
char* C3IFrame::ProcChildFrame_TD(char *pBuffer, C3IFrameHeader* pFHead, unsigned short& procFramePos)
{
    long lChildFrameLen = 0;//子帧剩余长度
    unsigned long ulChildFrameTime = 0;//子帧时间
    unsigned short usParamTable = 0;//表号
    unsigned short usParamCode = 0;//参数号
    unsigned short usParamLen = 0;//参数长
    unsigned char ucTransType = 0;//参数传输类型
    unsigned long ulParamDate = *(unsigned int*)&pFHead->date;

    //子帧数据区长度
    lChildFrameLen = *(unsigned short*)pBuffer;
    pBuffer += 2;
    procFramePos += 2;

    ulChildFrameTime = *(unsigned int*)pBuffer;
    pBuffer += 4;
    procFramePos += 4;

    usParamTable = *(unsigned short*)pBuffer;
    pBuffer += 2;
    procFramePos += 2;

    if (lChildFrameLen + procFramePos > pFHead->datalen)
        return 0;

    if (*(unsigned short*)(pBuffer + lChildFrameLen) != 0xffff)//子帧尾不对
        return 0;

    if (checkChannelToContinue(usParamTable))
    {
        //开始处理子帧的数据
        while (lChildFrameLen > 0)
        {
            //挑点帧要取参数号
            if (lChildFrameLen < 2)
            {
                //信源%1挑点帧%2第%3字节处前后异常%4号表%5参数
                QString str("SourceID:%1, Point Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 Data Length Error");
                qWarning() << str.arg(pFHead->sourse).arg(pFHead->framecounter).arg(procFramePos + 24).arg(usParamTable).arg(usParamCode);
                
                procFramePos = procFramePos + lChildFrameLen + 2;
                return pBuffer + lChildFrameLen + 2;
            }
            usParamCode = *(unsigned short*)pBuffer;
            pBuffer += 2;
            procFramePos += 2;
            lChildFrameLen -= 2;
            AbstractParam* t_param = m_pParamInfoRT->GetParam(usParamTable, usParamCode);
            if(t_param==NULL)
                break;
            ucTransType = t_param->GetParamTranType();

            if (ucTransType != 10 && ucTransType != 11)//非 字符串、原码
            {
                usParamLen = t_param->GetParamDataLen();
                if (usParamLen == 0)//没有这个参数
                {
                    //信源%1挑点帧%2第%3字节处前后异常%4号表%5参数不存在
                    QString str("SourceID:%1, Point Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 Can't find config info");
                    qWarning() << str.arg(pFHead->sourse).arg(pFHead->framecounter).arg(procFramePos + 24).arg(usParamTable).arg(usParamCode);

                    procFramePos = procFramePos + lChildFrameLen + 2;
                    return pBuffer + lChildFrameLen + 2;
                }
                if (lChildFrameLen >= usParamLen)
                {
                    char* pBuffer_temp = m_pParamInfoRT->SetParamValue(
                        t_param, pBuffer, ulChildFrameTime, ulParamDate);
                    if (pBuffer_temp == 0)
                    {
                        //信源%1挑点帧%2第%3字节处前后异常%4号表%5参数设置失败
                        QString str("SourceID:%1, Point Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 SetParamValue Error");
                        qWarning() << str.arg(pFHead->sourse).arg(pFHead->framecounter).arg(procFramePos + 24).arg(usParamTable).arg(usParamCode);

                        procFramePos = procFramePos + lChildFrameLen + 2;
                        return pBuffer + lChildFrameLen + 2;
                    }
                    procFramePos += usParamLen;
                    lChildFrameLen -= usParamLen;
                    pBuffer = pBuffer_temp;
                }
                else
                {
                    //信源%1挑点帧%2第%3字节处前后异常%4号表%5参数
                    QString str("SourceID:%1, Point Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 Unknown Error");
                    qWarning() << str.arg(pFHead->sourse).arg(pFHead->framecounter).arg(procFramePos + 24).arg(usParamTable).arg(usParamCode);

                    procFramePos = procFramePos + lChildFrameLen + 2;
                    return pBuffer + lChildFrameLen + 2;
                }
            }//非 字符串、原码
            else//字符串、原码
            {
                if (lChildFrameLen >= 3)
                {
                    char* pBuffer_temp = m_pParamInfoRT->SetParamValue(
                        t_param, pBuffer, ulChildFrameTime, ulParamDate);
                    if (pBuffer_temp == 0)
                    {
                        //信源%1挑点帧%2第%3字节处前后异常%4号表%5参数
                        QString str("SourceID:%1, Point Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 SetParamValue Error");
                        qWarning() << str.arg(pFHead->sourse).arg(pFHead->framecounter).arg(procFramePos + 24).arg(usParamTable).arg(usParamCode);

                        procFramePos = procFramePos + lChildFrameLen + 2;
                        return pBuffer + lChildFrameLen + 2;
                    }
                    procFramePos += pBuffer_temp - pBuffer;
                    lChildFrameLen -= pBuffer_temp - pBuffer;
                    pBuffer = pBuffer_temp;
                }
                else
                {
                    //信源%1挑点帧%2第%3字节处前后异常%4号表%5参数
                    QString str("SourceID:%1, Point Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 Data Length Error");
                    qWarning() << str.arg(pFHead->sourse).arg(pFHead->framecounter).arg(procFramePos + 24).arg(usParamTable).arg(usParamCode);
                
                    procFramePos = procFramePos + lChildFrameLen + 2;
                    return pBuffer + lChildFrameLen + 2;
                }
            }

        }//end while(usChildFrameLen > 0)
        procFramePos += 2;
        pBuffer += 2;
        return pBuffer;
    }
    else
    {
        procFramePos = procFramePos + lChildFrameLen + 2;
        pBuffer = pBuffer + lChildFrameLen + 2;
        return pBuffer;

    }
}



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
char* C3IFrame::ProcChildFrame_ZH(char *pBuffer, C3IFrameHeader* pFHead, unsigned short& procFramePos, unsigned long ulFrameTime)
{
    unsigned char ucChildFrameInfo = 0;
    long lChildFrameLen = 0;//子帧剩余长度
    unsigned short usParamTable = 0;//表号
    unsigned short usParamCode = 0;//参数号
    unsigned short usParamLen = 0;//参数长
    unsigned char ucTransType = 0;//参数传输类型
    unsigned long ulParamDate = *(unsigned int*)&pFHead->date;

    /*	if(usProcFrameLen < 7 )//不够一个子帧
    return 0;*/

    ucChildFrameInfo = *(unsigned char*)pBuffer;
    pBuffer += 1;
    procFramePos += 1;

    lChildFrameLen = *(unsigned short*)pBuffer;
    pBuffer += 2;
    procFramePos += 2;

    usParamTable = *(unsigned short*)pBuffer;
    pBuffer += 2;
    procFramePos += 2;

    if (lChildFrameLen + procFramePos > pFHead->datalen)
        return 0;

    if (*(unsigned short*)(pBuffer + lChildFrameLen) != 0xffff)//子帧尾不对
        return 0;
    if (checkChannelToContinue(usParamTable))
    {
        //开始处理子帧的数据
        while (lChildFrameLen > 0)
        {
            //指挥帧要取参数号
            if (lChildFrameLen < 2)
            {
                //信源%1指挥帧%2第%3字节处前后异常%4号表%5参数
                QString str("SourceID:%1, Command Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 Data Length Error");
                qWarning() << str.arg(pFHead->sourse).arg(pFHead->framecounter).arg(procFramePos + 24).arg(usParamTable).arg(usParamCode);

                procFramePos = procFramePos + lChildFrameLen + 2;
                return pBuffer + lChildFrameLen + 2;
            }
            usParamCode = *(unsigned short*)pBuffer;
            pBuffer += 2;
            procFramePos += 2;
            lChildFrameLen -= 2;
            AbstractParam* t_param = m_pParamInfoRT->GetParam(usParamTable, usParamCode);
            if(t_param==NULL)
                break;
            ucTransType = t_param->GetParamTranType();

            if (ucTransType != 10 &&
                ucTransType != 11)//非 字符串、原码
            {
                usParamLen = t_param->GetParamDataLen();
                if (usParamLen == 0)//没有这个参数
                {
                    //信源%1指挥帧%2第%3字节处前后异常%4号表%5参数不存在
                    QString str("SourceID:%1, Command Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 Can't find config info");
                    qWarning() << str.arg(pFHead->sourse).arg(pFHead->framecounter).arg(procFramePos + 24).arg(usParamTable).arg(usParamCode);

                    procFramePos = procFramePos + lChildFrameLen + 2;
                    return pBuffer + lChildFrameLen + 2;
                }
                if (lChildFrameLen >= usParamLen)
                {
                    char* pBuffer_temp = m_pParamInfoRT->SetParamValue(
                        t_param, pBuffer, ulFrameTime, ulParamDate);
                    if (pBuffer_temp == 0)
                    {
                        //信源%1指挥帧%2第%3字节处前后异常%4号表%5参数不存在
                        QString str("SourceID:%1, Command Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 SetParamValue error");
                        qWarning() << str.arg(pFHead->sourse).arg(pFHead->framecounter).arg(procFramePos + 24).arg(usParamTable).arg(usParamCode);

                        procFramePos = procFramePos + lChildFrameLen + 2;
                        return pBuffer + lChildFrameLen + 2;
                    }
                    procFramePos += usParamLen;
                    lChildFrameLen -= usParamLen;
                    pBuffer = pBuffer_temp;
                }
                else
                {
                    //信源%1指挥帧%2第%3字节处前后异常%4号表%5参数不存在
                    QString str("SourceID:%1, Command Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 Data Length error");
                    qWarning() << str.arg(pFHead->sourse).arg(pFHead->framecounter).arg(procFramePos + 24).arg(usParamTable).arg(usParamCode);

                    procFramePos = procFramePos + lChildFrameLen + 2;
                    return pBuffer + lChildFrameLen + 2;
                }
            }//非 字符串、原码
            else//字符串、原码
            {
                if (lChildFrameLen >= 3)
                {
                    char* pBuffer_temp = m_pParamInfoRT->SetParamValue(
                        t_param, pBuffer, ulFrameTime, ulParamDate);
                    if (pBuffer_temp == 0)
                    {
                        //信源%1指挥帧%2第%3字节处前后异常%4号表%5参数设置失败
                        QString str("SourceID:%1, Command Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 SetParamValue Error");
                        qWarning() << str.arg(pFHead->sourse).arg(pFHead->framecounter).arg(procFramePos + 24).arg(usParamTable).arg(usParamCode);

                        procFramePos = procFramePos + lChildFrameLen + 2;
                        return pBuffer + lChildFrameLen + 2;
                    }
                    procFramePos += pBuffer_temp - pBuffer;
                    lChildFrameLen -= pBuffer_temp - pBuffer;
                    pBuffer = pBuffer_temp;
                }
                else
                {
                    //信源%1指挥帧%2第%3字节处前后异常%4号表%5参数
                    QString str("SourceID:%1, Command Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 Data Length Error");
                    qWarning() << str.arg(pFHead->sourse).arg(pFHead->framecounter).arg(procFramePos + 24).arg(usParamTable).arg(usParamCode);

                    procFramePos = procFramePos + lChildFrameLen + 2;
                    return pBuffer + lChildFrameLen + 2;
                }
            }

        }//end while(usChildFrameLen > 0)
        procFramePos += 2;
        pBuffer += 2;
        return pBuffer;
    }
    else
    {
        procFramePos = procFramePos + lChildFrameLen + 2;
        pBuffer = pBuffer + lChildFrameLen + 2;
        return pBuffer;
    }

}


//C3I 带数长挑点帧
char* C3IFrame::ProcChildFrame_AddLength_TD(char *pBuffer, C3IFrameHeader *pFHead, unsigned short &usProcFrameLen)
{
    long lChildFrameLen = 0;//子帧剩余长度
    unsigned long ulChildFrameTime = 0;//子帧时间
    unsigned short usParamTable = 0;//表号
    unsigned short usParamCode = 0;//参数号
    unsigned char ParamCodeLength;//帧中自带参数长度
    unsigned short usParamLen = 0;//参数长
    unsigned char ucTransType = 0;//参数传输类型
    unsigned long ulParamDate = *(unsigned int*)&pFHead->date;

    //子帧数据区长度
    lChildFrameLen = *(unsigned short*)pBuffer;
    pBuffer += 2;
    usProcFrameLen += 2;

    ulChildFrameTime = *(unsigned long*)pBuffer;
    pBuffer += 4;
    usProcFrameLen += 4;

    usParamTable = *(unsigned short*)pBuffer;
    pBuffer += 2;
    usProcFrameLen += 2;

    if (lChildFrameLen + usProcFrameLen > pFHead->datalen)
        return 0;

    if (*(unsigned short*)(pBuffer + lChildFrameLen) != 0xffff)//子帧尾不对
        return 0;

    if (checkChannelToContinue(usParamTable))
    {
        //开始处理子帧的数据
        while (lChildFrameLen > 0)
        {
            //挑点帧要取参数号、数长
            if (lChildFrameLen < 3)
            {
                //信源%1带数长挑点帧第%2帧第%3字节处前后异常：表号%4帧长不够
                QString str("SourceID:%1, Point with Data length Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 Frame Length Error");
                qWarning() << str.arg(pFHead->sourse).arg(pFHead->framecounter).arg(usProcFrameLen + 24).arg(usParamTable);
                
                usProcFrameLen = usProcFrameLen + lChildFrameLen + 2;
                return pBuffer + lChildFrameLen + 2;
            }
            //参数号
            usParamCode = *(unsigned short*)pBuffer;
            pBuffer += 2;
            usProcFrameLen += 2;
            lChildFrameLen -= 2;

            //帧内自带参数长度
            ParamCodeLength = *(unsigned char*)pBuffer;
            pBuffer += 1;
            usProcFrameLen += 1;
            lChildFrameLen -= 1;

            AbstractParam* t_param = (AbstractParam*)m_pParamInfoRT->GetParam(usParamTable, usParamCode);

            //若指针为NULL，则不处理此参数，跳到帧的下一参数继续处理
            if (t_param == NULL)
            {
                //信源%1带数长挑点帧第%2帧第%3字节处前后异常：表号%4字段号%5异常
                QString str("SourceID:%1, Point with Data length Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 Unknown Error");
                qWarning() << str.arg(pFHead->sourse).arg(pFHead->datalen).arg(usProcFrameLen + 24).arg(usParamTable).arg(usParamCode);
                
                usProcFrameLen += ParamCodeLength;
                lChildFrameLen -= ParamCodeLength;
                pBuffer += ParamCodeLength;
                continue;
            }
            else if (t_param != NULL)
            {
                ucTransType = t_param->GetParamTranType();
                usParamLen = t_param->GetParamDataLen();
                if (!CheckParam(t_param))
                {
                    //信源%1带数长挑点帧第%2帧第%3字节处前后异常表号%4参数号%5类型与字长不一致不处理该子帧
                    QString str = QObject::tr("SourceID:%1, Point with Data length Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 Frame type and Length no match");
                    qWarning() << str.arg(pFHead->sourse).arg(pFHead->datalen).arg(usProcFrameLen + 24).arg(usParamTable).arg(usParamCode);

                }
                if (ucTransType != 10 && ucTransType != 11)//非 字符串、原码
                {
                    if (ParamCodeLength != usParamLen)
                    {
                        if (ParamCodeLength > lChildFrameLen)
                        {
                            //信源%1带数长挑点帧第%2帧第%3字节处前后异常：表号%4的字段号%5帧长错误
                            QString str("SourceID:%1, Point with Data length Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 Frame Length Error");
                            qWarning() << str.arg(pFHead->sourse).arg(pFHead->datalen).arg(usProcFrameLen + 24).arg(usParamTable).arg(usParamCode);

                            usProcFrameLen = usProcFrameLen + lChildFrameLen + 2;
                            return pBuffer + lChildFrameLen + 2;
                        }
                        usProcFrameLen += ParamCodeLength;
                        lChildFrameLen -= ParamCodeLength;
                        pBuffer += ParamCodeLength;
                        continue;
                    }
                    else
                    {
                        if (lChildFrameLen >= usParamLen)
                        {
                            char* pBuffer_temp = m_pParamInfoRT->SetParamValue(
                                t_param, pBuffer, ulChildFrameTime, ulParamDate);
                            if (pBuffer_temp == 0)
                            {
                                //信源%1带数长挑点帧第%2帧第%3字节处前后异常：表号%4字段号%5异常
                                QString str("SourceID:%1, Point with Data length Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 SetParamValue Error");
                                qWarning() << str.arg(pFHead->sourse).arg(pFHead->datalen).arg(usProcFrameLen + 24).arg(usParamTable).arg(usParamCode);

                                usProcFrameLen += usParamLen;
                                lChildFrameLen -= usParamLen;
                                pBuffer += usParamLen;
                                continue;
                            }
                            usProcFrameLen += usParamLen;
                            lChildFrameLen -= usParamLen;
                            pBuffer = pBuffer_temp;
                        }
                        else
                        {
                            //信源%1带数长挑点帧第%2帧第%3字节处前后异常：表号%4的字段号%5帧长不足
                            QString str("SourceID:%1, Point with Data length Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 Data length Error");
                            qWarning() << str.arg(pFHead->sourse).arg(pFHead->datalen).arg(usProcFrameLen + 24).arg(usParamTable).arg(usParamCode);
                      
                            usProcFrameLen = usProcFrameLen + lChildFrameLen + 2;
                            return pBuffer + lChildFrameLen + 2;
                        }
                    }
                }
                else//字符串、原码
                {
                    if (lChildFrameLen >= 3)
                    {
                        char* pBuffer_temp = m_pParamInfoRT->SetParamValue(
                            t_param, pBuffer, ulChildFrameTime, ulParamDate);
                        if (pBuffer_temp == 0)
                        {
                            //信源%1带数长挑点帧第%2帧第%3字节处前后异常：表号%4字段号%5异常
                            QString str("SourceID:%1, Point with Data length Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 Unknown Error");
                            qWarning() << str.arg(pFHead->sourse).arg(pFHead->datalen).arg(usProcFrameLen + 24).arg(usParamTable).arg(usParamCode);
                       
                            usParamLen = *(unsigned short*)pBuffer;
                            usProcFrameLen = usProcFrameLen + usParamLen + 3;
                            lChildFrameLen = lChildFrameLen - usParamLen - 3;
                            pBuffer = pBuffer + usParamLen + 3;
                            continue;
                        }
                        usProcFrameLen += pBuffer_temp - pBuffer;
                        lChildFrameLen -= pBuffer_temp - pBuffer;
                        pBuffer = pBuffer_temp;
                    }
                    else
                    {
                        //信源%1带数长挑点帧第%2帧第%3字节处前后异常：表号%4的字段号%5帧长不足
                        QString str("SourceID:%1, Point with Data length Frame:%2, Error occured at:%3. TableNo:%4 and ParaNo:%5 Frame Length Error");
                        qWarning() << str.arg(pFHead->sourse).arg(pFHead->datalen).arg(usProcFrameLen + 24).arg(usParamTable).arg(usParamCode);

                        usProcFrameLen = usProcFrameLen + lChildFrameLen + 2;
                        return pBuffer + lChildFrameLen + 2;
                    }
                }
            }

        }//end while(usChildFrameLen > 0)
        usProcFrameLen += 2;
        pBuffer += 2;
        return pBuffer;
    }
    else
    {
        usProcFrameLen = usProcFrameLen + lChildFrameLen + 2;
        pBuffer = pBuffer + lChildFrameLen + 2;
        return pBuffer;
    }
}

//检查指定表号的主备配置,看是否需要继续处理
bool C3IFrame::checkChannelToContinue(uint tableNo)
{
    return inPrimaryChannelThread == channelConfig->usingPrimaryChannelData(tableNo);
}

/*!
检查参数是否正确
@param stru_Param * param
@return bool
作者：cokkiy（张立民)
创建时间：2015/09/28 15:49:10
*/
bool C3IFrame::CheckParam(AbstractParam* param)
{
    if (param == NULL)
        return false;

    switch (param->GetParamTranType())
    {
    case tp_char:
    case tp_BYTE:
        return param->GetParamDataLen() == 1;
    case tp_short:
    case tp_WORD:
        return param->GetParamDataLen() == 2;
    case tp_long:
    case tp_DWORD:
    case tp_PMTime:
    case tp_Date:
    case tp_BCDTime:
    case tp_float:
    case tp_longE:
    case tp_UTCTime:
        return param->GetParamDataLen() == 4;
    case tp_double:
    case tp_ulong8:
    case tp_long8:
        return param->GetParamDataLen() == 8;
        //字符串与源码的参数长度会被改变，不能作为判断的依据
    case tp_String:
    case tp_Code:
        break;
    default:
        return false;
    }
    return true;
}


