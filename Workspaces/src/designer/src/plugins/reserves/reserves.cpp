/*********************************************************************
 ** 文件名：reserves.cpp
 ** Reserves插件类实现
 ** 用于绘制一个随获取的数据大小改变填充量的图元，可以指定背景矩形的大小、颜色和
 ** 填充矩形的颜色以及百分比的显示可选，显示样式包括横向和纵向可选。
 ** 版本：v1.0
 ** 作者：朱燕
 ** 2015-09-02 **
 ***********************************************************************/
#include "reserves.h"
#include<QPen>
#include<QStylePainter>
#include <QtMath>
#include <QColor>
#include <Net/NetComponents>

reserves::reserves(QWidget *parent) :
    QWidget(parent)
{
    setWindowTitle(tr("Reserves"));
    setData("0");

    //初始矩形中百分比：显示
    m_percentShow = Yes;

    //初始图元显示样式：纵向
    m_rectStyle = Vertical;

    //设置背景矩形
    m_bgcolor.setColor(Qt::white);//初始背景为white
    m_bgcolor.setStyle(Qt::SolidPattern);//Uniform color.
    //设置填充矩形
    m_fillcolor.setColor(Qt::blue);//初始填充为blue
    m_fillcolor.setStyle(Qt::SolidPattern);//Uniform color.
    //设置静态文本颜色
    m_txcolor.setRgb(0,0,0);//初始字体为black

    //设置背景矩形边框颜色透明，无画刷
    m_borderBrush.setStyle(Qt::NoBrush);
    m_borderBrush.setColor(Qt::transparent);

    // update when signals changed
    connect(this,SIGNAL(dataChanged(QString)),this,SLOT(update()));
    connect(this,SIGNAL(textcolorChanged(QColor)),this,SLOT(update()));
    connect(this,SIGNAL(backgroundColorChanged(QBrush)),this,SLOT(update()));
    connect(this,SIGNAL(fillColorChanged(QBrush)),this,SLOT(update()));
    connect(this,SIGNAL(percentShowChanged(PercentShow)),this,SLOT(update()));
    connect(this,SIGNAL(rectStyleChanged(RectStyle)),this,SLOT(update()));

    ////设置一个500ms定时器
    m_timer_id = startTimer(500);
    m_dc = NetComponents::getDataCenter();

    //初始化资源
    Q_INIT_RESOURCE(icons);
}

//初始化
reserves::~reserves()
{
    if(m_timer_id!=0)
    {
        killTimer(m_timer_id);//删除定时器
    }
    if(m_dc!=NULL)
    {
        delete m_dc;
        m_dc = NULL;
    }
}

/*********重写绘图函数*************************************************/
//    方法：在自己的QLabel类的派生类reserves中
// 重写QLabel类的paintEvent函数,绘图三次，分别为：背景矩形，填充矩形，百分比文本；
//判断data数值大小，判断绘图方式，以及判断百分比方式等
/*********************************************************************/
void reserves::paintEvent(QPaintEvent *event)
{
    QStylePainter painter3(this);

    //判断参数大小，大于1为1，小于0为0
    if(m_floatData>=1)
        m_floatData = 1;
    else if(m_floatData<=0)
        m_floatData = 0;

    //画背景矩形,使用第1支画笔pen
    //默认设置背景矩形边框颜色为透明
    QPen pen(borderBrush(),1,Qt::NoPen);
    painter3.setPen(pen);

    //设置背景矩形内部颜色，默认为红色
    painter3.setBrush(backgroundColor());

    painter3.save();

    QRect rect1(0,0,geometry().width(),geometry().height());

    painter3.drawRect(rect1);

    //画填充矩形,使用第2支画笔pen2
    //默认设置填充矩形边框颜色为透明
    QPen pen2(borderBrush(),1,Qt::NoPen);
    painter3.setPen(pen2);

    //设置填充矩形内部颜色，默认为黄色
    painter3.setBrush(fillColor());

    painter3.save();

    //判断图元显示样式，横向或纵向
    if(m_rectStyle==Vertical)//纵向
    {
        int yy = round(geometry().height()*(1-m_floatData));
        int hh = round(geometry().height()*m_floatData);
        QRect rect2(0,yy,geometry().width(),hh);

        painter3.drawRect(rect2);
    }
    else if(m_rectStyle==Horizontal)//横向
    {
        int ww = round(geometry().width()*m_floatData);
        QRect rect2(0,0,ww,geometry().height());

        painter3.drawRect(rect2);
    }

    //判断百分比方式，显示或不显示
    if(m_percentShow==Yes)//显示
    {
        QPen pen3(textColor());
        painter3.setPen(pen3);

        //读写文本
        QString percent = QString::number(m_floatData*100)+"%";
        //        setText(percent);
        painter3.drawText(rect1, Qt::AlignCenter, percent);
    }

    painter3.restore();
    painter3.end();

}

//定时器事件
void reserves::timerEvent(QTimerEvent *event)
{  
    ////使用数据处理中心的接口方法
    double ret;
    if(m_dc->getValue(m_data,ret))
    {
        m_floatData = ret;
        update();
    }
}


