// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：network.h
// 文件摘要：综合应用分系统网络类头文件，用来声明网络类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

#ifndef NETWORKIN_H
#define NETWORKIN_H

#include<stdlib.h>
#include<stdio.h>
#include <qsystemdetection.h>
#include<errno.h>
#include<sys/types.h>
#include <list>
#include <QHostAddress>

#ifdef Q_OS_LINUX
#include<netdb.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#else
#include <WS2TCPIP.H>
#include <WinSock2.h>
#pragma comment(lib, "WS2_32.Lib")
#endif

#include "../../../selfshare/src/baseobject.h"
#include "../../../selfshare/src/const.h"
//#include "../../../selfshare/src/config/config.h"
#include "../../../selfshare/src/config/netconfig.h"
#include "net_global.h"
#include "../../../selfshare/src/OS_Def.h"
using namespace std;

//***************************************************************
#ifdef LINUX
typedef int SOCKET;
#define INVALID_SOCKET -1
#ifndef s_errno
#define s_errno errno
#endif
#endif

#ifdef WIN
#ifndef s_errno
#define s_errno (::WSAGetLastError())
#endif
typedef int socklen_t;
#endif

//预定义NamedSocket，key：IP：port，value：SOCKET
typedef map<string, SOCKET> NamedSockets;

//***************************************************************
class NETSHARED_EXPORT Network : public BaseObject
{
	public:
        //构造函数
        Network() :m_socketCount(0), BaseObject() {}

        //析构函数
		~Network();

        //初始化windows系统socket环境
        bool StartupWindowsSocket();

        //获取NamedSocket的开始位置
        NamedSockets::const_iterator begin();

        //获取NamedSocket的结束位置
        NamedSockets::const_iterator end();
        
        //获取总共有多少个socket
        int getSocketCount();

        //初始化套接字
		bool InitSocket();
        
        //设置网络配置
        void setConfig(NetConfig*);   

        //获取网络配置
        NetConfig* getConfig();

        //获取指定ip和端口的socket,如果不存在，返回INVALID_SOCKET
        SOCKET getSocket(const QHostAddress& ip, int port);    

        //指定的socket是否是主用通道socket
        bool isPrimarySocket(SOCKET s);


        // 指定Ip是否是本机Ip
        //cokkiy 2015-09-06
        bool isLocalIP(const QString& strIP);

        //获取本机IP地址和网卡信息
        //cokkiy 2015-09-06
        void getNetworkInterfaceInfo();

private:
        NetConfig *m_config; //cokkiy 2015-09-06

        //已经创建的socket
        //key:  IP:Port
        //value: SOCKET
        NamedSockets m_sockets;

        //已经创建的socket数量
        int m_socketCount;

        //从一组IP地址列表中查找本机IP地址，如果没有，返回QHostAddress.NULL
        QHostAddress findHostIP(const list<QString>& ipAddresses);
        //检查指定的port是否已经被使用
        bool isPortUsed(const list<int>& usedPorts, int port);
        
        //创建并配置socket
        // host:本机IP地址
        // usedPorts:本机host Ip地址下已经使用的端口
        //recSetting: 配置文件中配置的接收设置
        // isPrimary: 是否是主用通道socket
        void createAndConfigSocket(const QHostAddress& host, list<int>& usedPorts, list<NetConfig::RecSetting>::const_iterator recSetting, bool isPrmary);

        //创建socket,如果成功创建，加入到m_sockets中
        //isPrimary:是否是主用通道socket
        bool createSocket(const QHostAddress & host, list<NetConfig::RecSetting>::const_iterator recSetting,bool isPrimary);

		//本级Ip地址列表
		//cokkiy 2015-09-06
		list<QHostAddress>m_IPAddresses;

        //主备通道socket 表,如果为true表示主用通道socket, false表示备用通道socket
        map<SOCKET, bool> socketsMap;
};

#endif



