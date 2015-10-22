#include "channelselectconfig.h"
#include "xdocument.h"
#include <QString>
#include <QDebug>

ChannelSelectConfig::ChannelSelectConfig()
{
}

//加载主备信道切换配置表
//param: dir为配置文件所在路径
bool ChannelSelectConfig::load(QString dir)
{
    QString fileName(dir);
    fileName = fileName.append("%1").arg(QString::fromLocal8Bit("主备信道切换配置.xml"));
    QFile file(fileName);
    if (!file.exists())
    {
        qCritical() << QObject::tr("The file: " )<< fileName << QObject::tr(" dose not exists!");
        return false;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << QObject::tr("Can't open net config file: ") << fileName << endl;
        return false;
    }


    bool result = false;
    XDocument doc;

    if ((result = doc.Load(&file)))
    {
        auto elements = doc.getChild().getChildrenByName(QString::fromLocal8Bit("主备切换"));
        for (XElement& element : elements)
        {
            Channel channel;
            channel.ChannelName = element.getChildValue(QString::fromLocal8Bit("信道名称"));
            QString tableNoString = element.getChildValue(QString::fromLocal8Bit("对应表号"));
            //匹配英文和中文逗号,增加容错性
            auto strTableNoList = tableNoString.split(QRegExp("[,，]"), QString::SkipEmptyParts);
            for (QString tableNo : strTableNoList)
            {
                channel.tableNoList.push_back(tableNo.toUInt());
            }
            channel.usingPrimaryChannel = element.getChildBoolValue(QString::fromLocal8Bit("主用信道"));
            //添加到通道列表中
            this->m_channelList.push_back(channel);

            if (!channel.usingPrimaryChannel)
            {
                m_backupChannelList.insert(m_backupChannelList.end(), channel.tableNoList.begin(), channel.tableNoList.end());
            }
        }
    }

    return result;
}

//检查指定的表号<parmref tableNo/>是否使用主用通道数据
//返回true表示使用主用通道数据,返回fasle表示使用备用通道数据
bool ChannelSelectConfig::usingPrimaryChannelData(int tableNo)
{
    for(unsigned int no : m_backupChannelList)
    {
        if (no == tableNo)
            return false;
    }
    return true;
}
