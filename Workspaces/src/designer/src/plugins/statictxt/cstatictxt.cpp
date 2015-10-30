/********************************************************************************
//  项目标识：跨平台指显
//  设计图元：绘制静态文本控件
//  实现功能：（1）可直接在图元上，写入文本;
            （2）定制静态文本控件的字体；
            （3）定制静态文本控件的颜色；
            （4）定制静态文本控件的背景；
            （5）图元能设置对齐方式，左对齐，右对齐，居中显示等;
            （6）静态文本控件可在平面上旋转一定的角度；

//  版本号：  v1.0
//  设计者：  张进宝
//  设计时间：2015.8.13
********************************************************************************/
#include "cstatictxt.h"
#include <stdlib.h>
// sdk
#include<QDebug>
#include<qpainter.h>
#include<QPen>
#include<QStylePainter>
#include<QTextOption>
#include <QtGui>
#include <QtMath>
#include <QColor>

Cstatictxt::Cstatictxt(QWidget *parent) :
    QLabel(parent)
{

    setWindowTitle(tr("Cstatictxt"));
    //设置初始旋转角度(为0度)
    unsetrotate();

    //设置背景矩形
    m_bgcolor.setColor(Qt::red);//初始背景为红色    
    m_width=162;//设置背景矩形的初始长和宽
    m_height=100;

    //设置字体

    m_txfont.setPointSize(23);;//初始字体大小23
//    m_txfont.setRawName("please input txt");

    m_txcolor.setRgb(0,0,0,255);//初始字体颜色为黑色

    m_txalignment = Qt::AlignCenter;//初始字体位置为居中对齐

    //m_rotate=0;
    m_bVisible = true;


    //设置背景矩形边框颜色为透明，无画刷
    m_borderBrush.setStyle(Qt::NoBrush);
    m_borderBrush.setColor(Qt::transparent);

    connect(this,SIGNAL(txtFontChanged(QFont)),this,SLOT(update()));
    connect(this,SIGNAL(txtcolorChanged(QColor)),this,SLOT(update()));
    connect(this,SIGNAL(txtAlignmentChanged(Qt::Alignment)),this,SLOT(update()));

    connect(this,SIGNAL(backgroundcolorChanged(QBrush)),this,SLOT(update()));
    connect(this,SIGNAL(rotateChanged(quint32)),this,SLOT(update()));

    connect(this,SIGNAL(rectWidthChanged(quint32)),this,SLOT(update()));
    connect(this,SIGNAL(rectHeightChanged(quint32)),this,SLOT(update()));
}

Cstatictxt::~Cstatictxt()
{
}



// Set rotate rotate
void Cstatictxt::setrotate(quint32 rotate)
{
    m_rotate=rotate;

    // calc rect size based on rotate rotate
    float w=width();
    float h=height();
    float r=qSqrt(qPow(w/2,2)+qPow(h/2,2));
    int newWidth=qAbs(qCos(qAtan(h/w)-qDegreesToRadians((float)m_rotate))*r)*2;
    int newHeight=qAbs(qSin(qAtan(h/w)+qDegreesToRadians((float)m_rotate))*r)*2;

    resize(newWidth,newHeight);

    emit rotateChanged(rotate);
}


// reset rotate
void Cstatictxt::unsetrotate()
{
    m_rotate=0;
}


// handle resize event
void Cstatictxt::resizeEvent(QResizeEvent *event)
{
    if(m_rotate==0)
    {
//        m_width=width()-borderWidth();
//        m_height=height()-borderWidth();
          m_width=width()-1;
          m_height=height()-1;
    }
}




/*********重写绘图函数 zjb add********************/
//    方法：在自己的QLabel类的派生类Cstatictxt中
// 重写QLabel类的paintEvent函数
//    步骤：可修改rotate(旋转角度属性)，根据角度值重画
/***********************************************/
void Cstatictxt::paintEvent(QPaintEvent *event)
{
    QStylePainter painter2(this);


    //画背景矩形,使用第1支画笔pen
    //设置背景矩形边框颜色为透明
    //QPen pen(borderBrush(),borderWidth(),Qt::NoPen);
    QPen pen(borderBrush(),1,Qt::NoPen);
    painter2.setPen(pen);

    //设置背景矩形内部颜色
    painter2.setBrush(backgroundColor());

    //取控件的中心点旋转
    painter2.setRenderHint(QPainter::Antialiasing);
    painter2.translate(width()/2,height()/2);

    painter2.rotate(m_rotate);

    painter2.save();

    QRect rect(-m_width/2,-m_height/2,m_width,m_height);

    painter2.drawRect(rect);



    //画内部静态文本,使用第2支画笔pen2
    //设置字体颜色
    QPen pen2(textColor());
    painter2.setPen(pen2);
    //设置字体
    painter2.setFont(font());

    QTextOption textOption;
    //设置字体对齐
    textOption.setAlignment(alignment());
    if(wordWrap())
    {
        textOption.setWrapMode(QTextOption::WordWrap);

    }
    else
    {
        textOption.setWrapMode(QTextOption::NoWrap);
    }
    //读写字体
    painter2.drawText(rect, alignment(), text());

    painter2.restore();
    painter2.end();
}



void Cstatictxt::visible()
{
    m_bVisible = !m_bVisible;
    if(!m_bVisible){
        hide();
    }
    else
    {
        show();
    }
}
