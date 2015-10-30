/****************************************************************************
 ** 文件名：rectangle.h
 ** Rectangle插件类定义，定义了Rectangle插件具有的属性和方法，用于绘制可以填充的矩形
 ** 版本：v1.0
 ** 作者：张立民
 ** 2015-08-13 **
 ****************************************************************************/
#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <QWidget>

class Rectangle : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QBrush backgroundColor READ bgBrush WRITE setBgBrush NOTIFY bgBrushChanged RESET unsetBgBrush)
    Q_PROPERTY(QBrush borderColor READ borderColor WRITE setBorderBrush NOTIFY borderBrushChanged RESET unsetBorderBrush)
    Q_PROPERTY(qint32 borderWidth READ borderWidth WRITE setBorderWidth NOTIFY borderWidthChanged RESET unsetBorderWidth)
    Q_PROPERTY(Qt::PenStyle borderStyle READ borderStyle WRITE setBorderStyle NOTIFY borderStyleChanged RESET unsetBorderStyle)
    Q_PROPERTY(qint32 radius READ radius WRITE setRadius NOTIFY radiusChanged RESET unsetRadius)
    Q_PROPERTY(qint32 rotate READ rotate WRITE setRotate NOTIFY rotateChanged RESET unsetRotate)
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)
    //设置图元大小策略的属性
    Q_PROPERTY(QSizePolicy sizePolicy READ sizePolicy WRITE setSizePolicy)
    Q_PROPERTY(QSize minimumSize READ minimumSize WRITE setMinimumSize)
    Q_PROPERTY(QSize maximumSize READ maximumSize WRITE setMaximumSize)
    Q_PROPERTY(QSize sizeIncrement READ sizeIncrement WRITE setSizeIncrement)
    Q_PROPERTY(QSize baseSize READ baseSize WRITE setBaseSize)


public:
    Rectangle(QWidget *parent = 0);

    //rect backgroud brush
    void setBgBrush(const QBrush & brush)
    {
        m_bgBrush=brush;
        emit bgBrushChanged(brush);
    }

    QBrush bgBrush() const
    {
        return m_bgBrush;
    }

    //rect border brush
    void setBorderBrush(const QBrush & brush)
    {
        m_borderBrush=brush;
        emit borderBrushChanged(brush);
    }

    QBrush borderColor() const
    {
        return m_borderBrush;
    }

    // rect border line style
    void setBorderStyle(Qt::PenStyle style)
    {
        m_borderStyle=style;
        emit borderStyleChanged(style);
    }

    Qt::PenStyle borderStyle() const
    {
        return m_borderStyle;
    }


    //矩形的边框宽度
    void setBorderWidth(quint32 borderWidth)
    {
        m_borderWidth=borderWidth;
        emit borderWidthChanged(borderWidth);
    }

    qint32 borderWidth() const
    {
        return m_borderWidth;
    }

    //矩形的4角的弧度
    void setRadius(quint32 radius)
    {
        m_radius=radius;
        emit radiusChanged(radius);
    }

    qint32 radius() const
    {
        return m_radius;
    }

    //rotate
    void setRotate(quint32 rotate);


    qint32 rotate() const
    {
        return m_rotate;
    }


    // reset all para
    void unsetBgBrush();
    void unsetBorderStyle();
    void unsetBorderBrush();
    void unsetBorderWidth();
    void unsetRadius();
    void unsetRotate();



signals:
    void borderWidthChanged(quint32);
    void radiusChanged(quint32);
    void rotateChanged(qint32);
    void bgBrushChanged(const QBrush &);
    void borderBrushChanged(const QBrush &);
    void borderStyleChanged(Qt::PenStyle);
    void rectHeightChanged(quint32);
    void rectWidthChanged(quint32);

public slots:

private:
    quint32 m_borderWidth;
    quint32 m_radius;
    QBrush m_bgBrush;
    QBrush m_borderBrush;
    Qt::PenStyle m_borderStyle;
    qint32 m_rotate;
    qint32 m_width,m_height;


protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent * event);
};

#endif
