
/****************************************************************************
 ** 文件名：ellipseangle.h
 ** ellipseangle插件类定义，定义了ellipseangle插件具有的属性和方法，用于绘制可以填充的ellipse
 ** 版本：v1.0
 ** 作者：zy
 ** 2015-08-13 **
 ****************************************************************************/
#ifndef ELLIPSEANGLE_H
#define ELLIPSEANGLE_H

#include <QWidget>

class Ellipseangle : public QWidget
{
    Q_OBJECT

    //背景画刷
    Q_PROPERTY(QBrush backgroundBrush READ bgBrush WRITE setBgBrush NOTIFY bgBrushChanged RESET unsetBgBrush)
    //画笔风格
    Q_PROPERTY(Qt::PenStyle borderStyle READ borderStyle WRITE setBorderStyle NOTIFY borderStyleChanged RESET unsetBorderStyle)
    //边框画刷
    Q_PROPERTY(QBrush borderBrush READ borderBrush WRITE setBorderBrush NOTIFY borderBrushChanged RESET unsetBorderBrush)
    //边框宽度
    Q_PROPERTY(qint32 borderWidth READ borderWidth WRITE setBorderWidth NOTIFY borderWidthChanged RESET unsetBorderWidth)
    //旋转角度
    Q_PROPERTY(qint32 rotate READ rotate WRITE setRotate NOTIFY rotateChanged RESET unsetRotate)

    //图元背景矩形坐标(x,y)width and height
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)

public:
    Ellipseangle(QWidget *parent = 0);

    //Ellipse backgroud brush
    void setBgBrush(const QBrush & brush)
    {
        m_bgBrush=brush;
        emit bgBrushChanged(brush);
    }

    QBrush bgBrush() const
    {
        return m_bgBrush;
    }

    //Ellipse border brush
    void setBorderBrush(const QBrush & brush)
    {
        m_borderBrush=brush;
        emit borderBrushChanged(brush);
    }

    QBrush borderBrush() const
    {
        return m_borderBrush;
    }

    // Ellipse border line style
    void setBorderStyle(Qt::PenStyle style)
    {
        m_borderStyle=style;
        emit borderStyleChanged(style);
    }

    Qt::PenStyle borderStyle() const
    {
        return m_borderStyle;
    }

    //Ellipse边框宽度
    void setBorderWidth(quint32 borderWidth)
    {
        m_borderWidth=borderWidth;
        emit borderWidthChanged(borderWidth);
    }

    qint32 borderWidth() const
    {
        return m_borderWidth;
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
    void unsetRotate();

signals:
    void borderWidthChanged(quint32);
    void rotateChanged(qint32);
    void bgBrushChanged(const QBrush &);
    void borderBrushChanged(const QBrush &);
    void borderStyleChanged(Qt::PenStyle);

public slots:

private:
    quint32 m_borderWidth;
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

