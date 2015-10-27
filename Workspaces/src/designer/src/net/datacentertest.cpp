#include "datacentertest.h"
#include <qmath.h>
class DataCenterTestPrivate {
public:
    explicit DataCenterTestPrivate();
    int getValue(QString computeStr,QVariant& result);
    int getHistoryData(QString computeStr,QVector<double>& result);
    int getNewHistoryData(QString computeStr,QVector<double>& result);
    int getCosDataX(QVector<double>& result);
    int getCosDataY(QVector<double>& result);
private:
    bool m_bAlready;
    int m_pos;
};


DataCenterTestPrivate::DataCenterTestPrivate():m_bAlready(false),m_pos(0)
{

}
int DataCenterTestPrivate::getValue(QString computeStr,QVariant& result)
{
    int randData = rand();
    int swit = randData%3;
    if(swit==0)
    {
        double data = randData;
        result = QVariant(data);
    }
    else if(swit==1)
    {
        QString str = QString::fromUtf8("This is a String!");
        result = QVariant(str);
    }
    else
    {
        QByteArray ba = QByteArray();
        ba.append(char(0x10));
        ba.append(char(0x13));
        ba.append(char(0x16));
        ba.append(char(0x12));
        ba.append(char(0x32));
        ba.append(char(0xf2));
        result = QVariant(ba);
    }
    return 1;
}
int DataCenterTestPrivate::getHistoryData(QString computeStr,QVector<double>& result)
{
    if(computeStr==QString::fromUtf8("cosx"))
    {
        return getCosDataX(result);
        qDebug()<<"get cosx data";
    }
    else if(computeStr==QString::fromUtf8("cosy"))
    {
        return getCosDataY(result);
    }
    if(!m_bAlready)
    {
        for(int v=0;v<10;v++)
        {
            result.append(v);
        }
        m_bAlready = true;
        qDebug()<<"All Data";
    }
    else
    {
        qDebug()<<"New Data";
        return getNewHistoryData(computeStr,result);
    }
    return 1;
}

int DataCenterTestPrivate::getNewHistoryData(QString computeStr,QVector<double>& result)
{
    for(int v=0;v<20;v++)
    {
        result.append(v);
    }
    return 1;
}

int DataCenterTestPrivate::getCosDataX(QVector<double>& result)
{
    double x = m_pos/250.0*3*M_PI;
    result.append(x);
    return 1;
}

int DataCenterTestPrivate::getCosDataY(QVector<double>& result)
{
    double y = qCos(m_pos/250.0*3*M_PI)*10;
    result.append(y);
    m_pos++;
    return 1;
}
DataCenterTest::DataCenterTest()
   :d_ptr(new DataCenterTestPrivate)
{
}
DataCenterTest::~DataCenterTest()
{
}
int DataCenterTest::getValue(QString computeStr,double& result)
{
    return -1;
}
int DataCenterTest::getValue(QString computeStr,QVariant& result)
{
    Q_D(DataCenterTest);
    return d->getValue(computeStr,result);
}
int DataCenterTest::getHistoryData(QString computeStr,QVector<double>& result)
{
    Q_D(DataCenterTest);
    return d->getHistoryData(computeStr,result);
}
int DataCenterTest::getHistoryData(QString computeStr,QVector<double>& result,QVector<unsigned int>& time)
{
    return -1;
}

int DataCenterTest::getHistoryData(QString computeStrX,QString computeStrY,QVector<double>& resultX,QVector<double>& resultY)
{
    return -1;
}

qint32 DataCenterTest::getHistoryDatas(qint32 count...)
{
    return -1;
}

int DataCenterTest::getString(QString computeStr,QString dispStr,QString& resultStr)
{
    return 1;
}
