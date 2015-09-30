#ifndef ABSTRACTSTATION_H
#define ABSTRACTSTATION_H
#include "net_global.h"
#include <QObject>
#include <QVector>

//装载指显工作站信息xml文件
class NETSHARED_EXPORT StationInterface : public QObject
{
    Q_OBJECT
public:
    explicit StationInterface(QObject *parent = 0);

    //装载"指显工作站信息xml"
    //QString dir  文件路径名
    //返回值:  获取成功或失败，1为成功，-1为失败
    virtual int load(QString dir) = 0;

    //获取本机的权限
    virtual QString getMyRights() = 0;

    //检查本机是否具有发令权限
    //返回值 false:否  true:是
    virtual bool canSendCmd() = 0;

    //检查本机是否具有发流程权限
    //返回值 false:否  true:是
    virtual bool canSendCourse() = 0;

    //查询本机是否记盘
    //返回值 false:否  true:是
    virtual bool queryRecord() = 0;

    //本机记盘路径
    virtual QString getRecordPath() = 0;
};

#endif // ABSTRACTSTATION_H
