// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：network.cpp
// 文件摘要：综合应用分系统网络类实现文件，用来实现网络类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

// 修改：张立民（cokkiy）
// 修改日期：2015-09-02
//版本：1.1
//2015-09-08 cokkiy 根据网络配置文件的变化重写，支持单播，组播，指定源组播，并
//支持多网卡环境
//本文件主要用来创建和初始化socket，通过begin和end来获取map<string,socket>的迭代器来访问
//创建好的socket
#include "network.h"
#include <QNetworkInterface>
#include <list>

//*****************************************************************
//析构函数
//
//
//*****************************************************************
Network::~Network()
{
	//清理socket环境
#ifdef WIN
    ::WSACleanup();
#endif
}



//初始化windows系统socket环境，在非windows环境下总是返回true
bool Network::StartupWindowsSocket()
{
#ifdef WIN
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
    int ressult = ::WSAStartup(wVersionRequested, &wsaData);
    if (ressult != 0)
    {
        qCritical() << "Loading Windows socket (WSAStartup) Failure. ErrorNo: "<<s_errno;
        return false;
    }
#endif 
    return true;
}

//获取NamedSocket的开始位置
NamedSockets::const_iterator Network::begin()
{
    return m_sockets.begin();
}

//获取NamedSocket的结束位置
NamedSockets::const_iterator Network::end()
{
    return m_sockets.end();
}

//获取总共有多少个socket
int Network::getSocketCount()
{
    return m_socketCount;
}

//*****************************************************************
//初始化套接字
//
//cokkiy 2015-09-07 完全重写
//*****************************************************************
bool Network::InitSocket()
{    
    //初始化windows环境socket
    if (!StartupWindowsSocket())
        return false;

    //获取本机IP地址 cokkiy 2015-09-06
    getNetworkInterfaceInfo();
    auto niSettings = m_config->getNISettings();

    //从配置文件中查找每个网卡的设置
    for (auto i =niSettings->begin(); i !=niSettings->end(); i++)
    {
        QHostAddress host = findHostIP(i->ipAddresses);
        if (host != QHostAddress::Null)
        {
            list<int> usedPorts; //本Ip下已经使用了的端口号

            //本机IP存在,为本IP下对应的所有端口号创建不同的socket，并加入不同的单播，组播，指定源组播
            for (auto j = i->primaryRecSettings.begin(); j != i->primaryRecSettings.end(); j++)
            {
                //主用配置
                createAndConfigSocket(host, usedPorts, j, true);
            }

            for (auto j = i->backupRecSettings.begin(); j != i->backupRecSettings.end(); j++)
            {
                //备用配置
                createAndConfigSocket(host, usedPorts, j, false);
            }
        }
    }

	return true;
}

//创建并配置socket
// host:本机IP地址
// usedPorts:本机host Ip地址下已经使用的端口
//recSetting: 配置文件中配置的接收设置
// isPrimary:是否是主用通道socket
void Network::createAndConfigSocket(const QHostAddress& host, list<int>& usedPorts, list<NetConfig::RecSetting>::const_iterator recSetting, bool isPrimary)
{
    if (!isPortUsed(usedPorts, recSetting->Port))
    {
        //如果没有创建socket，则先创建
        if (createSocket(host, recSetting, isPrimary))
        {
            //记录该IP地址下的该端口已被使用
            usedPorts.push_back(recSetting->Port);
        }
    }

    //已经创建了（ip，port）对应的socket
    SOCKET s = getSocket(host, recSetting->Port);
    if (s != INVALID_SOCKET)
    { 
        //根据配置设置组播、单播信息
        if (recSetting->DataMethod == NetConfig::DataMethod::Multicast)
        {
            //组播
            ip_mreq  source;
            source.imr_multiaddr.s_addr = inet_addr(recSetting->IPAddress.toStdString().c_str());
            source.imr_interface.s_addr = htonl(host.toIPv4Address());
            int result = setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&source, sizeof(source));
            if (result != 0)
            {
                //加入组播地址时发生错误
                qCritical() << "Error occured when add in multigroup: "<<recSetting->IPAddress
                            <<" ErrorNo:"<< s_errno;
            }
            else
            {
                qDebug()<<"Join multicast group: "<<recSetting->IPAddress;
            }
        }
        else if (recSetting->DataMethod == NetConfig::DataMethod::SpecifiedSource)
        {
            //指定源组播
            for (auto sAddr = recSetting->SpecifiedIPList.begin(); sAddr != recSetting->SpecifiedIPList.end(); sAddr++)
            {
                ip_mreq_source source;
                source.imr_sourceaddr.s_addr = inet_addr(sAddr->toStdString().c_str());
                source.imr_multiaddr.s_addr = inet_addr(recSetting->IPAddress.toStdString().c_str());
                source.imr_interface.s_addr = htonl(host.toIPv4Address());
                int result = setsockopt(s, IPPROTO_IP, IP_ADD_SOURCE_MEMBERSHIP, (char*)&source, sizeof(source));
                if (result != 0)
                {
                    //加入组播地址时发生错误
                    qCritical() << "Error occured when add in multigroup: "<<recSetting->IPAddress
                                << " Specified receive source: "<<*sAddr
                                <<" ErrorNo:"<< s_errno;
                }
                else
                {
                    qDebug()<<"Join multicast group: "<<recSetting->IPAddress
                           <<" Specified receive source:"<<*sAddr;
                }
            }

        }
        else  //unicast
        {
            //单播
        }
    }
}

//创建socket，如果成功创建,则socket被加入到m_Sockets中
// isPrimary: 是否是主用通道socket
bool Network::createSocket(const QHostAddress& host, list<NetConfig::RecSetting>::const_iterator recSetting, bool isPrimary)
{
    //尚未创建（ip，port）对应的socket
    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == INVALID_SOCKET)
    {
        qCritical() << "Fail to init socket. IP: " << host.toString() << " Port: "
                    << recSetting->Port
                    <<" ErroNO: "<< s_errno << endl;
        return false;
    }
    else
    {
        //本地IP地址结构
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        // 将网卡IP地址绑定到接收地址, 
        // 绑定到INADDR_ANY意味着从所有网卡接收,不指定具体的网卡
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(recSetting->Port);

        int len = sizeof(addr);			//绑定udp套接字
        int result = ::bind(s, (struct sockaddr *)&addr, len);
        if (result < 0)
        {
            qCritical() << "socket bind error！ IP:" << host.toString()
                        << " port : " << recSetting->Port
                        << " ErrorNo: "<< s_errno << endl;
            return false;
        }

        //显示创建的socket信息
        qDebug()<<"Successfully created socket("<<s<<") IP: "<<host.toString()
               <<" Port: "<<recSetting->Port;

        //设置每个socket的接收数据缓冲区大小
        //将接收缓冲区设为最大值524288	
        int size = recSetting->RecBuffSize;
        if (setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&size, sizeof(int)) < 0)
        {
            qWarning() << "Fail to set socket recv buffer！IP:" << host.toString()
                       << " Port : " << recSetting->Port
                       << " ErrorNo:"<< s_errno << endl;
        }


        //add socket to socket map
        string key = host.toString().append(":%1").arg(recSetting->Port).toStdString();
        auto r = m_sockets.insert({ key, s });
        if (r.second)
        {
            m_socketCount++;

            //加入到主备socket列表中
            socketsMap.insert({ s,isPrimary });
        }
    }
    return true;
}

//设置网络配置文件
// cokkiy 2015-09-06
void Network::setConfig(NetConfig* netConfig)
{
    this->m_config=netConfig;
}

//获取网络配置
NetConfig * Network::getConfig()
{
    return m_config;
}

//获取指定ip和端口的socket,如果不存在，返回INVALID_SOCKET
SOCKET Network::getSocket(const QHostAddress& ip, int port)
{
    QString key = ip.toString().append(":%1").arg(port);
    auto item=m_sockets.find(key.toStdString());
    if (item != m_sockets.end())
        return item->second;
    return INVALID_SOCKET;
}

//检查指定的socket是否是主用通道socket,如果是,返回<c>true</c>,否则发挥<code>false</code>
bool Network::isPrimarySocket(SOCKET s)
{
    return socketsMap.find(s)->second;
}

//获取本机网卡地址列表
// cokkiy 2015-09-06
void Network::getNetworkInterfaceInfo()
{
    //获取所有网卡的ip地址
    QNetworkInterface ni;
    QList<QNetworkInterface> allNI = ni.allInterfaces();
    for(auto ni=allNI.begin();ni!=allNI.end();ni++)
    {
        QList<QNetworkAddressEntry> entries=ni->addressEntries();
        for(auto entry=entries.begin();entry!=entries.end();entry++)
        {
            QHostAddress ip=entry->ip();
            if (!ip.isLoopback()&&ip.protocol()== QAbstractSocket::IPv4Protocol)
            {
                m_IPAddresses.push_back(ip);
            }
        }
    }
    //获取所有网卡的ip地址,下面代码在windows正常，linux下只能获取到127.0.0.1
    //    list<QHostAddress> addresses = ni.allAddresses().toStdList();
    //    for(auto i= addresses.begin();i!= addresses.end();i++)
    //    {
    //		if (!i->isLoopback()&&i->protocol()== QAbstractSocket::IPv4Protocol)
    //		{
    //			m_IPAddresses.push_back(*i);
    //        }
    //        qDebug()<<i->toString();
    //    }
}

//指定IP地址是否是本机IP
//cokkiy 2015-09-06
bool Network::isLocalIP(const QString& strIP)
{
	QHostAddress addr(strIP);
	for (auto i = m_IPAddresses.begin(); i != m_IPAddresses.end(); i++)
	{
		if (addr == (*i))
		{
			return true;
		}
	}
	return false;
}

//从一组IP地址列表中查找本机IP地址，如果没有，返回QHostAddress.NULL
QHostAddress Network::findHostIP(const list<QString>& ipAddresses)
{
    for (auto i = ipAddresses.begin(); i !=ipAddresses.end(); i++)
    {
        if (isLocalIP(*i))
        {
            return QHostAddress(*i);
        }
    }
    return QHostAddress::Null;
}

//检查指定的port是否已经被使用
bool Network::isPortUsed(const list<int>& usedPorts, int port)
{
    for (auto i = usedPorts.begin(); i != usedPorts.end(); i++)
    {
        if ((*i) == port)
            return true;
    }

    return false;
}









