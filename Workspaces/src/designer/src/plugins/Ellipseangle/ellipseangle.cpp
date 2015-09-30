/****************************************************************************
 ** 文件名：ellipiseangle.cpp
 **ellipiseangle插件类实现
 ** 用于绘制一个可被填充颜色的Ellipise，可以指定Ellipise的大小、边框宽度、颜色和4个角的弧度
 ** 版本：v1.0
 ** 作者：zy
 ** 2015-08-13 **
 ****************************************************************************/
#include <QtGui>
#include <QtMath>
#include "ellipseangle.h"

Ellipseangle::Ellipseangle(QWidget *parent) :
    QWidget(parent)
{

    //init para
    unsetBgBrush();
    unsetBorderBrush();
    unsetBorderStyle();
    unsetBorderWidth();
    unsetRotate();

    // default size
    setWindowTitle(tr("ellipseangle"));

    // update when para changed
    connect(this,SIGNAL(bgBrushChanged(QBrush)),this,SLOT(update()));
    connect(this,SIGNAL(borderBrushChanged(QBrush)),this,SLOT(update()));
    connect(this,SIGNAL(borderStyleChanged(Qt::PenStyle)),this,SLOT(update()));
    connect(this,SIGNAL(borderWidthChanged(quint32)),this,SLOT(update()));
    connect(this,SIGNAL(rotateChanged(qint32)),this,SLOT(update()));
}

// Set rotate angle
void Ellipseangle::setRotate(quint32 rotate)
{
    m_rotate=rotate;
    emit rotateChanged(rotate);
}

//reset Bg Brush
void Ellipseangle::unsetBgBrush()
{
    m_bgBrush.setStyle(Qt::SolidPattern);
    m_bgBrush.setColor(Qt::blue);
}

//reset border style
void Ellipseangle::unsetBorderStyle()
{
    m_borderStyle=Qt::SolidLine;
}

//reset border brush
void Ellipseangle::unsetBorderBrush()
{
    m_borderBrush.setStyle(Qt::SolidPattern);
    m_borderBrush.setColor(Qt::black);
}


// reset border width
void Ellipseangle::unsetBorderWidth()
{
    m_borderWidth=2;
}

// reset roatate
void Ellipseangle::unsetRotate()
{
    m_rotate=0;
}

// handle resize event
void Ellipseangle::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if(m_rotate==0)
    {
        m_width=geometry().width()-2*borderWidth();
        m_height=geometry().height()-2*borderWidth();
    }
}

//绘制Ellipse
void Ellipseangle::paintEvent(QPaintEvent *event)
{

    Q_UNUSED(event);
    QPainter painter(this);

    QPen pen(borderBrush(),borderWidth(),borderStyle());
    painter.setPen(pen);

    painter.setBrush(bgBrush());

    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(geometry().width()/2,geometry().height()/2);
    painter.rotate(m_rotate);
    painter.save();

    painter.drawEllipse(-m_width/2,-m_height/2,m_width,m_height);

    painter.restore();
    painter.end();
}
