#include "q2wmapPrivate.h"
#include "qwidget.h"

Q2wmapPrivate::Q2wmapPrivate(QWidget*wgt)
{
    m_parent = wgt;

    //参数x、y的默认参数名
    m_paramX = "[10502,19]";
    m_paramY = "[10502,20]";

    //实时曲线的默认颜色和粗细
    m_curveColor = Qt::blue;
    m_curveWidth = 3;

    //理论曲线的默认颜色和粗细
    m_LcurveColor = Qt::gray;
    m_LcurveWidth = 3;

    //坐标轴颜色
    m_AcurveColor = Qt::yellow;

    //创建曲线
    m_plot = new QCustomPlot(m_parent);

    //添加一个曲线图层-理论曲线
    m_plot->addGraph();

    //添加一个曲线图层-实时曲线
    m_plot->addGraph();

    //视窗相对地图左上角的位移量初始均为0，即视窗初始在地图左上角
    m_Pos_x = 0;
    m_Pos_y = 0;

    //图元的高度
    m_height = m_parent->height();

    //图元的宽度
    m_width = m_parent->width();

    //默认显示经纬度网格
    m_bShowGrid = true;

    //默认开启自动漫游
    m_bAutoCruise = true;

    //默认自动漫游类型-跳跃
    m_Cruisetype = true;

    //默认禁止鼠标拖动
    m_bEnableDrag = false;

    //数据接收类
    m_dci = NetComponents::getDataCenter();

    //工作站类
    m_si = NetComponents::getStation();

    //定时器类
    m_ti = NetComponents::getTimer();

    //关联定时器100ms的周期信号，这样可以使用全局的周期信号
    connect(m_ti, SIGNAL(timeout_100()), this, SLOT(timeEvent_100ms()));
}

//读取理论曲线文件，并生成理论曲线
//理论曲线文件的格式为：
//每一行是一个点的经度和纬度的浮点数值，以空格或制表符隔开
//非预期格式将无法获得正确的理论曲线
//如果某个点格式错误，该点将不会加入到理论曲线中
bool Q2wmapPrivate::ReadLLCv()
{
    if(m_strTFile.isNull())
    {
        qWarning()<<"Rika's Warning! Q2wmapPrivate::ReadLLCv() : 读取理论曲线文件失败：未配置文件!";
        return false;
    }

    QFile f(m_strTFile);

    //以读方式打开文件
    if(f.open(QIODevice::ReadOnly))
    {
        //先清空原来的理论曲线
        m_Lx.clear();
        m_Ly.clear();

        //循环读直到读完
        while(true)
        {
            char buf[100] = {0};

            //读一行
            qint64 ReadLen = f.readLine(buf, sizeof(buf));

            //声明经纬度
            double L,B;

            //为了使用sscanf，还需要两个char数组
            char  s1[100] = {0};
            char  s2[100] = {0};

            //从读到的数据中分离L,B
            sscanf(buf, "%s%s", s1,s2);

            //再变成QString，真麻烦
            QString ss1 = s1;
            QString ss2 = s2;

            //总算得到了经纬度的浮点数格式，还得防止读到的东西有问题，导致转换失败
            bool rr_ok1, rr_ok2;

            L = ss1.toDouble(&rr_ok1);
            B = ss2.toDouble(&rr_ok2);

            //如转换失败就不添加点了
            if( (rr_ok1 == true) && (rr_ok2 == true) )
            {
                //添加理论曲线上点的经度向量
                m_Lx.append(L);

                //添加理论曲线上点的纬度向量
                m_Ly.append(B);
            }
            else
            {
                qWarning()<<"Rika's Warning! Q2wmapPrivate::ReadLLCv() : Convert double from QString failed!";
            }

            //读完了
            if(ReadLen == -1)
            {
                //如曲线存在
                if(m_plot&&m_plot->graph(0))
                {
                    //设置理论曲线点集
                    m_plot->graph(0)->setData(m_Lx, m_Ly);
                }

                //跳出循环
                break;
            }
        }

        //关闭文件
        f.close();
    }
    else
    {
        //文件打不开，返回false
        qWarning()<<"Rika's Warning! Q2wmapPrivate::ReadLLCv() : 读取理论曲线文件失败：理论曲线文件打不开!";
        return false;
    }

    //工作结束，返回true
    return true;
}

Q2wmapPrivate::~Q2wmapPrivate()
{
    if(m_plot)
    {
        delete m_plot;
        m_plot = NULL;
    }
}

//设置实时曲线颜色
void Q2wmapPrivate::setColor(const QColor cc)
{
    m_curveColor=cc;
    m_plot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, getColor(),getCurveWidth()));
    update();
}

//设置实时曲线宽度
void Q2wmapPrivate::setCurveWidth(const qint32 cw)
{
    m_curveWidth=cw;
    m_plot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, getColor(),getCurveWidth()));
    update();
}

//设置理论曲线颜色
void Q2wmapPrivate::setLColor(const QColor cc)
{
    m_LcurveColor=cc;
    m_plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, getLColor(),getLCurveWidth()));
    update();
}

//设置坐标轴颜色
void Q2wmapPrivate::setAColor(const QColor cc)
{
    m_AcurveColor=cc;
    update();
}


//设置理论曲线宽度
void Q2wmapPrivate::setLCurveWidth(const qint32 cw)
{
    m_LcurveWidth=cw;
    m_plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, getLColor(),getLCurveWidth()));
    update();
}

//设置地图经度下限
void Q2wmapPrivate::setLLowLimit(const double jl)
{
    m_LLowLimit = jl;
    update();
}

//设置地图经度上限
void Q2wmapPrivate::setLUpLimit(const double ju)
{
    m_LUpLimit = ju;
    update();
}

//设置地图纬度下限
void Q2wmapPrivate::setBLowLimit(const double wl)
{
    m_BLowLimit = wl;
    update();
}

//设置地图纬度上限
void Q2wmapPrivate::setBUpLimit(const double wu)
{
    m_BUpLimit = wu;
    update();
}

//设置地图是否显示网格
void Q2wmapPrivate::setShowGrid(const bool bShowGrid)
{
    m_bShowGrid = bShowGrid;
    update();
}

//设置是否自动漫游
void Q2wmapPrivate::setAutoCruise(const bool bAutoCruise, bool type)
{
    m_bAutoCruise = bAutoCruise;
    m_Cruisetype = type;
    update();
}

//设置地图是否可用鼠标拖动
void Q2wmapPrivate::setEnableDrag(const bool bEnableDrag)
{
    m_bEnableDrag = bEnableDrag;
    update();
}

//设置理论曲线文件路径
void Q2wmapPrivate::setTFile(const QString file)
{
    m_strTFile = file;

    //读取理论曲线文件，并生成理论曲线
    ReadLLCv();

    update();
}

//视窗向地图左方移动
void Q2wmapPrivate::MoveLeft(qint32 pix)
{
    m_Pos_x = m_Pos_x - pix;
}

//视窗向地图右方移动
void Q2wmapPrivate::MoveRight(qint32 pix)
{
    m_Pos_x = m_Pos_x + pix;
}

//视窗向地图上方移动
void Q2wmapPrivate::MoveTop(qint32 pix)
{
    m_Pos_y = m_Pos_y - pix;
}

//视窗向地图下方移动
void Q2wmapPrivate::MoveBottom(qint32 pix)
{
    m_Pos_y = m_Pos_y + pix;
}

void Q2wmapPrivate::setPlot()
{
    //如曲线对象为空，则退出
    if(m_plot==NULL)
    {
        return;
    }

    if(!m_pixmap.isNull())
    {
        //声明一个画刷
        //Qt::IgnoreAspectRatio 图片适应矩形，不考虑原图片的长宽比
        //Qt::KeepAspectRatio 保留原图片的长宽比，图片不能伸出矩形外
        //Qt::KeepAspectRatioByExpanding  保留原图片的长宽比，图片可以伸出矩形外
        //Qt::SmoothTransformation 缩放时平滑
        //QBrush brush(m_pixmap.scaled(m_parent->size(),Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));


        //视窗的移动不能超出左边界
        if(m_Pos_x < 0)
        {
            m_Pos_x = 0;
        }

        //视窗的移动不能超过上边界
        if(m_Pos_y < 0)
        {
            m_Pos_y = 0;
        }

        //视窗的移动不能超过右边界
        if(m_Pos_x + m_parent->rect().width() > m_pixmap.rect().width())
        {
            m_Pos_x = m_pixmap.rect().width() - m_parent->rect().width();
        }

        //视窗的移动不能超过下边界
        if(m_Pos_y + m_parent->rect().height() > m_pixmap.rect().height())
        {
            m_Pos_y = m_pixmap.rect().height() - m_parent->rect().height();
        }

        //设置刷子，刷子装载部分图片。图片从原图左上角加上相对位移为原点，以视窗大小为装载大小
        QBrush brush(m_pixmap.copy(m_Pos_x, m_Pos_y, m_parent->rect().width(), m_parent->rect().height()));

        //设置背景为指定的画刷
        m_plot->setBackground(brush);
    }

    //禁止背景缩放
    m_plot->setBackgroundScaled(true);

    //rect
    QRect rect = m_parent->rect();

    //设置窗口外形
    m_plot->setGeometry(rect);


    //坐标轴为可拖动 QCP::iRangeDrag
    //坐标轴为可缩放 QCP::iRangeZoom
    //曲线及图表可以被选择 QCP::iSelectPlottables
    //m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom| QCP::iSelectPlottables);

    //理论曲线设置绘图方式：以点为圆心，按指定的颜色和粗细绘制
    m_plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, m_LcurveColor,m_LcurveWidth));

    //实时曲线设置绘图方式：以点为圆心，按指定的颜色和粗细绘制
    m_plot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, m_curveColor,m_curveWidth));

    //是否显示网格
    if(m_bShowGrid)
    {
        m_plot->xAxis->grid()->setVisible(true);
        m_plot->xAxis2->grid()->setVisible(true);
        m_plot->yAxis->grid()->setVisible(true);
        m_plot->yAxis2->grid()->setVisible(true);
    }
    else
    {
        m_plot->xAxis->grid()->setVisible(false);
        m_plot->xAxis2->grid()->setVisible(false);
        m_plot->yAxis->grid()->setVisible(false);
        m_plot->yAxis2->grid()->setVisible(false);
    }

    //是否自动分配每两个Tick之间的间距,默认为true
    m_plot->xAxis->setAutoTickStep(1);

    //设置每两个Tick之间的间距(如果setAutoTickStep(true),则不起作用)
    //m_plot->xAxis->setTickStep(5);
    m_plot->xAxis->setTickLength(1,-4);

    //图元的高度
    m_height = m_parent->height();

    //图元的宽度
    m_width = m_parent->width();

    //计算视窗边界
    CalcViewLB(m_ViewLLow, m_ViewLUp, m_ViewBLow, m_ViewBUp);

    //设置X轴上下限(左右经度)
    m_plot->xAxis->setRange(m_ViewLLow, m_ViewLUp);

    //设置Y轴上下限(上下纬度)
    m_plot->yAxis->setRange(m_ViewBLow, m_ViewBUp);

    //设置X轴2上下限(左右经度)
    m_plot->xAxis2->setRange(m_ViewLLow, m_ViewLUp);

    //设置Y轴2上下限(上下纬度)
    m_plot->yAxis2->setRange(m_ViewBLow, m_ViewBUp);

    //设置坐标轴两端到边框的距离为0
    m_plot->xAxis->setOffset(0);
    m_plot->xAxis2->setOffset(0);
    m_plot->yAxis->setOffset(0);
    m_plot->yAxis2->setOffset(0);

    //设置轴线颜色 黄色 (轴线的颜色还不打算允许自由设置)
    m_plot->xAxis->setBasePen(QPen(m_AcurveColor));
    m_plot->xAxis2->setBasePen(QPen(m_AcurveColor));
    m_plot->yAxis->setBasePen(QPen(m_AcurveColor));
    m_plot->yAxis2->setBasePen(QPen(m_AcurveColor));

    //设置刻度颜色 黄色
    m_plot->xAxis->setTickPen(QPen(m_AcurveColor));
    m_plot->xAxis2->setTickPen(QPen(m_AcurveColor));
    m_plot->yAxis->setTickPen(QPen(m_AcurveColor));
    m_plot->yAxis2->setTickPen(QPen(m_AcurveColor));

    //设置子刻度颜色 黄色
    m_plot->xAxis->setSubTickPen(QPen(m_AcurveColor));
    m_plot->xAxis2->setSubTickPen(QPen(m_AcurveColor));
    m_plot->yAxis->setSubTickPen(QPen(m_AcurveColor));
    m_plot->yAxis2->setSubTickPen(QPen(m_AcurveColor));

    //设置刻度文字颜色 黄色
    m_plot->xAxis->setTickLabelColor(m_AcurveColor);
    m_plot->xAxis2->setTickLabelColor(m_AcurveColor);
    m_plot->yAxis->setTickLabelColor(m_AcurveColor);
    m_plot->yAxis2->setTickLabelColor(m_AcurveColor);

    //设置坐标Tick显示位置位于坐标轴内侧
    m_plot->xAxis->setTickLabelSide(QCPAxis::lsInside);
    m_plot->xAxis2->setTickLabelSide(QCPAxis::lsInside);
    m_plot->yAxis->setTickLabelSide(QCPAxis::lsInside);
    m_plot->yAxis2->setTickLabelSide(QCPAxis::lsInside);

    //设置坐标轴2可见
    m_plot->xAxis2->setVisible(1);
    m_plot->yAxis2->setVisible(1);

    if(m_bShowGrid)
    {
        //设置坐标轴不可见
        m_plot->xAxis->setVisible(true);
        m_plot->yAxis->setVisible(true);
        m_plot->xAxis2->setVisible(true);
        m_plot->yAxis2->setVisible(true);
    }
    else
    {
        //设置坐标轴不可见
        m_plot->xAxis->setVisible(false);
        m_plot->yAxis->setVisible(false);
        m_plot->xAxis2->setVisible(false);
        m_plot->yAxis2->setVisible(false);
    }
}

//自动移动视窗，防止曲线当前点绘制到可见区域外
//L=曲线当前点经度
//B=曲线当前点纬度
void Q2wmapPrivate::AutoMoveView(const double L, const double B)
{
    //如曲线存在
    if(m_plot&&m_plot->graph(1))
    {
        //如曲线将要超出视窗边界(曲线经纬度中有一个到达1/20边界值)，则移动视窗中心至曲线当前点
        //首先，计算视窗的经纬度边界值
        CalcViewLB(m_ViewLLow, m_ViewLUp, m_ViewBLow, m_ViewBUp);

        //是否移动
        bool bMoveFlag = false;

        //自动漫游类型为跟随，则一直移动
        if(m_Cruisetype == false)
        {
            bMoveFlag = true;
        }
        else//自动漫游类型为跳跃，则需要计算是否到达视窗边界
        {
            //即将/已经超出左侧
            if( (m_ViewLUp - L) < (m_ViewLUp - m_ViewLLow) * 0.05 )
            {
                bMoveFlag = true;
            }else

            //即将/已经超出右侧
            if( (L - m_ViewLLow) < (m_ViewLUp - m_ViewLLow) * 0.05 )
            {
                bMoveFlag = true;
            }else

            //即将/已经超出顶部
            if( (m_ViewBUp - B) < (m_ViewBUp - m_ViewBLow) * 0.05 )
            {
                bMoveFlag = true;
            }else

            //即将/已经超出底部
            if( (B - m_ViewBLow) < (m_ViewLUp - m_ViewLLow) * 0.05 )
            {
                bMoveFlag = true;
            }
        }

        if(bMoveFlag == true)
        {
            //移动视窗中心至指定经纬度
            MoveToPointOfMapLB(L,B);

            //重绘
            update();
        }
    }
}

//视窗中心移动到指定的位置(地图经纬度)
//L=经度
//B=纬度
void Q2wmapPrivate::MoveToPointOfMapLB(double L, double B)
{
    //先取得当前视窗中心对应地图图像x、y坐标
    qint32 x = m_Pos_x + m_width/2;
    qint32 y = m_Pos_y + m_height/2;

    //指定位置必须在图像经纬度范围内才有意义
    if(L<=m_LUpLimit && L>=m_LLowLimit)
    {
        //每像素的经度值
        double L_per_pix = (m_LUpLimit - m_LLowLimit) /  m_pixmap.width();

        //x=经度差对应的像素个数
        x = qint32( (L - m_LLowLimit) / L_per_pix);
    }

    //指定位置必须在图像经纬度范围内才有意义
    if(B<=m_BUpLimit && B>=m_BLowLimit)
    {
        //每像素的纬度值
        double B_per_pix = (m_BUpLimit - m_BLowLimit) /  m_pixmap.height();

        //y=纬度差对应的像素个数
        y = qint32( (B - m_BUpLimit) / B_per_pix);
    }

    //调用按xy移动函数进行移动
    MoveToPointOfMap(x,y);
}

//视窗中心移动到指定的位置(地图图像的x和y，单位是像素)
void Q2wmapPrivate::MoveToPointOfMap(qint32 x, qint32 y)
{
    //移动距离最远只能到地图边缘
    if(x > (m_pixmap.width() - m_width/2) )
    {
        x = m_pixmap.width() - m_width/2;
    }

    m_Pos_x = x - m_width/2;

    if(y > (m_pixmap.height() - m_height/2) )
    {
        y = m_pixmap.height() - m_height/2;
    }

    m_Pos_y = y - m_height/2;
}

#include<math.h>

//设置曲线的点的x、y值,做为测试,这里用一个正弦曲线
void Q2wmapPrivate::TestgetData()
{
    static qint32 i = 0;
    static qint32 j = 0;
    if(j>5)
    {
        j = 0;
    }
    else
    {
        j++;
        return;
    }

    //经度
    double L = -1*i*0.01 + 100.30616;

    //纬度
    double B = sin(i*0.0001) + 41.2821;

    if(L<m_LLowLimit)
    {
        return;
    }

    //如曲线存在
    if(m_plot&&m_plot->graph(1))
    {
        //设置曲线点集。两种方法效果一样

        /*方法1：
        //添加点的经度向量
        m_x.append(L);
        //添加点的纬度向量
        m_y.append(B);
        //m_plot->graph(1)->setData(m_x, m_y);*/

        /*方法2：*/
        addData(L, B);

        if(m_bAutoCruise)
        {
            //自动移动视窗
            AutoMoveView(L, B);
        }

        i++;
    }
}

//设置曲线的点的x、y值,使用Net类取数据
void Q2wmapPrivate::getData()
{
    //定义两个数组用于存放取到的数据
    QVector<double> vx, vy;

    //准备插入到曲线的点
    double tx, ty;

    //取数据结果 1=成功 -1=失败
    int res = 0;

    //调用getHistoryDatas方法获取数据。这里采用同时取2个参数的方法
    res = m_dci->getHistoryDatas(2, &m_paramX, &vx, &m_paramY, &vy);

    //没有接收到数据或发生错误都会返回-1
    if(res == -1)
    {
        return;
    }

    //把取到的全部x的值存入数组m_x
    foreach (tx, vx)
    {
        m_x.append(tx);
    }

    //把取到的全部y的值存入数组m_y
    foreach (ty, vy)
    {
        m_y.append(ty);
    }

    //把曲线的值设置为刚才的两个数组
    if(m_plot&&m_plot->graph(1))
    {
        m_plot->graph(1)->setData(m_x, m_y);

        if(m_bAutoCruise)
        {
            //自动移动视窗
            AutoMoveView(tx, ty);
        }
    }
}

//添加数据,为曲线添加一个点（x，y）
void Q2wmapPrivate::addData(double x, double y)
{
    if(m_plot&&m_plot->graph(1))
    {
        //为第一个曲线添加数据
        m_plot->graph(1)->addData(x, y);

        //重绘
        m_plot->replot();
    }
}

//更新绘画
void Q2wmapPrivate::update()
{
    m_plot->replot();

    ///!!!!!!!!!以下代码仅为演示使用方法，对图元无实际影响!!!!!!!!!!!-rika

    //查看本机权限(文字形式)
    QString quanxian = m_si->getMyRights();

    //查看本机能否发令
    bool bfaling = m_si->canSendCmd();
}

//计算视窗的经纬度上下限
void Q2wmapPrivate::CalcViewLB(double &VLL, double &VLU, double &VBL, double &VBU)
{
    //每像素的经度值
    double LPerPix = ( m_LUpLimit - m_LLowLimit ) / m_pixmap.width();

    //每像素的纬度值
    double BPerPix = ( m_BUpLimit - m_BLowLimit ) / m_pixmap.height();

    //视窗经度下限
    VLL = m_Pos_x * LPerPix + m_LLowLimit;

    //视窗经度上限
    VLU = ( m_Pos_x + m_width ) * LPerPix + m_LLowLimit;

    //视窗纬度上限
    VBU = m_BUpLimit - ( m_Pos_y * BPerPix );

    //视窗纬度下限
    VBL = m_BUpLimit - ( ( m_Pos_y + m_height) * BPerPix );
}

//鼠标拖动
void Q2wmapPrivate::Drag(qint32 from_x, qint32 from_y, qint32 to_x, qint32 to_y)
{
    if(m_bEnableDrag)
    {
        MoveLeft(to_x - from_x);
        MoveTop(to_y - from_y);
    }
}

//100ms周期事件，用于更新画面
void Q2wmapPrivate::timeEvent_100ms()
{
    getData();
    setPlot();
    update();
}
