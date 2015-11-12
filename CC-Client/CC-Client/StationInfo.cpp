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
    this->m_ZXIsRunning = ref.m_ZXIsRunning;
    this->m_zxCPU = ref.m_zxCPU;
    this->m_ZXMemory = ref.m_ZXMemory;
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
    default:
        result = QObject::tr("Unknown");
        break;
    }
    result += QStringLiteral("  %1").arg(hint());
    return result;
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
