#ifndef STATICGRAPH_H
#define STATICGRAPH_H

#include <QWidget>
#include <qglobal.h>


class staticgraphPrivate;//提前在此声明，加快编译速度，相当于预先定义好数据成员，方便staticgraph类来访问

//这是一个静态曲线类
class staticgraph : public QWidget
{
    Q_OBJECT


    //多曲线配置对话框
    Q_PROPERTY(QString graphobj READ textString WRITE setTextString RESET rsttext)

    //共性部分

    //图元矩形
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)

    //背景属性
    //背景色
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setbackgroundColor)
    //背景图片
    Q_PROPERTY(QPixmap backgroundImage READ backgroundImage WRITE setbackgroundImage)

    //显示网格
    Q_PROPERTY(bool showGrid READ getShowGrid WRITE setShowGrid)

    //显示图例
    Q_PROPERTY(bool showLegend READ getShowLegend WRITE setShowLegend)

    //页边距属性
    Q_PROPERTY(qint32 leftMargin READ getleftMargin WRITE setleftMargin)
    Q_PROPERTY(qint32 topMargin READ gettopMargin WRITE settopMargin)
    Q_PROPERTY(qint32 rightMargin READ getrightMargin WRITE setrightMargin)
    Q_PROPERTY(qint32 bottomMargin READ getbottomMargin WRITE setbottomMargin)
   //个性部分


public:
    //构造函数
    staticgraph(QWidget *parent = 0);
    //析构函数
    virtual ~staticgraph();



    //多曲线配置
    QString textString() const
    {
        return m_textString;
    }
    void setTextString(const QString string);
    //用于点击时什么都不干
    void rsttext(){};


    //Json数据解析
    void parseJsonData();




   //共性部分

    //图元背景
    //图元背景颜色
    QColor backgroundColor()const;
    void setbackgroundColor(const QColor);
    //图元背景图片
    QPixmap backgroundImage()const;
    void setbackgroundImage(const QPixmap qq);
    //是否显示网格
    bool getShowGrid() const;
    void setShowGrid(const bool bShow);
    //是否显示图例
    bool getShowLegend() const;
    void setShowLegend(const bool bShow);
    //获取/设置理论页边距属性
    qint32 getleftMargin()const;
    qint32 getrightMargin()const;
    qint32 gettopMargin()const;
    qint32 getbottomMargin()const;
    void setleftMargin(const qint32 lm);
    void setrightMargin(const qint32 rm);
    void settopMargin(const qint32 tm);
    void setbottomMargin(const qint32 bm);

    //个性部分



signals:



protected slots:



protected:


    //事件-定时器触发超时事件
    void timerEvent(QTimerEvent *event);
    //事件-重写绘图事件
    void paintEvent(QPaintEvent *event);

    //定时器 用于更新图形绘制
    qint32 m_timer_id;




private:
    QString m_textString;


private:

    //数据封装（隐藏）
    //智能指针：
    //这是一个智能指针：它能保证这个作用域结束后，里面的所有指针都能够被自动delete掉，即其实就是一个比普通指针更多功能的指针。
    //此句相当于:  staticgraphPrivate *d_ptr;
    QScopedPointer<staticgraphPrivate> d_ptr;


    Q_DECLARE_PRIVATE(staticgraph)
    //宏展开为：staticgraphPrivate* d_func(){...}; //d_ptr指针
    //        staticgraphPrivate* d_func(){...}; //const指针
    //        friend class staticgraphPrivate;   //设为友元类
    //这样，可以在staticgraph这个类里面使用Q_D（staticgraph）以及Q_D(const staticgraph)
    //宏展开为：staticgraphPrivate* const d= d_func()
    //为此，QT为我们把d_func()获取staticgraphPrivate指针的代码给封装起来了。


    Q_DISABLE_COPY(staticgraph)
    //#define Q_DISABLE_COPY(Class) \

public slots:



};
QT_END_NAMESPACE


#endif
