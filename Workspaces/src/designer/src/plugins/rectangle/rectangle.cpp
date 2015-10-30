/****************************************************************************
 ** 文件名：rectangle.cpp
 ** rectangle插件类实现
 ** 用于绘制一个可被填充颜色的矩形，可以指定矩形的大小、边框宽度、颜色和4个角的弧度
 ** 版本：v1.0
 ** 作者：张立民
 ** 2015-08-13 **
 ****************************************************************************/
#include <QtGui>
#include <QtMath>
#include "rectangle.h"

Rectangle::Rectangle(QWidget *parent) :
    QWidget(parent)
{
    //init para
    unsetBgBrush();
    unsetBorderBrush();
    unsetBorderStyle();
    unsetBorderWidth();
    unsetRadius();
    unsetRotate();

    // default size
    m_width=145;
    m_height=95;


    // update when para changed
    connect(this,SIGNAL(rectWidthChanged(quint32)),this,SLOT(update()));
    connect(this,SIGNAL(rectHeightChanged(quint32)),this,SLOT(update()));
    connect(this,SIGNAL(bgBrushChanged(QBrush)),this,SLOT(update()));
    connect(this,SIGNAL(borderBrushChanged(QBrush)),this,SLOT(update()));
    connect(this,SIGNAL(borderStyleChanged(Qt::PenStyle)),this,SLOT(update()));
    connect(this,SIGNAL(borderWidthChanged(quint32)),this,SLOT(update()));
    connect(this,SIGNAL(radiusChanged(quint32)),this,SLOT(update()));
    connect(this,SIGNAL(rotateChanged(qint32)),this,SLOT(update()));
}

// Set rotate angle
void Rectangle::setRotate(quint32 rotate)
{
    m_rotate=rotate;

    // calc rect size based on rotate angle
//    float w=width();
//    float h=height();
//    float r=qSqrt(qPow(w/2,2)+qPow(h/2,2));
//    int newWidth=qAbs(qCos(qAtan(h/w)-qDegreesToRadians((float)m_rotate))*r)*2;
//    int newHeight=qAbs(qSin(qAtan(h/w)+qDegreesToRadians((float)m_rotate))*r)*2;

//    resize(newWidth,newHeight);

    emit rotateChanged(rotate);
}

//reset Bg Brush
void Rectangle::unsetBgBrush()
{
    m_bgBrush.setStyle(Qt::SolidPattern);
    m_bgBrush.setColor(Qt::red);
}

//reset border style
void Rectangle::unsetBorderStyle()
{
    m_borderStyle=Qt::SolidLine;
}


//reset border brush
void Rectangle::unsetBorderBrush()
{
    m_borderBrush.setStyle(Qt::SolidPattern);
    m_borderBrush.setColor(Qt::darkRed);
}


// reset border width
void Rectangle::unsetBorderWidth()
{
    m_borderWidth=5;
}


// reset radius
void Rectangle::unsetRadius()
{
    m_radius=10;
}


// reset roatate
void Rectangle::unsetRotate()
{
    m_rotate=0;
}


// handle resize event
void Rectangle::resizeEvent(QResizeEvent *event)
{
    if(m_rotate==0)
    {
        m_width=width()-borderWidth();
        m_height=height()-borderWidth();
    }
}

//绘制矩形
void Rectangle::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);

    QPen pen(borderColor(),borderWidth(),borderStyle());
    painter.setPen(pen);

    painter.setBrush(bgBrush());



    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width()/2,height()/2);
    painter.rotate(m_rotate);

    painter.save();

    QRect rect(-m_width/2,-m_height/2,m_width,m_height);
    if(this->radius()==0)
    {
        painter.drawRect(rect);
    }
    else
    {
        painter.drawRoundedRect(rect,
                                radius(),radius());
    }


    painter.restore();
    painter.end();
}
