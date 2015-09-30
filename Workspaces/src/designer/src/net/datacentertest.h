#ifndef DATACENTERTEST_H
#define DATACENTERTEST_H
#include "abstractdatacenter.h"
#include <net_global.h>
#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QVariant>
#include <QtCore/QDebug>
QT_BEGIN_NAMESPACE
class DataCenterTestPrivate;
class NETSHARED_EXPORT DataCenterTest: public DataCenterInterface
{
public:
    DataCenterTest();
    virtual ~DataCenterTest();
    virtual int getValue(QString computeStr,double& result);
    /*!
      通过输入公式化的参数表达式获取计算结果
      @param QString computeStr  字符串，包含参数的公式计算表达式
      @param QVariant result  公式计算的结果，为浮点数
      @return int  获取成功或失败，1为成功，-1为失败
    */
    virtual int getValue(QString computeStr,QVariant& result);
    /*!
      通过输入公式化的参数表达式和数据的显示格式获取数据的最终显示内容
      @param QString computeStr  字符串，表示参数的公式计算表达式
      @param QString dispStr  字符串，表示数据的显示格式
      @param QString& resultStr  字符串，表示数据的最终显示内容
      @return int  获取成功或失败，1为成功，-1为失败
    */
    virtual int getString(QString computeStr,QString dispStr,QString& resultStr);
    /*!
      通过输入公式化的参数表达式获取计算结果的历史数据
      @param QString computeStr  字符串，包含参数的公式计算表达式
      @param QVector<UnitData>  公式计算的历史数据
      @return int  获取成功或失败，1为成功，-1为失败
    */
    virtual int getHistoryData(QString computeStr,QVector<double>& result);

    virtual int getHistoryData(QString computeStrX,QString computeStrY,QVector<double>& resultX,QVector<double>& resultY);

    virtual qint32 getHistoryDatas(qint32 count...);

  private:
    int getNewHistoryData(QString computeStr,QVector<double>& result);
    int getCosDataX(QVector<double>& result);
    int getCosDataY(QVector<double>& result);
    bool m_bAlready;
    int m_pos;

private:
    QScopedPointer<DataCenterTestPrivate> d_ptr;
    Q_DECLARE_PRIVATE(DataCenterTest)
    Q_DISABLE_COPY(DataCenterTest)
};
QT_END_NAMESPACE
#endif // DATACENTERTEST_H
