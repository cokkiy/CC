#ifndef CSTATICTXT_H
#define CSTATICTXT_H

#include <QWidget>
#include <QLabel>


class Cstatictxt : public QLabel//
{
    Q_OBJECT
    //重新设计静态文本框属性系统
    //以下将函数注册到QT的属性系统中
    //文本属性
    //1、读写文本
    Q_PROPERTY(QString text READ text WRITE setText)

    //2、设置文本字体
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY txtFontChanged)
    //    Q_PROPERTY(QFont font READ font WRITE setFont)
    //5、设置对齐
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment NOTIFY txtAlignmentChanged)

    //3、设置字体颜色,自己定义的属性
    Q_PROPERTY(QColor txtcolor READ txtcolor WRITE
               settxtcolor NOTIFY txtcolorChanged)

    //背景属性
    //4、设置背景:背景矩形内部填充颜色,自己定义的属性
    Q_PROPERTY(QBrush backgroundcolor READ backgroundcolor WRITE
               setbackgroundcolor NOTIFY backgroundcolorChanged)
    //4、设置背景:背景矩形宽与高,自己定义的属性
    Q_PROPERTY(qint32 rectWidth READ rectWidth WRITE setRectWidth NOTIFY rectWidthChanged)
    Q_PROPERTY(qint32 rectHeight READ rectHeight WRITE setRectHeight NOTIFY rectHeightChanged)



    //7、设置旋转角度,自己定义的属性
    Q_PROPERTY(qint32 rotate READ rotate WRITE setrotate NOTIFY rotateChanged RESET unsetrotate)

    //6、设置控件边框，控件定位的必要变量坐标
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)

public:
    Cstatictxt(QWidget *parent = 0);
    ~Cstatictxt();


    //设置背景颜色,自己定义的属性
    void setbackgroundcolor(const QBrush & bgcolor)
    {
        m_bgcolor=bgcolor;
        emit backgroundcolorChanged(bgcolor);
    }
    QBrush backgroundcolor() const
    {
        return m_bgcolor;
    }

    //2、设置文本字体
    void setFont(const QFont &txfont)
    {
        m_txfont=txfont;
        emit txtFontChanged(txfont);
    }
    QFont font() const
    {
        return m_txfont;
    }


    //5、设置对齐
    void setAlignment(const Qt::Alignment &txalignment)
    {
        m_txalignment=txalignment;
        emit txtAlignmentChanged(txalignment);
    }
    Qt::Alignment alignment() const
    {
        return m_txalignment;
    }


    //设置字体颜色,自己定义的属性
    void settxtcolor(const QColor &txcolor)
    {
        m_txcolor=txcolor;
        emit txtcolorChanged(txcolor);
    }
    QColor txtcolor() const
    {
        return m_txcolor;
    }



    //设置旋转
    void setrotate(quint32 rotate);
    qint32 rotate()const
    {
      return m_rotate;
    }

    //设置背景矩形的宽和长rect width and height
    //rect width and height
    void setRectWidth(qint32 width)
    {
        m_width=width;
        emit rectWidthChanged(width);
    }

    qint32 rectWidth()
    {
        return m_width;
    }

    void setRectHeight(qint32 height)
    {
        m_height=height;
        emit rectHeightChanged(height);
    }

    qint32 rectHeight()
    {
        return m_height;
    }


    //专门用于重置画的背景矩形边框的属性，使边框看不到
    //包括颜色和宽度
    void setBorderBrush(const QBrush & brush)
    {
        m_borderBrush=brush;
    }

    QBrush borderBrush() const
    {
        return m_borderBrush;
    }


    void unsetrotate();


signals:

    void backgroundcolorChanged(const QBrush &);
    void txtFontChanged(const QFont &);
    void txtcolorChanged(const QColor &);
    void txtAlignmentChanged(const Qt::Alignment &);

    void rotateChanged(quint32);

    void rectHeightChanged(quint32);
    void rectWidthChanged(quint32);

public slots:
    void visible();

protected:
    //重写绘图函数 zjb add
    void paintEvent(QPaintEvent *event);
    //重画事件
    void resizeEvent(QResizeEvent * event);

private :


    QBrush m_bgcolor;//背景色
    QFont  m_txfont;//文本字体
    Qt::Alignment m_txalignment;//设置对齐



    QColor m_txcolor;//文本字体颜色
    quint32 m_rotate;//旋转角度
    bool m_bVisible;//可见性

    //quint32 m_borderWidth;//背景矩形的边框宽度
    QBrush m_borderBrush;//背景矩形的边框画刷：颜色和宽度
    qint32 m_width,m_height;//背景矩形的宽度、高度
};

#endif
