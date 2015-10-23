// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：infoprocthread.cpp
// 文件摘要：综合应用分系统信息处理进程信息处理线程类实现文件，用来实现信息处理进程信息处理线程类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：


//#include "../frame/frame.h"
#include "../../../selfshare/src/config/config.h"
#include "infoprocthread.h"
#include "../../../selfshare/src/historyparam/historyparam.h"
#include "C3IFrame.h"
#include "PDXPFrame.h"


extern Config g_cfg;
extern bool g_quitFlag;

//外部变量,主备选择配置表
extern ChannelSelectConfig channelSelectConfig;

//C3I/PDXP包处理对象
extern CParamInfoRT g_paramInfoRT;

//*********************************
//构造函数
//
//
//*********************************
InfoProcThread::InfoProcThread():PThread(),isParimaryDataThread(true)
{
	m_serialNo = 0;
}

//*********************************
//设置线程序号
//
//
//*********************************
void InfoProcThread::SetSerialNo(int no)
{
	m_serialNo = no;
}

//*********************************
//线程内容函数
//航天员、应用、飞船、空间实验室信息处理线程体函数
//
//*********************************
void InfoProcThread::run()
{
    //AbstractParam param;																							//参数类实例
	Buffer tempBuf;																								//临时缓冲区
	//HistoryParam hParam;
	char *tempFrame;																							//指向要处理的帧的指针	
		
	unsigned short inFrameLen = 0; 
	tempFrame = NULL;

    //C3I帧处理对象
    C3IFrame c3iFrame(&channelSelectConfig, isParimaryDataThread);
    c3iFrame.setParamInfo(&g_paramInfoRT);

    //PDXP帧处理对象
    PDXPFrame PDXPFrameProc(&channelSelectConfig, isParimaryDataThread);
    PDXPFrameProc.setParamInfo(&g_paramInfoRT);

	//param.InputInfoTable(&g_cfg.m_infoTabProc); 													//给参数加载信息约定表;
	
	while(1)
	{
		if(true == g_quitFlag) 
		{
			break;
		}

		tempBuf = m_NetConfig->m_inBufList[m_serialNo].GetBuffer();
		if (false == tempBuf.IsEmpty())
		{
			inFrameLen = tempBuf.GetWritePos();
			tempFrame = tempBuf.ReadBuffer(inFrameLen);
		}

		if( NULL == tempFrame ) 
		{
			OnHold();										// 无数据线程挂起
			continue;
		}

        if (PDXP_VERSION == (unsigned char)tempFrame[0])
        {
            //PDXP包
            PDXPFrameProc.parse(tempFrame, inFrameLen);
            //处理结束,释放缓冲区
            if (NULL != tempFrame)
            {
                delete []tempFrame;
                tempFrame = NULL;
            }
        }
        else
        {
            // C3I包
            c3iFrame.parse(tempFrame, inFrameLen);
            //处理结束,释放缓冲区
            if (NULL != tempFrame)
            {
                delete []tempFrame;
                tempFrame = NULL;
            }
        }
	}
//	pthread_exit(NULL);
}

//*********************************
//报告解帧错误函数
//
//
//*********************************
bool InfoProcThread::ReportError(const unsigned short Errorcode)
{
	char HintInfo[200];

	unsigned short errorPos = 0;

	//未报错，返回false
	if (0 == Errorcode) return false;		

		//取得报错位置
// 	if(inputFrame.GetReadPos() == 0)
// 	{
// 		errorPos = inputFrame.m_errorPos;
// 	}
// 	else
// 	{
// 		errorPos = inputFrame.GetReadPos();
// 	}

	//报错，返回true
	return true;
}

//设置网络配置文件
void InfoProcThread::SetNetConfig(NetConfig * netConfig)
{
    this->m_NetConfig = netConfig;
}










