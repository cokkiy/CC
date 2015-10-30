/****************************************************************************
 ** 文件名：qsingleline.cpp
 ** 直线插件类实现
 ** 用于绘制一条直线，可以指定直线的宽度、长度、颜色、样式、旋转角度
 ** 版本：v1.0
 ** 作者：王平
 ** 2015-08-19 **
 ****************************************************************************/
#include "qsingleline.h"
#include <QPainter>
#include <QRect>
QSingleLine::QSingleLine(QWidget *parent) :
    QWidget(parent)
{
    //直线属性初始化
    lineColor.setRgb(0,0,0);
    lineWidth = 2;    //直线初始化宽度
    lineLength = 500;//直线初始化长度
    lineStyle = Qt::SolidLine;
    unsetRotate();//角度
    connect(this,SIGNAL(rotateChanged(qint32)),this,SLOT(update()));//角度改变时更新
}
QSingleLine::~QSingleLine()
{
}

void QSingleLine::paintEvent(QPaintEvent *)
{

    QPainter painter(this);
    QPen pen = SetCustomPen(lineStyle,lineColor,lineWidth);
    painter.setPen(pen);
    painter.translate(width()/2,(height())/2);
    painter.rotate(m_rotate);
    painter.drawLine(-((int)lineLength)/2,0,((int)lineLength)/2,0);
}

//设置画笔风格
void QSingleLine::SetLineColor(const QColor Color)
{
    lineColor = Color;
    update();
    updateGeometry();
}

//设置画笔风格
void QSingleLine::SetLinePenStyle(const Qt::PenStyle style)
{

    lineStyle = style;
    update();
    updateGeometry();
}
//设置自定义画笔
QPen QSingleLine::SetCustomPen(Qt::PenStyle style,QColor color,uint width)
{
    QPen pen;
    pen.setColor(color);
    pen.setStyle(style);
    pen.setWidth(int (width));
    return pen;
}
//设置直线宽度
void QSingleLine::SetLineWidth(const uint width)
{
    lineWidth = width;
    update();
    updateGeometry();
}

//设置直线长度
void QSingleLine::SetLineLength(const uint length)
{
    lineLength=length;
    resize(length,width());
}

//设置直线旋转
void QSingleLine::setRotate(quint32 rotate)
{
    m_rotate=rotate;
    emit rotateChanged(rotate);
     update();
}

// 初始化角度
void QSingleLine::unsetRotate()
{
    m_rotate=0;
}


