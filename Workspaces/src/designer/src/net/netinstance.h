#ifndef NETINSTANCE_H
#define NETINSTANCE_H
#include "net_global.h"
#include "abstractnet.h"
#include "main/src/network/network.h"
#include "main/src/recvinfothread/recvinfothread.h"
#include <QThread>

class NETSHARED_EXPORT  NetInstance: public NetInterface
{
public:
    NetInstance(QObject *parent = 0);
    /*!
      装载某路径下的所有xml表,
      包含"zx_param_global_"字符串的信息约定表xml文件,
      包含"table_info_global_"字符串的表名约定表xml文件
      @param QString dir  文件路径名
      @return int  获取成功或失败，1为成功，-1为失败
    */
    virtual int load(QString dir);
    /*!
      开始接收和解码线程,
      @return int  获取成功或失败，1为成功，-1为失败
    */
    virtual int start();
    /*!
      停止接收和解码线程,
      @return int  获取成功或失败，1为成功，-1为失败
    */
    virtual int stop();

private slots:
    //线程运行函数
    void run();

private:
    //启动接收和解码线程
    bool startThread(bool);
    QThread* m_thread;

    //网络接口
    Network ni;

    //接收信息线程
    RecvInfoThread rithread;

    //网络配置
    NetConfig netConfig;
};

#endif // NETINSTANCE_H
