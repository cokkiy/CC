#pragma once
#include <QString>
#include <QMetaType>
#include <QObject>
#include <QVariant>
#include <QDateTime>
#include <time.h>
/**************************************
 @class: StationInfo 指显工作站信息类,定义了指显工作站
 @see IP, @see MAC @see Name
 **************************************/
class StationInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(State state READ state WRITE setState)
    Q_PROPERTY(float CPU READ CPU WRITE setCPU)
    Q_PROPERTY(float Memory READ Memory WRITE setMemory)
    Q_PROPERTY(float AppCPU READ AppCPU WRITE setAppCPU)
    Q_PROPERTY(int ProcCount READ ProcCount WRITE setProcCount)
    Q_PROPERTY(int AppThreadCount READ AppThreadCount WRITE setAppThreadCount)
    Q_PROPERTY(size_t TotalMemory READ TotalMemory WRITE setTotalMemory)
    Q_PROPERTY(float AppMemory READ AppMemory WRITE setAppMemory)
    Q_PROPERTY(bool AppIsRunning READ AppIsRunning WRITE setAppIsRunning)
    Q_PROPERTY(QString hint READ hint WRITE setHint)
    Q_PROPERTY(int ExecuteCounting READ ExecuteCounting WRITE setExecuteCounting)
    Q_PROPERTY(bool StationIsRunning READ StationIsRunning WRITE setStationIsRunning)
private:
    bool m_StationIsRunning=false;
public:
    //获取工作站是否在运行
    bool StationIsRunning()
    {
        return m_StationIsRunning;
    }
    //设置工作站是否在运行
    void setStationIsRunning(bool value)
    {
        if (m_StationIsRunning != value)
        {
            m_StationIsRunning = value;
            emit propertyChanged("StationIsRunning", this);
        }
    }


public:
    //工作站状态
    enum State
    {
        Unkonown = 0,/*未知*/
        Normal = 1,/*正常*/
        Warning,/*告警*/
        Error,/*异常*/
        Powering, /*加电中...*/
        AppStarting,/*指显软件启动中*/
        Shutdowning,/*关机中...*/
        Rebooting,/*重启中*/
        PowerOnFailure,/*加电失败*/
        AppStartFailure,/*指显程序启动失败*/
        ShutdownFailure,/*关机失败*/
        RebootFailure,/*重启失败*/
    };

    const int PowerOnTimeout = 180;//180秒,启动超时
    const int AppStartingTimeout = 30;//30秒,App启动超时
    const int RebootingTimeout = 240;//240秒,重启超时
    const int ShutingdownTimeout = 180;//180秒,关机

private:
    int m_ExecuteCounting = 0;
public:
    //获取执行计数
    int ExecuteCounting()
    {
        return m_ExecuteCounting;
    }
    //设置执行计数
    void setExecuteCounting(int value)
    {
        if (m_ExecuteCounting != value)
        {
            m_ExecuteCounting = value;
            m_hint = QStringLiteral("命令已发送%1秒...").arg(m_ExecuteCounting);
            emit propertyChanged("ExecuteCounting", this);
        }
    }

private:
    bool m_AppIsRunning = false;
public:
    //获取指显软件是否在运行
    bool AppIsRunning()
    {
        return m_AppIsRunning;
    }
    //设置指显软件是否在运行
    void setAppIsRunning(bool value)
    {
        if (m_AppIsRunning != value)
        {
            m_AppIsRunning = value;
            emit propertyChanged("AppIsRunning", this);
        }
    }

private:
    QString m_hint;
public:
    //获取提示信息
    QString hint()
    {
        return m_hint;
    }
    //设置提示信息
    void setHint(QString value)
    {
        if (m_hint != value)
        {
            m_hint = value;
            emit propertyChanged("hint", this);
        }
    }


private:
    float m_ZXMemory = 0;
public:
    //获取指显软件所占内存百分比
    float AppMemory()
    {
        return m_ZXMemory;
    }
    //设置指显软件所占百分比
    void setAppMemory(float value)
    {
        if (m_ZXMemory != value)
        {
            m_ZXMemory = value;
            emit propertyChanged("AppMemory", this);
        }
    }


private:
    size_t m_TotalMemory = 0;
public:
    //获取总内存
    size_t TotalMemory()
    {
        return m_TotalMemory;
    }
    //设置总内存
    void setTotalMemory(size_t value)
    {
        if (m_TotalMemory != value)
        {
            m_TotalMemory = value;
            emit propertyChanged("TotalMemory", this);
        }
    }


private:
    int m_zxThreadCount = 0;
public:
    //获取指显软件线程数量
    int AppThreadCount()
    {
        return m_zxThreadCount;
    }
    //设置指显软件数量
    void setAppThreadCount(int value)
    {
        if (m_zxThreadCount != value)
        {
            m_zxThreadCount = value;
            emit propertyChanged("AppThreadCount", this);
        }
    }


private:
    int m_ProcCount = 0;
public:
    //获取进程数量
    int ProcCount()
    {
        return m_ProcCount;
    }
    //设置进程数量
    void setProcCount(int value)
    {
        if (m_ProcCount != value)
        {
            m_ProcCount = value;
            emit propertyChanged("ProcCount", this);
        }
    }


private:
    float m_zxCPU = 0;
public:
    //获取应用CPU占用率
    float AppCPU()
    {
        return m_zxCPU;
    }
    //设置应用CPU占用率
    void setAppCPU(float value)
    {
        if (m_zxCPU != value)
        {
            m_zxCPU = value;
            emit propertyChanged("AppCPU", this);
        }
    }

private:
    float m_Memory = 0;
public:
    //获取系统内存总占用率
    float Memory()
    {
        return m_Memory;
    }
    //设置系统内存总占用率
    void setMemory(float value)
    {
        if (m_Memory != value)
        {
            m_Memory = value;
            emit propertyChanged("Memory", this);
        }
    }

private:
    State m_state = Unkonown;
public:
    //获取工作站状态
    State state()
    {
        return m_state;
    }
    //设置工作站状态
    void setState(State value)
    {
        if (m_state != value)
        {
            m_state = value;
            m_hint = QStringLiteral(""); //状态变化时清除原来提示信息
            emit propertyChanged("state", this);
            emit stateChanged(this);
        }
    }

private:
    float m_CPU;
public:
    //获取总CPU占用率
    float CPU()
    {
        return m_CPU;
    }
    //设置总CPU占用率
    void setCPU(float value)
    {
        if (m_CPU != value)
        {
            m_CPU = value;
            emit propertyChanged("CPU", this);
        }
    }

    /*默认构造函数*/
    StationInfo() = default;
    StationInfo(const StationInfo& ref);
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
     工作站标识,区别不同的工作站
     **/
    QString stationId;

    /*
    工作站操作系统名称
    **/
    QString osName;

    /*
    工作站操作系统版本
    **/
    QString osVersion;
    
    /*
    最后一次收到工作站状态时间 
     **/
    time_t lastTick;

public:
    /*!
    订阅属性变化事件
    @param const QObject * receiver 接收者
    @param const char * member 接收者处理函数
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/12 9:35:22
    */
    void subscribePropertyChanged(const QObject* receiver, const char* member);
    /*!
    订阅状态发生变化事件
    @param const QObject * receiver 接收者
    @param const char * member 接收者处理函数
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/13 10:55:42
    */
    void subscribeStateChanged(const QObject* receiver, const char* member);
    /*!
    是否在执行状态
    @return bool 如果在执行状态,返回true,否则返回false
    @remarks 执行状态包括：AppStarting Powering Shutdowning Rebooting
    作者：cokkiy（张立民)
    创建时间：2015/11/12 21:27:35
    */
    bool inExecutingState();

    /*!
    设置工作站标识
    @param const std::string & stationId 工作站唯一标识
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/24 10:41:20
    */
    void setStationId(const std::string& stationId);
    /*!
    设置工作站操作系统名称
    @param const std::string & osName 工作站操作系统名称
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/24 10:41:20
    */
    void setOSName(const std::string& osName);
    /*!
    设置工作站操作系统版本
    @param const std::string & osVersion 工作站操作系统版本
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/24 10:41:20
    */
    void setOSVersion(const std::string& osVersion);


    /*!
    设置最后收到状态时间
    @param const time_t & time 收到状态的时间
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/25 12:17:13
    */
    void setLastTick(const time_t& time = time(NULL));
signals:
    /*!
    属性发生变化
    @param QString propertyName 属性名称
    @param QVariant vlaue 属性值
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/11 21:25:59
    */
    void propertyChanged(const QString& propertyName, const QObject* owner);
    /*状态发生变化时事件*/
    void stateChanged(const QObject* owner);
};

//注册到metatype中
Q_DECLARE_METATYPE(StationInfo::State)
Q_DECLARE_METATYPE(StationInfo)
