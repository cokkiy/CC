#include "staticgraph.h"
#include "staticgraphPrivate.h"


#include <QJsonObject>
#include <QJsonParseError>
#include <string>
#include <QDebug>

#include <QJsonDocument>
#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>



//主类构造函数
staticgraph::staticgraph(QWidget *parent) :
   QWidget(parent), d_ptr(new staticgraphPrivate(this))
{

    m_timer_id = startTimer(500);//隔500毫秒间隔将触发超时事件timerEvent()
}

//主类析构函数
staticgraph::~staticgraph()
{
    killTimer(m_timer_id);

}



//接收传送的数据，并且将值传到私有类中,再进行Json解析
void staticgraph::setTextString(const QString string)
{
    Q_D(staticgraph);

    //从多曲线对话框返回的Json数组对应的字符串string被存在了m_textString中
    m_textString = string;

    //私有类中,将值传到私有类中
    d->setTextString(m_textString);

}


//超时事件处理函数
//虚函数timerEvent（）被重载来实现用户的超时事件处理函数
//在这里主要执行获取数据和更新绘图的操作
void staticgraph::timerEvent(QTimerEvent *event)
{

    //宏展开为：staticgraphPrivate *const d=d_func();
    //d指针为创建的私有类指针，自然可以调用私有类函数
    Q_D(staticgraph);


    d->getData_Test();//获取自己设计的测试数据

    d->getData();//获取从外部net模块接收的数据

    d->update(); //更新绘图
}

//staticgraph中，定义画图事件---事件驱动
void staticgraph::paintEvent(QPaintEvent *event)
{
    Q_D(staticgraph);//创建私有类指针d,d为staticgraphPrivate类对象的指针

    d->setPlot();

    d->update();      //更新绘图
}






//共性部分，只需要整体设置一次

//图元背景颜色
QColor staticgraph::backgroundColor()const
{
    Q_D(const staticgraph);
    return d->backgroundColor();
}
void staticgraph::setbackgroundColor(const QColor bb)
{
    Q_D(staticgraph);
    d->setbackgroundColor(bb);
}

//图元背景图
QPixmap staticgraph::backgroundImage()const
{
    Q_D(const staticgraph);
    return d->backgroundImage();
}
void staticgraph::setbackgroundImage(const QPixmap qq)
{
    Q_D(staticgraph);
    d->setbackgroundImage(qq);
}

//图元是否显示网格
bool staticgraph::getShowGrid() const
{
    Q_D(const staticgraph);
    return d->getShowGrid();
}
void staticgraph::setShowGrid(const bool bShow)
{
    Q_D(staticgraph);
    d->setShowGrid(bShow);
}

//图元是否显示图例
bool staticgraph::getShowLegend() const
{
    Q_D(const staticgraph);
    return d->getShowLegend();
}
void staticgraph::setShowLegend(const bool bShow)
{
    Q_D(staticgraph);
    d->setShowLegend(bShow);
}





//个性部分，需要每条曲线单独设置


//样式显示(曲线按照某种样式进行显示，如某个区间显示某种颜色，某种标记，大小等。)
//增加到"多曲线配置对话框属性"中




