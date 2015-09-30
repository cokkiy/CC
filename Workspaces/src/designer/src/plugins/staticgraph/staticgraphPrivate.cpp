#include "staticgraphPrivate.h"
#include "qwidget.h"


//私有类staticgraphPrivate构造函数
//功能：曲线初始化，主要是私有类成员变量的初始化
//staticgraphPrivate::staticgraphPrivate(QWidget*wgt):m_dci(this)
staticgraphPrivate::staticgraphPrivate(QWidget*wgt)
{

    //参数初始化

    m_xUp    = 80;   //x轴最大值
    m_xDown  = 0;    //x轴最小值
    m_yUp    = 5;    //y轴最大值
    m_yDown  = -5;   //y轴最小值

    //图元背景色
    //m_backgroundColor.setRgb(0, 0, 255, 255);//蓝色,最后一位0代表透明
    m_backgroundColor.setRgb(0, 0, 255);//蓝色
    m_backgroundColor.setAlpha(255);////代表透明度

    //理论曲线
    m_LgraphColor.setRgb(85,255,127);//初始理论曲线颜色:白色,QColor赋值
    m_LgraphWidth = 4;//初始理论曲线宽度

    //实时曲线
    m_paramX = "cosx"; //初始实时曲线输入单参数数据X变量
    m_paramY = "cosy"; //初始实时曲线输入单参数数据Y变量
    m_graphColor.setRgb(255,0,0);//初始实时曲线颜色:红色,QColor赋值(255,0,0)
    m_graphWidth = 2;//初始实时曲线宽度



    //0：必须要的准备
    //在私有类中，获取QCustomPlot类指针
    parent = wgt;
    m_plot = new QCustomPlot(parent);
//    m_plot2= new QCustomPlot(parent);


    //添加一个曲线图层-理论曲线
    m_plot->addGraph();

    //添加一个曲线图层-实时曲线
    //目标增减：曲线数量添加、删除
   // m_plot->addGraph(m_plot->xAxis,m_plot->yAxis);//xAxis为键值(自变量)，由yAxis作为数据值（因变量，即函数值）
    m_plot->addGraph();//xAxis为键值(自变量)，由yAxis作为数据值（因变量，即函数值）
    //m_plot->removeGraph(0);//0为int index

    //读取理论曲线文件，并生成理论曲线
    if(ReadLLCv() == false)
    {
        qWarning()<<"zjb's Warning! staticgraphPrivate::staticgraphPrivate() : ReadLLCv() function return error!";
    }
    //m_dci = NetComponents::getDataCenter();
    //数据接收类
    m_dci_x = NetComponents::getDataCenter();
    m_dci_y = NetComponents::getDataCenter();
 }

//私有类析构函数
staticgraphPrivate::~staticgraphPrivate()
{
    //画图基本工具类指针释放，QCustomPlot类继承自QWidget类
    if(m_plot)
    {
        delete m_plot;
        m_plot = NULL;
    }
//    if(m_plot2)
//    {
//        delete m_plot2;
//        m_plot2 = NULL;
//    }
    //数据接收类指针释放
    delete m_dci_x;
    delete m_dci_y;
}


//理论曲线
//读取理论曲线文件，并生成理论曲线
//理论曲线文件的格式为：
//每一行是一个点theory_X和theory_Y的浮点数值，以空格或制表符隔开
//非预期格式将无法获得正确的理论曲线
//如果某个点格式错误，该点将不会加入到理论曲线中
bool staticgraphPrivate::ReadLLCv()
{
    if(m_strTFile.isNull())
    {
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
            char buf[200] = {0};

            //读一行
            qint64 ReadLen = f.readLine(buf, sizeof(buf));

            //声明
            double theory_X,theory_Y;

            //为了使用sscanf，还需要两个char数组
            char  s1[200] = {0};
            char  s2[200] = {0};

            //从读到的数据中分离theory_X,theory_Y
            sscanf(buf, "%s%s", s1,s2);

            //再变成QString
            QString ss1 = s1;
            QString ss2 = s2;

            //得到了theory_X,theory_Y的浮点数格式，还得防止读到的东西有问题，导致转换失败
            bool rr_ok1, rr_ok2;

            theory_X = ss1.toDouble(&rr_ok1);
            theory_Y = ss2.toDouble(&rr_ok2);

            //如转换失败就不添加点
            if( (rr_ok1 == true) && (rr_ok2 == true) )
            {
                //添加理论曲线上点的theory_X向量
                m_Lx.append(theory_X);

                //添加理论曲线上点的theory_Y向量
                m_Ly.append(theory_Y);
            }
            else
            {
                qWarning()<<"zjb's Warning! staticgraphPrivate::staticgraphPrivate() : ReadLLCv() function return error!";
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
        return false;
    }

    //工作结束，返回true
    return true;
}

//设置图元背景颜色
void staticgraphPrivate::setbackgroundColor(const QColor bb)//图元背景颜色
{
    m_backgroundColor=bb;
}
void staticgraphPrivate::resetbackgroundColor()// 重设背景色
{
    //直接设置背景色
   // m_backgroundColor.setRgb(0, 0, 255);//蓝色
    m_backgroundColor.setAlpha(255);//代表透明度
    m_plot->setBackground(backgroundColor());
}

//设置图元背景图片
void staticgraphPrivate::setbackgroundImage(const QPixmap qq)//图元背景图片
{
    m_backgroundImage=qq;
}
void staticgraphPrivate::resetbackgroundImage()// 重设背景图片
{
    //直接设置背景图片
    m_plot->setBackground(backgroundImage());

}


//轴
void staticgraphPrivate::setShowGrid(const bool bShowGrid)//设置图元是否显示网格
{
    m_bShowGrid = bShowGrid;
    update();
}

void staticgraphPrivate::setShowLegend(const bool bShowLegend)//设置图元是否显示图例
{
    m_bShowLegend = bShowLegend;
    update();
}

void staticgraphPrivate::setXUp(const double xu)//x轴范围
{
    m_xUp = xu;
    m_plot->xAxis->setRangeUpper(xUp());
    update();
}
void staticgraphPrivate::setXDown(const double xd)
{
    m_xDown = xd;
    m_plot->xAxis->setRangeLower(xDown());
    update();
}


void staticgraphPrivate::setYUp(const double yu)//y轴范围
{
    m_yUp = yu;
    m_plot->yAxis->setRangeUpper(yUp());
    update();
}
void staticgraphPrivate::setYDown(const double yd)
{
    m_yDown = yd;
    m_plot->yAxis->setRangeLower(yDown());
    update();
}


void staticgraphPrivate::setlgraphColor(const QColor lc)//设置理论曲线颜色
{
    m_LgraphColor=lc;
    m_plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, LgraphColor(),LgraphWidth()));
    update();
}

void staticgraphPrivate::setLgraphWidth(const qint32 lw)//设置理论曲线宽度
{
    m_LgraphWidth=lw;
    m_plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, LgraphColor(),LgraphWidth()));
    update();
}



void staticgraphPrivate::setgraphColor(const QColor cc)//实时曲线颜色
{
    m_graphColor=cc;
    m_plot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, graphColor(),graphWidth()));
    update();
}

void staticgraphPrivate::setgraphWidth(const int cw)//实时曲线宽度
{
    m_graphWidth=cw;
    m_plot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, graphColor(),graphWidth()));
    update();
}

//设置画图参数
//设置共性特征,主要是理论弹道底图曲线及坐标轴等
void staticgraphPrivate::setPlot()
{
    //设置图元区域
    m_plot->setGeometry(parent->rect());//视图矩形区

    //设置曲线(控件)交互性:可拖动\可缩放\图表被选定
    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables); //曲线交互性


    //设置曲线可视性:可视
    m_plot->setVisible(true);    //可视

    //2、坐标轴参数
    /*
     已实现包括：网格显示、图例显示、x(或y)轴轴标签(包括x(或y)轴单位)、x(或y)轴的范围、x(或y)轴大的刻度的间隔值、x(或y)轴文本偏移
    坐标轴轴线颜色、坐标轴宽度、刻度颜色、刻度文字颜色、x(或y)轴刻度小线段显示在坐标轴的哪一边、坐标轴的可见性
    */

    //设置网格显示:可视
    m_bShowGrid=true;//网格可视
    if(m_bShowGrid)//网格是否可视
    {
        m_plot->xAxis->grid()->setVisible(true);//设置网格可视
        m_plot->yAxis->grid()->setVisible(true);
    }
    else
    {
        m_plot->xAxis->grid()->setVisible(false);//网格不可视
        m_plot->yAxis->grid()->setVisible(false);
    }


    //设置曲线图例
    //设置图例显示:可视
     m_bShowLegend=true;//曲线图例可视
     m_plot->legend->setVisible(m_bShowLegend);//true可视,false不可视

     //设置自动增加图表到图例
    m_plot->setAutoAddPlottableToLegend(true);

    QFont legendfont;
    legendfont.setPointSize(10);
    //设置图例文本字体:10号
    m_plot->legend->setFont(legendfont);

    //可选择性
    //选中图层1---理论弹道底图曲线
    //    m_plot->graph(0)->setSelected(Lgraphselected);
    //    if(Lgraphselected)
    //    {

    //        m_plot->legend->setTextColor(legendColor);//设置图例文本颜色

    //参数：QCPLegend::spItems,QCPLegend::spLegendBox,spNone
    //        m_plot->legend->setSelectableParts(QCPLegend::spItems);//设置图例项目可选择
    //        m_plot->legend->setSelectableParts(QCPLegend::spLegendBox);//设置图例框可选择

    //参数：QCPAxis::spNone,QCPAxis::spAxis,QCPAxis::spTickLabels,
    //     QCPAxis::spAxisLabel
    //  m_plot->legend->setSelectableParts();
    //  m_plot->legend->setSelectedTextColor();

    //        m_plot->graph(0)->setSelectedPen();
    //        m_plot->graph(0)->setSelectedBrush();
    //        m_plot->xAxis->setSelectedLabelFont();
    //        m_plot->xAxis->setSelectedBasePen();
    //        m_plot->legend->item(0);
    //}

    QColor LgraphlegendColor;
    LgraphlegendColor.setRgb(85,255,127);
    //选中图层1---理论弹道底图曲线
    bool Lgraphselected;
    Lgraphselected=true;
    m_plot->graph(0)->setSelected(Lgraphselected);
    if(Lgraphselected)
    {

        m_plot->legend->setTextColor(LgraphlegendColor);//设置图例文本颜色

        //参数：QCPLegend::spItems,QCPLegend::spLegendBox,spNone
        m_plot->legend->setSelectableParts(QCPLegend::spItems);//设置图例项目可选择
        m_plot->legend->setSelectableParts(QCPLegend::spLegendBox);//设置图例框可选择
    }



    QBrush legendbrush;
    legendbrush.setColor(QColor(0,0,255,0));
    //设置图例kuangtianchong颜色:blue
    m_plot->legend->setBrush(legendbrush);//设置图例kuangtianchong颜色 红色

    QPen legendBorderpen;
    legendBorderpen.setColor(QColor(255,255,255,255));
    legendBorderpen.setWidthF(0.5);
    //设置图例kuangtianchong颜色:blue
    m_plot->legend->setBorderPen(legendBorderpen);//设置图例kuang画笔 白色

    Qt::Alignment legendAlignment;
    legendAlignment=(Qt::AlignTop|Qt::AlignRight);
    //设置图例文本对齐方式:放在右上角，采用右对齐方式。
    m_plot->axisRect()->insetLayout()->setInsetAlignment(0,legendAlignment);//图例放置于右上角



    //设置曲线图例轴标签
    //设置x(或y)轴轴标签名称(包括x(或y)轴单位)
    m_plot->xAxis->setLabel(tr("X"));//x轴标签x
    m_plot->yAxis->setLabel(tr("Y"));//y轴标签y

    QFont LabelFont;
    LabelFont.setPointSize(13);
    //设置标签文字字体:13号
    m_plot->xAxis->setLabelFont(LabelFont);//设置标签文字字体
    m_plot->yAxis->setLabelFont(LabelFont);//设置标签文字字体

    QColor LabelColor;
    LabelColor.setRgb(255,0,0);
    //设置标签文字颜色:红色
    m_plot->xAxis->setLabelColor(LabelColor);//设置标签颜色 红色
    m_plot->yAxis->setLabelColor(LabelColor);//设置标签颜色  红色


    //设置x(或y)轴的范围
    m_plot->xAxis->setRange(xDown(), xUp());//x轴范围
    m_plot->yAxis->setRange(yDown(), yUp());//y轴范围


     qint32 TickStep;
     TickStep=1;
     //设置x(或y)轴的刻度台阶
     //void setTickStep(double step); //设置刻度台阶为2
     m_plot->xAxis->setTickStep(TickStep);//设置x(或y)轴的刻度台阶为2
     m_plot->yAxis->setTickStep(TickStep);


     qint32 TickLengthinside,TickLengthinoutside;
     TickLengthinside=8;
     TickLengthinoutside=-2;
     //void setTickLength(int inside, int outside=0);//设置刻度小线段长度:向里伸出多少，向外伸出多少
     m_plot->xAxis->setTickLength(TickLengthinside,TickLengthinoutside);
     m_plot->yAxis->setTickLength(TickLengthinside,TickLengthinoutside);



     qint32 Offset;
     Offset=1;
     //设置x(或y)轴文本偏移
     //设置坐标轴两端到边框的距离为1
     m_plot->xAxis->setOffset(Offset);
     m_plot->yAxis->setOffset(Offset);


     QPen penAxis;
     penAxis.setColor(QColor(255,255,255));
     penAxis.setWidthF(2);
     //设置坐标轴   轴线颜色:白色,轴线宽度:2
     m_plot->xAxis->setBasePen(penAxis);
     m_plot->yAxis->setBasePen(penAxis);
     m_plot->xAxis2->setBasePen(penAxis);
     m_plot->yAxis2->setBasePen(penAxis);


     QPen penTick;
     penTick.setColor(QColor(255,255,255));
     penTick.setWidthF(1);
     //设置刻度  颜色:白色,宽度:1
     m_plot->xAxis->setTickPen(penTick);
     m_plot->yAxis->setTickPen(penTick);


     QPen penSubTick;
     penSubTick.setColor(QColor(255,255,255));
     penSubTick.setWidthF(1);
     //设置子刻度 颜色:白色,宽度:1
     m_plot->xAxis->setSubTickPen(penSubTick);
     m_plot->yAxis->setSubTickPen(penSubTick);


     QFont TickLabelFont;
     TickLabelFont.setPointSize(9);
     //设置刻度文字字体：9号
     m_plot->xAxis->setTickLabelFont(TickLabelFont);

     QColor TickLabelColor;
     TickLabelColor.setRgb(255,0,0);
     //设置刻度文字颜色:红色
     m_plot->xAxis->setTickLabelColor(TickLabelColor);
     m_plot->yAxis->setTickLabelColor(TickLabelColor);



     //设置x(或y)轴刻度小线段标签显示在坐标轴的哪一边
     //设置坐标Tick显示位置位于坐标轴外侧
     m_plot->xAxis->setTickLabelSide(QCPAxis::lsOutside);//位于坐标轴内侧为QCPAxis::lsInside
     m_plot->yAxis->setTickLabelSide(QCPAxis::lsOutside);


     qint32 Axis1Visible,Axis2Visible;
     Axis1Visible=1;
     Axis2Visible=1;
     //设置所有坐标轴的可见性
     //设置坐标轴1可见
     m_plot->xAxis->setVisible(Axis1Visible);//设1为可见,0为不可见
     m_plot->yAxis->setVisible(Axis1Visible);
     //设置坐标轴2可见
     m_plot->xAxis2->setVisible(Axis2Visible);//设置坐标轴2轴线可见
     m_plot->yAxis2->setVisible(Axis2Visible);//设置坐标轴2轴线可见
     m_plot->xAxis2->setTicks(0);//但隐藏坐标轴2刻度标示等
     m_plot->yAxis2->setTicks(0);//但隐藏坐标轴2刻度标示等

     /********理论曲线******/
     /*
      已实现包括：曲线风格、曲线画笔、曲线形状、曲线名称、
                支持特殊显示(如:直线连线、钻石形标记等)、是否显示某条曲线、

     */

//     void setSize(double size);//设置散点大小
//     void setShape(ScatterShape shape);//设置散点形状
//     void setPen(const QPen &pen);//设置画笔
//     void setBrush(const QBrush &brush);//设置画刷
//     void setPixmap(const QPixmap &pixmap);//设置象素映射
//     void setCustomPath(const QPainterPath &customPath);//设置规定的路径

     //设置曲线风格:直线/散点
     QCPGraph::LineStyle GraphStyle;
     //设置特殊显示
     GraphStyle=QCPGraph::lsNone;

     if(!(GraphStyle==QCPGraph::lsNone))//直线连线
     {
         //LineStyle:lsNone,lsLine,lsStepLeft,lsStepRight,lsStepCenter,lsImpulse
         GraphStyle=QCPGraph::lsLine;
         m_plot->graph(0)->setLineStyle(GraphStyle);

         QCPScatterStyle linescatterStyle;//设置直线散点形式
         //散点大小
         linescatterStyle.setSize(LgraphWidth());
         //散点画笔
         QPen ScatterStylepen;
         ScatterStylepen.setColor(LgraphColor());
         ScatterStylepen.setWidth(1);
         linescatterStyle.setPen(ScatterStylepen);
         //散点画刷
         QBrush ScatterStylebrush;
         ScatterStylebrush.setColor(LgraphColor());
         linescatterStyle.setBrush(ScatterStylebrush);

         m_plot->graph(0)->setScatterStyle(linescatterStyle);

     }
     else//散点
     {
         GraphStyle=QCPGraph::lsNone;

         QCPScatterStyle scatterscatterStyle;//设置点散点形式
         //散点大小
         scatterscatterStyle.setSize(LgraphWidth());
         //散点画笔
         QPen ScatterStylepen;
         ScatterStylepen.setColor(LgraphColor());
         ScatterStylepen.setWidth(1);
         scatterscatterStyle.setPen(ScatterStylepen);
         //散点画刷
         QBrush ScatterStylebrush;
         ScatterStylebrush.setColor(LgraphColor());
         scatterscatterStyle.setBrush(ScatterStylebrush);
         //散点形状
         QCPScatterStyle::ScatterShape scatterShape;
         scatterShape=QCPScatterStyle::ssDiamond;//散点形状:QCPScatterStyle::ssDiamond
         //scatterShape:ssNone,ssDot,ssCross,ssPlus,ssCircle,ssDisc,ssSquare,ssDiamond,
         //     ssStar,ssTriangle,,ssTriangleInverted,,ssCrossSquare,ssPlusSquare,ssCrossCircle,
         //     ssPlusCircle,ssPeace,ssPixmap,ssCustom
         scatterscatterStyle.setShape(scatterShape);

         m_plot->graph(0)->setScatterStyle(scatterscatterStyle);
     }



     //显示曲线1
     m_plot->graph(0)->setVisible(true);

     //设置曲线名称
     m_plot->graph(0)->setName(tr("X-Y曲线"));//显示在图例上的名称


     /*****需求******
     //1 目标增减
     //2 曲线缓冲区：60000

     /*****其他需求*******/
//     （1）staticgraph工程剩余需求：
//          //1 原始坐标体系。  (右键可选择点击：在实时模式进行了拖动、缩放之后，右键可以选择单击回复到一开始的定制画面。)
//          //2 自适应数据。   （右键可选择点击：找出x、y数据各自的最大值最小值，把这个作出的矩形放到画面中间）
//          //3 T0获取。      (T0获取并根据T0(靶弹，未定义，拦截弹）计算相对时弹道及清屏。)
//          //4 样式显示。     (曲线按照某种样式进行显示，如某个区间显示某种颜色，某种标记，大小等。)(增加到属性中)
//          //5 显示模式。     (固定显示模式（在staticgraph工程）、滚动显示模式（在rollinggraph工程）。)
//          //6 界面修改。    （双击修改界面部分简单参数配置的功能。）
//     （2）应用QTabWidget类实现多曲线配置中配置相关函数的添加。
//     （3）实现多曲线配置对话框的配置数据的封装（用类或结构体）及转化（json的序列化与反序列化），并实现向主类的数据传输。




}

void staticgraphPrivate::setPlot2()//实时曲线
{

    //曲线参数

    /*********实时曲线********/


    //显示曲线2
    m_plot->graph(1)->setVisible(true);

    //设置曲线画笔：曲线颜色、宽度、虚实
    m_plot->graph(1)->setLineStyle(QCPGraph::lsNone);

     //设置绘图方式：以圆心点为散点形状，按指定的颜色和粗细绘制
     m_plot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, graphColor(),graphWidth()));

     QColor graphlegendColor;
     graphlegendColor.setRgb(85,255,127);
     //选中图层2---实时曲线
     bool graphselected;
     graphselected=true;
     m_plot->graph(1)->setSelected(graphselected);
     if(graphselected)
     {

         m_plot->legend->setTextColor(graphlegendColor);//设置图例文本颜色

         //参数：QCPLegend::spItems,QCPLegend::spLegendBox,spNone
         m_plot->legend->setSelectableParts(QCPLegend::spItems);//设置图例项目可选择
         m_plot->legend->setSelectableParts(QCPLegend::spLegendBox);//设置图例框可选择
     }

     //设置曲线名称
     m_plot->graph(1)->setName(tr("T-H 曲线"));//显示在图例上的名称

}



//更新重绘
void staticgraphPrivate::update()
{
    //重设背景颜色
    resetbackgroundColor();
    //重设背景图片
    resetbackgroundImage();
    //再画
    m_plot->replot();


}

//获取外部数据
//graphdatatype 表示输入的曲线（数据）类型:
//0---理论曲线
//1---实时曲线
//2---实时曲线(从外部net模块接收)
void staticgraphPrivate::getData(qint32 graphdatatype)
{
    if(graphdatatype==0)//0---理论曲线
    {
        QVector<double>Lx(200),Ly(200);
        for(int i=0;i<200;++i)
        {

            Lx[i] = i;
            Ly[i] =2*sin((((double)Lx[i])/20.0)*2*3.1415);
        }
        //(variable, container)
        foreach (double tLx, Lx) {
            m_Lx.append(tLx);//在容器m_x末尾添加tx
            qDebug()<<QString("insert x %1 \n").arg(tLx);//tx替代前面的1
        }
        foreach (double tLy, Ly) {
            m_Ly.append(tLy);//在容器m_y末尾添加ty
            qDebug()<<QString("insert y %1 \n").arg(tLy);//ty替代前面的1
        }
        //QCustomPlot指针(对象)存在且已经创建了一个曲线1----实时曲线
        if(m_plot&&m_plot->graph(0)){
            m_plot->graph(0)->setData(m_Lx, m_Ly);//键值，值
            m_plot->replot();//绘制曲线
        }
    }

    if(graphdatatype==1)//1---实时曲线
    {

        QVector<double>x(200),y(200);
        for(int i=0;i<200;++i)
        {

            x[i] = i;
            y[i] =sin((((double)x[i])/50.0)*2*3.1415);
        }
        //(variable, container)
        foreach (double tx, x) {
            m_x.append(tx);//在容器m_x末尾添加tx
            qDebug()<<QString("insert x %1 \n").arg(tx);//tx替代前面的1
        }
        foreach (double ty, y) {
            m_y.append(ty);//在容器m_y末尾添加ty
            qDebug()<<QString("insert y %1 \n").arg(ty);//ty替代前面的1
        }
        //QCustomPlot指针(对象)存在且已经创建了一个曲线1----实时曲线
        if(m_plot&&m_plot->graph(1)){

            m_plot->graph(1)->setData(m_x, m_y);//键值，值
            m_plot->replot();//绘制曲线
        }
    }

/*
    if(graphdatatype==2)//2---实时曲线(从外部net模块接收)
    {
        QVector<double> x, y;
        m_dci_x->getHistoryData(paramX(),x);
        m_dci_y->getHistoryData(paramY(),y);


        foreach (double tx, x) {
            m_x.append(tx);//在容器m_x末尾添加tx
            qDebug()<<QString("insert x %1 \n").arg(tx);//tx替代前面的1
        }
        foreach (double ty, y) {
            m_y.append(ty);//在容器m_y末尾添加ty
            qDebug()<<QString("insert y %1 \n").arg(ty);//ty替代前面的1
        }
        //QCustomPlot指针(对象)存在且已经创建了一个曲线2----实时曲线(从外部net模块接收)
        if(m_plot&&m_plot->graph(0)){
            m_plot->graph(0)->setData(m_x, m_y);//键值，值
            m_plot->replot();//绘制曲线
        }
    }
    */



}
