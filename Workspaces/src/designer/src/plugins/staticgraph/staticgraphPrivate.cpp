#include "staticgraphPrivate.h"
#include "qwidget.h"

//私有类staticgraphPrivate构造函数
//功能：曲线初始化，主要是私有类成员变量的初始化
//staticgraphPrivate::staticgraphPrivate(QWidget*wgt):m_dci(this)
staticgraphPrivate::staticgraphPrivate(QWidget*wgt)
{

    //0：必须要的准备
    //在私有类中，获取QCustomPlot类指针
    parent = wgt;
    m_plot = new QCustomPlot(parent);


    //共性部分

    //图元背景色
    //m_backgroundColor.setRgb(0, 0, 255, 255);//蓝色,最后一位0代表透明
    m_backgroundColor.setRgb(0, 0, 255);//蓝色
    m_backgroundColor.setAlpha(255);//代表透明度
//    //理论曲线
//    m_LgraphColor.setRgb(85,255,127);//初始理论曲线颜色:白色,QColor赋值
//    m_LgraphWidth = 4;//初始理论曲线宽度

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

    //数据接收类指针释放
    delete m_dci_x;
    delete m_dci_y;
}


//Json数据解析
void staticgraphPrivate::parseJsonData()
{
    QString str = textString();//将多曲线对话框配置的字符串存入str中，方便取出来进行解析

    //第1步，将传送过来的json数据，先转化到一个QJsonDocument文件doc1中
    QJsonParseError jerr;//解析报错
    QJsonDocument doc1;
    //str.toUtf8(),使之具备传送中文字符的能力
    doc1= QJsonDocument::fromJson(str.toUtf8(), &jerr);//创建一个QJsonDocument从字符串str转化而来

    if(jerr.error == QJsonParseError::NoError)//如果解析成功
    {
        //第2步，判断传送过来的json数据的最外层的数据类型
        //一般有4个判断的函数：
        //bool isArray() const 判断是否json数组（每个数组元素可能就是一个对象，一个对象可包含多个属性）
        //bool isEmpty() const 判断是否空的QJsonDocument文件
        //bool isNull() const 判断是否不存在的QJsonDocument文件
        //bool isObject() const 判断是否仅是单个对象，一个对象可包含多个属性
        if(doc1.isEmpty())//创建的QJsonDocument是空的
        {
            return;
        }
        if(doc1.isNull())//创建的QJsonDocument是Null的
        {
            return;
        }

        if(!(doc1.isArray()))
        {
            qDebug()<<" not a json array,must be json array!!!";
            return;
        }

        //清空之前的曲线
        for(int i=0; i<m_vctgraphObj.size(); i++)
        {
            m_vctgraphObj[i].m_pgraph->clearData();
            m_vctgraphObj[i].m_pLgraph->clearData();
        }

        m_vctgraphObj.clear();

        if(doc1.isArray())
        {

            //第3步，知道是json array后，用下列三个函数将传送来的json array数据转化为相应的json array数据
            //QJsonArray array() const 转化为json数组
            //QJsonObject object() const 转化为json单个对象
            //QVariant toVariant() const 转化为map
            QJsonArray GraphArray;//Json数组
            //取得数组
            GraphArray=doc1.array();


            //第4步，用QJsonArray的函数或迭代器遍历取得想要值
            //若是QJsonObject,则用QJsonObject的函数或迭代器遍历取得想要值
            qint32 arraysize;
            arraysize=GraphArray.size();

            for(qint32 i=0;i<arraysize;i++)
            {
                //取得一个数组的值
                QJsonValue value = GraphArray.at(i);

                //如果值是一个对象
                if(value.isObject())
                {
                    //转换成json对象
                    QJsonObject name = value.toObject();

                    //下面开始挨个解析
                    PlotGraphObject pgj;

                    //x轴最大值
                    pgj.m_Xmax_plot = name["Xmax"].toDouble();
                    //x轴最小值
                    pgj.m_Xmin_plot = name["Xmin"].toDouble();
                    //y轴最大值
                    pgj.m_Ymax_plot = name["Ymax"].toDouble();
                    //y轴最小值
                    pgj.m_Ymin_plot = name["Xmin"].toDouble();
                    //x轴标签文本偏移
                    pgj.m_Xoffset_plot =name["Xoffset"].toInt();
                    //y轴标签文本偏移
                    pgj.m_Yoffset_plot =name["Yoffset"].toInt();
                    //x轴标签
                    pgj.m_XAxisLabel_plot =name["XAxisLabel"].toString();
                    //y轴标签
                    pgj.m_YAxisLabel_plot =name["YAxisLabel"].toString();
                    //x轴刻度位置
                    pgj.m_Scaleplace_x_plot =name["Scaleplace_x"].toString();
                    //y轴刻度位置
                    pgj.m_Scaleplace_y_plot =name["Scaleplace_y"].toString();
                    //是否显示x轴
                    pgj.m_XAxisdisplay_plot =name["XAxisdisplay"].toBool();
                    //是否显示y轴
                    pgj.m_YAxisdisplay_plot =name["YAxisdisplay"].toBool();

                    //实时曲线名称
                    pgj.m_GraphName_plot = name["GraphName"].toString();
                    //实时曲线宽度
                    pgj.m_GraphWidth_plot =name["GraphWidth"].toInt();
                    //实时曲线缓冲区大小
                    pgj.m_GraphBuffer_plot = name["GraphBuffer"].toDouble();
                    //理论曲线颜色
                    pgj.m_strgraphColor_plot =name["strgraphColor"].toString();

                    //理论曲线名称
                    pgj.m_LGraphName_plot = name["GraphName"].toString();
                    //理论曲线宽度
                    pgj.m_LGraphWidth_plot = name["LGraphWidth"].toInt();
                    //理论曲线缓冲区大小
                    pgj.m_LGraphBuffer_plot = name["LGraphBuffer"].toDouble();
                    //理论曲线颜色
                    pgj.m_strLgraphColor_plot = name["strLgraphColor"].toString();

                    //实时曲线x参数
                    pgj.m_xParam_plot = name["xParam"].toString();
                    //实时曲线y参数
                    pgj.m_yParam_plot = name["yParam"].toString();

                    //理论曲线路径
                    pgj.m_Lgraphfile_plot = name["Lgraphfile"].toString();

                    //添加一个曲线图层-理论曲线
                    pgj.m_pLgraph = m_plot->addGraph();
                    //添加一个曲线图层-实时曲线
                    pgj.m_pgraph = m_plot->addGraph();

                    //读取理论弹道
                    pgj.ReadLLGraph();

                    m_vctgraphObj.push_back(pgj);

                }

             }

           }

   }

}


//理论曲线
//读取理论曲线文件，并生成理论曲线
//理论曲线文件的格式为：
//每一行是一个点value_X和value_Y的浮点数值，以空格或制表符隔开
//非预期格式将无法获得正确的理论曲线
//如果某个点格式错误，该点将不会加入到理论曲线中
bool PlotGraphObject::ReadLLGraph()
{
    if(m_Lgraphfile_plot.isNull())
    {
        qWarning()<<"zjb's Warning! Q2wmapObject::ReadLLgraph() : 读取理论曲线文件失败：未配置文件!";
        return false;
    }

    QFile f(m_Lgraphfile_plot);

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

            //X,Y
            double value_X,value_Y;

            //为了使用sscanf，还需要两个char数组
            char  s1[100] = {0};
            char  s2[100] = {0};

            //从读到的数据中分离value_X,value_Y
            sscanf(buf, "%s%s", s1,s2);

            //再变成QString，真麻烦
            QString ss1 = s1;
            QString ss2 = s2;

            //总算得到了浮点数格式，还得防止读到的东西有问题，导致转换失败
            bool rr_ok1, rr_ok2;

            value_X = ss1.toDouble(&rr_ok1);
            value_Y = ss2.toDouble(&rr_ok2);

            //如转换失败就不添加点了
            if( (rr_ok1 == true) && (rr_ok2 == true) )
            {
                //添加理论曲线的点
                m_Lx.push_back(value_X);
                m_Ly.push_back(value_Y);
            }
            else
            {
                qWarning()<<"zjb's Warning! PlotGraphObject::ReadLLgraph() : Convert double from QString failed!";
            }

            //读完了
            if(ReadLen == -1)
            {
                //跳出循环
                break;
            }
        }

        //设置理论曲线点集
        m_pLgraph->setData(m_Lx, m_Ly);

        //关闭文件
        f.close();
    }
    else
    {
        //文件打不开，返回false
        qWarning()<<"zjb's Warning! PlotGraphObject::ReadLLgraph() : 读取理论曲线文件失败：理论曲线文件打不开!";
        return false;
    }

    //工作结束，返回true
    return true;
}


//共性部分

//设置图元背景颜色
void staticgraphPrivate::setbackgroundColor(const QColor bb)
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

//设置图元是否显示网格
void staticgraphPrivate::setShowGrid(const bool bShowGrid)//设置图元是否显示网格
{
    m_bShowGrid = bShowGrid;

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

    update();
}

//设置图元是否显示图例
void staticgraphPrivate::setShowLegend(const bool bShowLegend)//设置图元是否显示图例
{
    m_bShowLegend = bShowLegend;

    m_plot->legend->setVisible(m_bShowLegend);//true可视,false不可视

    update();
}



//个性部分

void staticgraphPrivate::setTextString(const QString string)
{
    if(m_plot == NULL)
    {
        return;
    }

    m_textString=string;
    //    QString graphstr=string;
    //私有类中,把多曲线配置对话框传过来的数据解析出来
    parseJsonData();//得到赋值的结构体变量dialogdata

    setPlot();

    update();
}



//void setPlot()//全新的画图参数配置
void staticgraphPrivate::setPlot()
{
    //如曲线对象为空，则退出
    if(m_plot==NULL)
    {
        return;
    }

    //共性部分，在主类属性栏进行设置

    //设置图元区域
    m_plot->setGeometry(parent->rect());//视图矩形区

    //设置曲线(控件)交互性:可拖动\可缩放\图表被选定
    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables); //曲线交互性

    //设置曲线可视性:可视
    m_plot->setVisible(true);    //可视

    //设置网格显示:可视
//    m_bShowGrid=true;//网格可视
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

     //设置图例显示:可视
//     m_bShowLegend=true;//曲线图例可视
     m_plot->legend->setVisible(m_bShowLegend);//true可视,false不可视

     //设置自动增加图表到图例
    m_plot->setAutoAddPlottableToLegend(true);

    //设置图例文本字体
    QFont legendfont;
    legendfont.setPointSize(10);
    //设置图例文本字体:10号
    m_plot->legend->setFont(legendfont);

    //设置图例文本颜色
    QColor LgraphlegendColor;
    LgraphlegendColor.setRgb(85,255,127);
    m_plot->legend->setTextColor(LgraphlegendColor);//设置图例文本颜色

    //设置图例框填充颜色
    QBrush legendbrush;
    legendbrush.setColor(QColor(0,0,255,0));
    //设置图例框填充颜色:红色
    m_plot->legend->setBrush(legendbrush);//设置图例框填充颜色 红色

    //设置图例框画笔颜色
    QPen legendBorderpen;
    legendBorderpen.setColor(QColor(255,255,255,255));
    legendBorderpen.setWidthF(0.5);
    //设置图例框画笔颜色:白色
    m_plot->legend->setBorderPen(legendBorderpen);//设置图例框画笔 白色

    //设置图例文本对齐方式
    Qt::Alignment legendAlignment;
    legendAlignment=(Qt::AlignTop|Qt::AlignRight);
    //设置图例文本对齐方式:放在右上角，采用右对齐方式。
    m_plot->axisRect()->insetLayout()->setInsetAlignment(0,legendAlignment);//图例放置于右上角

    //设置标签文字字体
    QFont LabelFont;
    LabelFont.setPointSize(13);
    //设置标签文字字体:13号
    m_plot->xAxis->setLabelFont(LabelFont);//设置标签文字字体
    m_plot->yAxis->setLabelFont(LabelFont);//设置标签文字字体

    //设置标签文字颜色
    QColor LabelColor;
    LabelColor.setRgb(255,0,0);
    //设置标签文字颜色:红色
    m_plot->xAxis->setLabelColor(LabelColor);//设置标签颜色 红色
    m_plot->yAxis->setLabelColor(LabelColor);//设置标签颜色  红色

    //设置x(或y)轴的刻度台阶
    qint32 TickStep;
    TickStep=1;
    //设置x(或y)轴的刻度台阶
    //void setTickStep(double step); //设置刻度台阶为2
    m_plot->xAxis->setTickStep(TickStep);//设置x(或y)轴的刻度台阶为2
    m_plot->yAxis->setTickStep(TickStep);

    //设置坐标轴轴线颜色/宽度
    QPen penAxis;
    penAxis.setColor(QColor(255,255,255));
    penAxis.setWidthF(2);
    //设置坐标轴   轴线颜色:白色,轴线宽度:2
    m_plot->xAxis->setBasePen(penAxis);
    m_plot->yAxis->setBasePen(penAxis);
    m_plot->xAxis2->setBasePen(penAxis);
    m_plot->yAxis2->setBasePen(penAxis);

    //设置刻度颜色
    QPen penTick;
    penTick.setColor(QColor(255,255,255));
    penTick.setWidthF(1);
    //设置刻度  颜色:白色,宽度:1
    m_plot->xAxis->setTickPen(penTick);
    m_plot->yAxis->setTickPen(penTick);

    //设置子刻度 颜色
    QPen penSubTick;
    penSubTick.setColor(QColor(255,255,255));
    penSubTick.setWidthF(1);
    //设置子刻度 颜色:白色,宽度:1
    m_plot->xAxis->setSubTickPen(penSubTick);
    m_plot->yAxis->setSubTickPen(penSubTick);

    //设置刻度文字字体
    QFont TickLabelFont;
    TickLabelFont.setPointSize(9);
    //设置刻度文字字体：9号
    m_plot->xAxis->setTickLabelFont(TickLabelFont);

    //设置刻度文字颜色
    QColor TickLabelColor;
    TickLabelColor.setRgb(255,0,0);
    //设置刻度文字颜色:红色
    m_plot->xAxis->setTickLabelColor(TickLabelColor);
    m_plot->yAxis->setTickLabelColor(TickLabelColor);

    //设置x(或y)轴刻度小线段标签显示在坐标轴的哪一边
    //设置坐标Tick显示位置位于坐标轴外侧
    m_plot->xAxis->setTickLabelSide(QCPAxis::lsOutside);//位于坐标轴内侧为QCPAxis::lsInside
    m_plot->yAxis->setTickLabelSide(QCPAxis::lsOutside);



    //个性部分

    for(int i=0; i<m_vctgraphObj.size(); i++)
    {

        //坐标轴
        double m_xUp    = m_vctgraphObj[i].m_Xmax_plot;   //x轴最大值
        m_plot->xAxis->setRangeUpper(m_xUp);

        double m_xDown  = m_vctgraphObj[i].m_Xmin_plot;    //x轴最小值
        m_plot->xAxis->setRangeLower(m_xDown);

        double m_yUp    = m_vctgraphObj[i].m_Ymax_plot;    //y轴最大值
        m_plot->yAxis->setRangeUpper(m_yUp);

        double m_yDown  = m_vctgraphObj[i].m_Ymin_plot;   //y轴最小值
        m_plot->yAxis->setRangeLower(m_yDown);

        //设置x(或y)轴文本偏移
        qint32 Offset_x,Offset_y;
        Offset_x=m_vctgraphObj[i].m_Xoffset_plot;//设置x轴文本偏移
        Offset_y=m_vctgraphObj[i].m_Yoffset_plot;//设置y轴文本偏移
        //设置x(或y)轴文本偏移
        //设置坐标轴两端到边框的距离为1
        m_plot->xAxis->setOffset(Offset_x);
        m_plot->yAxis->setOffset(Offset_y);


        //设置x(或y)轴轴标签名称(包括x(或y)轴单位)
        QString XAxisLabel,YAxisLabel;
        XAxisLabel = m_vctgraphObj[i].m_XAxisLabel_plot;
        YAxisLabel = m_vctgraphObj[i].m_YAxisLabel_plot;
        m_plot->xAxis->setLabel(XAxisLabel);//x轴标签x
        m_plot->yAxis->setLabel(YAxisLabel);//y轴标签y

        //设置所有坐标轴的可见性
        qint32 Axis1Visible,Axis2Visible;
        if(m_vctgraphObj[i].m_XAxisdisplay_plot)
        {
            Axis1Visible=1;
        }
        else
        {
            Axis1Visible=0;
        }
        if(m_vctgraphObj[i].m_YAxisdisplay_plot)
        {
            Axis2Visible=1;
        }
        else
        {
            Axis2Visible=0;
        }
        //设置所有坐标轴的可见性
        //设置坐标轴1可见
        m_plot->xAxis->setVisible(Axis1Visible);//设1为可见,0为不可见
        m_plot->yAxis->setVisible(Axis1Visible);
        //设置坐标轴2可见
        m_plot->xAxis2->setVisible(Axis2Visible);//设置坐标轴2轴线可见
        m_plot->yAxis2->setVisible(Axis2Visible);//设置坐标轴2轴线可见
        m_plot->xAxis2->setTicks(0);//但隐藏坐标轴2刻度标示等
        m_plot->yAxis2->setTicks(0);//但隐藏坐标轴2刻度标示等

        //设置刻度小线段位置
        //设置刻度小线段长度:向里伸出多少，向外伸出多少
        qint32 TickLengthinside,TickLengthinoutside;
        if(QString(m_vctgraphObj[i].m_Scaleplace_x_plot)==QString("上"))
        {
            TickLengthinside=10;
            TickLengthinoutside=0;
        }
        else if(QString(m_vctgraphObj[i].m_Scaleplace_x_plot)==QString("中"))
        {
            TickLengthinside=5;
            TickLengthinoutside=-5;
        }
        else                                             //"下"
        {
            TickLengthinside=0;
            TickLengthinoutside=-10;
        }
        if(QString(m_vctgraphObj[i].m_Scaleplace_y_plot)==QString("右"))
        {
            TickLengthinside=10;
            TickLengthinoutside=0;
        }
        else if(QString(m_vctgraphObj[i].m_Scaleplace_y_plot)==QString("中"))
        {
            TickLengthinside=5;
            TickLengthinoutside=-5;
        }
        else                                          //"右"
        {
            TickLengthinside=0;
            TickLengthinoutside=-10;
        }
        //    TickLengthinside=8;
        //    TickLengthinoutside=-2;
        //void setTickLength(int inside, int outside=0);//设置刻度小线段长度:向里伸出多少，向外伸出多少
        m_plot->xAxis->setTickLength(TickLengthinside,TickLengthinoutside);
        m_plot->yAxis->setTickLength(TickLengthinside,TickLengthinoutside);



        //实时曲线
        //设置实时曲线名称
        QString graphname = m_vctgraphObj[i].m_GraphName_plot;
        m_vctgraphObj[i].m_pgraph->setName(graphname);//显示在图例上的名称

        //设置实时曲线宽度/颜色
        qint32 m_graphWidth;
        QColor m_graphColor;
        m_graphWidth=m_vctgraphObj[i].m_GraphWidth_plot;
        m_graphColor.setNamedColor(m_vctgraphObj[i].m_strgraphColor_plot);
        m_vctgraphObj[i].m_pgraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, m_graphColor,m_graphWidth));

        //设置实时曲线缓冲区大小
        double m_GraphBuffer;
        m_GraphBuffer = m_vctgraphObj[i].m_GraphBuffer_plot;

        //设置实时曲线x参数
        QString m_paramX;
        m_paramX = m_vctgraphObj[i].m_xParam_plot; //初始实时曲线输入单参数数据X变量

        //设置实时曲线y参数
        QString m_paramY;
        m_paramY = m_vctgraphObj[i].m_yParam_plot; //初始实时曲线输入单参数数据Y变量


        //理论曲线

        //设置理论曲线名称
        QString graphname_temp = m_vctgraphObj[i].m_GraphName_plot;
        QString Lgraphname = QString("%1%2").arg(graphname_temp).arg(tr("底图"));
        m_vctgraphObj[i].m_pLgraph->setName(Lgraphname);//显示在图例上的名称

        //设置理论曲线宽度/颜色
        qint32 m_LgraphWidth;
        QColor m_LgraphColor;

        m_LgraphWidth=m_vctgraphObj[i].m_LGraphWidth_plot;
        m_LgraphColor.setNamedColor(m_vctgraphObj[i].m_strLgraphColor_plot);
//        m_LgraphWidth=3;
//        m_LgraphColor.setRgb(223,223,223,223);//灰色
        m_vctgraphObj[i].m_pLgraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, m_LgraphColor,m_LgraphWidth));

        //设置理论曲线缓冲区大小
        double m_LGraphBuffer;
        m_LGraphBuffer = m_vctgraphObj[i].m_LGraphBuffer_plot;

        //设置理论曲线数据(文件(路径))

    }
}





/*

//共性部分，在主类属性栏进行设置

//共性部分（公共配置部分）
void staticgraphPrivate::setPlot()
{


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


}

//个性部分，需要每条曲线单独设置
//设置个性特征----在多曲线配置对话框进行配置


//理论曲线
void staticgraphPrivate::setPlot1()
{
    //显示曲线1
    m_plot->graph(0)->setVisible(true);

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



}

*/





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



//数据驱动---获取外部数据
//
//
//接收从外部net模块接收数据
//设置曲线的点的x、y值,使用Net类取数据
void staticgraphPrivate::getData()
{
    if(m_plot == NULL)
    {
        return;
    }
    for(int i=0; i<m_vctgraphObj.size(); i++)
    {
        //定义两个数组用于存放取到的数据
        QVector<double> x, y;
        //准备插入到曲线的点
        double tx, ty;

        m_dci_x->getHistoryData(m_vctgraphObj[i].m_xParam_plot,x);
        m_dci_y->getHistoryData(m_vctgraphObj[i].m_yParam_plot,y);
        //把取到的全部x的值存入数组m_Cx
        foreach (double tx, x) {
            m_vctgraphObj[i].m_Cx.append(tx);//在容器m_Cx末尾添加tx
        }
        //把取到的全部y的值存入数组m_Cy
        foreach (double ty, y) {
            m_vctgraphObj[i].m_Cy.append(ty);//在容器m_y末尾添加ty
        }
        //把曲线的值设置为刚才的两个数组
        m_vctgraphObj[i].m_pgraph->setData(m_vctgraphObj[i].m_Cx, m_vctgraphObj[i].m_Cy);//键值，值
    }
}


//接收自己设计的测试数据
/*
//跟随曲线
void staticgraphPrivate::getData_Test()
{
    if(m_plot == NULL)
    {
        return;
    }
    for(int j=0; j<m_vctgraphObj.size(); j++)
    {
        //定义两个数组用于存放取到的数据
        QVector<double>Lx,Ly;

        //准备插入到曲线的点
        double tLx, tLy;

        m_dci_x->getHistoryData(m_vctgraphObj[j].m_xParam_plot,Lx);
        m_dci_y->getHistoryData(m_vctgraphObj[j].m_yParam_plot,Ly);

        //(variable, container)
        foreach (double tLx, Lx) {
            m_vctgraphObj[j].m_Lx.append(tLx);//在容器m_x末尾添加tx
        }
        foreach (double tLy, Ly) {
            m_vctgraphObj[j].m_Ly.append(tLy);//在容器m_y末尾添加ty
        }

        m_vctgraphObj[j].m_pLgraph->setData(m_vctgraphObj[j].m_Lx, m_vctgraphObj[j].m_Ly);//键值，值
    }
}
*/
//静态曲线
void staticgraphPrivate::getData_Test()
{
    if(m_plot == NULL)
    {
        return;
    }
    QVector<double>Lx(200),Ly(200);
    for(int i=0;i<200;++i)
    {

        Lx[i] = i;
        Ly[i] =40+10*sin((((double)Lx[i])/20.0)*2*3.1415);
    }
    for(int j=0; j<m_vctgraphObj.size(); j++)
    {
        //(variable, container)
        foreach (double tLx, Lx) {
            m_vctgraphObj[j].m_Lx.append(tLx);//在容器m_x末尾添加tx
        }
        foreach (double tLy, Ly) {
            m_vctgraphObj[j].m_Ly.append(tLy);//在容器m_y末尾添加ty
        }

        m_vctgraphObj[j].m_pLgraph->setData(m_vctgraphObj[j].m_Lx, m_vctgraphObj[j].m_Ly);//键值，值
    }
}




/*****需求******
//曲线
//已实现包括：(1) 曲线风格、曲线画笔、曲线形状、曲线名称、是否显示某条曲线
//            支持特殊显示(如:直线连线、钻石形标记等)、曲线缓冲区;
//          (2) 目标增减、应用QTabWidget类实现多曲线配置中配置相关函数的添加
//          (3) 实现多曲线配置对话框的配置数据的封装（json生成），并实现向主类的数据传输;
//               及在主类的Json数据解析（json解析）,并赋值到到曲线类中。

/*****其他需求*******/
// staticgraph工程剩余需求：
//1 原始坐标体系。  (右键可选择点击：在实时模式进行了拖动、缩放之后，右键可以选择单击回复到一开始的定制画面。)
//2 自适应数据。   （右键可选择点击：找出x、y数据各自的最大值最小值，把这个作出的矩形放到画面中间）
//3 T0获取。      (T0获取并根据T0(靶弹，未定义，拦截弹）计算相对时弹道及清屏。)
//4 样式显示。     (曲线按照某种样式进行显示，如某个区间显示某种颜色，某种标记，大小等。)(增加到属性中)
//5 显示模式。     (固定显示模式（在staticgraph工程）、滚动显示模式（在rollinggraph工程）。)
//6 界面修改。    （双击修改界面部分简单参数配置的功能。）


