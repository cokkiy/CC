#ifndef ABSTRACTNET_H
#define ABSTRACTNET_H
#include "net_global.h"
#include <QObject>
class NETSHARED_EXPORT NetInterface : public QObject
{
    Q_OBJECT
public:
    explicit NetInterface(QObject *parent = 0);
    /*!
      装载某路径下的所有xml表,
      包含"zx_param_global_"字符串的信息约定表xml文件,
      包含"table_info_global_"字符串的表名约定表xml文件
      @param QString dir  文件路径名
      @return int  获取成功或失败，1为成功，-1为失败
    */
    virtual int load(QString dir) = 0;
    /*!
      开始接收和解码线程,
      @return int  获取成功或失败，1为成功，-1为失败
    */
    virtual int start() = 0;
    /*!
      停止接收和解码线程,
      @return int  获取成功或失败，1为成功，-1为失败
    */
    virtual int stop() = 0;
};

#endif // ABSTRACTNET_H
