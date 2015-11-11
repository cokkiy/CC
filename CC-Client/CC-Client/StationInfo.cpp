#include "StationInfo.h"
#include "..\..\Qt5.5.0\5.5\Src\qtbase\src\corelib\kernel\qobject.h"

/*!
返回表示状态的字符
@return QString \see State表示状态的字符
作者：cokkiy（张立民)
创建时间：2015/11/06 16:22:51
*/
QString StationInfo::state2String()
{
    switch (state)
    {
    case StationInfo::Unkonown:
        return QObject::tr("Unknown");
        break;
    case StationInfo::Normal:
        return QObject::tr("Normal");
        break;
    case StationInfo::Warning:
        return QObject::tr("Warning");
        break;
    case StationInfo::Error:
        return QObject::tr("Error");
        break;
    case Powering:
        return QObject::tr("Powering");
        break;
    case  AppStarting:
        return QObject::tr("AppStarting");
        break;
    default:
        return QObject::tr("Unknown");
        break;
    }
}
