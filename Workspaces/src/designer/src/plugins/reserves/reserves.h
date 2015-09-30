/****************************************************************************
 ** reserves.h
 ** Reserves插件类定义，定义了Reserves插件具有的属性和方法，用于绘制储量的图元
 ** 版本：v1.0
 ** 作者：朱燕
 ** 2015-09-02 **
 ****************************************************************************/

#ifndef RESERVES_H
#define RESERVES_H

#include <QWidget>
#include <QLabel>
#include<QTimerEvent>
#include <Net/NetComponents>

class reserves : public QWidget
{
    Q_OBJECT
    //1.参数
    Q_PROPERTY(QString data READ getData WRITE setData NOTIFY dataChanged)

    //2.矩形中的文本属性
    //    Q_PROPERTY(QString text READ text WRITE setText) //1、读写文本
    Q_PROPERTY(QFont font READ font WRITE setFont)//2、设置文本字体
    Q_PROPERTY(QColor textcolor READ textcolor WRITE settextcolor NOTIFY textcolorChanged)//3.文本颜色

    //3、设置背景:背景矩形颜色
    Q_PROPERTY(QBrush backgroundColor  READ backgroundColor  WRITE setBackgroundColor  NOTIFY backgroundColorChanged)

    //4、设置填充颜色
    Q_PROPERTY(QBrush fillColor READ fillColor WRITE setFillColor NOTIFY fillColorChanged)

    //5.判断百分比方式，显示或不显示
    Q_PROPERTY(PercentShow percentShow READ percentShow WRITE setPercentShow NOTIFY percentShowChanged)

    //6.判断图元显示样式，横向或纵向
    Q_PROPERTY(RectStyle rectStyle READ rectStyle WRITE setRectStyle NOTIFY rectStyleChanged)

    //7.图元背景矩形坐标
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)

    //8.定义枚举类型
    Q_ENUMS(PercentShow)
    Q_ENUMS(RectStyle)

public:
    reserves(QWidget *parent = 0);
    ~reserves();

    //定义枚举成员
    enum PercentShow { Yes, No };
    enum RectStyle { Horizontal, Vertical };

    //判断百分比方式
    void setPercentShow(PercentShow percentShow)
    {
        m_percentShow = percentShow;
        emit percentShowChanged(percentShow);
    }
    reserves::PercentShow percentShow() const
    { return m_percentShow; }

    //判断图元显示样式
    void setRectStyle(RectStyle rectStyle)
    {
        m_rectStyle = rectStyle;
        emit rectStyleChanged(rectStyle);
    }
    reserves::RectStyle rectStyle() const
    { return m_rectStyle; }

    //获取/设置  参数
    void setData(const QString &newData)
    {
        m_data=newData;
        emit dataChanged(newData);
    }

    QString getData() const
    {
        return m_data;
    }

    //设置字体颜色
    void settextcolor(const QColor &txcolor)
    {
        m_txcolor=txcolor;
        emit textcolorChanged(txcolor);
    }

    QColor textcolor() const
    {
        return m_txcolor;
    }

    //设置背景颜色
    void setBackgroundColor(const QBrush & newbgcolor)
    {
        m_bgcolor=newbgcolor;
        emit backgroundColorChanged(newbgcolor);
    }

    QBrush backgroundColor() const
    {
        return m_bgcolor;
    }

    //设置填充颜色
    void setFillColor(const QBrush & newfillColor)
    {
        m_fillcolor=newfillColor;
        emit fillColorChanged(newfillColor);
    }

    QBrush fillColor() const
    {
        return m_fillcolor;
    }

    //专门用于重画的背景矩形边框的属性：包括颜色和宽度
    //画刷
    void setBorderBrush(const QBrush & brush)
    {
        m_borderBrush=brush;
        emit borderBrushChanged(brush);
    }

    QBrush borderBrush() const
    {
        return m_borderBrush;
    }

signals:
    void dataChanged(const QString &);
    void backgroundColorChanged(const QBrush &);
    void fillColorChanged(const QBrush &);
    void textcolorChanged(const QColor &);
    void borderBrushChanged(const QBrush &);
    void percentShowChanged(PercentShow);
    void rectStyleChanged(RectStyle);

private:
    QString m_data;//参数
    float m_floatData;//参数数据
    DataCenterInterface* m_dc;//
    QColor m_txcolor;//文本颜色
    QBrush m_bgcolor;//背景矩形颜色
    QBrush m_fillcolor;//填充颜色
    QBrush m_borderBrush;//画刷
    PercentShow m_percentShow;//判断百分比方式
    RectStyle m_rectStyle;//判断图元显示样式
    //定时器 用于更新图形绘制
    int m_timer_id;//定时器

protected:
    //重写绘图函数
    void paintEvent(QPaintEvent *event);

protected slots:
    //事件-定时器触发
    void timerEvent(QTimerEvent *event);

};

#endif
