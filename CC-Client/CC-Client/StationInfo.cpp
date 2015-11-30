#include "StationInfo.h"
#include <QObject>

/*复制构造函数*/
StationInfo::StationInfo(const StationInfo& ref)
{
    this->name = ref.name;
    this->IP = ref.IP;
    this->mac = ref.mac;
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
