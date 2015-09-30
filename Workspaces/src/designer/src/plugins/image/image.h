/****************************************************************************
 ** image.h
 ** 工作内容： 图片处理插件类定义，定义了image插件具有的属性和方法；
 ** 用于绘制*.bmp,*.png格式图片的读取，拉伸显示；图片背景可以是透明和不透明的。
 ** 用于显示的图片必须事先保存到资源文件。
 ** 存在的问题：无法显示*.jpg格式图片。
 ** 版本：v1.0
 ** 作者：朱燕
 ** 2015-08-25 **
 ****************************************************************************/

#ifndef OPENPIC_H
#define OPENPIC_H

#include <QWidget>
#include<QPainter>
#include<QSize>

class image : public QWidget
{
    Q_OBJECT

    //1.读取图片
    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap)

    //2、设置控件边框，控件定位的必要变量坐标
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)

public:
    image(QWidget *parent = 0);

    //读取图片
    void setPixmap(const QPixmap newPixmap)
    {
        m_pixmap = newPixmap;
    }
    QPixmap pixmap() const
    {
        return m_pixmap;
    }

protected:
    //事件 绘制
    void paintEvent(QPaintEvent *event);

private:
    QPixmap m_pixmap;

};

#endif
