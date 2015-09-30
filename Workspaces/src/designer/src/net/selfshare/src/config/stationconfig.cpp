// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：stationconfig.cpp
// 文件摘要：StationConfig类实现，实现了加载网络配置文档
// 原始版本：1.0
// 作   者：秦志刚
// 完成日期：2015-09-17
//**************************************************************//
#include "stationconfig.h"
#include <QMutex>
#include <QDebug>
#include <QFile>
#include <QString>
#include "xdocument.h"
#include "xelement.h"
#include "../../../main/src/network/network.h"


//构造函数
StationConfig::StationConfig()
{
    m_NISettings=0;
}

//析构函数
StationConfig::~StationConfig()
{
    if(m_NISettings!=0)
    {
        delete m_NISettings;
        m_NISettings=0;
    }
}

//加载配置文件 配置文件的名字是"指显工作站信息.xml"，不可更改
//返回值 false:失败 true:成功
bool StationConfig::load(const QString& dir)
{
    m_NISettings=new list<Station>;

    QString fileName(dir);

    fileName = fileName.append("%1").arg(QString::fromLocal8Bit("指显工作站信息.xml"));

    QFile file(fileName);

    //配置文件不存在
    if(!file.exists())
    {
        qCritical() << "The file: "<<fileName<<" dose not exists!" ;
        return false;
    }

    //配置文件无法打开
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

        ElementList::const_iterator iterator;

        //每个工作站配置的关键词是"指显工作站"
        ElementList niList = root.getChildrenByName(QString::fromLocal8Bit("指显工作站"));

        for(iterator=niList.begin(); iterator!=niList.end(); iterator++)
        {
            Station station;

            //名称
            station.strName = iterator->getChildValue(QString::fromLocal8Bit("名称"));

            //IP地址
            station.ipAddresses = iterator->getChildValue(QString::fromLocal8Bit("IP"));

            //MAC地址
            station.strMACAddress = iterator->getChildValue(QString::fromLocal8Bit("MAC"));

            //工作模式
            station.strWorkMode = iterator->getChildValue(QString::fromLocal8Bit("工作模式"));

            //是否记盘
            station.bRecord = transBool(iterator->getChildValue(QString::fromLocal8Bit("记盘")));

            //记盘路径
            station.strRecordPath = iterator->getChildValue(QString::fromLocal8Bit("记盘路径"));

            m_NISettings->push_back(station);
        }
    }

    file.close();

    //获取本机配置
    GetMyConfig();

    return result;
}

//获取本机配置
void StationConfig::GetMyConfig()
{
    //声明一个网络类，用于取本机IP
    Network ni;

    //取得本机信息
    ni.getNetworkInterfaceInfo();

    list<Station>::const_iterator it;

    //查找配置文件中与本机IP地址一致的配置，并将其配置保存起来
    //如果本机有多个地址，只匹配第一个地址
    for(it=m_NISettings->begin();it!=m_NISettings->end();it++)
    {
        if(ni.isLocalIP(it->ipAddresses))
        {
            m_MySatation = *it;
            break;
        }
    }

    //本机权限（以值表示）
    m_iRights = transRight(m_MySatation.strWorkMode);
}

//获取本机权限（以文字表示）
QString StationConfig::getMyRights()
{
    return m_MySatation.strWorkMode;
}

//将权限以文字形式转化为值形式
quint16 StationConfig::transRight(QString str)
{
    //返回的值
    quint16 val = 0;

    //转换结果是否正确
    bool res;

    //转换成十进制整数值，如转换失败，值为0（空字符串也会转换失败）
    val = str.toUShort(&res);

    //转换成功，则返回转换的值
    if(res == true)
    {
        return val;
    }

    //转换不成功，则按文字处理

    //删除文字前的空格、回车、制表等多余符号
    str.trimmed();

    //变成小写
    str = str.toLower();

    //下面是以文字形式的工作模式描述
    if(str == "发流程" || str == "发指挥流程" || str == "发流程用户" || str == "发指挥流程用户")
    {
        return 0x1;
    }

    if(str == "发令" || str == "发指挥命令" || str == "发令用户" || str == "发指挥命令用户")
    {
        return 0x2;
    }

    if(str == "泰坦" || str == "titan")
    {
        return 0x4;
    }

    if(str == "admin" || str == "superuser" || str == "管理员" || str == "指挥员" || str == "超级用户")
    {
        return 0xffff;
    }

    //其他通通都是操作员(普通用户)
    return 0;
}

//将布尔值以文字形式转化为值形式
bool StationConfig::transBool(QString str)
{
    //删除文字前的空格、回车、制表等多余符号
    str.trimmed();

    if(str == "1" || str == "是" || str == "yes")
    {
        return true;
    }

    return false;
}

//检查本机是否具有发令权限
//返回值 false:否  true:是
bool StationConfig::canSendCmd()
{
    return m_iRights & 0x02;
}

//检查本机是否具有发流程权限
//返回值 false:否  true:是
bool StationConfig::canSendCourse()
{
    return m_iRights & 0x01;
}

//查询本机是否记盘
//返回值 false:否  true:是
bool StationConfig::queryRecord()
{
    return m_MySatation.bRecord;
}

//本机记盘路径
QString StationConfig::getRecordPath()
{
    return m_MySatation.strRecordPath;
}
