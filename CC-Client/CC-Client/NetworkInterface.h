#pragma once
#include <stdarg.h>
#include <QString>
#include <QMap>
/***************************************************
 NetworkInterface类,定义了网卡信息类
 ***************************************************/
class NetworkInterface
{
private:
    /*
     mac地址
     **/
    std::string mac;
    /*
     IP地址列表
     **/
    std::list<std::string> IPs;
public:
    /*
     默认构造函数
     **/
    NetworkInterface() = default;
    ~NetworkInterface() = default;
    
    /*!
    创建一个\class NetworkInterface 对象
    @param std::string mac MAC地址
    @return 
    作者：cokkiy（张立民)
    创建时间：2015/12/02 10:42:46
    */
    NetworkInterface(std::string mac);
    /*!
    创建一个\class NetworkInterface 对象
    @param std::string mac MAC地址
    @param std::string ips  IP地址列表
    @param ...  IP地址2
    @return 
    作者：cokkiy（张立民)
    创建时间：2015/12/02 9:49:33
    */
    NetworkInterface(std::string mac, std::list<std::string> ips);


    /*!
    创建一个\class NetworkInterface 对象
    @param QString mac MAC地址
    @param QStringList  ips IP地址列表
    @param ...  IP地址2
    @return
    作者：cokkiy（张立民)
    创建时间：2015/12/02 9:49:33
    */
    NetworkInterface(QString mac, QStringList ips);


    /*!
    创建一个\class NetworkInterface 对象
    @param std::string mac MAC地址
    @param std::string ip  IP地址
    @return 
    作者：cokkiy（张立民)
    创建时间：2015/12/02 10:39:32
    */
    NetworkInterface(std::string mac, std::string ip);

    /*!
    创建一个\class NetworkInterface 对象
    @param std::string mac MAC地址
    @param std::string ip1  IP地址1
    @param std::string ip2  IP地址2
    @return 
    作者：cokkiy（张立民)
    创建时间：2015/12/02 10:40:41
    */
    NetworkInterface(std::string mac, std::string ip1,std::string ip2);

    /*!
    添加一个IP地址
    @param std::string ip IP地址
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/02 10:41:43
    */
    void AddIP(std::string ip);

    /*!
    获取表示所有IP的字符串,多个IP用逗号分隔
    @return void QString 表示所有IP的字符串,多个IP用逗号分隔
    作者：cokkiy（张立民)
    创建时间：2015/12/02 11:09:39
    */
    QString getAllIPString();

    //把NetworkInterface类转换为QPair<QString, QStringList>结构
    explicit operator QPair<QString, QStringList>();
    //把NetworkInterface类转换为 std::pair<std::string, std::list<std::string>>结构
    explicit operator std::pair<std::string, std::list<std::string>>();
    /*!
    获取表示MAC地址的字符串表示
    @return QString 表示MAC地址的字符串表示
    作者：cokkiy（张立民)
    创建时间：2015/12/02 11:40:46
    */
    QString getMAC()
    {
        return QString::fromStdString(mac);
    }

    /*!
    检查是否具有指定的MAC和IP配置
    @param const std::pair<const std::string ,std::list<std::string>> & ni 指定的MAC和IP配置
    @return bool 
    作者：cokkiy（张立民)
    创建时间：2015/12/02 11:46:44
    */
    bool hasMacAndIPConfig(const std::pair<const std::string, std::list<std::string>> & ni);
    /*!
    获取IP列表
    @return std::list<std::string>IP列表
    作者：cokkiy（张立民)
    创建时间：2015/12/03 9:11:03
    */
    std::list<std::string> getIPList();
};

