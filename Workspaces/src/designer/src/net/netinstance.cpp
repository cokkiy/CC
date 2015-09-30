#include "netinstance.h"
#include<iostream>
#include "selfshare/src/const.h"
#include "selfshare/src/config/config.h"
#include "main/src/network/network.h"
#include "main/src/recvinfothread/recvinfothread.h"
#include "main/src/infoprocthread/infoprocthread.h"
#include "selfshare/src/config/netconfig.h"
#include "C3I/CParamInfoRT.h"
#include "selfshare/src/config/channelselectconfig.h"
#include <QDebug>

extern Config g_cfg;

//信息处理线程
InfoProcThread 	g_InfoProcThread[MAX_THREAD_NUM];

//C3I包信息处理类
CParamInfoRT g_paramInfoRT;

//全局变量,主备信道切换表
ChannelSelectConfig channelSelectConfig;


bool g_quitFlag = false;

NetInstance::NetInstance(QObject *parent) :
    NetInterface(parent)
{
    m_thread = new QThread(parent);
    connect(m_thread,&QThread::started,this,&NetInstance::run);    
}

int NetInstance::load(QString dir)
{
    //std::string temp = dir.toStdString();
    if(g_cfg.LoadConfig(dir) == false)
    {
        cout<<"加载信息处理配置信息失败，请检查配置重启软件！"<<endl;
        return -1;
    }

    if (!channelSelectConfig.load(dir))
    {
        qWarning() << QObject::tr("Loading channel config file error.");
    }

    if(!netConfig.load(dir))
    {
        qCritical()<<"Loading net config file failure. Please check then restart.";
        return -1;
    }

    if (!g_paramInfoRT.LoadParamFromXML(dir))
    {
        qWarning() << QObject::tr("Atleast one config file loading failure.Please check prior log.");
    }

    ni.setConfig(&netConfig);
    this->rithread.setNetwork(&ni);

    if(ni.InitSocket() == false)
    {
        qCritical()<<"Init socket failure, Please check net config file then restart"<<endl;
        return -1;
    }

    return 1;
}

bool NetInstance::startThread(bool b)
{
    g_quitFlag = false;

    for (int i = 0; i < netConfig.primaryThreadCount;i++)
    {
        g_InfoProcThread[i].SetNetConfig(&netConfig);
        g_InfoProcThread[i].SetSerialNo(i);
        g_InfoProcThread[i].start(QThread::NormalPriority);
        g_InfoProcThread[i].isParimaryDataThread = true;
    }
    for (int i = netConfig.primaryThreadCount; i < netConfig.primaryThreadCount + netConfig.backupThreadCount; i++)
    {
        g_InfoProcThread[i].SetNetConfig(&netConfig);
        g_InfoProcThread[i].SetSerialNo(i);
        g_InfoProcThread[i].start(QThread::NormalPriority);
        g_InfoProcThread[i].isParimaryDataThread = false;
    }

    rithread.start(QThread::NormalPriority);

    return true;
}

void NetInstance::run()
{
    startThread(true);
}
int NetInstance::start()
{
    m_thread->start();
    return 1;
}

int NetInstance::stop()
{
    m_thread->exit();
    return 1;
}
