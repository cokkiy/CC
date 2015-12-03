#include "NetworkInterface.h"

NetworkInterface::operator std::pair<std::string, std::list<std::string>>()
{
    return std::pair<std::string, std::list<std::string>>({ mac,IPs });
}

NetworkInterface::NetworkInterface(std::string mac, std::string ip1, std::string ip2)
{
    this->mac = mac;
    this->IPs.push_back(ip1);
    this->IPs.push_back(ip2);
}

NetworkInterface::NetworkInterface(std::string mac, std::string ip)
{
    this->mac = mac;
    this->IPs.push_back(ip);
}

NetworkInterface::NetworkInterface(std::string mac, std::list<std::string> ips)
{
    this->mac = mac;
    this->IPs.insert(this->IPs.end(), ips.begin(), ips.end());
}

NetworkInterface::NetworkInterface(std::string mac)
{
    this->mac = mac;
}

NetworkInterface::NetworkInterface(QString mac, QStringList ips)
{
    this->mac = mac.toStdString();
    for (QString& ip:ips)
    {
        this->IPs.push_back(ip.toStdString());
    }
}

void NetworkInterface::AddIP(std::string ip)
{
    this->IPs.push_back(ip);
}

QString NetworkInterface::getAllIPString()
{
    QString ipString = QStringLiteral("");
    for (std::string& ip : IPs)
    {
        ipString += QString::fromStdString(ip) + ",";
    }
    return ipString.left(ipString.length() - 1);
}

NetworkInterface::operator QPair<QString, QStringList>()
{
    QStringList list;
    for (std::string& ip : IPs)
    {
        list.push_back(QString::fromStdString(ip));
    }
    return QPair<QString, QStringList>({ QString::fromStdString(mac),list });
}

bool NetworkInterface::hasMacAndIPConfig(const std::pair<const std::string, std::list<std::string>> & ni)
{
    if (mac == ni.first)
    {
        for (auto& ip : IPs)
        {
            for (auto& refIP : ni.second)
            {
                if (ip == refIP)
                    return true; //只要有一个一致的就认为具有相同的配置
            }
        }
    }
    return false;
}

std::list<std::string> NetworkInterface::getIPList()
{
    return IPs;
}
