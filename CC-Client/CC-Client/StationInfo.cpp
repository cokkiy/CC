#include "StationInfo.h"
#include <QObject>
#include <assert.h>

bool StationInfo::IsRunning()
{
    return m_state != Unknown;
}


QString StationInfo::Name()
{
    return m_Name;
}

void StationInfo::setName(QString value)
{
    if (m_Name != value)
    {
        m_Name = value;
        emit propertyChanged("Name", this);
    }
}

int StationInfo::ExecuteCounting()
{
    return m_ExecuteCounting;
}

void StationInfo::setExecuteCounting(int value)
{
    if (m_ExecuteCounting != value)
    {
        m_ExecuteCounting = value;
        m_state.setAttachMessage(QStringLiteral("命令已发送%1秒...").arg(m_ExecuteCounting));
        emit propertyChanged("ExecuteCounting", this);
    }
}

bool StationInfo::AppIsRunning()
{
    return m_AppIsRunning;
}

void StationInfo::setAppIsRunning(bool value)
{
    if (m_AppIsRunning != value)
    {
        m_AppIsRunning = value;
        emit propertyChanged("AppIsRunning", this);
    }
}


float StationInfo::AppMemory()
{
    return m_ZXMemory;
}

void StationInfo::setAppMemory(float value)
{
    if (m_ZXMemory != value)
    {
        m_ZXMemory = value;
        emit propertyChanged("AppMemory", this);
    }
}

size_t StationInfo::TotalMemory()
{
    return m_TotalMemory;
}

void StationInfo::setTotalMemory(size_t value)
{
    if (m_TotalMemory != value)
    {
        m_TotalMemory = value;
        emit propertyChanged("TotalMemory", this);
    }
}

int StationInfo::AppThreadCount()
{
    return m_zxThreadCount;
}

void StationInfo::setAppThreadCount(int value)
{
    if (m_zxThreadCount != value)
    {
        m_zxThreadCount = value;
        emit propertyChanged("AppThreadCount", this);
    }
}

int StationInfo::ProcCount()
{
    return m_ProcCount;
}

void StationInfo::setProcCount(int value)
{
    if (m_ProcCount != value)
    {
        m_ProcCount = value;
        emit propertyChanged("ProcCount", this);
    }
}

float StationInfo::AppCPU()
{
    return m_zxCPU;
}

void StationInfo::setAppCPU(float value)
{
    if (m_zxCPU != value)
    {
        m_zxCPU = value;
        emit propertyChanged("AppCPU", this);
    }
}

float StationInfo::Memory()
{
    return m_Memory;
}

void StationInfo::setMemory(float value)
{
    if (m_Memory != value)
    {
        m_Memory = value;
        emit propertyChanged("Memory", this);
    }
}

StationInfo::State StationInfo::state()
{
    return m_state;
}

void StationInfo::setState(State value)
{
    if (m_state != value)
    {
        m_state = value;
        emit propertyChanged("state", this);
        emit stateChanged(this);
    }
}
//设置操作状态和消息
void StationInfo::setState(OperatingStatus status, QString message)
{
    m_state.setStatus(status, message);
}

float StationInfo::CPU()
{
    return m_CPU;
}

void StationInfo::setCPU(float value)
{
    if (m_CPU != value)
    {
        m_CPU = value;
        emit propertyChanged("CPU", this);
    }
}

/*复制构造函数*/
StationInfo::StationInfo(const StationInfo& ref)
{
    this->m_Name = ref.m_Name;
    this->NetworkIntefaces = ref.NetworkIntefaces;
    this->m_state = ref.m_state;
    this->m_CPU = ref.m_CPU;
    this->m_Memory = ref.m_Memory;
    this->m_ProcCount = ref.m_ProcCount;
    this->m_TotalMemory = ref.m_TotalMemory;
    this->m_zxThreadCount = ref.m_zxThreadCount;
    this->m_AppIsRunning = ref.m_AppIsRunning;
    this->m_zxCPU = ref.m_zxCPU;
    this->m_ZXMemory = ref.m_ZXMemory;
    this->m_ExecuteCounting = ref.m_ExecuteCounting;
    this->stationId = ref.stationId;
    this->lastTick = ref.lastTick;
    this->startAppList = ref.startAppList;
    this->monitorProcessList = ref.monitorProcessList;
}


/*!
是否在执行状态
@return bool 如果在执行状态,返回true,否则返回false
@remarks 执行状态包括：AppStarting Powering Shutdowning Rebooting
作者：cokkiy（张立民)
创建时间：2015/11/12 21:27:35
*/
bool StationInfo::inExecutingState()
{
    return state() == AppStarting || state() == Powering
        || state() == Shutdowning || state() == Rebooting;
}

/*!
设置工作站标识
@param const std::string & stationId 工作站唯一标识
@return void
作者：cokkiy（张立民)
创建时间：2015/11/24 10:41:20
*/
void StationInfo::setStationId(const std::string& stationId)
{
    this->stationId = QString(stationId.c_str());
}

/*!
设置工作站操作系统名称
@param const std::string & osName 工作站操作系统名称
@return void
作者：cokkiy（张立民)
创建时间：2015/11/24 10:41:20
*/
void StationInfo::setOSName(const std::string& osName)
{
    this->osName = QString(osName.c_str());
    emit propertyChanged("osName", this);
}

/*!
设置工作站操作系统版本
@param const std::string & osVersion 工作站操作系统版本
@return void
作者：cokkiy（张立民)
创建时间：2015/11/24 10:41:20
*/
void StationInfo::setOSVersion(const std::string& osVersion)
{
    this->osVersion = QString(osVersion.c_str());
    emit propertyChanged("osVersion", this);
}

/*!
设置最后收到状态时间
@param const time_t & time 收到状态的时间
@return void
作者：cokkiy（张立民)
创建时间：2015/11/25 12:17:13
*/
void StationInfo::setLastTick(const time_t& time /*= time(NULL)*/)
{
    this->lastTick = time;
}

/*!
返回描述工作站的字符串
@return QString 描述工作站的字符串
作者：cokkiy（张立民)
创建时间：2015/12/01 16:35:23
*/
QString StationInfo::toString()
{
    QString allIP = QStringLiteral("");
    for (auto& ni : NetworkIntefaces)
    {
        allIP += ni.getAllIPString() + ",";
    }
    allIP = allIP.left(allIP.length() - 1);
    return QStringLiteral("%1 IP(%2)").arg(m_Name).arg(allIP);
}

/*!
获取表示所有IP地址的字符串,多个IP用逗号分隔
@return QString 表示所有IP地址的字符串,多个IP用逗号分隔
作者：cokkiy（张立民)
创建时间：2015/12/01 16:50:44
*/
QString StationInfo::IP()
{
    QString allIP = QStringLiteral("");
    for (auto& ni : NetworkIntefaces)
    {
        allIP = allIP.append(ni.getAllIPString()).append(",");
    }
    return allIP.left(allIP.length() - 1);
}

/*!
获取表示所有MAC地址的字符串,多个MAC地址用逗号分隔
@return QString 表示所有MAC地址的字符串,多个MAC地址用逗号分隔
作者：cokkiy（张立民)
创建时间：2015/12/01 17:09:46
*/
QString StationInfo::MAC()
{
    QString result;
    for (auto& ni : NetworkIntefaces)
    {
        result = result.append(ni.getMAC()).append(",");
    }
    return result.left(result.length() - 1);
}

/*!
返回工作站所有配置信息的XML字符串
@return QString 工作站所有配置信息的XML字符串
作者：cokkiy（张立民)
创建时间：2015/12/02 22:49:13
*/
QString StationInfo::toXmlString()
{
    QString xml = QStringLiteral("  <指显工作站>\r\n")
        + QStringLiteral("    <名称>%1</名称>\r\n").arg(m_Name);
    for (auto&ni : NetworkIntefaces)
    {
        xml += QStringLiteral("    <网卡>\r\n")
            + QStringLiteral("      <MAC>%1</MAC>\r\n").arg(ni.getMAC());
        for (auto&ip : ni.getIPList())
        {
            xml += QStringLiteral("      <IP>%1</IP>\r\n").arg(QString::fromStdString(ip));
        }
        xml += QStringLiteral("    </网卡>\r\n");
    }
    xml += QStringLiteral("    <启动程序>\r\n"); 
    for (auto& app : getStartAppNames())
    {
        xml += QStringLiteral("      <程序>%1</程序>\r\n").arg(app);
    }
    xml += QStringLiteral("    </启动程序>\r\n");
    xml += QStringLiteral("    <监视进程>\r\n");
    for (auto& proc : getMonitorProcNames())
    {
        xml += QStringLiteral("      <进程>%1</进程>\r\n").arg(proc);
    }
    xml += QStringLiteral("    </监视进程>\r\n");
    xml += QStringLiteral("  </指显工作站>\r\n");
    return xml;
}



void StationInfo::clearMonitorProc()
{
    this->monitorProcessList.clear();
}

void StationInfo::addMonitorProc(QString procName)
{
    this->monitorProcessList.push_back(procName);
}

QStringList StationInfo::getStartAppNames()
{
    return startAppList;
}

void StationInfo::addStartApp(QString name)
{
    startAppList.push_back(name);
}

void StationInfo::addStartApps(QStringList names)
{
    startAppList.append(names);
}

void StationInfo::clearStartApp()
{
    startAppList.clear();
}

QStringList StationInfo::getMonitorProcNames()
{
    return monitorProcessList;
}

void StationInfo::UpdateStation()
{
    emit stationChanged(this);
}

/*!
判断两个工作站信息是否是同一个对象
@param const StationInfo & ref 比较对象
@return bool 相同返回true,否则返回false
作者：cokkiy（张立民)
创建时间：2015/12/03 16:02:23
*/
bool StationInfo::operator==(const StationInfo& ref)
{
    return this == &ref;
}

/*!
订阅属性变化事件
@param const QObject * receiver 接收者
@param const char * member 接收者处理函数
@return void
作者：cokkiy（张立民)
创建时间：2015/11/12 9:35:22
*/
void StationInfo::subscribePropertyChanged(const QObject* receiver, const char* member)
{
    connect(this, SIGNAL(propertyChanged(const QString&, const QObject*)), receiver, member);
}

/*!
订阅状态发生变化事件
@param const QObject * receiver 接收者
@param const char * member 接收者处理函数
@return void
作者：cokkiy（张立民)
创建时间：2015/11/13 10:55:42
*/
void StationInfo::subscribeStateChanged(const QObject* receiver, const char* member)
{
    connect(this, SIGNAL(stateChanged(const QObject*)), receiver, member);
}

/*!
订阅由于用户编辑导致的工作站发生变化事件
@param const QObject * receiver 接收者
@param const char * member 接收者处理函数
@return void
作者：cokkiy（张立民)
创建时间：2015/11/13 10:55:42
*/
void StationInfo::subscribeStationChanged(const QObject* receiver, const char* member)
{
    connect(this, SIGNAL(stationChanged(const QObject*)), receiver, member);
}

bool StationInfo::State::operator!=(const OperatingStatus& status)
{
    return operatingStatus != status;
}

bool StationInfo::State::operator!=(const RunningState& state)
{
    return runningState != state;
}

bool StationInfo::State::operator==(const OperatingStatus& status)
{
    return operatingStatus == status;
}

bool StationInfo::State::operator==(const RunningState& state)
{
    return runningState == state;
}

/*
设置操作状态(切换操作状态会改变运行状态)
**/
StationInfo::State::State(const OperatingStatus& opStatus)
{
    setOperatingStatus(opStatus);
}

/*!
设置运行状态(切换运行状态会切换操作状态)
@param RunningState runningState 运行状态,只允许设置为Unknown或Normal,其他状态通过操作状态设置
@return
作者：cokkiy（张立民)
创建时间：2015/12/03 21:33:25
*/
StationInfo::State::State(const RunningState& runningState)
{
    assert(runningState == Unknown || runningState == Normal);
    this->runningState = runningState;
    switch (runningState)
    {
    case Unknown:
        this->operatingStatus = PowerOffOrNetworkFailure;
        break;
    case  Normal:
        this->operatingStatus = Running;
        break;
    default:
        break;
    }
}

/*
 两个状态是否不相同
 **/
bool StationInfo::State::operator!=(const State& right)
{
    return this->runningState != right.runningState
        || this->operatingStatus != right.operatingStatus;
}
/*
两个状态是否相同
**/
bool StationInfo::State::operator==(const State& right)
{
    return this->runningState == right.runningState
        && this->operatingStatus == right.operatingStatus;
}

/*!
设置操作状态和附加消息
@param const OperatingStatus & status 操作状态
@param QString attachMessage 附加消息
@return void
作者：cokkiy（张立民)
创建时间：2015/12/04 9:10:31
*/
void StationInfo::State::setStatus(const StationInfo::OperatingStatus& status, const QString& attachMessage /*= QStringLiteral("")*/)
{
    setOperatingStatus(status);
    this->attachMessage = attachMessage;
}

/*!
设置附加消息
@param QString message 附加消息
@return void
作者：cokkiy（张立民)
创建时间：2015/12/04 10:37:55
*/
void StationInfo::State::setAttachMessage(const QString& message)
{
    this->attachMessage = message;
}

/*!
获取代表当前状态（包括运行和操作)的字符串
@return QString 代表当前状态（包括运行和操作)的字符串
作者：cokkiy（张立民)
创建时间：2015/12/03 22:14:30
*/
QString StationInfo::State::toString()
{
    QString result = QStringLiteral("状态：%1 %2 %3");    
    return result.arg(translate(runningState)).arg(translate(operatingStatus)).arg(attachMessage);
}


/*!
获取当前运行状态
@return StationInfo::RunningState 当前运行状态
作者：cokkiy（张立民)
创建时间：2015/12/04 9:24:57
*/
StationInfo::RunningState StationInfo::State::getRunningState()
{
    return runningState;
}

/*!
获取当前操作状态
@return StationInfo::RunningState 当前操作状态
作者：cokkiy（张立民)
创建时间：2015/12/04 9:24:57
*/
StationInfo::OperatingStatus StationInfo::State::getOperatingStatus()
{
    return operatingStatus;
}

/*
 设置操作状态.同时切换运行状态
 **/
void StationInfo::State::setOperatingStatus(const OperatingStatus& opStatus)
{
    this->operatingStatus = opStatus;
    switch (opStatus)
    {
    case StationInfo::PowerOffOrNetworkFailure:
        runningState = Unknown;
        break;
    case  StationInfo::NoHeartbeat:
        runningState = Warning;
        break;
    case StationInfo::Running:
        runningState = Normal;
        break;
    case StationInfo::Powering:
        runningState = Unknown;
        break;
    case StationInfo::Shutdowning:
    case StationInfo::Rebooting:
        runningState = Warning;
        break;
    case StationInfo::PowerOnFailure:
    case StationInfo::AppStartFailure:
    case StationInfo::ShutdownFailure:
    case StationInfo::RebootFailure:
        runningState = Error;
    case CPUTooHigh:
    case  MemoryTooHigh:
    case DiskFull:
        runningState = Warning;
        break;
    default:
        break;
    }
}

/*翻译运行状态*/
QString StationInfo::State::translate(RunningState state)
{
    QString result = QStringLiteral("");
    switch (state)
    {
    case StationInfo::Unknown:
        result = tr("Unknown");
        break;
    case StationInfo::Normal:
        result = tr("Normal");
        break;
    case StationInfo::Warning:
        result = tr("Warning");
        break;
    case StationInfo::Error:
        result = tr("Error");
        break;
    default:
        break;
    }
    return result;
}
/*翻译操作状态*/
QString StationInfo::State::translate(OperatingStatus status)
{
    QString result = QStringLiteral("");
    switch (status)
    {
    case StationInfo::PowerOffOrNetworkFailure:
        result = tr("PowerOffOrNetworkFailure");
        break;
    case StationInfo::NoHeartbeat:
        result = tr("NoHeartbeat");
        break;
    case StationInfo::Powering:
        result = tr("Powering");
        break;
    case StationInfo::AppStarting:
        result = tr("AppStarting");
        break;
    case StationInfo::Shutdowning:
        result = tr("Shutdowning");
        break;
    case StationInfo::Rebooting:
        result = tr("Rebooting");
        break;
    case StationInfo::PowerOnFailure:
        result = tr("PowerOnFailure");
        break;
    case StationInfo::AppStartFailure:
        result = tr("AppStartFailure");
        break;
    case StationInfo::ShutdownFailure:
        result = tr("ShutdownFailure");
        break;
    case StationInfo::RebootFailure:
        result = tr("RebootFailure");
        break;
    case StationInfo::MemoryTooHigh:
        result = tr("MemoryTooHigh");
        break;
    case StationInfo::CPUTooHigh:
        result = tr("CPUTooHigh");
        break;
    case StationInfo::DiskFull:
        result = tr("DiskFull");
        break;
    case StationInfo::Running:
        result = tr("Running");
        break;
    default:
        break;
    }

    return result;
}
