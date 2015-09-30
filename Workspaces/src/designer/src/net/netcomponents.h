#ifndef NETCOMPONENTS_H
#define NETCOMPONENTS_H
#include "net_global.h"
#include "abstractnet.h"
#include "abstractinformation.h"
#include "abstractdatacenter.h"
#include "abstractstation.h"
#include "abstracttask.h"
#include "abstracttimer.h"
class NETSHARED_EXPORT NetComponents
{
public:
    //DataCenterInterface的类型
    enum DataCenterType{
        Normal,//获取实时网络数据
        Test,//程序模拟数据用于测试
    };
public:
    /*!
      获取NetInterface的指针,该指针指向为全局唯一NetInterface实例,
        程序退出时需要自己清除。
      @param QObject* parent  用于构建线程的参数
      @return NetInterface*  NetInterface的指针
    */
    static NetInterface * getNetCenter(QObject* parent);
    /*!
      获取InformationInterface的指针,该指针指向为全局唯一InformationInterface实例,
        程序退出时需要自己清除.
      @return InformationInterface*  InformationInterface的指针
    */
    static InformationInterface* getInforCenter();
    /*!
      获取DataCenterInterface的指针,该指针指向为临时的DataCenterInterface实例,
        在不使用的时候需要自己手动清除。
      @param DataCenterType type = Normal  构建DataCenterInterface的类型,
        默认为Normal模式.
      @return InformationInterface*  InformationInterface的指针
    */
    static DataCenterInterface* getDataCenter(DataCenterType type = Normal);

    /*!
      获取StationInterface的指针,该指针指向为全局唯一StationInterface实例,
        程序退出时需要自己清除。
      @return StationInterface*  StationInterface的指针
    */
    static StationInterface * getStation();

    /*!
      获取TaskInterface的指针,该指针指向为全局唯一TaskInterface实例。
      @return TaskInterface*  TaskInterface的指针
    */
    static TaskInterface* getTastCenter();

    /*!
      获取TimerInterface的指针,该指针指向为全局唯一TimerInterface实例。
      @return TimerInterface*  TimerInterface的指针
    */
    static TimerInterface* getTimer();
private:
    NetComponents();
};

#endif // NETCOMPONENTS_H
