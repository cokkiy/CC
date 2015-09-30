#include "staticgraph.h"
#include "staticgraphPrivate.h"


//主类构造函数
staticgraph::staticgraph(QWidget *parent) :
   QWidget(parent), d_ptr(new staticgraphPrivate(this))
{
    m_timer_id = startTimer(500);//隔500毫秒间隔将触发超时事件timerEvent()
    qDebug()<<QString("staticgraph startTimer %1 \n").arg(m_timer_id);
}

//主类析构函数
staticgraph::~staticgraph()
{
    killTimer(m_timer_id);
    qDebug()<<QString("staticgraph killTimer %1 \n").arg(m_timer_id);
}


void staticgraph::setTextString(const QString string)
{
    m_textString = string;
//    update();
}



//超时事件处理函数
//虚函数timerEvent（）被重载来实现用户的超时事件处理函数
//在这里主要执行获取数据和更新绘图的操作
void staticgraph::timerEvent(QTimerEvent *event)
{

    //宏展开为：staticgraphPrivate *const d=d_func();
    //d指针为创建的私有类指针，自然可以调用私有类函数
    Q_D(staticgraph);

//    d->getData(2);//获取外部数据
    d->getData(0);//获取理论曲线数据
    d->getData(1);//获取实时曲线数据

    d->update(); //更新绘图
}

//staticgraph中，定义画图事件---事件驱动
void staticgraph::paintEvent(QPaintEvent *event)
{
    Q_D(staticgraph);//创建私有类指针d,d为staticgraphPrivate类对象的指针
    d->setPlot();     //设置理论弹道底图曲线参数
    d->setPlot2();    //设置实时曲线曲线参数
    d->update();      //更新绘图
    qDebug()<<"QCustomPlot::paintEvent(event)";
}


//获取/设置图元的名字
QString staticgraph::getName() const//获取/设置图元的名字
{
    Q_D(const staticgraph);
    return d->getName();
}

void staticgraph::setName(const QString stringName)
{
    Q_D(staticgraph);
    d->setName(stringName);
}

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


   //x轴刻度范围
double staticgraph::xUp()const// x轴最大值
{
    Q_D(const staticgraph);
    return d->xUp();
}
void staticgraph::setXUp(const double xu)
{
    Q_D(staticgraph);
    d->setXUp(xu);
}

double staticgraph::xDown()const// x轴最小值
{
    Q_D(const staticgraph);
    return d->xDown();
}
void staticgraph::setXDown(const double xd)
{
    Q_D(staticgraph);
    d->setXDown(xd);
}


 //y轴刻度范围
double staticgraph::yUp()const// y轴最大值
{
    Q_D(const staticgraph);
    return d->yUp();
}
void staticgraph::setYUp(const double yu)
{
    Q_D(staticgraph);
    d->setYUp(yu);
}

double staticgraph::yDown()const//y轴最小值
{
    Q_D(const staticgraph);
    return d->yDown();
}
void staticgraph::setYDown(const double yd)
{
    Q_D(staticgraph);
    d->setYDown(yd);
}


//实时曲线
QString staticgraph::paramX()const//实时曲线输入单参数数据X变量
{
    Q_D(const staticgraph);
    return d->paramX();
}
void staticgraph::setParamX(const QString px)
{
    Q_D(staticgraph);
    d->setParamX(px);
}

QString staticgraph::paramY()const//实时曲线输入单参数数据Y变量
{
    Q_D(const staticgraph);
    return d->paramY();
}
void staticgraph::setParamY(const QString py)
{
    Q_D(staticgraph);
    d->setParamY(py);
}

QColor staticgraph::graphColor()const//实时曲线颜色
{
    Q_D(const staticgraph);
    return d->graphColor();
}
void staticgraph::setgraphColor(const QColor cc)
{
    Q_D(staticgraph);
    d->setgraphColor(cc);
}

int staticgraph::graphWidth()const//实时曲线宽度
{
    Q_D(const staticgraph);
    return d->graphWidth();
}
void staticgraph::setgraphWidth(const int cw)
{
    Q_D(staticgraph);
    d->setgraphWidth(cw);
}


//理论曲线
QString staticgraph::getLgraphTFile() const//获取理论曲线文件路径
{
    Q_D(const staticgraph);
    return d->getTFile();
}
void staticgraph::setLgraphTFile(const QString file)//设置理论曲线文件路径
{
    Q_D(staticgraph);
    //d->setTFile(file);
}


QColor staticgraph::lgraphColor()const//获取理论曲线颜色
{
    Q_D(const staticgraph);
    return d->LgraphColor();
}
void staticgraph::setlgraphColor(const QColor co)//设置理论曲线颜色
{
    Q_D(staticgraph);
    d->setlgraphColor(co);
}

qint32 staticgraph::LgraphWidth()const//获取理论曲线宽度
{
    Q_D(const staticgraph);
    return d->LgraphWidth();
}
void staticgraph::setLgraphWidth(const qint32 wi)//设置理论曲线宽度
{
    Q_D(staticgraph);
    d->setLgraphWidth(wi);
}

