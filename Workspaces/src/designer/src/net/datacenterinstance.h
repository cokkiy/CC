#ifndef DATACENTERINSTANCE_H
#define DATACENTERINSTANCE_H
#include "net_global.h"
#include "abstractdatacenter.h"
#include "LFormula.h"
#include <QVector>

class NETSHARED_EXPORT DataCenterInstance : public DataCenterInterface
{
public:
    DataCenterInstance();
    virtual int getValue(QString computeStr,double& result);
    virtual int getValue(QString computeStr,QVariant& result);
    virtual int getHistoryData(QString computeStr,QVector<double>& result);
    virtual int getHistoryData(QString computeStrX,QString computeStrY,QVector<double>& resultX,QVector<double>& resultY);
    virtual qint32 getHistoryDatas(qint32 count...);
    virtual int getString(QString computeStr,QString dispStr,QString& resultStr);
private:
    int m_savTime;
    int m_savDate;
    //公式解释器
    LFormula m_LFormula;
};

#endif // DATACENTERINSTANCE_H
