#ifndef ABSTRACTDATACENTER_H
#define ABSTRACTDATACENTER_H
#include "net_global.h"
#include <QObject>
class NETSHARED_EXPORT DataCenterInterface: QObject
{
    Q_OBJECT
public:
    DataCenterInterface(QObject *parent = 0);
    virtual ~DataCenterInterface();
    /*!
      通过输入公式化的参数表达式获取计算结果
      @param QString computeStr  字符串，包含参数的公式计算表达式
      @param double result  公式计算的结果，为浮点数
      @return int  获取成功或失败，1为成功，-1为失败
    */
    virtual int getValue(QString computeStr,double& result) = 0;
    /*!
      通过输入公式化的参数表达式获取计算结果
      @param QString computeStr  字符串，包含参数的公式计算表达式
      @param QVariant result  公式计算的结果，为QVariant
      @return int  获取成功或失败，1为成功，-1为失败
    */
    virtual int getValue(QString computeStr,QVariant& result) = 0;
    /*!
      通过输入公式化的参数表达式和数据的显示格式获取数据的最终显示内容
      @param QString computeStr  字符串，表示参数的公式计算表达式
      @param QString dispStr  字符串，表示数据的显示格式
      @param QString& resultStr  字符串，表示数据的最终显示内容
      @return int  获取成功或失败，1为成功，-1为失败
    */
    virtual int getString(QString computeStr,QString dispStr,QString& resultStr) = 0;
    /*!
      通过输入公式化的参数表达式获取单数据的计算结果的历史数据
      @param QString computeStr  字符串，包含参数的公式计算表达式
      @param QVector<double>  公式计算的历史数据
      @return int  获取成功或失败，1为成功，-1为失败
    */
    virtual int getHistoryData(QString computeStr,QVector<double>& result) = 0;
    /*!
      通过输入公式化的参数表达式获取单数据的计算结果的历史数据
      @param QString computeStr  字符串，包含参数的公式计算表达式
      @param QVector<double>  公式计算的历史数据
      @param QVector<unsigned int>  公式计算的历史数据的时间
      @return int  获取成功或失败，1为成功，-1为失败
    */
    virtual int getHistoryData(QString computeStr,QVector<double>& result,QVector<unsigned int>& time) = 0;
    /*!
      通过输入公式化的参数表达式获取双数据的计算结果的历史数据
      @param QString computeStrX  字符串X，包含参数的公式计算表达式
      @param QString computeStrY  字符串Y，包含参数的公式计算表达式
      @param QVector<double> resultX  公式计算X的历史数据
      @param QVector<double> resultY  公式计算Y的历史数据
      @return int  获取成功或失败，1为成功，-1为失败
    */
    virtual int getHistoryData(QString computeStrX,QString computeStrY,QVector<double>& resultX,QVector<double>& resultY) = 0;

    //通过输入公式化的参数表达式获取指定个数参数的历史数据
    //count = 需要获得的参数个数
    //... = 需要获得的参数列表 格式为计算公式,历史数据
    //例如：int count=3; QString computeStr1,computeStr2,computeStr3;QVector<double> v1,QVector<double> v2,QVector<double> v3;
    //getHistoryData(3, &computeStr1, &v1, &computeStr2, &v2, &computeStr3, &v3)
    //返回值 = 获取成功或失败，1为成功，-1为失败
    virtual qint32 getHistoryDatas(qint32 count...)=0;

Q_SIGNALS:
    /*!
      通知某表号和参数号的参数已经到达。
      @param unsigned short tabNo  表号
      @param unsigned short ParamNo  参数号
      @return
    */
    void setValue(unsigned short tabNo, unsigned short paramNo);
};

#endif // ABSTRACTDATACENTER_H
