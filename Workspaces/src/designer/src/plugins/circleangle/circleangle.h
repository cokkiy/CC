/****************************************************************************
 ** 文件名：circleangle.h
 ** circleangle插件类定义，定义了circleangle插件具有的属性和方法，用于绘制可以填充的矩形
 ** 版本：v1.0
 ** 作者：ZY
 ** 2015-08-13 **
 ****************************************************************************/
#ifndef CIRCLEANGLE_H
#define CIRCLEANGLE_H

#include <QWidget>

class circleangle : public QWidget
{
    Q_OBJECT

    //画刷背景
    Q_PROPERTY(QBrush backgroundBrush READ bgBrush WRITE setBgBrush NOTIFY bgBrushChanged RESET unsetBgBrush)
    //画笔风格
    Q_PROPERTY(Qt::PenStyle borderStyle READ borderStyle WRITE setBorderStyle NOTIFY borderStyleChanged RESET unsetBorderStyle)
    //画刷边框
    Q_PROPERTY(QBrush borderBrush READ borderBrush WRITE setBorderBrush NOTIFY borderBrushChanged RESET unsetBorderBrush)
    //边框宽度
    Q_PROPERTY(qint32 borderWidth READ borderWidth WRITE setBorderWidth NOTIFY borderWidthChanged RESET unsetBorderWidth)
    //图元背景矩形坐标(x,y)width and height
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)

public:
    circleangle(QWidget *parent = 0);

    //circle backgroud brush
    void setBgBrush(const QBrush & brush)
    {
        m_bgBrush=brush;
        emit bgBrushChanged(brush);
    }

    QBrush bgBrush() const
    {
        return m_bgBrush;
    }

    //circle border brush
    void setBorderBrush(const QBrush & brush)
    {
        m_borderBrush=brush;
        emit borderBrushChanged(brush);
    }

    QBrush borderBrush() const
    {
        return m_borderBrush;
    }

    // circle border style
    void setBorderStyle(Qt::PenStyle style)
    {
        m_borderStyle=style;
        emit borderStyleChanged(style);
    }

    Qt::PenStyle borderStyle() const
    {
        return m_borderStyle;
    }

    //Circle borderwidth
    void setBorderWidth(quint32 borderWidth)
    {
        m_borderWidth=borderWidth;
        emit borderWidthChanged(borderWidth);
    }

    qint32 borderWidth() const
    {
        return m_borderWidth;
    }

    // reset all para
    void unsetBgBrush();
    void unsetBorderStyle();
    void unsetBorderBrush();
    void unsetBorderWidth();

    //Notifier signal
signals:
    void borderWidthChanged(quint32);
    void bgBrushChanged(const QBrush &);
    void borderBrushChanged(const QBrush &);
    void borderStyleChanged(Qt::PenStyle);

    //public slots:
private:
    quint32 m_borderWidth;
    QBrush m_bgBrush;
    QBrush m_borderBrush;
    Qt::PenStyle m_borderStyle;
    qint8 m_rotate;

protected:
    void paintEvent(QPaintEvent *event);
};

#endif
