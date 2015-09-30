#include "stationinstance.h"
#include<iostream>
#include <QDebug>

StationInstance::StationInstance()
{

}

//装载指显工作站xml配置文件,
//QString dir  文件路径名
//返回值 false:失败 true:成功
int StationInstance::load(QString dir)
{
    std::string temp = dir.toStdString();
    if(m_stationConfig.load(temp.c_str()) == false)
    {
        cout<<"加载指显工作站配置失败，请检查配置重启软件！"<<endl;
        return -1;
    }

    return 1;
}

QString StationInstance::getMyRights()
{
    return m_stationConfig.getMyRights();
}

//检查本机是否具有发令权限
//返回值 false:否  true:是
bool StationInstance::canSendCmd()
{
    return m_stationConfig.canSendCmd();
}

//检查本机是否具有发流程权限
//返回值 false:否  true:是
bool StationInstance::canSendCourse()
{
    return m_stationConfig.canSendCourse();
}

//查询本机是否记盘
//返回值 false:否  true:是
bool StationInstance::queryRecord()
{
    return m_stationConfig.queryRecord();
}

//本机记盘路径
QString StationInstance::getRecordPath()
{
    return m_stationConfig.getRecordPath();
}
