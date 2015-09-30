// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：ChannelSelectConfig.h
// 文件摘要：主备信道切换配置类头文件，用来声明主备信道切换配置类。
//
// 原始版本：1.0
// 当前版本：1.0
// 作    者：张立民
// 完成日期：2015-09-02
//*************************************************************//
#ifndef CHANNELSELECTCONFIG_H
#define CHANNELSELECTCONFIG_H
#include <QString>
#include <list>
using namespace std;


//主备信道切换配置表
class ChannelSelectConfig
{
public:
    //信道定义
    struct Channel
    {
        //信道名称
        QString ChannelName;

        //表号列表
        list<quint32> tableNoList;

        //是否使用主用信道，true为使用主用通道,false为使用备用通道
        bool usingPrimaryChannel;
    };

public:
    ChannelSelectConfig();


    //获取信道列表开始位置
    list<Channel>::const_iterator begin()
    {
        return m_channelList.begin();
    }

    //获取信道列表结束位置
    list<Channel>::const_iterator end()
    {
        return m_channelList.end();
    }

    //加载主备信道切换配置表
    //param: dir为配置文件所在路径
    bool load(QString dir);


    //检查指定的表号<parmref tableNo/>是否使用主用通道数据
    //返回true表示使用主用通道数据,返回fasle表示使用备用通道数据
    bool usingPrimaryChannelData(int tableNo);


private:
    //主备信道切换配置信息，包含每个信道的主备切换信息
    list<Channel> m_channelList;
    //备用通道列表,不在备用通道列表内的全部用主用通道
    list<quint32> m_backupChannelList;
};

#endif // CHANNELSELECTCONFIG_H
