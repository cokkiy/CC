#ifndef ABSTRACTINFORMATION_H
#define ABSTRACTINFORMATION_H
#include "net_global.h"
#include "C3I/paramstyle/paramstyleconfig.h"
#include <QObject>
#include "abstractparam.h"
#include <QVector>

//存放某表所有参数的队列
typedef QVector<AbstractParam > SingleTableParamVector;
//存放包含(表号，参数号，参数实例)的映射
typedef QMap<unsigned short, QMap<unsigned short, AbstractParam> > ParamMap;
//存放包含(表号，表名)的映射
typedef QMap<unsigned short, QString > TabMap;

//装载所有信息约定表，并提供获取信息约定表的接口
class NETSHARED_EXPORT InformationInterface : public QObject
{
    Q_OBJECT
public:
    explicit InformationInterface(QObject *parent = 0);
    /*!
      装载某路径下的所有xml表,
      包含"zx_param_global_"字符串的信息约定表xml文件,
      包含"table_info_global_"字符串的表名约定表xml文件
      @param QString dir  文件路径名
      @return int  获取成功或失败，1为成功，-1为失败
    */
    virtual int load(QString dir) = 0;
    /*!
      获取所有表的表名
      @return TabList  所有表的表名列表
    */
    virtual TabMap getTabList() = 0;
    /*!
      获取所有参数的映射
      @return ParamMap  所有参数的映射
    */
    virtual ParamMap getParamMap() = 0;
    /*!
      通过表号获取某表的所有参数的映射
      @param unsigned short tabNo  表号
      @return ParamVector  某表的所有参数的映射
    */
    virtual SingleTableParamVector getParamVector(unsigned short tabNo) = 0;
    /*!
      通过表号和参数号获取某参数
      @param unsigned short tabNo  表号
      @param unsigned short paramNo  参数号
      @return AbstractParam*  某参数指针,NULL表示为空
    */
    virtual AbstractParam* getParam(unsigned short tabNo,unsigned short paramNo) = 0;
    /*!
      通过数据字符串获取某参数指针
      @param QString data  数据字符串
      @return AbstractParam*  某参数指针,NULL表示为空
    */
    virtual AbstractParam* getParam(QString data) = 0;

    //样式链表
    virtual list<QParamStyle>* getParamStyleList() = 0;
};

#endif // ABSTRACTINFORMATION_H
