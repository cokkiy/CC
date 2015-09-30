// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：netconfig.h
// 文件摘要：网络配置信息类头文件，用来声明网络配置信息类。
//
// 原始版本：1.0
// 当前版本：1.0
// 作    者：张立民
// 完成日期：2015-08-27
//*************************************************************//

#ifndef NETCONFIG_H
#define NETCONFIG_H
#include "net_global.h"
#include <string>
#include <list>
#include "xelement.h"
#include "../netbuffer/netbuffer.h"

using namespace std;


//网络配置类定义
class NETSHARED_EXPORT NetConfig
{
public:
    //数据发送(接收)类型（组播，单播，指定源组播）
    enum DataMethod{
        Multicast=0,  //组播
        Unicast,  //单播
        SpecifiedSource //指定源组播
    };

    //网络接收设置
    struct NETSHARED_EXPORT RecSetting
    {
        //数据源名称
        QString Name;

        //数据发送(接收)类型（组播，单播，指定源组播）
        enum DataMethod DataMethod;

        //接收端口
        int Port;

        //接收地址，在组播时，指组播地址，在单播时，指指定的单播源IP，如果想接收
        //所有单播源数据，则设置为0.0.0.0
        QString IPAddress;

        //组播源地址列表，只有指定源组播时才有用
        list<QString> SpecifiedIPList;

        //接收缓冲区大小
        int RecBuffSize;

        //信息处理线程数量
        int ThreadNum;

    };


    //嵌套类
    //网卡设置
    class NETSHARED_EXPORT NISetting
    {
    public:
        // 本机ip地址（包含一个本机ip地址的列表，也可能有其他主机的ip地址，为了方便拷贝到其他工作站）
        list<QString> ipAddresses;

        //主用接收设置
        list<RecSetting> primaryRecSettings;

        //备用接收设置
        list<RecSetting> backupRecSettings;
    };

public:

    //构造函数，初始化资源
    NetConfig();

    //析构函数，释放资源
    ~NetConfig();

    //获取网卡设置
    list<NISetting>* getNISettings();

    //加载配置文件
    bool load(const QString&);

    //网络源码输入输出帧队列   从后入,从前出
    NetBuffer m_inBufList[MAX_THREAD_NUM], m_outBufList;

    //主用通道处理线程数量
    int primaryThreadCount;
    //备用通道处理线程数量
    int backupThreadCount;
private:

    //网卡设置列表
    list<NISetting>* m_NISettings;
    
    //构建接收设置
    void buildRecSetting(ElementList& recList, NISetting& niSetting, bool isPrimary);

};

#endif // NETCONFIG_H
