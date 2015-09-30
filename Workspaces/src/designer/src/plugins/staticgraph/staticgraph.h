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
    Q_PROPERTY(QString graphdialog READ textString WRITE setTextString)

    //图元矩形
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)

    //图元的名字
    Q_PROPERTY(QString Name READ getName WRITE setName)

    //背景属性
    //背景色
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setbackgroundColor)
    //背景图片
    Q_PROPERTY(QPixmap backgroundImage READ backgroundImage WRITE setbackgroundImage)


    //2、坐标轴
    //显示网格
    Q_PROPERTY(bool showGrid READ getShowGrid WRITE setShowGrid)
    //显示图例
    Q_PROPERTY(bool showLegend READ getShowLegend WRITE setShowLegend)

    //x轴刻度范围
    Q_PROPERTY(double xUp READ xUp WRITE setXUp)     //曲线的x轴的最大刻度值
    Q_PROPERTY(double xDown READ xDown WRITE setXDown) //曲线的x轴的最小刻度值

    //y轴刻度范围
    Q_PROPERTY(double yUp READ yUp WRITE setYUp)      //曲线的y轴的最大刻度值
    Q_PROPERTY(double yDown READ yDown WRITE setYDown) //曲线的y轴的最小刻度值

    /*******理论曲线****/
    //理论曲线文件路径
    Q_PROPERTY(QString Lgraphfile READ getLgraphTFile WRITE setLgraphTFile)
    //理论曲线颜色
    Q_PROPERTY(QColor lgraphColor READ lgraphColor WRITE setlgraphColor)
    //理论曲线宽度
    Q_PROPERTY(qint32 LgraphWidth READ LgraphWidth WRITE setLgraphWidth)

    /*******实时曲线****/
    Q_PROPERTY(int graphWidth READ graphWidth WRITE setgraphWidth)       //实时曲线的宽度
    Q_PROPERTY(QColor graphColor READ graphColor WRITE setgraphColor)    //实时曲线的颜色

    Q_PROPERTY(QString paramX READ paramX WRITE setParamX)//实时曲线的x变量，用单参数paramX（）输入
    Q_PROPERTY(QString paramY READ paramY WRITE setParamY)//实时曲线的Y变量，用单参数paramY（）输入




public:
    //构造函数
    staticgraph(QWidget *parent = 0);
    //析构函数
    virtual ~staticgraph();

    //多曲线配置
    QString textString() const {return m_textString;}
    void setTextString(const QString string);

   //获取/设置图元的名字
    QString getName() const;
    void setName(const QString stringName);

    //图元背景
    QColor backgroundColor()const;//图元背景颜色
    void setbackgroundColor(const QColor);

    QPixmap backgroundImage()const;//图元背景图片
    void setbackgroundImage(const QPixmap qq);

     //2、坐标轴
    //是否显示网格
    bool getShowGrid() const;
    void setShowGrid(const bool bShow);
    //是否显示图例
    bool getShowLegend() const;
    void setShowLegend(const bool bShow);

    //x轴参数刻度范围
    double xUp()const;//曲线的x轴的最大刻度值
    void setXUp(const double);
    double xDown()const;//曲线的x轴的最小刻度值
    void setXDown(const double);


    //y轴刻度范围
    double yUp()const;//曲线的y轴的最大刻度值
    void setYUp(const double);
    double yDown()const; //曲线的y轴的最小刻度值
    void setYDown(const double);


    //曲线参数x输入
    QString paramX()const;//实时曲线的x变量，用单参数paramX（）输入
    void setParamX(const QString);
    //曲线参数y输入
    QString paramY()const;//实时曲线的Y变量，用单参数paramY（）输入
    void setParamY(const QString);

    //获取/设置理论曲线文件路径
    QString getLgraphTFile() const;
    void setLgraphTFile(const QString file);
    //获取/设置理论曲线颜色
    QColor lgraphColor()const;
    void setlgraphColor(const QColor co);
    //获取/设置理论曲线宽度
    qint32 LgraphWidth()const;
    void setLgraphWidth(const qint32 wi);

    QColor graphColor()const;//实时曲线的颜色
    void setgraphColor(const QColor);
    int graphWidth()const;//实时曲线的宽度
    void setgraphWidth(const int);








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

    //多曲线配置
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
