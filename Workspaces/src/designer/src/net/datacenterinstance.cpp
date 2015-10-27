#include "datacenterinstance.h"
#include "showtype/dataresultstring.h"
#include "LFormula.h"
extern Config g_cfg;

DataCenterInstance::DataCenterInstance():m_LFormula(&g_cfg)
{
    m_savTime = 0;
    m_savDate = 0;
}

int DataCenterInstance::getValue(QString computeStr,double& result)
{
    QVariant t_result;
    if(m_LFormula.compute(computeStr.toStdString(),t_result)==1)
    {
        if(!t_result.isNull()&&t_result.type()==QVariant::Double)
        {
            result =t_result.value<double>();
            return 1;
        }
    }
    return -1;
}

int DataCenterInstance::getValue(QString computeStr,QVariant& result)
{
    return m_LFormula.compute(computeStr.toStdString(),result);
}

int DataCenterInstance::getHistoryData(QString computeStr,QVector<double>& result)
{
    return m_LFormula.compute(computeStr.toStdString(),result);
}

int DataCenterInstance::getHistoryData(QString computeStr,QVector<double>& result,QVector<unsigned int>& time)
{
    return m_LFormula.compute(computeStr.toStdString(),result,time);
}

int DataCenterInstance::getHistoryData(QString computeStrX,QString computeStrY,QVector<double>& resultX,QVector<double>& resultY)
{
    return m_LFormula.compute(computeStrX.toStdString(),computeStrX.toStdString(),resultX,resultY);
}

//通过输入公式化的参数表达式获取指定个数参数的历史数据
//count = 需要获得的参数个数
//... = 需要获得的参数列表 格式为计算公式,历史数据
//例如：int count=3; QString computeStr1,computeStr2,computeStr3;QVector<double> v1,QVector<double> v2,QVector<double> v3;
//getHistoryData(3, &computeStr1, &v1, &computeStr2, &v2, &computeStr3, &v3)
//返回值 = 获取成功或失败，1为成功，-1为失败
qint32 DataCenterInstance::getHistoryDatas(qint32 count...)
{
    //声明一个参数列表
    va_list arg_ptr;

    //宏-起始参数。第一个实参count表示本次函数需要取得的参数个数
    va_start(arg_ptr, count);

    //返回值
    qint32 res;

    //保存公式解释器的时标，用于获取数据完后重置
    int last_data = m_LFormula.getDate();
    int last_time = m_LFormula.getTime();

    //循环取得所有参数，填进输出QVector
    for(qint32 i=0;i<count;i++)
    {
        //以m_LFormula为母本新建一个公式解释器
        LFormula rr_LFormula = m_LFormula;

        //取得计算公式
        QString* s = va_arg(arg_ptr, QString*);

        //输出结果
        QVector<double>* vx = va_arg(arg_ptr, QVector<double> *);

        //调用公式解释器的compute方法获得结果数据
        res = rr_LFormula.compute(s->toStdString(), *vx);

        //任何一个参数没取到，都要返回失败
        if(res == -1)
        {
            return -1;
        }

        last_data = rr_LFormula.getDate();
        last_time = rr_LFormula.getTime();
    }

    //重置公式解释器的时标。如果不这样做，每次compute调用得到的数据会不断增加
    m_LFormula.setDate(last_data);
    m_LFormula.setTime(last_time);

    //结束参数列表的调用，其实没有什么用处...
    va_end(arg_ptr);

    return 1;
}

int DataCenterInstance::getString(QString computeStr,QString dispStr,QString& resultStr)
{
    QDataResultString dataResultString(&m_LFormula,computeStr.toStdString(),dispStr.toStdString());
    resultStr = tr(dataResultString.getResultString().c_str());
    return 1;
}
