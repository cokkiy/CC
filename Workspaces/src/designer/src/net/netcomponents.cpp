#include "netcomponents.h"
#include "netinstance.h"
#include "informationinstance.h"
#include "datacenterinstance.h"
#include "datacentertest.h"
#include "stationinstance.h"
#include "taskinstance.h"
#include "timerinstance.h"

InformationInterface* m_Information = NULL;
NetInterface * m_Net = NULL;
StationInterface * s_Station = NULL;
TaskInterface * s_Task = NULL;
TimerInterface * s_Timer = NULL;
NetComponents::NetComponents()
{
}

NetInterface * NetComponents::getNetCenter(QObject* p)
{
    if(m_Net==NULL)
    {
        m_Net = new NetInstance(p);
    }
    return m_Net;
}

InformationInterface* NetComponents::getInforCenter()
{
    if(m_Information==NULL)
    {
        m_Information = new InformationInstance();
    }
    return m_Information;
}

DataCenterInterface* NetComponents::getDataCenter(DataCenterType type)
{
    switch(type)
    {
    case Normal:
        return new DataCenterInstance();
        break;
    case Test:
    default:
        return new DataCenterTest();
        break;
    }
}

StationInterface * NetComponents::getStation()
{
    if(s_Station==NULL)
    {
        s_Station = new StationInstance();
    }
    return s_Station;
}

TaskInterface* NetComponents::getTastCenter()
{
    if(s_Task==NULL)
    {
        s_Task = new TaskInstance();
    }
    return s_Task;
}

TimerInterface* NetComponents::getTimer()
{
    if(s_Timer==NULL)
    {
        s_Timer = new TimerInstance();
    }
    return s_Timer;
}

