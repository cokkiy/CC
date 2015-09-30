#ifndef STATIONINSTANCE_H
#define STATIONINSTANCE_H
#include "net_global.h"
#include "abstractstation.h"
#include "selfshare/src/config/stationconfig.h"

class NETSHARED_EXPORT  StationInstance: public StationInterface
{
public:
    StationInstance();

    //装载指显工作站xml配置文件,
    //QString dir  文件路径名
    //返回值 false:失败 true:成功
    virtual int load(QString dir);

    //获取本机的权限
    virtual QString getMyRights();

    //检查本机是否具有发令权限
    //返回值 false:否  true:是
    virtual bool canSendCmd();

    //检查本机是否具有发流程权限
    //返回值 false:否  true:是
    virtual bool canSendCourse();

    //查询本机是否记盘
    //返回值 false:否  true:是
    virtual bool queryRecord();

    //本机记盘路径
    virtual QString getRecordPath();

private:

    //指显工作站配置
    StationConfig m_stationConfig;
};

#endif // STATIONINSTANCE_H
