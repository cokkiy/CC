#ifndef PARAMSTYLE_H
#define PARAMSTYLE_H
#include "../stru.h"
#include <QString>
class QParamStyle
{
private:
    //样式名称
    QString m_StyleName;
	//子样式
    vector<stru_ParamStyle> m_ParamStyleVector;
public:
    QParamStyle();
    //样式名称
    QString getStyleName() const {return m_StyleName;}
    void setStyleName(const QString str);
    void addParamStyle(const QString,const double ,const double);
    vector<stru_ParamStyle >* getParamStyleVector();
};

#endif // PARAMSTYLE_H
