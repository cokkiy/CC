#pragma once
#include <QString>
#include <QMetaType>
/**************************************
 @class: StationInfo 指显工作站信息类,定义了指显工作站
 @see IP, @see MAC @see Name
 **************************************/
class StationInfo
{
public:
    //工作站状态
    enum State
    {
        Unkonown=0,/*未知*/
        Normal=1,/*正常*/
        Warning,/*告警*/
        Error,/*异常*/
        Powering, /*加电中...*/
        AppStarting,/*指显软件启动中*/
    };

    /*!
    返回表示状态的字符
    @return QString \see State表示状态的字符
    作者：cokkiy（张立民)
    创建时间：2015/11/06 16:22:51
    */
    QString state2String();

    /*
    工作站名称
    **/
    QString name;
    /*
    工作站MAC地址
    **/
    QString mac;
    /*
     工作站IP地址
     **/
    QString IP;

    /*
     工作站状态
     **/
    State state;

    /*
     总cpu占用率
     **/
    float CPU=0;
    /*
     总内存占用率
     **/
    float Memory=0;

    /*
     总内存
     **/
    float totalMemory = 0;

    /*
     总进程数
     **/
    int procCount = 0;

    /*
     指显程序内存占用率
     **/
    float zxMemory = 0;

    /*
     指显程序cpu占用率
     **/
    float zxCPU = 0;
    /*
     指显程序线程数
     **/
    int zxThreadCount = 0;

    /*
     指显程序是否运行
     **/
    bool zxIsRunning = false;
};

//注册到metatype中
Q_DECLARE_METATYPE(StationInfo)
