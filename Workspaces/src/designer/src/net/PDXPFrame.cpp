//******************************************************************
//文件名:PDXPFrame.cpp 
// 功能：提供解析PDXP包的所有方法
// 作者：cokkiy 张立民
// 日期:2015-09-28
// Version:1.0
//*****************************************************************
#include "PDXPFrame.h"
#include "FrameHeader.h"
#include "selfshare/src/config/channelselectconfig.h"
#include "C3I/stru.h"
#include "C3I/CParamInfoRT.h"
#include <QTime>
#include <QDebug>


/*!
构造函数，初始化PDXP包解析类实例
@param ChannelSelectConfig * channelConfig 主备配置信息
@param bool inPrimaryChannelThread 是否在主用处理线程中 
@return 
作者：cokkiy（张立民)
创建时间：2015/09/28 11:44:26
*/
PDXPFrame::PDXPFrame(ChannelSelectConfig *channelConfig, bool inPrimaryChannelThread)
{
    this->channelConfig = channelConfig;
    this->inPrimaryChannelThread = inPrimaryChannelThread;
}


PDXPFrame::~PDXPFrame()
{
}

/*!
设置信息配置表
@param CParamInfoRT * paramInfo  信息配置表
@return void
作者：cokkiy（张立民)
创建时间：2015/09/28 11:48:14
*/
void PDXPFrame::setParamInfo(CParamInfoRT* paramInfo)
{
    this->m_pParamInfoRT = paramInfo;
}


/*!
分析输入数据,生成指显参数并存入到显示缓冲队列中
@param char * buf 原始数据缓冲区
@param unsigned short frameLen  原始数据长度
@return bool 解析过程中是否发生错误
作者：cokkiy（张立民)
创建时间：2015/09/28 11:49:47
*/
bool PDXPFrame::parse(char* buf, unsigned short frameLen)
{
    //首先，解析帧头
    PDXPFrameHeader* pFHead = (PDXPFrameHeader*)buf;
   
    if (pFHead->DataLength + sizeof(PDXPFrameHeader) > frameLen)
    {
        //帧错误,帧比数据长
        qWarning() << QObject::tr("Frame error, Frame length larger than data length.");
        return false;
    }

    //数据域
    char* pRevBuffer = buf + 32;
    //当前处理位置
    unsigned short procPos = 0;

    //判定数据类型，只使用末4位
    switch (pFHead->BID & 0x0f)
    {
    case 0x00://表示本帧是单表完整信息帧
        this->ProcFrame_STable_WZ(pRevBuffer, pFHead, procPos);
        break;
    case 0x01://表示本帧是单表挑点数据帧
        this->ProcFrame_STable_TD(pRevBuffer, pFHead, procPos);
        break;
    case 0x02://表示本帧是多表挑点数据帧
        this->ProcFrame_MultiTable_TD(pRevBuffer, pFHead, procPos);
        break;
    case 0x03://表示本帧是多表完整数据帧
        while (procPos < pFHead->DataLength && pRevBuffer)
        {
            pRevBuffer = this->ProcFrame_MultiTable_WZ(pRevBuffer, pFHead, procPos);
            if (pRevBuffer == 0)
            {
                break;
            }
        }
        break;
    case 0x04://表示本帧是多单表挑点数据帧
        while (procPos < pFHead->DataLength && pRevBuffer)
        {
            pRevBuffer = this->ProcFrame_MultiSTable_TD(pRevBuffer, pFHead, procPos);
            if (pRevBuffer == 0)
            {
                break;
            }
        }
        break;
    default:
        break;
    }

    return true;
}


/*!
PDXP 单表完整帧处理函数
@param char * pBuffer 数据缓冲区
@param PDXPFrameHeader * pFHead  帧头
@param unsigned short & procFramePos 当前处理位置
@return void
作者：cokkiy（张立民)
创建时间：2015/09/28 15:45:07
*/
void PDXPFrame::ProcFrame_STable_WZ(char *pBuffer, PDXPFrameHeader *pFHead, unsigned short &procFramePos)
{
    long lChildFrameLen = 0;//子帧剩余长度
    unsigned short usParamTable = 0;//表号
    unsigned short usParamCode = 0;//参数号
    unsigned short usParamLen = 0;//参数长
    unsigned char ucTransType = 0;//参数传输类型

    unsigned long ulParamDate = ChangeDateToBCD(pFHead->Date);
    //表号
    usParamTable = *(unsigned short*)pBuffer;
    pBuffer += 2;
    procFramePos += 2;

    //帧的数据区长,减去2字节的表号
    lChildFrameLen = pFHead->DataLength - 2;
    if (checkChannelToContinue(usParamTable))
    {
        //开始处理子帧的数据
        while (lChildFrameLen > 0)
        {
            usParamCode++;//完整帧参数号自动累加
            auto transType = m_pParamInfoRT->GetTransType(usParamTable, usParamCode);

            stru_Param* pParam = (stru_Param*)m_pParamInfoRT->GetParam(usParamTable, usParamCode);
            if (pParam == NULL)
            {
                QString str = QObject::tr("Error occured on PDXP single table full frame packet at %5.Source:%1, Frame:%2, Table:%3, ParamNo:%4");
                qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);
                
                procFramePos = procFramePos + lChildFrameLen;
                return;
            }
            ucTransType = pParam->ucTransType;
            if (!CheckParam(pParam))
            {
                QString str = QObject::tr("Error occured on PDXP single table full frame packet at %5.Source:%1, Frame:%2, Table:%3, ParamNo:%4. Frame type not match data length.");
                qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);
               
                return;
            }
            if (ucTransType != 10 && ucTransType != 11)//非 字符串、原码
            {
                usParamLen = pParam->usParamLen;
                if (usParamLen == 0)//没有这个参数
                {
                    QString str = QObject::tr("Error occured on PDXP single table full frame packet at %5.Source:%1, Frame:%2, Table:%3, ParamNo:%4. No such param.");
                    qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);

                    procFramePos = procFramePos + lChildFrameLen;
                    return;
                }
                if (lChildFrameLen >= usParamLen)
                {
                    char* pBuffer_temp = m_pParamInfoRT->SetParamValue(
                        usParamTable, usParamCode, pBuffer, pFHead->Time, ulParamDate);
                    if (pBuffer_temp == 0)
                    {
                        QString str = QObject::tr("Error occured on PDXP single table full frame packet at %5.Source:%1, Frame:%2, Table:%3, ParamNo:%4. Unknown error.");
                        qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);
              
                        procFramePos = procFramePos + lChildFrameLen;
                        return;
                    }
                    procFramePos += usParamLen;
                    lChildFrameLen -= usParamLen;
                    pBuffer = pBuffer_temp;
                }
                else
                {
                    QString str = QObject::tr("Error occured on PDXP single table full frame packet at %5.Source:%1, Frame:%2, Table:%3. Frame length error.");
                    qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable);
                                        
                    procFramePos = procFramePos + lChildFrameLen;
                    return;
                }
            }//非 字符串、原码
            else//字符串、原码
            {
                if (lChildFrameLen >= 3)
                {
                    char* pBuffer_temp = m_pParamInfoRT->SetParamValue(
                        usParamTable, usParamCode, pBuffer, pFHead->Time, ulParamDate);
                    if (pBuffer_temp == 0)
                    {
                        QString str = QObject::tr("Error occured on PDXP single table full frame packet at %5.Source:%1, Frame:%2, Table:%3. Data length error.");
                        qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable);

                        procFramePos = procFramePos + lChildFrameLen;
                        return;
                    }
                    procFramePos += pBuffer_temp - pBuffer;
                    lChildFrameLen -= pBuffer_temp - pBuffer;
                    pBuffer = pBuffer_temp;
                }
                else
                {
                    QString str = QObject::tr("Error occured on PDXP single table full frame packet at %3.Source:%1, Frame:%2, Table:%4. Frame length error.");
                    qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable);

                    procFramePos = procFramePos + lChildFrameLen;
                    return;
                }
            }

        }//end while(usChildFrameLen > 0)
    }
}

//将日期转换为BCD码格式
unsigned int PDXPFrame::ChangeDateToBCD(unsigned int uldate)
{
    //用当前日期作为初始化日期
    QDateTime t1 = QDateTime::currentDateTime();
    int date = uldate;
    if (date > -7300 && date < 10220)//积日
    {
        QDateTime t2(QDate(2000, 1, 1), QTime(0, 0, 0));
        t1 = t2.addDays(uldate - 1);
    }
    HeaderDate headerDate;
    headerDate.Year_1000 = t1.date().year() / 1000;
    headerDate.Year_100 = (t1.date().year() % 1000) / 100;
    headerDate.Year_10 = (t1.date().year() % 1000 % 100) / 10;
    headerDate.Year = t1.date().year() % 1000 % 100 % 10;
    headerDate.Month_10 = t1.date().month() / 10;
    headerDate.Month = t1.date().month() % 10;
    headerDate.Day_10 = t1.date().day() / 10;
    headerDate.Day = t1.date().day() % 10;
    return	*(unsigned long*)&headerDate;
}


//PDXP 单表挑点数据帧
void PDXPFrame::ProcFrame_STable_TD(char *pBuffer, PDXPFrameHeader *pFHead, unsigned short &procFramePos)
{
    long lChildFrameLen = 0;//子帧剩余长度
    unsigned short usParamTable = 0;//表号
    unsigned short usParamCode = 0;//参数号
    unsigned short usParamLen = 0;//参数长
    unsigned char ucTransType = 0;//参数传输类型

    unsigned long ulParamDate = ChangeDateToBCD(pFHead->Date);
    //表号
    usParamTable = *(unsigned short*)pBuffer;
    pBuffer += 2;
    procFramePos += 2;

    //帧的数据区长,减去2字节的表号
    lChildFrameLen = pFHead->DataLength - 2;

    if (checkChannelToContinue(usParamTable))
    {
        //开始处理子帧的数据
        while (lChildFrameLen > 0)
        {
            //挑点帧要取参数号
            if (lChildFrameLen < 2)
            {
                QString str = QObject::tr("PDXP single table point frame error.Source:%1, Frame:%2, at:%3 .TableNo:%4. Frame length Error.");
                qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable);
                
                return;
            }
            //取得参数序号
            usParamCode = *(unsigned short*)pBuffer;
            pBuffer += 2;
            procFramePos += 2;
            lChildFrameLen -= 2;

            stru_Param* pParam = (stru_Param*)m_pParamInfoRT->GetParam(usParamTable, usParamCode);
            if (pParam == NULL)
            {
                QString str = QObject::tr("PDXP single table point frame error.Source:%1, Frame:%2, at:%3 .TableNo:%4. Unknown Error.");
                qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable);

                procFramePos = procFramePos + lChildFrameLen;
                return;
            }
            ucTransType = pParam->ucTransType;
            if (!CheckParam(pParam))
            {
                QString str = QObject::tr("PDXP single table point frame error.Source:%1, Frame:%2, at:%3 .TableNo:%4, FrameNo:%5. Frame data type not match data length.");
                qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);
  
                return;
            }
            if (ucTransType != 10 && ucTransType != 11)//非 字符串、原码
            {
                usParamLen = pParam->usParamLen;
                if (usParamLen == 0)//没有这个参数
                {
                    QString str = QObject::tr("PDXP single table point frame error.Source:%1, Frame:%2, at:%3 .TableNo:%4, FrameNo:%5. No such param.");
                    qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);

                    return;
                }
                if (lChildFrameLen >= usParamLen)
                {
                    char* pBuffer_temp = m_pParamInfoRT->SetParamValue(
                        usParamTable, usParamCode, pBuffer, pFHead->Time, ulParamDate);
                    if (pBuffer_temp == 0)
                    {
                        QString str = QObject::tr("PDXP single table point frame error.Source:%1, Frame:%2, at:%3 .TableNo:%4, FrameNo:%5. Unknown error.");
                        qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);

                        return;
                    }
                    procFramePos += usParamLen;
                    lChildFrameLen -= usParamLen;
                    pBuffer = pBuffer_temp;
                }
                else
                {
                    QString str = QObject::tr("PDXP single table point frame error.Source:%1, Frame:%2, at:%3 .TableNo:%4, FrameNo:%5. Frame length error.");
                    qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);
                    
                    return;
                }
            }//非 字符串、原码
            else//字符串、原码
            {
                if (lChildFrameLen >= 3)
                {
                    char* pBuffer_temp = m_pParamInfoRT->SetParamValue(
                        usParamTable, usParamCode, pBuffer, pFHead->Time, ulParamDate);
                    if (pBuffer_temp == 0)
                    {
                        QString str = QObject::tr("PDXP single table point frame error.Source:%1, Frame:%2, at:%3 .TableNo:%4, FrameNo:%5. Unknown error.");
                        qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);

                        return;
                    }
                    procFramePos += pBuffer_temp - pBuffer;
                    lChildFrameLen -= pBuffer_temp - pBuffer;
                    pBuffer = pBuffer_temp;
                }
                else
                {
                    QString str = QObject::tr("PDXP single table point frame error.Source:%1, Frame:%2, at:%3 .TableNo:%4, FrameNo:%5. Unknown error.");
                    qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);
                    return;
                }
            }

        }//end while(usChildFrameLen > 0)
    }
}

//PDXP 多表挑点数据帧
void PDXPFrame::ProcFrame_MultiTable_TD(char *pBuffer, PDXPFrameHeader *pFHead, unsigned short &procFramePos)
{
    long lChildFrameLen = 0;//子帧剩余长度
    unsigned short usParamTable = 0;//表号
    unsigned short usParamCode = 0;//参数号
    unsigned short usParamLen = 0;//参数长
    unsigned char ucTransType = 0;//参数传输类型

    unsigned long ulParamDate = ChangeDateToBCD(pFHead->Date);

    //帧的数据区长
    lChildFrameLen = pFHead->DataLength;
    //开始处理子帧的数据
    while (lChildFrameLen > 0)
    {
        //挑点帧要取表号和参数号
        if (lChildFrameLen < 4)
        {
            QString str = QObject::tr("PDXP multi table point frame error.Source:%1,Frame: %2, at:%3. Frame Length error");
            qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32);
            
            return;
        }

        //表号
        usParamTable = *(unsigned short*)pBuffer;
        pBuffer += 2;
        procFramePos += 2;
        lChildFrameLen -= 2;
        if (checkChannelToContinue(usParamTable))
        {
            //取得参数序号
            usParamCode = *(unsigned short*)pBuffer;
            pBuffer += 2;
            procFramePos += 2;
            lChildFrameLen -= 2;

            stru_Param* pParam = (stru_Param*)m_pParamInfoRT->GetParam(usParamTable, usParamCode);
            if (pParam == NULL)
            {
                QString str = QObject::tr("PDXP multi table point frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. Frame Length error");
                qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);

                return;
            }
            ucTransType = pParam->ucTransType;
            if (!CheckParam(pParam))
            {
                QString str = QObject::tr("PDXP multi table point frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. Data type not match data length");
                qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);

                return;
            }
            if (ucTransType != 10 && ucTransType != 11)//非 字符串、原码
            {
                usParamLen = pParam->usParamLen;
                if (usParamLen == 0)//没有这个参数
                {
                    QString str = QObject::tr("PDXP multi table point frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. No such param");
                    qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);

                    return;
                }
                if (lChildFrameLen >= usParamLen)
                {
                    char* pBuffer_temp = m_pParamInfoRT->SetParamValue(
                        usParamTable, usParamCode, pBuffer, pFHead->Time, ulParamDate);
                    if (pBuffer_temp == 0)
                    {
                        QString str = QObject::tr("PDXP multi table point frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. Unknown error");
                        qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);

                        return;
                    }
                    procFramePos += usParamLen;
                    lChildFrameLen -= usParamLen;
                    pBuffer = pBuffer_temp;
                }
                else
                {
                    QString str = QObject::tr("PDXP multi table point frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. Unknown error");
                    qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);
                    return;
                }
            }//非 字符串、原码
            else//字符串、原码
            {
                if (lChildFrameLen >= 3)
                {
                    char* pBuffer_temp = m_pParamInfoRT->SetParamValue(
                        usParamTable, usParamCode, pBuffer, pFHead->Time, ulParamDate);
                    if (pBuffer_temp == 0)
                    {
                        QString str = QObject::tr("PDXP multi table point frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. SetParamValue error");
                        qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);

                        return;
                    }
                    procFramePos += pBuffer_temp - pBuffer;
                    lChildFrameLen -= pBuffer_temp - pBuffer;
                    pBuffer = pBuffer_temp;
                }
                else
                {
                    QString str = QObject::tr("PDXP multi table point frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. Frame length error");
                    qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);

                    return;
                }
            }

        }
        else//不需要存入参数
        {
            //取得参数序号
            usParamCode = *(unsigned short*)pBuffer;
            pBuffer += 2;
            procFramePos += 2;
            lChildFrameLen -= 2;

            stru_Param* pParam = (stru_Param*)m_pParamInfoRT->GetParam(usParamTable, usParamCode);
            if (pParam == NULL)
            {
                QString str = QObject::tr("PDXP multi table point frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. No such param");
                qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);
                
                return;
            }
            ucTransType = pParam->ucTransType;
            if (!CheckParam(pParam))
            {
                QString str = QObject::tr("PDXP multi table point frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. Data type length not match data type");
                qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);

                return;
            }
            if (ucTransType != 10 && ucTransType != 11)//非字符串、原码
            {
                usParamLen = pParam->usParamLen;
                pBuffer += usParamLen;
                procFramePos += usParamLen;
                lChildFrameLen -= usParamLen;
            }
            else//字符串、原码
            {
                if (lChildFrameLen >= 3)
                {
                    usParamLen = *(unsigned short*)pBuffer;
                    procFramePos = procFramePos + usParamLen + 3;
                    lChildFrameLen = lChildFrameLen - usParamLen - 3;
                    pBuffer = pBuffer + usParamLen + 3;
                }
                else
                {
                    QString str = QObject::tr("PDXP multi table point frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. Unknwon error");
                    qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);

                    return;

                }
            }

        }

    }//end while(usChildFrameLen > 0)
}
//PDXP 多表完整数据帧
char* PDXPFrame::ProcFrame_MultiTable_WZ(char *pBuffer, PDXPFrameHeader *pFHead, unsigned short &procFramePos)
{
    long lChildFrameLen = 0;//子帧剩余长度
    unsigned long ulChildFrameTime = 0;//子帧时间
    unsigned short usParamTable = 0;//表号
    unsigned short usParamCode = 0;//参数号
    unsigned short usParamLen = 0;//参数长
    unsigned char ucTransType = 0;//参数传输类型
    unsigned long ulParamDate = ChangeDateToBCD(pFHead->Date);

    char* t_pcBackBuffer = pBuffer;
    unsigned short t_usBankFrameLen = procFramePos;

    //子帧的数据区长
    lChildFrameLen = *(unsigned short*)pBuffer;
    long t_lBackChildFrameLen = lChildFrameLen;
    pBuffer += 2;
    procFramePos += 2;
    //子帧时间
    ulChildFrameTime = *(unsigned long*)pBuffer;
    pBuffer += 4;
    procFramePos += 4;
    //表号
    usParamTable = *(unsigned short*)pBuffer;
    pBuffer += 2;
    procFramePos += 2;

    //子帧尾不对，就放弃后面的帧处理
    if (*(unsigned short*)(pBuffer + lChildFrameLen) != 0xffff)//子帧尾不对
    {
        QString str = QObject::tr("PDXP multi table full frame error.Source:%1,Frame: %2, at:%3, TableNo:%4. Frame endness error.");
        qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable);

        return 0;
    }
    if (checkChannelToContinue(usParamTable))
    {
        //开始处理子帧的数据
        while (lChildFrameLen > 0)
        {
            //完整帧参数号自动累加
            usParamCode++;
            stru_Param* pParam = (stru_Param*)m_pParamInfoRT->GetParam(usParamTable, usParamCode);
            //参数不存在则放弃该帧的处理
            if (pParam == NULL)
            {
                QString str = QObject::tr("PDXP multi table full frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. No such param.");
                qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);

                procFramePos = t_usBankFrameLen + t_lBackChildFrameLen + 10;
                return t_pcBackBuffer + t_lBackChildFrameLen + 10;
            }
            ucTransType = pParam->ucTransType;
            if (!CheckParam(pParam))
            {
                QString str = QObject::tr("PDXP multi table full frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. Param type not match data length.");
                qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);

                procFramePos = t_usBankFrameLen + t_lBackChildFrameLen + 10;
                return t_pcBackBuffer + t_lBackChildFrameLen + 10;
            }

            if (ucTransType != 10 && ucTransType != 11)// 字符串、原码
            {
                usParamLen = pParam->usParamLen;
                if (usParamLen == 0)//没有这个参数
                {
                    QString str = QObject::tr("PDXP multi table full frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. No such param.");
                    qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);

                    procFramePos = t_usBankFrameLen + t_lBackChildFrameLen + 10;
                    return t_pcBackBuffer + t_lBackChildFrameLen + 10;
                }
                if (lChildFrameLen >= usParamLen)
                {
                    char* pBuffer_temp = m_pParamInfoRT->SetParamValue(
                        usParamTable, usParamCode, pBuffer, ulChildFrameTime, ulParamDate);
                    if (pBuffer_temp == 0)
                    {
                        QString str = QObject::tr("PDXP multi table full frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. SetParamValue param.");
                        qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);
                        
                        procFramePos = t_usBankFrameLen + t_lBackChildFrameLen + 10;
                        return t_pcBackBuffer + t_lBackChildFrameLen + 10;
                    }
                    //应该判断pBuffer_temp-pBuffer是否与usParamLen相等
                    //如果不相等则参数设置错误，应丢弃该子帧
                    if (pBuffer_temp - pBuffer != usParamLen)
                    {
                        QString str = QObject::tr("PDXP multi table full frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. Data type not match data length.");
                        qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);

                        procFramePos = t_usBankFrameLen + t_lBackChildFrameLen + 10;
                        return t_pcBackBuffer + t_lBackChildFrameLen + 10;
                    }
                    procFramePos += usParamLen;
                    lChildFrameLen -= usParamLen;
                    pBuffer = pBuffer_temp;
                }
                else
                {
                    QString str = QObject::tr("PDXP multi table full frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. Frame length error.");
                    qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);
      
                    procFramePos = t_usBankFrameLen + t_lBackChildFrameLen + 10;
                    return t_pcBackBuffer + t_lBackChildFrameLen + 10;
                }
            }//非 字符串、原码
            else//字符串、原码
            {
                if (lChildFrameLen >= 3)
                {
                    char* pBuffer_temp = m_pParamInfoRT->SetParamValue(
                        usParamTable, usParamCode, pBuffer, ulChildFrameTime, ulParamDate);
                    if (pBuffer_temp == 0)
                    {
                        QString str = QObject::tr("PDXP multi table full frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. Unknown error.");
                        qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);
            
                        procFramePos = t_usBankFrameLen + t_lBackChildFrameLen + 10;
                        return t_pcBackBuffer + t_lBackChildFrameLen + 10;
                    }
                    procFramePos += pBuffer_temp - pBuffer;
                    lChildFrameLen -= pBuffer_temp - pBuffer;
                    pBuffer = pBuffer_temp;
                }
                else
                {
                    QString str = QObject::tr("PDXP multi table full frame error.Source:%1,Frame: %2, at:%3, TableNo:%4. Frame length error.");
                    qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable);

                    procFramePos = t_usBankFrameLen + t_lBackChildFrameLen + 10;
                    return t_pcBackBuffer + t_lBackChildFrameLen + 10;
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
        procFramePos = t_usBankFrameLen + t_lBackChildFrameLen + 10;
        return t_pcBackBuffer + t_lBackChildFrameLen + 10;
    }

}
//PDXP 多单表挑点数据帧
char* PDXPFrame::ProcFrame_MultiSTable_TD(char *pBuffer, PDXPFrameHeader *pFHead, unsigned short &procFramePos)
{
    long lChildFrameLen = 0;//子帧剩余长度
    unsigned long ulChildFrameTime = 0;//子帧时间
    unsigned short usParamTable = 0;//表号
    unsigned short usParamCode = 0;//参数号
    unsigned short usParamLen = 0;//参数长
    unsigned char ucTransType = 0;//参数传输类型
    unsigned long ulParamDate = ChangeDateToBCD(pFHead->Date);

    unsigned short t_usBankFrameLen = procFramePos;
    char *t_pcBackBuffer = pBuffer;
    //子帧数据区长度
    lChildFrameLen = *(unsigned short*)pBuffer;
    long t_lBackChildFrameLen = lChildFrameLen;
    pBuffer += 2;
    procFramePos += 2;

    ulChildFrameTime = *(unsigned long*)pBuffer;
    pBuffer += 4;
    procFramePos += 4;

    usParamTable = *(unsigned short*)pBuffer;
    pBuffer += 2;
    procFramePos += 2;

    //子帧尾不对，就放弃后面的帧处理
    if (*(unsigned short*)(pBuffer + lChildFrameLen) != 0xffff)//子帧尾不对
    {
        QString str = QObject::tr("PDXP multi single table frame error.Source:%1,Frame: %2, at:%3, TableNo:%4. Frame endness error.");
        qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable);
                
        return 0;
    }
    if (checkChannelToContinue(usParamTable))
    {
        //开始处理子帧的数据
        while (lChildFrameLen > 0)
        {
            //挑点帧要取参数号
            if (lChildFrameLen < 2)
            {
                QString str = QObject::tr("PDXP multi single table frame error.Source:%1,Frame: %2, at:%3, TableNo:%4. Frame length error.");
                qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable);
       
                procFramePos = t_usBankFrameLen + t_lBackChildFrameLen + 10;
                return t_pcBackBuffer + t_lBackChildFrameLen + 10;
            }
            usParamCode = *(unsigned short*)pBuffer;
            pBuffer += 2;
            procFramePos += 2;
            lChildFrameLen -= 2;

            stru_Param* pParam = (stru_Param*)m_pParamInfoRT->GetParam(usParamTable, usParamCode);
            if (pParam == NULL)
            {
                QString str = QObject::tr("PDXP multi single table frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. No such param.");
                qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);

                procFramePos = t_usBankFrameLen + t_lBackChildFrameLen + 10;
                return t_pcBackBuffer + t_lBackChildFrameLen + 10;
            }
            ucTransType = pParam->ucTransType;
            if (!CheckParam(pParam))
            {
                QString str = QObject::tr("PDXP multi single table frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. Param type not match data length.");
                qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);
     
                procFramePos = t_usBankFrameLen + t_lBackChildFrameLen + 10;
                return t_pcBackBuffer + t_lBackChildFrameLen + 10;
            }
            if (ucTransType != 10 && ucTransType != 11)//非 字符串、原码
            {
                usParamLen = pParam->usParamLen;
                if (usParamLen == 0)//没有这个参数
                {
                    QString str = QObject::tr("PDXP multi single table frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. No such param.");
                    qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);

                    procFramePos = t_usBankFrameLen + t_lBackChildFrameLen + 10;
                    return t_pcBackBuffer + t_lBackChildFrameLen + 10;
                }
                if (lChildFrameLen >= usParamLen)
                {
                    char* pBuffer_temp = m_pParamInfoRT->SetParamValue(
                        usParamTable, usParamCode, pBuffer, ulChildFrameTime, ulParamDate);
                    if (pBuffer_temp == 0)
                    {
                        QString str = QObject::tr("PDXP multi single table frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. SetParamValue error.");
                        qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);
                    
                        procFramePos = t_usBankFrameLen + t_lBackChildFrameLen + 10;
                        return t_pcBackBuffer + t_lBackChildFrameLen + 10;
                    }
                    procFramePos += usParamLen;
                    lChildFrameLen -= usParamLen;
                    pBuffer = pBuffer_temp;
                }
                else
                {
                    QString str = QObject::tr("PDXP multi single table frame error.Source:%1,Frame: %2, at:%3, TableNo:%4. Frame length error.");
                    qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable);

                    procFramePos = t_usBankFrameLen + t_lBackChildFrameLen + 10;
                    return t_pcBackBuffer + t_lBackChildFrameLen + 10;
                }
            }//非 字符串、原码
            else//字符串、原码
            {
                if (lChildFrameLen >= 3)
                {
                    char* pBuffer_temp = m_pParamInfoRT->SetParamValue(
                        usParamTable, usParamCode, pBuffer, ulChildFrameTime, ulParamDate);
                    if (pBuffer_temp == 0)
                    {
                        QString str = QObject::tr("PDXP multi single table frame error.Source:%1,Frame: %2, at:%3, TableNo:%4, FrameNo:%5. SetParamValue error.");
                        qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable).arg(usParamCode);

                        procFramePos = t_usBankFrameLen + t_lBackChildFrameLen + 10;
                        return t_pcBackBuffer + t_lBackChildFrameLen + 10;
                    }
                    procFramePos += pBuffer_temp - pBuffer;
                    lChildFrameLen -= pBuffer_temp - pBuffer;
                    pBuffer = pBuffer_temp;
                }
                else
                {
                    QString str = QObject::tr("PDXP multi single table frame error.Source:%1,Frame: %2, at:%3, TableNo:%4. Frame length error.");
                    qWarning() << str.arg(pFHead->SID).arg(pFHead->FrameNo).arg(procFramePos + 32).arg(usParamTable);

                    procFramePos = t_usBankFrameLen + t_lBackChildFrameLen + 10;
                    return t_pcBackBuffer + t_lBackChildFrameLen + 10;
                }
            }

        }//end while(usChildFrameLen > 0)
        procFramePos += 2;
        pBuffer += 2;
        return pBuffer;
    }
    else
    {
        procFramePos = t_usBankFrameLen + t_lBackChildFrameLen + 10;
        return t_pcBackBuffer + t_lBackChildFrameLen + 10;
    }
}

//检查指定表号的主备配置,看是否需要继续处理
bool PDXPFrame::checkChannelToContinue(unsigned int tableNo)
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
bool PDXPFrame::CheckParam(stru_Param* param)
{
    if (param == NULL)
        return false;

    switch (param->ucTransType)
    {
    case tp_char:
    case tp_BYTE:
        return param->usParamLen == 1;
    case tp_short:
    case tp_WORD:
        return param->usParamLen == 2;
    case tp_long:
    case tp_DWORD:
    case tp_PMTime:
    case tp_Date:
    case tp_BCDTime:
    case tp_float:
    case tp_longE:
    case tp_UTCTime:
        return param->usParamLen == 4;
    case tp_double:
    case tp_ulong8:
    case tp_long8:
        return param->usParamLen == 8;
        //字符串与源码的参数长度会被改变，不能作为判断的依据
    case tp_String:
    case tp_Code:
        break;
    default:
        return false;
    }
    return true;
}