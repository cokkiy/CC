#pragma once
#include <QString>
#include <QMetaType>
#include <QObject>
#include <QVariant>
#include <time.h>
#include <list>
#include <QMetaEnum>
#include "NetworkInterface.h"
#include "IController.h"
#include "IStationStateReceiver.h"

//性能计数历史数据点数
#define  CounterHistoryDataSize 100

/**************************************
 @class: StationInfo 指显工作站信息类,定义了指显工作站
 @see IP, @see MAC @see Name
 **************************************/
class StationInfo : public QObject
{
    Q_OBJECT
        Q_PROPERTY(float CPU READ CPU WRITE setCPU)
        Q_PROPERTY(float Memory READ Memory WRITE setMemory)
        Q_PROPERTY(float AppCPU READ AppCPU WRITE setAppCPU)
        Q_PROPERTY(int ProcCount READ ProcCount WRITE setProcCount)
        Q_PROPERTY(int AppThreadCount READ AppThreadCount WRITE setAppThreadCount)
        Q_PROPERTY(size_t TotalMemory READ TotalMemory WRITE setTotalMemory)
        Q_PROPERTY(float AppMemory READ AppMemory WRITE setAppMemory)
        Q_PROPERTY(int ExecuteCounting READ ExecuteCounting WRITE setExecuteCounting)
        Q_PROPERTY(QString Name READ Name WRITE setName)

public:
    /*
     @enum RunningState 工作站运行状态
     **/
    enum RunningState
    {
        Unknown = 0,/*未知*/
        Normal = 1,/*正常*/
        Warning,/*告警*/
        Error,/*异常*/
    };
    Q_ENUM(RunningState)
        /*
         工作站操作状态
         **/
    enum OperatingStatus
    {
        PowerOffOrNetworkFailure,/*没有加电或网络故障*/
        NoHeartbeat,/*没有心跳检测信号*/
        Powering, /*加电中...*/
        AppStarting,/*软件启动中*/
        Shutdowning,/*关机中...*/
        Rebooting,/*重启中*/
        PowerOnFailure,/*加电失败*/
        AppStartFailure,/*程序启动失败*/
        AppCloseFailure,/*程序退出失败*/
        ShutdownFailure,/*关机失败*/
        RebootFailure,/*重启失败*/
        AppStarted,/*软件已启动*/
        AppNotRunning,/*软件未启动*/
        SomeAppNotRunning,/*部分软件没有启动*/
        MemoryTooHigh,/*内存占用率太高*/
        CPUTooHigh,/*CPU占用率太高*/
        DiskFull,/*磁盘满*/
        Running,/*正常运行*/
        GeneralError,/*一般性错误*/
        CanNotOperating,/*不能操作*/
    };
    Q_ENUM(OperatingStatus)

        /*
         工作站状态
         **/
    class State
    {
    public:
        /*默认构造函数*/
        State();

    public:
        /*判断运行状态是否相等*/
        bool operator ==(const RunningState& state);
        /*判断操作状态是否相等*/
        bool operator ==(const OperatingStatus& status);
        /*判断运行状态是否不等于*/
        bool operator!=(const RunningState& state);
        /*判断操作状态是否不等于*/
        bool operator !=(const OperatingStatus& status);
        /*两个状态是否不相等*/
        bool operator != (const State& right);
        /*两个状态是否相等*/
        bool operator ==(const State& right);

        /*!
        设置操作状态和附加消息
        @param const OperatingStatus & status 操作状态
        @param QString attachMessage 附加消息
        @return void
        作者：cokkiy（张立民)
        创建时间：2015/12/04 9:10:31
        */
        void setStatus(const OperatingStatus& status, QString attachMessage = QStringLiteral(""));


        /*!
        设置附加消息
        @param QString message 附加消息
        @return void
        作者：cokkiy（张立民)
        创建时间：2015/12/04 10:37:55
        */
        void setAttachMessage(QString message);

        /*!
        获取代表当前状态（包括运行和操作)的字符串
        @return QString 代表当前状态（包括运行和操作)的字符串
        作者：cokkiy（张立民)
        创建时间：2015/12/03 22:14:30
        */
        QString toString();


        /*!
        返回表示当前状态的Html字符串
        @return QString 表示当前状态的Html字符串
        作者：cokkiy（张立民)
        创建时间：2015/12/11 16:08:49
        */
        QString toHtmlString();

        /*!
        获取当前运行状态
        @return StationInfo::State::RunningState
        作者：cokkiy（张立民)
        创建时间：2015/12/04 9:24:57
        */
        RunningState getRunningState();

        /*!
        获取当前操作状态
        @return StationInfo::State::OperatingStatus
        作者：cokkiy（张立民)
        创建时间：2015/12/04 9:24:41
        */
        OperatingStatus getOperatingStatus();
    private:
        /*
         工作站运行状态
         **/
        StationInfo::RunningState runningState;
        /*
         工作站操作状态
         **/
        StationInfo::OperatingStatus operatingStatus;
        /*
         附加的消息
         **/
        QString attachMessage;

        /*
         设置操作状态
         **/
        void setOperatingStatus(const   StationInfo::OperatingStatus& opStatus);
        /*翻译运行状态*/
        QString translate(RunningState state);
        /*翻译操作状态*/
        QString translate(OperatingStatus status);
    };

    /*
    性能数据
    **/
    struct PerfCounterData
    {
        double cpuData[CounterHistoryDataSize];
        double memoryData[CounterHistoryDataSize];
    };

    const int PowerOnTimeout = 180;//180秒,启动超时
    const int AppStartingTimeout = 30;//30秒,App启动超时
    const int RebootingTimeout = 240;//240秒,重启超时
    const int ShutingdownTimeout = 180;//180秒,关机    
private:
    QString m_Name;
public:
    QString Name();
    void setName(QString value);

public:
    /*!
    获取工作站是否在运行
    @return bool
    作者：cokkiy（张立民)
    创建时间：2015/12/03 21:11:40
    */
    bool IsRunning();

private:
    int m_ExecuteCounting = 0;
public:
    //获取执行计数
    int ExecuteCounting();
    //设置执行计数
    void setExecuteCounting(int value);


private:
    float m_ZXMemory = 0;
public:
    //获取指显软件所占内存百分比
    float AppMemory();
    //设置指显软件所占百分比
    void setAppMemory(float value);


private:
    size_t m_TotalMemory = 0;
public:
    //获取总内存
    size_t TotalMemory();
    //设置总内存
    void setTotalMemory(size_t value);


private:
    int m_zxThreadCount = 0;
public:
    //获取指显软件线程数量
    int AppThreadCount();
    //设置指显软件数量
    void setAppThreadCount(int value);


private:
    int m_ProcCount = 0;
public:
    //获取进程数量
    int ProcCount();
    //设置进程数量
    void setProcCount(int value);


private:
    float m_zxCPU = 0;
public:
    //获取应用CPU占用率
    float AppCPU();
    //设置应用CPU占用率
    void setAppCPU(float value);

private:
    float m_Memory = 0;
public:
    //获取系统内存总占用率
    float Memory();
    //设置系统内存总占用率
    void setMemory(float value);

private:
    State m_state;
public:
    //获取工作站状态
    State state();

    //设置操作状态和消息
    void setState(OperatingStatus status, const QString& message=QStringLiteral(""));

private:
    float m_CPU = 0;
public:
    //获取总CPU占用率
    float CPU();
    //设置总CPU占用率
    void setCPU(float value);

    /*默认构造函数*/
    StationInfo() = default;
    StationInfo(const StationInfo& ref);

    /*
    工作站网卡信息
    **/
    std::list<NetworkInterface> NetworkIntefaces;


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

    /*
    自动监视远程启动程序
    */
    bool autoMonitorRemoteStartApp = true;
private:

    /*
    应用程序运行状态
     **/
    std::list<CC::AppRunningState> AppsRunningState;

    /*
    需要启动程序名称列表
    **/
    std::list<std::pair<QString, QString>> startAppList;

    /*
    需要监视进程列表
    **/
    QStringList monitorProcessList;

    /*
     需要监视的远程启动程序进程信息
     **/
    std::map<std::string, int> tempMonitorProcesses;

    /*
     所有监视进程名称列表,远程启动的进程用Id:xxx代表,xxx表示进程Id
     **/
    QStringList allMonitorProcess;

    /*
     操作历史消息
     **/
    std::list<QString> operatingHistoryMessages;

    /*
     应该监视进程总数
     **/
    int shouldMonitorProcessesCount = 0;   

public:

    /*
    所有被监视性能数据集合
    **/
    std::map<std::string, PerfCounterData> counterData;

    /*!
    获取应该监视进程数量
    @return int 应该监视进程数量=监视进程列表数量+（如果自动监控)远程启动程序数量
    作者：cokkiy（张立民)
    创建时间：2015/12/14 15:36:46
    */
    int getShouldMonitorProcessesCount();

    /*
    构建全部监视进程列表
    **/
    void buildAllMonitorProcessList();

    /*!
    设置应用程序运行状态
    @param const ::CC::AppsRunningState & appsRunningState 应用程序运行状态
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/11 16:58:29
    */
    void setAppsRunningState(const ::CC::AppsRunningState& appsRunningState);

    /*!
    清空历史消息
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/11 16:00:45
    */
    void clearOperatingHistoryMessages();

    /*!
    获取操作历史消息
    @return std::list<QString> 操作历史消息
    作者：cokkiy（张立民)
    创建时间：2015/12/11 16:01:06
    */
    std::list<QString> getOperatingHistoryMessages();

    /*
     *控制代理,通过此代理向工作站发送控制命令
     **/
    CC::IControllerPrx controlProxy;
    /*!
    获取需要启动程序名称列表
    @return std::list<std::pair<QString, QString>> 需要启动程序名称和参数列表
    作者：cokkiy（张立民)
    创建时间：2015/12/01 22:25:20
    */
    std::list<std::pair<QString, QString>> getStartAppNames();

    /*!
    添加启动程序
    @param QString path 启动程序路径
    @param QString arguments 启动程序参数
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/02 16:47:03
    */
    void addStartApp(const QString& path, const QString& arguments);

    /*!
    添加一系列启动程序
    @param const std::list<std::pair<QString, QString>>& apps 启动程序名称列表
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/02 16:47:31
    */
    void addStartApps(const std::list<std::pair<QString, QString>>& apps);

    /*!
    清空启动程序列表
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/03 21:45:48
    */
    void clearStartApp();

    /*!
    获取需要监视进程列表
    @return QStringList 需要监视进程列表
    作者：cokkiy（张立民)
    创建时间：2015/12/01 22:26:23
    */
    QStringList getMonitorProcNames();

    /*!
    清空监视进程列表
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/02 16:48:20
    */
    void clearMonitorProc();
    /*!
    添加一个监视进程名称
    @param QString procName
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/02 16:48:48
    */
    void addMonitorProc(const QString& procName);

    /*!
    添加需要监视的进程列表
    @param const std::list<QString>& processes 需要监视的进程列表
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/04 16:19:47
    */
    void addMonitorProcs(const std::list<QString>& processes);
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
    订阅由于用户编辑导致的工作站发生变化事件
    @param const QObject * receiver 接收者
    @param const char * member 接收者处理函数
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/13 10:55:42
    */
    void subscribeStationChanged(const QObject* receiver, const char* member);

    /*!
    是否在执行状态
    @return bool 如果在执行状态,返回true,否则返回false
    @remarks 执行状态包括：AppStarting Powering Shutdowning Rebooting
    作者：cokkiy（张立民)
    创建时间：2015/11/12 21:27:35
    */
    bool inExecutingState();

    /*!
    是否在关机或重启状态
    @return bool
    作者：cokkiy（张立民)
    创建时间：2015/12/16 17:01:53
    */
    bool inRebootingOrShutdown();

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

    /*!
    返回描述工作站的字符串
    @return QString 描述工作站的字符串
    作者：cokkiy（张立民)
    创建时间：2015/12/01 16:35:23
    */
    QString toString();


    /*!
    获取表示所有IP地址的字符串,多个IP用逗号分隔
    @return QString 表示所有IP地址的字符串,多个IP用逗号分隔
    作者：cokkiy（张立民)
    创建时间：2015/12/01 16:50:44
    */
    QString IP();
    /*!
    获取表示所有MAC地址的字符串,多个MAC地址用逗号分隔
    @return QString 表示所有MAC地址的字符串,多个MAC地址用逗号分隔
    作者：cokkiy（张立民)
    创建时间：2015/12/01 17:09:46
    */
    QString MAC();

    /*!
    返回工作站所有配置信息的XML字符串
    @return QString 工作站所有配置信息的XML字符串
    作者：cokkiy（张立民)
    创建时间：2015/12/02 22:49:13
    */
    QString toXmlString();


    /*!
    更新工作站,用户编辑工作站属性后,调用该函数通知更新
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/03 11:27:57
    */
    void UpdateStation();

    /*!
    检查是否执行动作是否超时
    @return bool
    作者：cokkiy（张立民)
    创建时间：2015/12/10 20:51:14
    */
    bool timeout();

    /*!
    获取需要监视的进程名称列表（包括已经启动的程序)
    @return std::list<std::string> 需要监视的进程名称列表（包括已经启动的程序)
    作者：cokkiy（张立民)
    创建时间：2015/12/11 9:03:22
    */
    std::list<std::string> getAllMonitorProc();

    /*!
    清空远程启动进程监视列表
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/14 10:09:56
    */
    void clearTempMonitorProcess();

    /*!
    添加远程启动进程到临时监视列表
    @param const ::std::string&  startParam 启动参数 = 路径 && 参数 
    @param int Id 进程Id
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/11 10:07:37
    */
    void addTempMonitorProcess(const ::std::string& startParam, int Id);

    /*!
    获取全部正在运行的被监控程序进程Id
    @return  list<int> 全部正在运行的被监控程序进程Id
    作者：cokkiy（张立民)
    创建时间：2015/12/16 10:34:18
    */
    std::list<int> getRunningProcessesId();

    /*!
    获取指定远程启动程序进程Id
    @param const ::std::string&  startParam 启动参数 = 路径 && 参数 
    @return int 进程Id
    作者：cokkiy（张立民)
    创建时间：2015/12/16 10:37:21
    */
    int getRemoteStartedProcessId(const ::std::string&  startParam);

    /*!
    判断两个工作站信息是否是同一个对象（指针相同)
    @param const StationInfo & ref 比较对象
    @return bool 相同返回true,否则返回false
    作者：cokkiy（张立民)
    创建时间：2015/12/03 16:02:23
    */
    bool operator ==(const StationInfo& ref);

signals:
    /*!
    属性发生变化事件
    @param QString propertyName 属性名称
    @param QVariant vlaue 属性值
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/11/11 21:25:59
    */
    void propertyChanged(const QString& propertyName, const QObject* owner);
    /*状态发生变化时事件*/
    void stateChanged(const QObject* owner);


    /*!
    工作站发生变化（用户编辑了工作站属性)事件
    @param const QObject * owner 属性发生变化的工作站
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/03 11:24:32
    */
    void stationChanged(const QObject* owner);

};

//注册到metatype中
Q_DECLARE_METATYPE(StationInfo::State)
Q_DECLARE_METATYPE(StationInfo)
