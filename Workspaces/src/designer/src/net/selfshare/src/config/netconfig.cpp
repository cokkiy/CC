// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：netconfig.cpp
// 文件摘要：NetConfig类实现，实现了加载网络配置文档
// 原始版本：1.0
// 作   者：张立民
// 完成日期：2015-08-26
//**************************************************************//
#include "netconfig.h"
#include <QMutex>
#include <QDebug>
#include <QFile>
#include <QString>
#include "xdocument.h"
#include "xelement.h"


//构造函数
NetConfig::NetConfig()
{
    m_NISettings=0;
}

//析构函数
NetConfig::~NetConfig()
{
    if(m_NISettings!=0)
    {
        delete m_NISettings;
        m_NISettings=0;
    }
}

//获取网卡设置列表
list<NetConfig::NISetting>* NetConfig::getNISettings()
{
    //首先检查实例是否已经存在，如果不存在，则创建一个
//    if(m_Instance==0)
//    {
//        //为了保证多线程下的唯一性
//        QMutex mutex;
//        mutex.lock();
//        if(m_Instance==0)
//        {
//            m_Instance=new NetConfig();

//            //加载配置文件
//            if(!m_Instance->loadConfig())
//            {
//                qDebug()<<"加载网络配置文件失败！"<<endl;
//            }
//        }
//        mutex.unlock();
//    }


    return m_NISettings;
}

//构建接收设置
void NetConfig::buildRecSetting(ElementList &recList, NISetting &niSetting, bool isPrimary)
{
    for(ElementList::const_iterator i=recList.begin();i!=recList.end();i++)
    {
        RecSetting recSetting;
        QString MethodValue=((XElement)(i->getChildrenByName(QString::fromLocal8Bit("接收类型")).front())).Value;
        if(MethodValue==QString::fromLocal8Bit("单播"))
        {
            recSetting.DataMethod=DataMethod::Unicast;
        }
        else if(MethodValue==QString::fromLocal8Bit("组播"))
        {
            recSetting.DataMethod=DataMethod::Multicast;
        }
        else if(MethodValue==QString::fromLocal8Bit("指定源组播"))
        {
            recSetting.DataMethod=DataMethod::SpecifiedSource;
        }
        else
        {
            recSetting.DataMethod=DataMethod::Unicast;
        }

        recSetting.IPAddress=i->getChildValue(QString::fromLocal8Bit("接收地址"));
        recSetting.Name=i->getChildValue(QString::fromLocal8Bit("名称"));
        recSetting.Port=i->getChildIntValue(QString::fromLocal8Bit("端口号"));
        recSetting.RecBuffSize=i->getChildIntValue(QString::fromLocal8Bit("接收缓冲区大小"));
        recSetting.ThreadNum=i->getChildIntValue(QString::fromLocal8Bit("处理线程数"));
        ElementList specSourceList=i->getChildrenByName(QString::fromLocal8Bit("指定源地址/IP地址"));
        for(ElementList::const_iterator i=specSourceList.begin();i!=specSourceList.end();i++)
        {
            recSetting.SpecifiedIPList.push_back(i->Value);
        }

        if(isPrimary)
        {
            niSetting.primaryRecSettings.push_back(recSetting);
        }
        else
        {
            niSetting.backupRecSettings.push_back(recSetting);
        }
    }
}

//加载网络配置
bool NetConfig::load(const QString& dir)
{
    m_NISettings=new list<NetConfig::NISetting>();
    QString fileName(dir);
    fileName=fileName.append("%1").arg(QString::fromLocal8Bit("网络设置.xml"));
    QFile file(fileName);
    if(!file.exists())
    {
        qCritical() << "The file: "<<fileName<<" dose not exists!" ;
        return false;
    }
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        qCritical()<<"Can't open net config file: "<<fileName<<endl;
        return false;
    }


    bool result=false;
    XDocument doc;

    if((result=doc.Load(&file)))
    {
        XElement root=doc.getChild();


        //获取处理线程数量
        this->primaryThreadCount = root.getChildIntValue(QString::fromLocal8Bit("主用通道处理线程数"));
        this->backupThreadCount = root.getChildIntValue(QString::fromLocal8Bit("备用通道处理线程数"));

        ElementList::const_iterator iterator;
        ElementList niList=root.getChildrenByName(QString::fromLocal8Bit("网卡设置"));
        for(iterator=niList.begin();iterator!=niList.end();iterator++)
        {
            NISetting niSetting;
            //本机地址
            ElementList ipList=iterator->getChildrenByName(QString::fromLocal8Bit("本机地址/IP地址"));
            for(ElementList::iterator i=ipList.begin();i!=ipList.end();i++)
            {
                niSetting.ipAddresses.push_back(i->Value);
            }

            //主用
            ElementList primaryList=iterator->getChildrenByName(QString::fromLocal8Bit("主用/接收设置"));
            buildRecSetting(primaryList,niSetting,true);
            //备用
            ElementList backupList=iterator->getChildrenByName(QString::fromLocal8Bit("备用/接收设置"));
            buildRecSetting(backupList,niSetting,false);

            this->m_NISettings->push_back(niSetting);
        }
    }

    file.close();

    return result;
}
