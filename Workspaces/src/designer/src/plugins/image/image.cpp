
/****************************************************************************
 ** image.cpp
 ** 工作内容：实现绘制*.bmp,*.png格式图片的读取，拉伸显示；图片背景可以是透明和不透明的。
 ** 用于显示的图片必须事先保存到资源文件
 ** 存在的问题：无法显示*.jpg格式图片。
 ** 版本：v1.0
 ** 作者：朱燕
 ** 2015-08-25 **
 ****************************************************************************/
#include "image.h"
#include<QWidget>
#include<QPainter>

image::image(QWidget *parent) :
    QWidget(parent)
{
    //init初始化图片
    m_pixmap.load("://images/pentacle.png");//目前测试可用的图片格式*.bmp/*.png

}


//重绘事件
void image::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawPixmap(0, 0,geometry().width(),geometry().height(), m_pixmap);

}

