// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：stationconfig.h
// 文件摘要：工作站配置类头文件，用来声明工作站配置类。
//
// 原始版本：1.0
// 当前版本：1.0
// 作    者：秦志刚
// 完成日期：2015-09-17
// 功    能：读取"指显工作站信息.xml"并取得指显工作站的配置
//*************************************************************//

#ifndef STATIONCONFIG_H
#define STATIONCONFIG_H
#include <string>
#include <list>
#include <QString>

using namespace std;

//权限的枚举
enum Rights
{
    Browse = 0x0,//浏览权限
    SendCourse = 0x1,//发指挥流程权限
    SendCmd = 0x2,//发令权限
    Titan = 0x4//保留权限
};

//工作站配置（单台）
class Station
{
public:

    //名称
    QString strName;

    //IP地址
    QString ipAddresses;

    //MAC地址
    QString strMACAddress;

    //工作模式
    QString strWorkMode;

    //是否记盘
    bool bRecord;

    //记盘路径
    QString strRecordPath;

    //默认的构造函数
    Station()
    {
        strName = "";
        ipAddresses = "";
        strMACAddress = "";
        strWorkMode = "";
        bRecord = false;
        strRecordPath = "";
    }
};

//工作站配置类定义
class StationConfig
{

public:

    //构造函数，初始化资源
    StationConfig();

    //析构函数，释放资源
    ~StationConfig();

    //加载配置文件
    //返回值 false:失败 true:成功
    bool load(const QString&);

    //获取本机权限（以文字表示）
    QString getMyRights();

    //检查本机是否具有发令权限
    //返回值 false:否  true:是
    bool canSendCmd();

    //检查本机是否具有发流程权限
    //返回值 false:否  true:是
    bool canSendCourse();

    //查询本机是否记盘
    //返回值 false:否  true:是
    bool queryRecord();

    //本机记盘路径
    QString getRecordPath();

private:

    //本机配置
    Station m_MySatation;

    //本机权限（以值表示）
    quint16 m_iRights;

    //获取本机配置
    void GetMyConfig();

    //指显工作站设置列表
    list<Station>* m_NISettings;

    //将权限以文字形式转化为值形式
    quint16 transRight(QString str);

    //将布尔值以文字形式转化为值形式
    bool transBool(QString str);

};

#endif // NETCONFIG_H
