/****************************************************************************
 ** 文件名：circleangle.cpp
 ** circleangle插件类实现
 ** 用于绘制一个可被填充颜色的circle，可以指定circle的大小、边框宽度、颜色和弧度
 ** 版本：v1.0
 ** 作者：ZY
 ** 2015-08-13 **
 ****************************************************************************/
#include <QtGui>
#include "circleangle.h"

circleangle::circleangle(QWidget *parent) :
    QWidget(parent)
{
    //init para
    unsetBgBrush();
    unsetBorderBrush();
    unsetBorderStyle();
    unsetBorderWidth();

    // default size
    setWindowTitle(tr("circleangle"));

    // update when para changed
    connect(this,SIGNAL(bgBrushChanged(QBrush)),this,SLOT(update()));
    connect(this,SIGNAL(borderBrushChanged(QBrush)),this,SLOT(update()));
    connect(this,SIGNAL(borderStyleChanged(Qt::PenStyle)),this,SLOT(update()));
    connect(this,SIGNAL(borderWidthChanged(quint32)),this,SLOT(update()));
}

//reset
void circleangle::unsetBgBrush()
{
    m_bgBrush.setStyle(Qt::SolidPattern);
    m_bgBrush.setColor(Qt::blue);
}

void circleangle::unsetBorderStyle()
{
    m_borderStyle=Qt::SolidLine;
}

void circleangle::unsetBorderBrush()
{
    m_borderBrush.setStyle(Qt::SolidPattern);
    m_borderBrush.setColor(Qt::black);
}

void circleangle::unsetBorderWidth()
{
    m_borderWidth=2;
}

//绘制Circle，重写painterEvent
void circleangle::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    QPen pen(borderColor(),borderWidth(),borderStyle());
    painter.setPen(pen);

    painter.setBrush(bgBrush());
    painter.save();

  //要求长宽取值相同，判断长宽，取较大值作为长宽取值
    if(geometry().width()>=geometry().height())
    {
        resize(geometry().width(),geometry().width());
    }
    else
    {
        resize(geometry().height(),geometry().height());
    }

    //画圆
    painter.drawEllipse(borderWidth(),borderWidth(),
                        geometry().width()-2*borderWidth(),geometry().height()-2*borderWidth());

    painter.restore();
    painter.end();
}
