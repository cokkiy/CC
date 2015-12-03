#include "StationInfo.h"
#include <QObject>

bool StationInfo::StationIsRunning()
{
    return m_StationIsRunning;
}

void StationInfo::setStationIsRunning(bool value)
{
    if (m_StationIsRunning != value)
    {
        m_StationIsRunning = value;
        emit propertyChanged("StationIsRunning", this);
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
        m_hint = QStringLiteral("命令已发送%1秒...").arg(m_ExecuteCounting);
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

QString StationInfo::hint()
{
    return m_hint;
}

void StationInfo::setHint(QString value)
{
    if (m_hint != value)
    {
        m_hint = value;
        emit propertyChanged("hint", this);
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
        m_hint = QStringLiteral(""); //状态变化时清除原来提示信息
        emit propertyChanged("state", this);
        emit stateChanged(this);
    }
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
    this->name = ref.name;
    this->NetworkIntefaces.insert(this->NetworkIntefaces.begin(), ref.NetworkIntefaces.begin(), ref.NetworkIntefaces.end());
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
    this->m_hint = ref.m_hint;
    this->m_StationIsRunning = ref.m_StationIsRunning;
    this->stationId = ref.stationId;
    this->lastTick = ref.lastTick;
}

/*!
返回表示状态的字符
@return QString \see State表示状态的字符
作者：cokkiy（张立民)
创建时间：2015/11/06 16:22:51
*/
QString StationInfo::state2String()
{
    QString result;
    switch (m_state)
    {
    case StationInfo::Unkonown:
        result = QObject::tr("Unknown");
        break;
    case StationInfo::Normal:
        result = QObject::tr("Normal");
        break;
    case StationInfo::Warning:
        result = QObject::tr("Warning");
        break;
    case StationInfo::Error:
        result = QObject::tr("Error");
        break;
    case Powering:
        result = QObject::tr("Powering");
        break;
    case  AppStarting:
        result = QObject::tr("AppStarting");
        break;
    case Shutdowning:
        result = QObject::tr("Shutdowning");
        break;
    case  Rebooting:
        result = QObject::tr("Rebooting");
        break;
    case PowerOnFailure:
        result = QObject::tr("PowerOnFailure");
        break;
    case RebootFailure:
        result = QObject::tr("RebootFailure");
        break;
    case AppStartFailure:
        result = QObject::tr("AppStartFailure");
        break;
    case ShutdownFailure:
        result = QObject::tr("ShutdownFailure");
        break;
    default:
        result = QObject::tr("Unknown");
        break;
    }
    result += QStringLiteral("  %1").arg(hint());
    return result;
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
    return QStringLiteral("%1 IP(%2)").arg(name).arg(allIP);
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
        + QStringLiteral("    <名称>%1</名称>\r\n").arg(name);
    for (auto&ni : NetworkIntefaces)
    {
        xml += QStringLiteral("    <网卡>")
            + QStringLiteral("      <MAC>%1</MAC>").arg(ni.getMAC());
        for (auto&ip : ni.getIPList())
        {
            xml += QStringLiteral("      <IP>%1</IP>").arg(QString::fromStdString(ip));
        }
        xml += QStringLiteral("    </网卡>");
    }
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
