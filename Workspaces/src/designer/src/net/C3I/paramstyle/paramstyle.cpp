#include "paramstyle.h"
QParamStyle::QParamStyle()
{
    m_StyleName = QObject::tr("");
    m_ParamStyleVector.clear();
}
//样式名称
void QParamStyle::setStyleName(const QString str)
{
    m_StyleName = str;
}
void QParamStyle::addParamStyle(const QString index,const double max,const double min)
{
    stru_ParamStyle style;
    style.m_Uprlmt = max;
    style.m_Lorlmt = min;
    style.m_IndexName = index;
    m_ParamStyleVector.push_back(style);
}
vector<stru_ParamStyle >* QParamStyle::getParamStyleVector()
{
    return &m_ParamStyleVector;
}
