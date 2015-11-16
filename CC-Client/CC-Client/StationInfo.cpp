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
