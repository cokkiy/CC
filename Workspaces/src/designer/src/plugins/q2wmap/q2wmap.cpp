#include "q2wmap.h"
#include "q2wmapPrivate.h"

Q2wmap::Q2wmap(QWidget *parent) :
    QWidget(parent), d_ptr(new Q2wmapPrivate(this))
{
    //初始鼠标未按下
    m_bLButtonPressed = false;

    //初始菜单的文字内容
    act_ShowLBGrid = new QAction("显示经纬度网格",this);
    act_AutoCruise_flw = new QAction("自动漫游(跟随)",this);
    act_AutoCruise_jmp = new QAction("自动漫游(跳跃)",this);
    act_EnableDrag = new QAction("允许鼠标拖动",this);

    //设置菜单为可选择
    act_ShowLBGrid->setCheckable(true);
    act_AutoCruise_flw->setCheckable(true);
    act_AutoCruise_jmp->setCheckable(true);
    act_EnableDrag->setCheckable(true);

    //设置已开启的菜单为已选择/未选择
    act_ShowLBGrid->setChecked(true);
    act_AutoCruise_flw->setChecked(false);
    act_AutoCruise_jmp->setChecked(true);
    act_EnableDrag->setChecked(false);

    //添加槽函数关联
    connect(act_ShowLBGrid,SIGNAL(triggered(bool)),this,SLOT(ShowLBGrid()));
    connect(act_AutoCruise_flw,SIGNAL(triggered(bool)),this,SLOT(AutoCruise_flw()));
    connect(act_AutoCruise_jmp,SIGNAL(triggered(bool)),this,SLOT(AutoCruise_jmp()));
    connect(act_EnableDrag,SIGNAL(triggered(bool)),this,SLOT(EnableDrag()));
}

Q2wmap::~Q2wmap()
{
    delete act_ShowLBGrid;
    delete act_AutoCruise_flw;
    delete act_AutoCruise_jmp;
    delete act_EnableDrag;
}

//事件-鼠标按下
void Q2wmap::mousePressEvent(QMouseEvent *event)
{
    //左键按下
    if(event->button() == Qt::LeftButton && act_EnableDrag->isChecked())
    {
        //更新状态
        m_bLButtonPressed = true;

        //初始点
        m_x = event->x();
        m_y = event->y();

        //设置光标形状为拖动状态
        this->setCursor(Qt::SizeAllCursor);
    }
}

//事件-鼠标移动
void Q2wmap::mouseMoveEvent(QMouseEvent *event)
{
    //左键按下
    if(m_bLButtonPressed && act_EnableDrag->isChecked())
    {
        //取得事件发生点坐标
        qint32 x = event->x();
        qint32 y = event->y();

        //如果相对初始点发生了移动，则调用移动事件进行处理
        if(x!=m_x || y!=m_y)
        {
            Q_D(Q2wmap);

            d->Drag(m_x,m_y,x,y);
        }

        //更新初始点
        m_x = x;
        m_y = y;
    }
}

//事件-鼠标松开
void Q2wmap::mouseReleaseEvent(QMouseEvent *event)
{
    //左键松开
    if(event->button() == Qt::LeftButton && act_EnableDrag->isChecked())
    {
        //更新状态
        m_bLButtonPressed = false;

        //还原光标形状
        this->setCursor(Qt::ArrowCursor);
    }
}

////定时器事件
//void Q2wmap::timerEvent(QTimerEvent *event)
//{
//    Q_D(Q2wmap);

//    d->getData();
//    d->setPlot();
//    d->update();
//}

//重绘事件
void Q2wmap::paintEvent(QPaintEvent *event)
{
    Q_D(Q2wmap);
    d->setPlot();
    d->update();
    qDebug()<<"Q2wmap::paintEvent(event)";
}

//图元大小改变事件
void Q2wmap::resizeEvent(QResizeEvent *event)
{
    Q_D(Q2wmap);
    d->setPlot();
    d->update();
    qDebug()<<"Q2wmap::resizeEvent(event)";
}

//获取参数x的参数名
QString Q2wmap::getParamX()const
{
    Q_D(const Q2wmap);
    return d->getParamX();
}

//设置参数x的参数名
void Q2wmap::setParamX(const QString px)
{
    Q_D(Q2wmap);
    d->setParamX(px);
}

//获取参数y的参数名
QString Q2wmap::getParamY()const
{
    Q_D(const Q2wmap);
    return d->getParamY();
}

//设置参数y的参数名
void Q2wmap::setParamY(const QString py)
{
    Q_D(Q2wmap);
    d->setParamY(py);
}

//获取曲线颜色
QColor Q2wmap::getColor()const
{
    Q_D(const Q2wmap);
    return d->getColor();
}

//设置曲线颜色
void Q2wmap::setColor(const QColor co)
{
    Q_D(Q2wmap);
    d->setColor(co);
}

//获取曲线宽度
qint32 Q2wmap::getCurveWidth()const
{
    Q_D(const Q2wmap);
    return d->getCurveWidth();
}

//设置曲线宽度
void Q2wmap::setCurveWidth(const qint32 wi)
{
    Q_D(Q2wmap);
    d->setCurveWidth(wi);
}

//获取理论曲线颜色
QColor Q2wmap::getLColor()const
{
    Q_D(const Q2wmap);
    return d->getLColor();
}

//设置理论曲线颜色
void Q2wmap::setLColor(const QColor co)
{
    Q_D(Q2wmap);
    d->setLColor(co);
}

//获取坐标轴颜色
QColor Q2wmap::getAColor()const
{
    Q_D(const Q2wmap);
    return d->getAColor();
}

//设置坐标轴颜色
void Q2wmap::setAColor(const QColor co)
{
    Q_D(Q2wmap);
    d->setAColor(co);
}

//获取理论曲线宽度
qint32 Q2wmap::getLCurveWidth()const
{
    Q_D(const Q2wmap);
    return d->getLCurveWidth();
}

//设置理论曲线宽度
void Q2wmap::setLCurveWidth(const qint32 wi)
{
    Q_D(Q2wmap);
    d->setLCurveWidth(wi);
}

//获取地图经度下限
double Q2wmap::getLLowLimit() const
{
    Q_D(const Q2wmap);
    return d->getLLowLimit();
}

//设置地图经度下限
void Q2wmap::setLLowLimit(const double jl)
{
    Q_D(Q2wmap);
    d->setLLowLimit(jl);
}

//获取地图经度上限
double Q2wmap::getLUpLimit()  const
{
    Q_D(const Q2wmap);
    return d->getLUpLimit();
}

//设置地图经度上限
void Q2wmap::setLUpLimit(const double ju)
{
    Q_D(Q2wmap);
    d->setLUpLimit(ju);
}

//获取地图纬度下限
double Q2wmap::getBLowLimit()  const
{
    Q_D(const Q2wmap);
    return d->getBLowLimit();
}

//设置地图纬度下限
void Q2wmap::setBLowLimit(const double wl)
{
    Q_D(Q2wmap);
    d->setBLowLimit(wl);
}

//获取地图纬度上限
double Q2wmap::getBUpLimit()  const
{
    Q_D(const Q2wmap);
    return d->getBUpLimit();
}

//设置地图纬度上限
void Q2wmap::setBUpLimit(const double wu)
{
    Q_D(Q2wmap);
    d->setBUpLimit(wu);
}

//获取图片
QPixmap Q2wmap::getPixmap()const
{
    Q_D(const Q2wmap);
    return d->getPixmap();
}

//设置图片
void Q2wmap::setPixmap(const QPixmap mp)
{
    Q_D(Q2wmap);
    d->setPixmap(mp);
}

//获取理论曲线文件路径
QString Q2wmap::getTFile() const
{
    Q_D(const Q2wmap);
    return d->getTFile();
}

//设置理论曲线文件路径
void Q2wmap::setTFile(const QString file)
{
    Q_D(Q2wmap);
    d->setTFile(file);
}

//右键菜单事件(包括右键点击空白处和使用键盘上的菜单键)
void Q2wmap::contextMenuEvent(QContextMenuEvent*)
{
    //创建并显示菜单
    QCursor cur=this->cursor();
    QMenu* menu = new QMenu(this);
    QMenu* mFun = menu->addMenu("图元功能");
    mFun->addAction(act_ShowLBGrid);
    mFun->addSeparator();
    mFun->addAction(act_AutoCruise_flw);
    mFun->addAction(act_AutoCruise_jmp);
    mFun->addSeparator();
    mFun->addAction(act_EnableDrag);

    //自定义菜单风格
    //menu->setStyleSheet("QMenu::item{padding-left:28px;padding-right:28px;} QMenu::item::selected{background-color:red;color:#FFFFFF;padding-left:18px;padding-right:18px;}");
    menu->exec(cur.pos());
    delete menu;
}

//显示经纬度网格
void Q2wmap::ShowLBGrid()
{
    Q_D(Q2wmap);

    if(act_ShowLBGrid->isChecked())
    {
        d->setShowGrid(true);
    }
    else
    {
        d->setShowGrid(false);
    }
}

//自动漫游-跟随
void Q2wmap::AutoCruise_flw()
{
    //允许跟随模式则禁止跳跃模式
    if(act_AutoCruise_flw->isChecked())
    {
        act_AutoCruise_jmp->setChecked(false);
        AutoCruise(false);
    }
}

//自动漫游-跳跃
void Q2wmap::AutoCruise_jmp()
{
    //允许跳跃则禁止跟随
    if(act_AutoCruise_jmp)
    {
        act_AutoCruise_flw->setChecked(false);
        AutoCruise(true);
    }
}

//自动漫游 参数type: 0=跟随 1=跳跃
void Q2wmap::AutoCruise(bool type)
{
    Q_D(Q2wmap);

    //自动漫游时，禁止鼠标拖动
    d->setEnableDrag(false);
    act_EnableDrag->setChecked(false);

    d->setAutoCruise(true, type);
}

//允许鼠标拖动
void Q2wmap::EnableDrag()
{
    Q_D(Q2wmap);

    //如果允许鼠标拖动，则关闭自动漫游
    if(act_EnableDrag->isChecked())
    {
        act_AutoCruise_flw->setChecked(false);
        act_AutoCruise_jmp->setChecked(false);
        d->setAutoCruise(false, 0);
        d->setEnableDrag(true);
    }
    else
    {
        d->setEnableDrag(false);
    }
}
