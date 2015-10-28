// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：recvinfothread.cpp
// 文件摘要：综合应用分系统网络收发进程接收信息线程类实现文件，用来实现网络收发进程接收信息线程类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：
// cokkiy 2015-09 修改

#include "../network/network.h"
#include "../../../selfshare/src/const.h"
#include "../../../selfshare/src/buffer/buffer.h"
#include "recvinfothread.h"
#include "../infoprocthread/infoprocthread.h"
#include "../../../selfshare/src/config/config.h"
#include "FrameHeader.h"
#include "SimpleLogger.h"
 

extern InfoProcThread 	g_InfoProcThread[MAX_THREAD_NUM];
extern bool g_quitFlag;
extern SimpleLogger simpleLogger; //外部变量,记录原始数据

//构造函数
RecvInfoThread::RecvInfoThread():PThread()
{
	m_primaryThreadNo = 0;
    m_backupThreadNo = 0;
}

///************************************
/// 方法:    isValidPacket
/// 可访问性:    private 
/// <param name="dataLen">接收到数据长度</param>
/// <param name="buf">接收到的数据（缓冲区)</param>
/// <returns>如果有效,返回true,否则返回false</returns>
/// <remarks>根据包类型判断包的有效性</remarks>
/// 作者：cokkiy（张立民)
/// 创建时间：2015/09/23 20:30:03
///************************************
bool RecvInfoThread::isValidPacket(int dataLen, char* dataBuf)
{
    //检查帧的第一个字节，确定帧的数据格式。
    //0x80表示PDXP
    if (PDXP_VERSION == (unsigned char)dataBuf[0])
    {
        //接收数据后，检查接收的pRevBuffer的字节数要大于32个字节
        if (dataLen <= 32)
        {
            qWarning() << QObject::tr("Received PDXP packet length must greater than 32 bytes");
            return false;
        }
        PDXPFrameHeader* pFHead = (PDXPFrameHeader*)dataBuf;
        //数据域长度
        if (pFHead->DataLength + 32 != dataLen)
        {
            //数长错误
            qWarning() << QObject::tr("PDXP data length not equals the received data length.");
            return false;
        }
    }
    //C3I格式
    //00h：表示本帧是完整信息帧；
    //01h：表示本帧是挑点信息帧；
    //04h：表示本帧是指挥信息帧；
    //11h：表示本帧是带数长挑点信息帧；
    //f3h：指显历史曲线数据应答帧；
    else if (dataBuf[0] == 0 || dataBuf[0] == 1 || dataBuf[0] == 4
        || dataBuf[0] == 0xf3 || dataBuf[0] == 0x11)
    {
        //接收数据后，检查接收的pRevBuffer的字节数要大于28个字节（帧头加帧尾共计28字节）
        if (dataLen < 28)
        {
            qWarning()<<QObject::tr("C3I Frame length must greater than 28 bytes!");
            return false;
        }
        C3IFrameHeader* pFHead = (C3IFrameHeader*)dataBuf;
        //信息类别不检查
        //任务代号不检查		
        //数长
        if (pFHead->datalen + 28 != dataLen)
        {
            //数长错误
            qWarning() << QObject::tr("C3I frame length must greater than 28 bytes");
            return false;
        }

        //帧尾
        if (*(unsigned int*)(dataBuf + 24 + pFHead->datalen)  != 0x89abcdef)
        {
            //帧尾错误
            qWarning() << QObject::tr("C3I frame endness error.");
            return false;
        }

        //信源不检查
        //信宿不检查
        //帧序号、日期、时间、备用字段都不检查

    }
    else
    {
        //帧类别错误
        QString str = QObject::tr("Frame type error. Unknown type: %1");
        qWarning() << str.arg(dataBuf[0]);
        return false;
    }

    return true;
}

//*********************************
//线程内容函数
//接收网络信息
//修改： cokkiy
//增加了对多socket的支持,增加了主备通道信息
//*********************************
void RecvInfoThread::run()
{
	char udpbuf[64000];
	struct sockaddr_in client_address;				//客户端地址
	fd_set readfds, testfds;
    Buffer buf;																//临时缓冲区

    //帧内数据区长度
    //unsigned short frameDataLen = 0;    

    //网络配置文件
    NetConfig* netConfig = ni->getConfig();
  
	//初始化文件描述符集合,udp套接字加入集合,同时将较大的文件描述符存入maxfd	
	FD_ZERO(&readfds);
    for (auto i = ni->begin(); i != ni->end(); i++)
    {
        //把socket加入到描述符中
        FD_SET(i->second, &readfds);
    }	

	//等待客户连接请求
	while(1)
	{
		if(true == g_quitFlag) 
		{
			break;
		}
		
		testfds = readfds;
        timeval timeout{ 0,100 }; //超时时间 100ms
		int result = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, &timeout);
		if(result < 1)
		{
			continue;
		}

        for (auto i = ni->begin(); i != ni->end(); i++)
        {
            if (FD_ISSET(i->second, &testfds) > 0)
            {
                int client_len = sizeof(struct sockaddr);
                result = recvfrom(i->second, udpbuf, 64000, 0, (struct sockaddr *)&client_address, (socklen_t*)&client_len);
                if (0 >= result)
                {
                    continue;
                }
               
                //记录数据
                simpleLogger.log(udpbuf, result);
                //判断是否是有效的包
                if (!isValidPacket(result, udpbuf))
                {
                    continue;
                }

                //放入到临时缓冲区，等待信息处理线程处理
                if (buf.InitializeBuffer(result) == false)
                {
                    continue;
                }

                if (false == buf.WriteBuffer(udpbuf, result))
                {
                    continue;
                }         

                //client_address.sin_addr.s_addr 源IP地址,组播数据也是源客户端IP
                // client_address.sin_port 源端口
               
                if (ni->isPrimarySocket(i->second))
                {
                    //主用通道
                    //将数据存储于不同线程的缓冲区队列，然后激活对应的线程
                    netConfig->m_inBufList[m_primaryThreadNo].PutBuffer(buf);
                    g_InfoProcThread[m_primaryThreadNo].Resume();
                    m_primaryThreadNo++;

                    if (m_primaryThreadNo >= netConfig->primaryThreadCount)
                    {
                        m_primaryThreadNo = 0;
                    }
                }
                else
                {
                    //备用通道
                    //将数据存储于不同线程的缓冲区队列，然后激活对应的线程
                    netConfig->m_inBufList[netConfig->primaryThreadCount+m_backupThreadNo].PutBuffer(buf);
                    g_InfoProcThread[netConfig->primaryThreadCount + m_backupThreadNo].Resume();
                    m_backupThreadNo++;

                    if (m_backupThreadNo >= netConfig->backupThreadCount)
                    {
                        m_backupThreadNo = 0;
                    }
                }                   
                
            }
        }		
	}
}

//为信息接收线程设置工作用的Network对象
void RecvInfoThread::setNetwork(Network *ni)
{
    this->ni=ni;
}












