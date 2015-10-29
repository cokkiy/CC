#include "staticgraphPrivate.h"
#include "qwidget.h"
#include <math.h>

//私有类staticgraphPrivate构造函数
//功能：曲线初始化，主要是私有类成员变量的初始化
//staticgraphPrivate::staticgraphPrivate(QWidget*wgt):m_dci(this)
staticgraphPrivate::staticgraphPrivate(QWidget*wgt)
{

    //0：必须要的准备
    //在私有类中，获取QCustomPlot类指针
    //QWidget*wgt
    parent = wgt;
    m_plot = new QCustomPlot(parent);


    //共性部分

    //图元背景色
    //m_backgroundColor.setRgb(0, 0, 255, 255);//蓝色,最后一位0代表透明
    m_backgroundColor.setRgb(0, 0, 255);//蓝色(0, 0, 255),白色(255, 255, 255)
    m_backgroundColor.setAlpha(255);//代表透明度

    m_bShowGrid = true; //曲线网格可视
    m_bShowLegend = false; //曲线图例可视
    m_leftmargin = 15;
    m_rightmargin = 15;
    m_topmargin  = 15;
    m_bottommargin = 15;

    //数据接收类
    m_dci_x = NetComponents::getDataCenter();
    m_dci_y = NetComponents::getDataCenter();

    //将动态范围发射到设置范围中
//    connect(m_plot->xAxis, SIGNAL(rangeChanged(QCPRange)), m_plot->xAxis2, SLOT(setRange(QCPRange)));
//    connect(m_plot->xAxis, SIGNAL(rangeChanged(QCPRange)), m_plot->xAxis2, SLOT(setRange(QCPRange)));


    // 使得左下轴们传递他们的范围到右上轴们:
//    connect(m_plot->xAxis, SIGNAL(rangeChanged(QCPRange)), m_plot->xAxis2, SLOT(setRange(QCPRange)));
//    connect(m_plot->yAxis, SIGNAL(rangeChanged(QCPRange)), m_plot->yAxis2, SLOT(setRange(QCPRange)));
    //


    //    // 安装一个定时器这样重复地调用主类的实时数据槽 MainWindow::realtimeDataSlot:
    //    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    //    dataTimer.start(0); // 间隔0，意味着尽可能快地刷新

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
            m_vctgraphObj[i].m_pgraphLast->clearData();
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
                    pgj.m_Ymin_plot = name["Ymin"].toDouble();
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
                    //原点的位置：左下、左上、右下、右上、正中
                    pgj.m_OriginPlace_plot =name["OriginPlace"].toString();
                    //刻度标签的位置：内侧、外侧
                    pgj.m_Scalelabelplace_x_plot =name["Scalelabelplace_x"].toString();
                    pgj.m_Scalelabelplace_y_plot =name["Scalelabelplace_y"].toString();
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
                    //实时曲线颜色
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

                    pgj.m_XAxiswideth_plot = name["XAxiswideth"].toInt();

                    pgj.m_chooseXAxisColor_plot = name["chooseXAxisColor"].toString();
                    //轴标签、轴刻度标签的颜色全部统一成实时曲线的颜色
                    pgj.m_chooseXAxisLabelColor_plot = name["strgraphColor"].toString();
                    pgj.m_chooseYAxisLabelColor_plot = name["strgraphColor"].toString();
                    pgj.m_chooseXAxisScaleLabelColor_plot=name["strgraphColor"].toString();
                    pgj.m_chooseYAxisScaleLabelColor_plot=name["strgraphColor"].toString();

                    pgj.m_chooseXAxislabeldisplay_plot = name["chooseXAxislabeldisplay"].toBool();
                    pgj.m_chooseYAxislabeldisplay_plot = name["chooseYAxislabeldisplay"].toBool();
                    pgj.m_chooseXAxisScalelabeldisplay_plot = name["chooseXAxisScalelabeldisplay"].toBool();
                    pgj.m_chooseYAxisScalelabeldisplay_plot = name["chooseYAxisScalelabeldisplay"].toBool();
                    pgj.m_chooseXAxisScaleTickdisplay_plot = name["chooseXAxisScaleTickdisplay"].toBool();
                    pgj.m_chooseYAxisScaleTickdisplay_plot = name["chooseYAxisScaleTickdisplay"].toBool();

                    pgj.m_XAxislabelFont_plot = name["XAxislabelFont"].toString();
                    pgj.m_XAxisScalelabelFont_plot = name["XAxisScalelabelFont"].toString();
                    pgj.m_YAxisScalelabelFont_plot = name["YAxisScalelabelFont"].toString();

                    pgj.m_numOfXAxisScale_plot = name["numOfXAxisScale"].toInt();
                    pgj.m_numOfYAxisScale_plot = name["numOfYAxisScale"].toInt();
                    //刻度小线段长度的设置
                    pgj.m_XAxisScaleRuler_plot = name["XAxisScaleRuler"].toInt();
                    pgj.m_YAxisScaleRuler_plot = name["YAxisScaleRuler"].toInt();

                    pgj.m_XAxisScaleprecision_plot = name["XAxisScaleprecision"].toInt();
                    pgj.m_YAxisScaleprecision_plot = name["YAxisScaleprecision"].toInt();

                    pgj.m_XAxisScalelabeloffset_x_plot = name["XAxisScalelabeloffset_x"].toString();
                    pgj.m_XAxisScalelabeloffset_y_plot = name["XAxisScalelabeloffset_y"].toString();
                    pgj.m_YAxisScalelabeloffset_x_plot = name["YAxisScalelabeloffset_x"].toString();
                    pgj.m_YAxisScalelabeloffset_y_plot = name["YAxisScalelabeloffset_y"].toString();



                    //获取原点位置
                    QString OriginStr = name["OriginPlace"].toString();
                    //根据原点位置决定添加图层到哪套轴
                    //原点的位置：左下、左上、右下、右上、正中
                    if(OriginStr == QString("左下"))//使用QCPAxis::atBottom,QCPAxis::atLeft组合为坐标系统
                    {
                        //添加一套坐标轴系统
                        qint32 j=i;
                        //第一套曲线为默认曲线，不需要额外增加；并且保证增加后曲线总数不超过20条
                        if((!(i==0))&&(i<20))
                        {
                            m_plot->axisRect()->addAxis(QCPAxis::atLeft,0);
                            m_plot->axisRect()->addAxis(QCPAxis::atBottom,0);
//                            m_plot->axisRect()->addAxis(QCPAxis::atLeft);
//                            m_plot->axisRect()->addAxis(QCPAxis::atBottom);
                        }
                         //设置轴偏移，以便让每条增加的坐标轴都看的见
                        if(!(i==0))
                        {
                            m_plot->axisRect()->axis(QCPAxis::atBottom, i)->setOffset(-5+3*i);
                            m_plot->axisRect()->axis(QCPAxis::atLeft, i)->setOffset(-5+3*i);
                        }

                        //添加一个曲线图层-理论曲线
                        pgj.m_pLgraph = m_plot->addGraph(m_plot->axisRect()->axis(QCPAxis::atBottom, i),
                                                         m_plot->axisRect()->axis(QCPAxis::atLeft, i));
                        //添加一个曲线图层-实时曲线
                        pgj.m_pgraph = m_plot->addGraph(m_plot->axisRect()->axis(QCPAxis::atBottom, i),
                                                        m_plot->axisRect()->axis(QCPAxis::atLeft, i));
                        //添加一个曲线图层-实时曲线(最后收到的点)---闪烁点
                        pgj.m_pgraphLast = m_plot->addGraph(m_plot->axisRect()->axis(QCPAxis::atBottom, i),
                                                            m_plot->axisRect()->axis(QCPAxis::atLeft, i));
                    }
                    else if(OriginStr == QString("左上"))//使用QCPAxis::atTop,QCPAxis::atLeft组合为坐标系统
                    {
                        //添加一套坐标轴系统
                        qint32 j=i;
                        //第一套曲线为默认曲线，不需要额外增加；并且保证增加后曲线总数不超过20条
                        if((!(i==0))&&(i<20))
                        {
                            m_plot->axisRect()->addAxis(QCPAxis::atLeft,0);
                            m_plot->axisRect()->addAxis(QCPAxis::atTop,0);
                        }
                         //设置轴偏移，以便让每条增加的坐标轴都看的见
                        if(!(i==0))
                        {
                            m_plot->axisRect()->axis(QCPAxis::atBottom, i)->setOffset(-5+3*i);
                            m_plot->axisRect()->axis(QCPAxis::atTop, i)->setOffset(-5+3*i);
                        }

                        //添加一个曲线图层-理论曲线
                        pgj.m_pLgraph = m_plot->addGraph(m_plot->axisRect()->axis(QCPAxis::atTop, i),
                                                         m_plot->axisRect()->axis(QCPAxis::atLeft, i));
                        //添加一个曲线图层-实时曲线
                        pgj.m_pgraph = m_plot->addGraph(m_plot->axisRect()->axis(QCPAxis::atTop, i),
                                                        m_plot->axisRect()->axis(QCPAxis::atLeft, i));
                        //添加一个曲线图层-实时曲线(最后收到的点)---闪烁点
                        pgj.m_pgraphLast = m_plot->addGraph(m_plot->axisRect()->axis(QCPAxis::atTop, i),
                                                            m_plot->axisRect()->axis(QCPAxis::atLeft, i));

                    }
                    else if(OriginStr == QString("右下"))//使用QCPAxis::atBottom,QCPAxis::atRight组合为坐标系统
                    {
                        //添加一套坐标轴系统
                        qint32 j=i;
                        //第一套曲线为默认曲线，不需要额外增加；并且保证增加后曲线总数不超过20条
                        if((!(i==0))&&(i<20))
                        {
                            m_plot->axisRect()->addAxis(QCPAxis::atRight,0);
                            m_plot->axisRect()->addAxis(QCPAxis::atBottom,0);
                        }
                         //设置轴偏移，以便让每条增加的坐标轴都看的见
                        if(!(i==0))
                        {
                            m_plot->axisRect()->axis(QCPAxis::atRight, i)->setOffset(-5+3*i);
                            m_plot->axisRect()->axis(QCPAxis::atBottom, i)->setOffset(-5+3*i);
                        }

                        //添加一个曲线图层-理论曲线
                        pgj.m_pLgraph = m_plot->addGraph(m_plot->axisRect()->axis(QCPAxis::atBottom, i),
                                                         m_plot->axisRect()->axis(QCPAxis::atRight, i));
                        //添加一个曲线图层-实时曲线
                        pgj.m_pgraph = m_plot->addGraph(m_plot->axisRect()->axis(QCPAxis::atBottom, i),
                                                        m_plot->axisRect()->axis(QCPAxis::atRight, i));
                        //添加一个曲线图层-实时曲线(最后收到的点)---闪烁点
                        pgj.m_pgraphLast = m_plot->addGraph(m_plot->axisRect()->axis(QCPAxis::atBottom, i),
                                                            m_plot->axisRect()->axis(QCPAxis::atRight, i));

                    }
                    else if(OriginStr == QString("右上"))//使用QCPAxis::atTop,QCPAxis::atRight组合为坐标系统
                    {
                        //添加一套坐标轴系统
                        qint32 j=i;
                        //第一套曲线为默认曲线，不需要额外增加；并且保证增加后曲线总数不超过20条
                        if((!(i==0))&&(i<20))
                        {
                            m_plot->axisRect()->addAxis(QCPAxis::atRight,0);
                            m_plot->axisRect()->addAxis(QCPAxis::atTop,0);
//                            m_plot->axisRect()->addAxis(QCPAxis::atRight);
//                            m_plot->axisRect()->addAxis(QCPAxis::atTop);
                        }
                         //设置轴偏移，以便让每条增加的坐标轴都看的见
                        if(!(i==0))
                        {
                            m_plot->axisRect()->axis(QCPAxis::atRight, i)->setOffset(-5+3*i);
                            m_plot->axisRect()->axis(QCPAxis::atTop, i)->setOffset(-5+3*i);
                        }

                        //添加一个曲线图层-理论曲线
                        pgj.m_pLgraph = m_plot->addGraph(m_plot->axisRect()->axis(QCPAxis::atTop, i),
                                                         m_plot->axisRect()->axis(QCPAxis::atRight, i));
                        //添加一个曲线图层-实时曲线
                        pgj.m_pgraph = m_plot->addGraph(m_plot->axisRect()->axis(QCPAxis::atTop, i),
                                                        m_plot->axisRect()->axis(QCPAxis::atRight, i));
                        //添加一个曲线图层-实时曲线(最后收到的点)---闪烁点
                        pgj.m_pgraphLast = m_plot->addGraph(m_plot->axisRect()->axis(QCPAxis::atTop, i),
                                                            m_plot->axisRect()->axis(QCPAxis::atRight, i));

                    }
                    else//if(OriginStr == QString("正中"))//使用最常用坐标系统只是稍作位移
                    {
                        //添加一套坐标轴系统
                        qint32 j=i;
                        //第一套曲线为默认曲线，不需要额外增加；并且保证增加后曲线总数不超过20条
                        if((!(i==0))&&(i<20))
                        {
//                            m_plot->axisRect()->addAxis(QCPAxis::atCenterXAxis,0);
//                            m_plot->axisRect()->addAxis(QCPAxis::atCenterYAxis,0);
                            m_plot->axisRect()->addAxis(QCPAxis::atLeft,0);
                            m_plot->axisRect()->addAxis(QCPAxis::atBottom,0);
                        }
                        //设置轴偏移，以便让每条增加的坐标轴都看的见

                        //添加一个曲线图层-理论曲线
                        pgj.m_pLgraph = m_plot->addGraph(m_plot->axisRect()->axis(QCPAxis::atBottom, i),
                                                         m_plot->axisRect()->axis(QCPAxis::atLeft, i));
                        //添加一个曲线图层-实时曲线
                        pgj.m_pgraph = m_plot->addGraph(m_plot->axisRect()->axis(QCPAxis::atBottom, i),
                                                        m_plot->axisRect()->axis(QCPAxis::atLeft, i));
                        //添加一个曲线图层-实时曲线(最后收到的点)---闪烁点
                        pgj.m_pgraphLast = m_plot->addGraph(m_plot->axisRect()->axis(QCPAxis::atBottom, i),
                                                            m_plot->axisRect()->axis(QCPAxis::atLeft, i));
                    }

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
        qWarning()<<"zjb's Warning! PlotGraphObject::ReadLLgraph() : 读取理论曲线文件失败：未配置文件!";
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

//页边距属性配置
//获取/设置理论页边距属性
//Margins
void staticgraphPrivate::setleftMargin(const qint32 lm)
{
    m_leftmargin = lm;
    QMargins plotmargins(m_leftmargin,m_topmargin,m_rightmargin,m_bottommargin);
    QRect plotRect_temp =parent->rect();
    m_plot->setViewport(plotRect_temp);
    m_plot->setGeometry(plotRect_temp);
    QRect plotRect = plotRect_temp.marginsRemoved(plotmargins);
    m_plot->setGeometry(plotRect_temp);//外部窗口--父窗口
    m_plot->setViewport(plotRect);//视图矩形区--小于父窗口---m_plot真正起作用的窗口
}
void staticgraphPrivate::setrightMargin(const qint32 rm)
{
    m_rightmargin = rm;
    QMargins plotmargins(m_leftmargin,m_topmargin,m_rightmargin,m_bottommargin);
    QRect plotRect_temp =parent->rect();
    m_plot->setViewport(plotRect_temp);
    m_plot->setGeometry(plotRect_temp);
    QRect plotRect = plotRect_temp.marginsRemoved(plotmargins);
    m_plot->setGeometry(plotRect_temp);//外部窗口--父窗口
    m_plot->setViewport(plotRect);//视图矩形区--小于父窗口---m_plot真正起作用的窗口
}
void staticgraphPrivate::settopMargin(const qint32 tm)
{
    m_topmargin = tm;
    QMargins plotmargins(m_leftmargin,m_topmargin,m_rightmargin,m_bottommargin);
    QRect plotRect_temp =parent->rect();
    m_plot->setViewport(plotRect_temp);
    m_plot->setGeometry(plotRect_temp);
    QRect plotRect = plotRect_temp.marginsRemoved(plotmargins);
    m_plot->setGeometry(plotRect_temp);//外部窗口--父窗口
    m_plot->setViewport(plotRect);//视图矩形区--小于父窗口---m_plot真正起作用的窗口
}
void staticgraphPrivate::setbottomMargin(const qint32 bm)
{
    m_bottommargin = bm;
    QMargins plotmargins(m_leftmargin,m_topmargin,m_rightmargin,m_bottommargin);
    QRect plotRect_temp =parent->rect();
    m_plot->setViewport(plotRect_temp);
    m_plot->setGeometry(plotRect_temp);
    QRect plotRect = plotRect_temp.marginsRemoved(plotmargins);
    m_plot->setGeometry(plotRect_temp);//外部窗口--父窗口
    m_plot->setViewport(plotRect);//视图矩形区--小于父窗口---m_plot真正起作用的窗口
}




//个性部分

void staticgraphPrivate::setTextString(const QString string)
{
    if(m_plot == NULL)
    {
        return;
    }

    m_textString=string;
    //私有类中,把多曲线配置对话框传过来的数据解析出来
    parseJsonData();//得到赋值的结构体变量dialogdata

    setPlot();

    update();
}

//   ！！！！！储备代码部分！！！！
//设置滚动曲线需要这个
//position:轴的起始点
//size:新的轴的范围
//alignment:轴线向哪边延伸   Qt::AlignLeft, Qt::AlignRight or Qt::AlignCenter
//m_plot->axisRect()->axis(QCPAxis::atBottom, i)->setRange(double position, double size, Qt::AlignmentFlag alignment)

//        void QCPAxis::setRange(double position, double size, Qt::AlignmentFlag alignment)
//        {
//          if (alignment == Qt::AlignLeft)
//            setRange(position, position+size);
//          else if (alignment == Qt::AlignRight)
//            setRange(position-size, position);
//          else // alignment == Qt::AlignCenter
//            setRange(position-size/2.0, position+size/2.0);
//        }
//        double pixelToCoord(double value) const;//象素到坐标
//        double coordToPixel(double value) const;//坐标到象素

//    //QWidget*wgt
//    parent = wgt;
//    m_plot = new QCustomPlot(parent);



//void setPlot()//全新的画图参数配置
void staticgraphPrivate::setPlot()
{
    //如曲线对象为空，则退出
    if(m_plot==NULL)
    {
        return;
    }

    // !!!!!!!!共性部分，在主类属性栏进行设置!!!!!!!

    //  --------共性属性1-----设置图元页边距
    //以下为基本靠谱的代码
    //QMargins(int left, int top, int right, int bottom)
    QMargins plotmargins(m_leftmargin,m_topmargin,m_rightmargin,m_bottommargin);
    QRect plotRect_temp =parent->rect();
    m_plot->setViewport(plotRect_temp);
    m_plot->setGeometry(plotRect_temp);
    QRect plotRect = plotRect_temp.marginsRemoved(plotmargins);
    m_plot->setGeometry(plotRect_temp);//外部窗口--父窗口
    m_plot->setViewport(plotRect);//视图矩形区--小于父窗口---m_plot真正起作用的窗口

    //  --------共性属性2-----设置图元交互性
    //坐标轴为可拖动 QCP::iRangeDrag
    //坐标轴为可缩放 QCP::iRangeZoom
    //曲线及图表可以被选择 QCP::iSelectPlottables
    //轴线可选择 QCP::iSelectAxes
    //图例可选择 QCP::iSelectLegend
    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom
                 | QCP::iSelectPlottables|QCP::iSelectLegend); //曲线交互性


    //        if(!(m_plot->axisRect()->graphs().at(i)==NULL))

    // !!!!!只有图例还在外面晃悠!!!!
    //设置图例可视性:可视
    m_bShowLegend = true;
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
    //设置图例交互性
    m_plot->legend->setSelectableParts(QCPLegend::spItems);
    m_plot->replot();

    //---------!!!!!!!
    //    m_plot->axisRect()->axis(QCPAxis::atBottom, i);    //x轴
    //    m_plot->axisRect()->axis(QCPAxis::atLeft, i);      //y轴
    //    m_plot->axisRect()->axis(QCPAxis::atTop, i);       //x2轴
    //    m_plot->axisRect()->axis(QCPAxis::atRight, i);     //y2轴

    QCPAxis::AxisType m_xAxistype,m_yAxistype;
    //赋初值
    m_xAxistype = QCPAxis::atBottom;
    m_yAxistype = QCPAxis::atLeft;


    for(int i=0; i<m_vctgraphObj.size(); i++)
    {
        //根据各类原点确定轴类型组合
        //根据原点位置决定给哪套轴赋参数
        //原点的位置：左下、左上、右下、右上、正中
        if(QString(m_vctgraphObj[i].m_OriginPlace_plot)==QString("左下"))
        {
            m_xAxistype = QCPAxis::atBottom;
            m_yAxistype = QCPAxis::atLeft;
            //设置范围反转
            bool xAxisRangereversed =false;//设true为反转,false为不反转
            bool yAxisRangereversed =false;
            m_plot->axisRect()->axis(m_xAxistype, i)->setRangeReversed(xAxisRangereversed);
            m_plot->axisRect()->axis(m_yAxistype, i)->setRangeReversed(yAxisRangereversed);
            //其他情况都清除原点位于正中的标志,传送到qcustomplot的配置中
            m_plot->axisRect()->axis(m_xAxistype,i)->setCenterOrigin(false);
            m_plot->axisRect()->axis(m_yAxistype,i)->setCenterOrigin(false);
        }
        else if(QString(m_vctgraphObj[i].m_OriginPlace_plot)==QString("左上"))
        {
            m_xAxistype = QCPAxis::atTop;
            m_yAxistype = QCPAxis::atLeft;
            //设置范围反转
            bool xAxisRangereversed =false;//设true为反转,false为不反转
            bool yAxisRangereversed =false;
            m_plot->axisRect()->axis(m_xAxistype, i)->setRangeReversed(xAxisRangereversed);
            m_plot->axisRect()->axis(m_yAxistype, i)->setRangeReversed(yAxisRangereversed);
            //其他情况都清除原点位于正中的标志,传送到qcustomplot的配置中
            m_plot->axisRect()->axis(m_xAxistype,i)->setCenterOrigin(false);
            m_plot->axisRect()->axis(m_yAxistype,i)->setCenterOrigin(false);
        }
        else if(QString(m_vctgraphObj[i].m_OriginPlace_plot)==QString("右下"))
        {
            m_xAxistype = QCPAxis::atBottom;
            m_yAxistype = QCPAxis::atRight;
            //设置范围反转
            bool xAxisRangereversed =false;//设true为反转,false为不反转
            bool yAxisRangereversed =false;
            m_plot->axisRect()->axis(m_xAxistype, i)->setRangeReversed(xAxisRangereversed);
            m_plot->axisRect()->axis(m_yAxistype, i)->setRangeReversed(yAxisRangereversed);
            //其他情况都清除原点位于正中的标志,传送到qcustomplot的配置中
            m_plot->axisRect()->axis(m_xAxistype,i)->setCenterOrigin(false);
            m_plot->axisRect()->axis(m_yAxistype,i)->setCenterOrigin(false);
        }
        else if(QString(m_vctgraphObj[i].m_OriginPlace_plot)==QString("右上"))
        {
            m_xAxistype = QCPAxis::atTop;
            m_yAxistype = QCPAxis::atRight;
            //设置范围反转
            bool xAxisRangereversed =false;//设true为反转,false为不反转
            bool yAxisRangereversed =false;
            m_plot->axisRect()->axis(m_xAxistype, i)->setRangeReversed(xAxisRangereversed);
            m_plot->axisRect()->axis(m_yAxistype, i)->setRangeReversed(yAxisRangereversed);
            //其他情况都清除原点位于正中的标志,传送到qcustomplot的配置中
            m_plot->axisRect()->axis(m_xAxistype,i)->setCenterOrigin(false);
            m_plot->axisRect()->axis(m_yAxistype,i)->setCenterOrigin(false);
        }
        else//(QString(m_vctgraphObj[i].m_OriginPlace_plot)==QString("正中"))
        {
            m_xAxistype = QCPAxis::atBottom;
            m_yAxistype = QCPAxis::atLeft;
            //设置范围反转
            bool xAxisRangereversed =false;//设true为反转,false为不反转
            bool yAxisRangereversed =false;
            m_plot->axisRect()->axis(m_xAxistype, i)->setRangeReversed(xAxisRangereversed);
            m_plot->axisRect()->axis(m_yAxistype, i)->setRangeReversed(yAxisRangereversed);
            //原点位于正中的标志,传送到qcustomplot的配置中
            m_plot->axisRect()->axis(m_xAxistype,i)->setCenterOrigin(true);
            m_plot->axisRect()->axis(m_yAxistype,i)->setCenterOrigin(true);
        }





        //设置x轴起始刻度（最小值）,结束刻度（最大值）
        double m_xDown  = m_vctgraphObj[i].m_Xmin_plot;    //x轴起始刻度（最小值）
        m_plot->axisRect()->axis(m_xAxistype, i)->setRangeLower(m_xDown);
        double m_xUp    = m_vctgraphObj[i].m_Xmax_plot;   //x轴结束刻度（最大值）
        m_plot->axisRect()->axis(m_xAxistype, i)->setRangeUpper(m_xUp);

        //设置y轴起始刻度（最小值）,结束刻度（最大值）
        double m_yDown  = m_vctgraphObj[i].m_Ymin_plot;   //y轴起始刻度（最小值）
        m_plot->axisRect()->axis(m_yAxistype, i)->setRangeLower(m_yDown);
        double m_yUp    = m_vctgraphObj[i].m_Ymax_plot;    //y轴结束刻度（最大值）
        m_plot->axisRect()->axis(m_yAxistype, i)->setRangeUpper(m_yUp);
//        bool rangechanged_yes;
//        if(rangechanged_yes)
//        {
//            m_yDown =
//            m_yUp   =
//            //根据变化范围,设置y轴起始刻度（最小值）,结束刻度（最大值）
//            m_plot->axisRect()->axis(m_yAxistype, i)->setRangeLower(m_yDown);
//            m_plot->axisRect()->axis(m_yAxistype, i)->setRangeUpper(m_yUp);
//        }


        //  --------共性属性3-----设置图元可视性
        //包括：曲线可视性、网格可视性、图例可视性、坐标轴可视性
        //设置曲线可视性:可视
        m_plot->setVisible(true);    //可视
        //设置网格可视性:可视
        if((!(m_plot->axisRect()->axis(m_xAxistype, i)==NULL))&&(!(m_plot->axisRect()->axis(m_yAxistype, i)==NULL)))
        {
            //m_bShowGrid = false;
            if(m_bShowGrid)//网格是否可视
            {
//                //将不是所选轴类型的网格线设为false
//                if ((m_xAxistype == QCPAxis::atTop)&&(m_yAxistype == QCPAxis::atLeft))
//                {
//                    if(!(m_plot->axisRect()->axis(QCPAxis::atBottom, i)->grid()==NULL))
//                    {
//                       m_plot->axisRect()->axis(QCPAxis::atBottom, i)->grid()->setVisible(false);//设置网格可视
//                    }
//                    if(!(m_plot->axisRect()->axis(QCPAxis::atRight, i)->grid()==NULL))
//                    {
//                        m_plot->axisRect()->axis(QCPAxis::atRight, i)->grid()->setVisible(false);
//                    }
//                }
//                else if((m_xAxistype == QCPAxis::atBottom)&&(m_yAxistype == QCPAxis::atRight))
//                {
//                    if(!(m_plot->axisRect()->axis(QCPAxis::atTop, i)->grid()==NULL))
//                    {
//                        m_plot->axisRect()->axis(QCPAxis::atTop, i)->grid()->setVisible(false);//设置网格可视
//                    }
//                    if(!(m_plot->axisRect()->axis(QCPAxis::atLeft, i)->grid()==NULL))
//                    {
//                        m_plot->axisRect()->axis(QCPAxis::atLeft, i)->grid()->setVisible(false);
//                    }
//                }
//                else if((m_xAxistype == QCPAxis::atTop)&&(m_yAxistype == QCPAxis::atRight))
//                {
//                    if(!(m_plot->axisRect()->axis(QCPAxis::atBottom, i)->grid()==NULL))
//                    {
//                        m_plot->axisRect()->axis(QCPAxis::atBottom, i)->grid()->setVisible(false);//设置网格可视
//                    }
//                    if(!(m_plot->axisRect()->axis(QCPAxis::atLeft, i)->grid()==NULL))
//                    {
//                        m_plot->axisRect()->axis(QCPAxis::atLeft, i)->grid()->setVisible(false);
//                    }
//                }
//                else if((m_xAxistype == QCPAxis::atBottom)&&(m_yAxistype == QCPAxis::atLeft))
//                {
//                    if(!(m_plot->axisRect()->axis(QCPAxis::atTop, i)->grid()==NULL))
//                    {
//                        m_plot->axisRect()->axis(QCPAxis::atTop, i)->grid()->setVisible(false);//设置网格可视
//                    }
//                    if(!(m_plot->axisRect()->axis(QCPAxis::atRight, i)->grid()==NULL))
//                    {
//                        m_plot->axisRect()->axis(QCPAxis::atRight, i)->grid()->setVisible(false);
//                    }
//                }


                m_plot->axisRect()->axis(m_xAxistype, i)->grid()->setVisible(true);//设置网格可视
                m_plot->axisRect()->axis(m_yAxistype, i)->grid()->setVisible(true);

                //设置网格的高级特性
                //m_plot->axisRect()->axis(m_xAxistype, i)->grid()->setSubGridVisible(true);//设置子网格可见性
                //m_plot->axisRect()->axis(m_xAxistype, i)->grid()->setAntialiasedSubGrid(true);//设置抗锯齿子网格
                m_plot->axisRect()->axis(m_xAxistype, i)->grid()->setAntialiasedZeroLine(true);//设置抗锯齿零线
                //void setPen(const QPen &pen);//设置画笔
                //void setSubGridPen(const QPen &pen);//设置子网格画笔
                QPen ZeroLinePen;
                ZeroLinePen.setColor(QColor(255,0,0,255));
                m_plot->axisRect()->axis(m_xAxistype, i)->grid()->setZeroLinePen(ZeroLinePen);//设置零线画笔
                m_plot->axisRect()->axis(m_yAxistype, i)->grid()->setZeroLinePen(ZeroLinePen);//设置零线画笔
                //m_plot->axisRect()->axis(m_xAxistype, i)->grid()->setGridnumofXAxis(m_vctgraphObj[0].m_numOfXAxisScale_plot);//zjb add :传入固定的刻度数 m_GridnumofXAxis
                //m_plot->axisRect()->axis(m_yAxistype, i)->grid()->setGridnumofYAxis(m_vctgraphObj[0].m_numOfYAxisScale_plot);//zjb add :传入固定的刻度数 m_GridnumofYAxis
            }
            else
            {
                m_plot->axisRect()->axis(m_xAxistype, i)->grid()->setVisible(false);//网格不可视
                m_plot->axisRect()->axis(m_yAxistype, i)->grid()->setVisible(false);
            }
        }



        //设置坐标轴可视性-----初始值先全部设为可见
        //包括：两套默认坐标轴的基本轴、刻度、刻度标签、子刻度、子刻度标签
        if((!(m_plot->axisRect()->axis(m_xAxistype, i)==NULL))&&(!(m_plot->axisRect()->axis(m_yAxistype, i)==NULL)))
        {
            //---1设置坐标轴的基本轴可视性
            //设置坐标轴1的基本轴可视性
            m_plot->axisRect()->axis(m_xAxistype, i)->setVisible(true);//默认坐标轴1的x轴的基本轴,设1为可见,0为不可见
            m_plot->axisRect()->axis(m_yAxistype, i)->setVisible(true);//默认坐标轴1的y轴的基本轴,设1为可见,0为不可见

            //---2设置坐标轴的刻度可视性
            //设置坐标轴1的刻度可视性
            m_plot->axisRect()->axis(m_xAxistype, i)->setTicks(true);//设true为可见,false为不可见
            m_plot->axisRect()->axis(m_yAxistype, i)->setTicks(true);

            //---3设置坐标轴的刻度标签可视性
            //设置坐标轴1的刻度标签可视性
            m_plot->axisRect()->axis(m_xAxistype, i)->setTickLabels(true);//设true为可见,false为不可见
            m_plot->axisRect()->axis(m_yAxistype, i)->setTickLabels(true);

            //    ---4设置坐标轴的子刻度可视性
            //    ---5设置坐标轴的子刻度标签可视性
        }


        //  --------共性属性4-----设置图元必要的初始值
        //设置坐标轴轴线颜色/宽度
        QPen penXAxis_temp,penYAxis_temp;
        penXAxis_temp.setColor(QColor(255,255,255));
        penXAxis_temp.setWidth(2);
        penYAxis_temp.setColor(QColor(255,255,255));
        penYAxis_temp.setWidth(2);
        //设置坐标轴   轴线颜色:白色,轴线宽度:2
        m_plot->axisRect()->axis(m_xAxistype, i)->setBasePen(penXAxis_temp);
        m_plot->axisRect()->axis(m_yAxistype, i)->setBasePen(penYAxis_temp);


        //设置刻度颜色
        QPen penTick_temp;
        penTick_temp.setColor(QColor(255,255,255));
        penTick_temp.setWidthF(1);
        //设置刻度  颜色:白色,宽度:1
        m_plot->axisRect()->axis(m_xAxistype, i)->setTickPen(penTick_temp);
        m_plot->axisRect()->axis(m_yAxistype, i)->setTickPen(penTick_temp);

        //设置刻度标签文字颜色
        m_plot->axisRect()->axis(m_xAxistype, i)->setTickLabelColor(QColor(255,255,255));
        m_plot->axisRect()->axis(m_yAxistype, i)->setTickLabelColor(QColor(255,255,255));

        //设置子刻度 颜色
        QPen penSubTick_temp;
        penSubTick_temp.setColor(QColor(255,255,255));
        penSubTick_temp.setWidthF(1);
        //设置子刻度 颜色:白色,宽度:1
        m_plot->axisRect()->axis(m_xAxistype, i)->setSubTickPen(penSubTick_temp);
        m_plot->axisRect()->axis(m_yAxistype, i)->setSubTickPen(penSubTick_temp);

        //设置x(或y)轴刻度小线段标签显示在坐标轴的哪一边
        //设置坐标Tick显示位置位于坐标轴外侧
        m_plot->axisRect()->axis(m_xAxistype, i)->setTickLabelSide(QCPAxis::lsOutside);//位于坐标轴内侧为QCPAxis::lsInside
        m_plot->axisRect()->axis(m_yAxistype, i)->setTickLabelSide(QCPAxis::lsOutside);

        //设置刻度标签的旋转（-90度到90度之间）
        double xAxisTickLabeldegrees =0;
        double yAxisTickLabeldegrees =0;
        m_plot->axisRect()->axis(m_xAxistype, i)->setTickLabelRotation(xAxisTickLabeldegrees);
        m_plot->axisRect()->axis(m_yAxistype, i)->setTickLabelRotation(yAxisTickLabeldegrees);


        //设置关闭各种自动分配刻度数和子刻度数（刻度数固定、子刻度数固定）
        //设true为自动,false为关闭自动
        m_plot->axisRect()->axis(m_xAxistype, i)->setAutoTicks(true); //设置关闭自动刻度
        m_plot->axisRect()->axis(m_yAxistype, i)->setAutoTickLabels(true);//设置关闭自动刻度标签
//        m_plot->axisRect()->axis(m_xAxistype, i)->setAutoTickStep(false); //设置自动刻度间距
//        m_plot->axisRect()->axis(m_yAxistype, i)->setAutoTickStep(false);//设置自动刻度间距

//        m_plot->axisRect()->axis(m_xAxistype, i)->setAutoTicks(false); //设置关闭自动刻度
//        m_plot->axisRect()->axis(m_yAxistype, i)->setAutoTickLabels(true);//设置关闭自动刻度标签
//        m_plot->axisRect()->axis(m_xAxistype, i)->setAutoTickStep(true); //设置自动刻度间距
//        m_plot->axisRect()->axis(m_yAxistype, i)->setAutoTickStep(true);//设置自动刻度间距

        //    void setAutoSubTicks(bool on);//设置自动子刻度标签
        //    void setAutoTickStep(bool on);//设置自动刻度间距
        //设置子刻度数
        qint32 subticknum =5;
        m_plot->axisRect()->axis(m_xAxistype, i)->setSubTickCount(subticknum);//设置子刻度数
        m_plot->axisRect()->axis(m_yAxistype, i)->setSubTickCount(subticknum);//设置子刻度数






        //以下为字体部分
        //    bool QFont::fromString(const QString & descrip)
        bool XAxislabelfonttempok;
        QFont XAxislabelfonttemp;
        XAxislabelfonttempok = XAxislabelfonttemp.fromString(m_vctgraphObj[i].m_XAxislabelFont_plot);
        m_plot->axisRect()->axis(m_xAxistype, i)->setLabelFont(XAxislabelfonttemp);//设置标签文字字体


        //y轴赋成与x轴一样的字体
        bool YAxislabelfonttempok;
        QFont YAxislabelfonttemp;
        YAxislabelfonttempok = YAxislabelfonttemp.fromString(m_vctgraphObj[i].m_XAxislabelFont_plot);
        m_plot->axisRect()->axis(m_yAxistype, i)->setLabelFont(YAxislabelfonttemp);//设置标签文字字体

        bool XAxisScalelabelfonttempok;
        QFont XAxisScalelabelfonttemp;
        XAxisScalelabelfonttempok = XAxisScalelabelfonttemp.fromString(m_vctgraphObj[i].m_XAxisScalelabelFont_plot);
        m_plot->axisRect()->axis(m_xAxistype, i)->setTickLabelFont(XAxisScalelabelfonttemp);//设置刻度标签文字字体

        bool YAxisScalelabelfonttempok;
        QFont YAxisScalelabelfonttemp;
        YAxisScalelabelfonttempok = YAxisScalelabelfonttemp.fromString(m_vctgraphObj[i].m_YAxisScalelabelFont_plot);
        m_plot->axisRect()->axis(m_yAxistype, i)->setTickLabelFont(YAxisScalelabelfonttemp);//设置刻度标签文字字体


        //坐标轴

        //设置坐标轴轴线颜色/宽度
        //y轴赋成与x轴一样的轴线颜色/宽度
        QPen penXAxis,penYAxis;
        qint32 m_XAxiswideth = m_vctgraphObj[i].m_XAxiswideth_plot;
        qint32 m_YAxiswideth = m_vctgraphObj[i].m_XAxiswideth_plot;
        QColor m_chooseXAxisColor,m_chooseYAxisColor;
        m_chooseXAxisColor.setNamedColor(m_vctgraphObj[i].m_chooseXAxisColor_plot);
        m_chooseYAxisColor.setNamedColor(m_vctgraphObj[i].m_chooseXAxisColor_plot);
        penXAxis.setColor(m_chooseXAxisColor);
        penXAxis.setWidth(m_XAxiswideth);
        penYAxis.setColor(m_chooseYAxisColor);
        penYAxis.setWidth(m_YAxiswideth);
        //设置坐标轴
        m_plot->axisRect()->axis(m_xAxistype, i)->setBasePen(penXAxis);
        m_plot->axisRect()->axis(m_yAxistype, i)->setBasePen(penYAxis);

        //轴标签、轴刻度标签的颜色全部统一成实时曲线的颜色；
        //设置轴标签文字颜色
        QColor m_chooseXAxisLabelColor,m_chooseYAxisLabelColor;
        m_chooseXAxisLabelColor.setNamedColor(m_vctgraphObj[i].m_chooseXAxisLabelColor_plot);
        m_chooseYAxisLabelColor.setNamedColor(m_vctgraphObj[i].m_chooseYAxisLabelColor_plot);
        //设置轴标签文字颜色
        m_plot->axisRect()->axis(m_xAxistype, i)->setLabelColor(m_chooseXAxisLabelColor);//设置标签颜色
        m_plot->axisRect()->axis(m_yAxistype, i)->setLabelColor(m_chooseYAxisLabelColor);//设置标签颜色



        //设置刻度标签文字颜色
        QColor m_chooseXAxisScaleLabelColor,m_chooseYAxisScaleLabelColor;
        m_chooseXAxisScaleLabelColor.setNamedColor(m_vctgraphObj[i].m_chooseXAxisScaleLabelColor_plot);
        m_chooseYAxisScaleLabelColor.setNamedColor(m_vctgraphObj[i].m_chooseYAxisScaleLabelColor_plot);
        //设置刻度标签文字颜色
        m_plot->axisRect()->axis(m_xAxistype, i)->setTickLabelColor(m_chooseXAxisScaleLabelColor);
        m_plot->axisRect()->axis(m_yAxistype, i)->setTickLabelColor(m_chooseYAxisScaleLabelColor);





        //设置轴轴刻度小线段,刻度标签的可见性
        //是否设置空白的轴线,包括: 轴轴刻度小线段,刻度标签
        //设置轴刻度小线段的可见性
        m_plot->axisRect()->axis(m_xAxistype, i)->setTicks(m_vctgraphObj[i].m_chooseXAxisScaleTickdisplay_plot);
        m_plot->axisRect()->axis(m_yAxistype, i)->setTicks(m_vctgraphObj[i].m_chooseYAxisScaleTickdisplay_plot);
        //设置刻度标签是否显示
        m_plot->axisRect()->axis(m_xAxistype, i)->setTickLabels(m_vctgraphObj[i].m_chooseXAxisScalelabeldisplay_plot);
        m_plot->axisRect()->axis(m_yAxistype, i)->setTickLabels(m_vctgraphObj[i].m_chooseYAxisScalelabeldisplay_plot);
        //设置轴标签的可见性
        //设置x(或y)轴轴标签名称(包括x(或y)轴单位)
        QString XAxisLabel,YAxisLabel;
        XAxisLabel = m_vctgraphObj[i].m_XAxisLabel_plot;
        YAxisLabel = m_vctgraphObj[i].m_YAxisLabel_plot;
        if(!(m_vctgraphObj[i].m_chooseXAxislabeldisplay_plot))
        {
            m_plot->axisRect()->axis(m_xAxistype, i)->setLabel(" ");
        }
        else
        {
            m_plot->axisRect()->axis(m_xAxistype, i)->setLabel(XAxisLabel);//x轴标签x
        }
        if(!(m_vctgraphObj[i].m_chooseYAxislabeldisplay_plot))
        {
            m_plot->axisRect()->axis(m_yAxistype, i)->setLabel(" ");
        }
        else
        {
            m_plot->axisRect()->axis(m_yAxistype, i)->setLabel(YAxisLabel);//y轴标签y
        }




        //获取整个配置范围
        //        double XAxisTickRange,YAxisTickRange;
        //配置范围是不断变化的

        XAxisTickRange = m_xUp-m_xDown;
        YAxisTickRange = m_yUp-m_yDown;
        //        XAxisTickRange = m_plot->axisRect()->axis(m_xAxistype, i)->range().size();
        //        YAxisTickRange = m_plot->axisRect()->axis(m_yAxistype, i)->range().size();

        //由获取的刻度数计算刻度间距
        qint32 XAxisnum,YAxisnum;
        XAxisnum = m_vctgraphObj[i].m_numOfXAxisScale_plot;
        YAxisnum = m_vctgraphObj[i].m_numOfYAxisScale_plot;
        //得到设置到曲线的刻度间距（比例尺）的初始值---还未对小数点进行处理
        double NumberOfXAxisScaleprecision_temp = XAxisTickRange/XAxisnum;
        double NumberOfYAxisScaleprecision_temp = YAxisTickRange/YAxisnum;
        //这个很关键，必须设置好，才能很好地控制小数点后面的位数，包括末尾补0!!!
        m_plot->axisRect()->axis(m_xAxistype, i)->setNumberFormat("fb");
        m_plot->axisRect()->axis(m_yAxistype, i)->setNumberFormat("fb");

        m_plot->axisRect()->axis(m_xAxistype, i)->setNumberPrecision(m_vctgraphObj[i].m_XAxisScaleprecision_plot);//设置x(或y)轴的刻度数字精度
        m_plot->axisRect()->axis(m_yAxistype, i)->setNumberPrecision(m_vctgraphObj[i].m_YAxisScaleprecision_plot);


        bool openXAxissetAutoTickCount,openYAxissetAutoTickCount;
        if(!(NumberOfXAxisScaleprecision_temp ==0))
        {
            openXAxissetAutoTickCount = false;
        }
        if(!(NumberOfXAxisScaleprecision_temp ==0))
        {
            openYAxissetAutoTickCount = false;
        }
        m_plot->axisRect()->axis(m_xAxistype, i)->setAutoTickStep(openXAxissetAutoTickCount);//设置刻度间距，开关函数setAutoTickStep()必须设置为false
        m_plot->axisRect()->axis(m_yAxistype, i)->setAutoTickStep(openYAxissetAutoTickCount);//设置刻度间距，开关函数setAutoTickStep()必须设置为false
        m_plot->axisRect()->axis(m_xAxistype, i)->setTickStep(NumberOfXAxisScaleprecision_temp);//设置x(或y)轴的刻度间距
        m_plot->axisRect()->axis(m_yAxistype, i)->setTickStep(NumberOfYAxisScaleprecision_temp);
        //返回x轴共分配了多少个刻度
        qint32 xAxisTickCount = m_plot->axisRect()->axis(m_xAxistype, i)->autoTickCount();
        //返回y轴共分配了多少个刻度
        qint32 yAxisTickCount = m_plot->axisRect()->axis(m_yAxistype, i)->autoTickCount();


        //获得设置刻度标签数字的有效位数，以对刻度文本偏移的自动设置提供帮助
        //先对刻度间距取整获得整数位数，再加上小数位数（精度值）即可
        //x轴
        QString str1 =QString::number((qint32)(NumberOfXAxisScaleprecision_temp));
        qint32 numxOfinteger = str1.length();
        qint32 numxofall = numxOfinteger + m_vctgraphObj[i].m_XAxisScaleprecision_plot+1;
        //y轴
        QString str2 =QString::number((qint32)(NumberOfYAxisScaleprecision_temp));
        qint32 numyOfinteger = str2.length();
        qint32 numyofall = numyOfinteger + m_vctgraphObj[i].m_YAxisScaleprecision_plot+1;

        //设置刻度文本偏移
        //设置刻度标签位移（单位为象素）
        //在这里从界面设置获取刻度标签的位移量（单位为象素），再在主类中调用私有类指针将位移量传输到私有类中
        // !!!!!这个特别重要！！！！
        //QCPAxisPainterPrivate *mAxisPainter;//画轴的父类指针
        //先将QString转化到double类型
        double XAxisScalelabeloffset_x =(m_vctgraphObj[i].m_XAxisScalelabeloffset_x_plot).toDouble();
        double XAxisScalelabeloffset_y =(m_vctgraphObj[i].m_XAxisScalelabeloffset_y_plot).toDouble();
        double YAxisScalelabeloffset_x =(m_vctgraphObj[i].m_YAxisScalelabeloffset_x_plot).toDouble();
        double YAxisScalelabeloffset_y =(m_vctgraphObj[i].m_YAxisScalelabeloffset_y_plot).toDouble();

        //再将界面设置值赋值到程序中
        //XAxisScalelabeloffset_x为刻度标签的位移量,numxofall刻度标签数字的有效位数(含小数点)
        m_plot->axisRect()->axis(m_xAxistype, i)->setXAxisTickLabeloffset_x(XAxisScalelabeloffset_x,numxofall);
        m_plot->axisRect()->axis(m_xAxistype, i)->setXAxisTickLabeloffset_y(XAxisScalelabeloffset_y,numxofall);
        m_plot->axisRect()->axis(m_yAxistype, i)->setYAxisTickLabeloffset_x(YAxisScalelabeloffset_x,numyofall);
        m_plot->axisRect()->axis(m_yAxistype, i)->setYAxisTickLabeloffset_y(YAxisScalelabeloffset_y,numyofall);



        //刻度、子刻度全部统一成轴颜色；
        //设置刻度颜色
        QPen penTick;
        QColor m_chooseXAxisTickColor;
        m_chooseXAxisTickColor.setNamedColor(m_vctgraphObj[i].m_chooseXAxisColor_plot);
        penTick.setColor(m_chooseXAxisTickColor);
        double Tickwidthf = (double)((m_vctgraphObj[i].m_XAxiswideth_plot)*0.618);
        penTick.setWidthF(Tickwidthf);//刻度小线段宽度的设置
        //设置刻度  颜色:白色,宽度:Tickwidthf
        m_plot->axisRect()->axis(m_xAxistype, i)->setTickPen(penTick);
        m_plot->axisRect()->axis(m_yAxistype, i)->setTickPen(penTick);

        //设置子刻度
        QPen penSubTick;
        QColor m_chooseXAxisSubTickColor;
        m_chooseXAxisSubTickColor.setNamedColor(m_vctgraphObj[i].m_chooseXAxisColor_plot);
        //设置子刻度颜色
        penSubTick.setColor(m_chooseXAxisSubTickColor);
        //设置子刻度小线段粗细为刻度小线段粗细的0.618倍
        double SubTickWidthf = Tickwidthf *0.618;
        penSubTick.setWidthF(SubTickWidthf);
        //设置子刻度 颜色:白色,宽度:SubTickWidthf
        m_plot->axisRect()->axis(m_xAxistype, i)->setSubTickPen(penSubTick);
        m_plot->axisRect()->axis(m_yAxistype, i)->setSubTickPen(penSubTick);



        //设置x(或y)轴刻度小线段标签显示在坐标轴的哪一边
        //设置坐标刻度小线段标签显示于坐标轴外侧
        if(QString(m_vctgraphObj[i].m_Scalelabelplace_x_plot)==QString("内侧"))
        {
            m_plot->axisRect()->axis(m_xAxistype, i)->setTickLabelSide(QCPAxis::lsInside);//位于坐标轴内侧为QCPAxis::lsInside
        }
        else
        {
            m_plot->axisRect()->axis(m_xAxistype, i)->setTickLabelSide(QCPAxis::lsOutside);//位于坐标轴外侧为QCPAxis::lsOutside

        }

        if(QString(m_vctgraphObj[i].m_Scalelabelplace_y_plot)==QString("内侧"))
        {
            m_plot->axisRect()->axis(m_yAxistype, i)->setTickLabelSide(QCPAxis::lsInside);//位于坐标轴内侧为QCPAxis::lsInside
        }
        else
        {
            m_plot->axisRect()->axis(m_yAxistype, i)->setTickLabelSide(QCPAxis::lsOutside);//位于坐标轴外侧为QCPAxis::lsOutside

        }


        //设置刻度小线段位置
        //刻度小线段长度的设置
        //qint32 m_XAxisScaleRuler_plot ;
        //qint32 m_YAxisScaleRuler_plot ;
        //设置刻度小线段长度:向里伸出多少，向外伸出多少
        qint32 XAxisTickLengthinside,XAxisTickLengthoutside;
        qint32 YAxisTickLengthinside,YAxisTickLengthoutside;
        if(QString(m_vctgraphObj[i].m_Scaleplace_x_plot)==QString("上"))
        {
            XAxisTickLengthinside = m_vctgraphObj[i].m_XAxisScaleRuler_plot;
            XAxisTickLengthoutside=0;
        }
        else if(QString(m_vctgraphObj[i].m_Scaleplace_x_plot)==QString("中"))
        {
            XAxisTickLengthinside=(m_vctgraphObj[i].m_XAxisScaleRuler_plot)/2;
            XAxisTickLengthoutside=(m_vctgraphObj[i].m_XAxisScaleRuler_plot)/2;
        }
        else                                             //"下"
        {
            XAxisTickLengthinside = 0;
            XAxisTickLengthoutside = m_vctgraphObj[i].m_XAxisScaleRuler_plot;
        }

        if(QString(m_vctgraphObj[0].m_Scaleplace_y_plot)==QString("右"))
        {
            YAxisTickLengthinside = m_vctgraphObj[i].m_YAxisScaleRuler_plot;
            YAxisTickLengthoutside=0;
        }
        else if(QString(m_vctgraphObj[i].m_Scaleplace_y_plot)==QString("中"))
        {
            YAxisTickLengthinside = (m_vctgraphObj[i].m_YAxisScaleRuler_plot)/2;
            YAxisTickLengthoutside=(m_vctgraphObj[i].m_YAxisScaleRuler_plot)/2;
        }
        else                                          //"左"
        {
            YAxisTickLengthinside = 0;
            YAxisTickLengthoutside = m_vctgraphObj[i].m_YAxisScaleRuler_plot;
        }

        m_plot->axisRect()->axis(m_xAxistype, i)->setTickLengthIn(XAxisTickLengthinside);
        m_plot->axisRect()->axis(m_xAxistype, i)->setTickLengthOut(XAxisTickLengthoutside);
        m_plot->axisRect()->axis(m_yAxistype, i)->setTickLengthIn(YAxisTickLengthinside);
        m_plot->axisRect()->axis(m_yAxistype, i)->setTickLengthOut(YAxisTickLengthoutside);
        //子刻度的小线段长度设置为刻度的小线段长度的0.618倍
        m_plot->axisRect()->axis(m_xAxistype, i)->setSubTickLengthIn(XAxisTickLengthinside*0.618);
        m_plot->axisRect()->axis(m_xAxistype, i)->setSubTickLengthOut(XAxisTickLengthoutside*0.618);
        m_plot->axisRect()->axis(m_yAxistype, i)->setSubTickLengthIn(YAxisTickLengthinside*0.618);
        m_plot->axisRect()->axis(m_yAxistype, i)->setSubTickLengthOut(YAxisTickLengthoutside*0.618);

        //对原点位于轴矩形区正中心的情况进行轴位移
        if((QString(m_vctgraphObj[i].m_OriginPlace_plot)==QString("正中")))
        {

            //-----计算原点位于轴矩形区正中心的情况
            //计算原点移动导致的轴位移量(以像素为单位是整数)
            qint32 yAxis_offset = (qint32)((m_plot->axisRect()->width())/2);
            qint32 xAxis_offset = (qint32)((m_plot->axisRect()->height())/2);
            //设置x轴、y轴的轴位移量
//            m_plot->axisRect()->axis(m_xAxistype, i)->setOffset(-xAxis_offset);
//            m_plot->axisRect()->axis(m_yAxistype, i)->setOffset(-yAxis_offset);
            m_plot->axisRect()->axis(m_xAxistype, i)->setOffset(0);
            m_plot->axisRect()->axis(m_yAxistype, i)->setOffset(0);
//            m_plot->axisRect()->axis(m_xAxistype, i)->setTickLabelSide(QCPAxis::lsInside);//位于坐标轴内侧为QCPAxis::lsInside
//            m_plot->axisRect()->axis(m_yAxistype, i)->setTickLabelSide(QCPAxis::lsInside);//位于坐标轴内侧为QCPAxis::lsInside
            XAxisTickLengthinside=5;
            XAxisTickLengthoutside=5;
            YAxisTickLengthinside = 5;
            YAxisTickLengthoutside=5;
            //将原点设为正中的情况传送到qcustomplot的配置中
            m_plot->axisRect()->axis(m_xAxistype,i)->setCenterOrigin(true);
            m_plot->axisRect()->axis(m_yAxistype,i)->setCenterOrigin(true);
        }
        else
        {
            //其他情况都清除原点位于正中的标志,传送到qcustomplot的配置中
            m_plot->axisRect()->axis(m_xAxistype,i)->setCenterOrigin(false);
            m_plot->axisRect()->axis(m_yAxistype,i)->setCenterOrigin(false);
        }
        m_plot->axisRect()->axis(m_xAxistype, i)->setTickLengthIn(XAxisTickLengthinside);
        m_plot->axisRect()->axis(m_xAxistype, i)->setTickLengthOut(XAxisTickLengthoutside);
        m_plot->axisRect()->axis(m_yAxistype, i)->setTickLengthIn(YAxisTickLengthinside);
        m_plot->axisRect()->axis(m_yAxistype, i)->setTickLengthOut(YAxisTickLengthoutside);
        //子刻度的小线段长度设置为刻度的小线段长度的0.618倍
        m_plot->axisRect()->axis(m_xAxistype, i)->setSubTickLengthIn(XAxisTickLengthinside*0.618);
        m_plot->axisRect()->axis(m_xAxistype, i)->setSubTickLengthOut(XAxisTickLengthoutside*0.618);
        m_plot->axisRect()->axis(m_yAxistype, i)->setSubTickLengthIn(YAxisTickLengthinside*0.618);
        m_plot->axisRect()->axis(m_yAxistype, i)->setSubTickLengthOut(YAxisTickLengthoutside*0.618);



        //设置所有坐标轴的可见性
        bool XAxisVisible,YAxisVisible;
        if(m_vctgraphObj[0].m_XAxisdisplay_plot)
        {
            XAxisVisible=true;
        }
        else
        {
            XAxisVisible=false;
        }
        if(m_vctgraphObj[i].m_YAxisdisplay_plot)
        {
            YAxisVisible=true;
        }
        else
        {
            YAxisVisible=false;
        }
        //设置坐标轴可视性-----最后根据设置确定坐标轴的可见性
        //包括：两套默认坐标轴的基本轴、刻度、刻度标签、子刻度、子刻度标签
        //---1设置坐标轴的基本轴可视性
        //设置坐标轴1的基本轴可视性
        if((!(m_plot->axisRect()->axis(m_xAxistype, i)==NULL))&&(!(m_plot->axisRect()->axis(m_yAxistype, i)==NULL)))
        {
            m_plot->axisRect()->axis(m_xAxistype, i)->setVisible(XAxisVisible);//默认坐标轴1的x轴的基本轴,设true为可见,false为不可见
            m_plot->axisRect()->axis(m_yAxistype, i)->setVisible(YAxisVisible);//默认坐标轴1的y轴的基本轴,设1为可见,false为不可见
            //将不是所选轴类型的基本轴可视性设为false
            if(!(m_plot->axisRect()->axis(m_xAxistype, i)==NULL))
                if ((m_xAxistype == QCPAxis::atTop)&&(m_yAxistype == QCPAxis::atLeft))
                {
                    if(!(m_plot->axisRect()->axis(QCPAxis::atBottom, i)==NULL))
                    {
                        m_plot->axisRect()->axis(QCPAxis::atBottom, i)->setVisible(false);//设true为可见,false为不可见
                        //                m_plot->axisRect()->removeAxis(m_plot->axisRect()->axis(QCPAxis::atBottom, i));
                    }
                    if(!(m_plot->axisRect()->axis(QCPAxis::atRight, i)==NULL))
                    {
                        m_plot->axisRect()->axis(QCPAxis::atRight, i)->setVisible(false);
                        //                m_plot->axisRect()->removeAxis(m_plot->axisRect()->axis(QCPAxis::atRight, i));
                    }

                }
                else if((m_xAxistype == QCPAxis::atBottom)&&(m_yAxistype == QCPAxis::atRight))
                {
                    if(!(m_plot->axisRect()->axis(QCPAxis::atTop, i)==NULL))
                    {
                        m_plot->axisRect()->axis(QCPAxis::atTop, i)->setVisible(false);
                        //                m_plot->axisRect()->removeAxis(m_plot->axisRect()->axis(QCPAxis::atTop, i));
                    }
                    if(!(m_plot->axisRect()->axis(QCPAxis::atLeft, i)==NULL))
                    {
                        m_plot->axisRect()->axis(QCPAxis::atLeft, i)->setVisible(false);
                        //                m_plot->axisRect()->removeAxis(m_plot->axisRect()->axis(QCPAxis::atLeft, i));
                    }
                }
                else if((m_xAxistype == QCPAxis::atTop)&&(m_yAxistype == QCPAxis::atRight))
                {
                    if(!(m_plot->axisRect()->axis(QCPAxis::atBottom, i)==NULL))
                    {
                        m_plot->axisRect()->axis(QCPAxis::atBottom, i)->setVisible(false);
                        //                m_plot->axisRect()->removeAxis(m_plot->axisRect()->axis(QCPAxis::atBottom, i));
                    }
                    if(!(m_plot->axisRect()->axis(QCPAxis::atLeft, i)==NULL))
                    {
                        m_plot->axisRect()->axis(QCPAxis::atLeft, i)->setVisible(false);
                        //                m_plot->axisRect()->removeAxis(m_plot->axisRect()->axis(QCPAxis::atLeft, i));
                    }
                }
                else if((m_xAxistype == QCPAxis::atBottom)&&(m_yAxistype == QCPAxis::atLeft))
                {
                    if(!(m_plot->axisRect()->axis(QCPAxis::atTop, i)==NULL))
                    {
                        m_plot->axisRect()->axis(QCPAxis::atTop, i)->setVisible(false);
                        //                m_plot->axisRect()->removeAxis(m_plot->axisRect()->axis(QCPAxis::atTop, i));
                    }
                    if(!(m_plot->axisRect()->axis(QCPAxis::atRight, i)==NULL))
                    {
                        m_plot->axisRect()->axis(QCPAxis::atRight, i)->setVisible(false);
                        //                m_plot->axisRect()->removeAxis(m_plot->axisRect()->axis(QCPAxis::atRight, i));
                    }
                }
        }


        //实时曲线
        //设置实时曲线名称
        QString graphname = m_vctgraphObj[i].m_GraphName_plot;
        m_vctgraphObj[i].m_pgraph->setName(graphname);//显示在图例上的名称

        //设置实时曲线宽度/颜色
        qint32 m_graphWidth;
        QColor m_graphColor;

        //设置理论曲线风格:直线/散点
        QCPGraph::LineStyle GraphStyle;
        //设置理论特殊显示----显示为散点
        GraphStyle=QCPGraph::lsNone;
        m_vctgraphObj[i].m_pgraph->setLineStyle(GraphStyle);

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


        //绘制当前点，以黄色绘制宝石形状---闪烁点
        m_vctgraphObj[i].m_pgraphLast->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDiamond, Qt::yellow, m_vctgraphObj[i].m_GraphWidth_plot+6));



        //理论曲线

        //设置理论曲线名称
        QString graphname_temp = m_vctgraphObj[i].m_GraphName_plot;
        QString Lgraphname = QString("%1%2").arg(graphname_temp).arg(tr("底图"));
        m_vctgraphObj[i].m_pLgraph->setName(Lgraphname);//显示在图例上的名称

        //设置理论曲线宽度/颜色
        qint32 m_LgraphWidth;
        QColor m_LgraphColor;

        //设置理论曲线风格:直线/散点
        QCPGraph::LineStyle LGraphStyle;
        //设置理论特殊显示----显示为散点
        LGraphStyle=QCPGraph::lsNone;
        m_vctgraphObj[i].m_pLgraph->setLineStyle(LGraphStyle);

        m_LgraphWidth=m_vctgraphObj[i].m_LGraphWidth_plot;
        m_LgraphColor.setNamedColor(m_vctgraphObj[i].m_strLgraphColor_plot);
        m_vctgraphObj[i].m_pLgraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, m_LgraphColor,m_LgraphWidth));


        //设置理论曲线缓冲区大小
        double m_LGraphBuffer;
        m_LGraphBuffer = m_vctgraphObj[i].m_LGraphBuffer_plot;

        m_plot->replot();

        //设置理论曲线数据(文件(路径))

        /*
        //设置实时曲线风格
        //散点
        //设置显示实时曲线
//        m_vctgraphObj[i].m_pgraph->setVisible(true);
        //设置曲线风格:直线/散点
        QCPGraph::LineStyle GraphStyle;
        //设置特殊显示----显示为散点
        GraphStyle=QCPGraph::lsNone;
//        //设置特殊显示----显示为直线
//        GraphStyle=QCPGraph::lsLine;

        if(GraphStyle==QCPGraph::lsNone)//散点
        {
            GraphStyle=QCPGraph::lsNone;

            QCPScatterStyle scatterscatterStyle;//设置点散点形式
            //散点大小
            scatterscatterStyle.setSize(m_graphWidth);
            //散点画笔
            QPen ScatterStylepen;
            ScatterStylepen.setColor(m_graphColor);
            ScatterStylepen.setWidth(1);
            scatterscatterStyle.setPen(ScatterStylepen);
            //散点画刷
            QBrush ScatterStylebrush;
            ScatterStylebrush.setColor(m_graphColor);
            scatterscatterStyle.setBrush(ScatterStylebrush);
            //散点形状
            QCPScatterStyle::ScatterShape scatterShape;
            scatterShape=QCPScatterStyle::ssDiamond;
            //散点形状:QCPScatterStyle::ssDiamond(宝石型)，
            //scatterShape(散点形状)包括以下种类：
            //     ssNone,ssDot,ssCross,ssPlus,ssCircle,ssDisc,ssSquare,ssDiamond,
            //     ssStar,ssTriangle,,ssTriangleInverted,,ssCrossSquare,ssPlusSquare,
            //     ssCrossCircle,ssPlusCircle,ssPeace,ssPixmap,ssCustom
            scatterscatterStyle.setShape(scatterShape);

            m_vctgraphObj[i].m_pgraph->setScatterStyle(scatterscatterStyle);
        }
        else//直线连线
        {
            //LineStyle:lsNone,lsLine,lsStepLeft,lsStepRight,lsStepCenter,lsImpulse
            GraphStyle=QCPGraph::lsLine;
            m_vctgraphObj[i].m_pgraph->setLineStyle(GraphStyle);

            QCPScatterStyle linescatterStyle;//设置直线点形式
            //直线点点大小
            linescatterStyle.setSize(m_graphWidth);
            //直线点点画笔
            QPen ScatterStylepen;
            ScatterStylepen.setColor(m_graphColor);
            ScatterStylepen.setWidth(1);
            linescatterStyle.setPen(ScatterStylepen);
            //直线点点画刷
            QBrush ScatterStylebrush;
            ScatterStylebrush.setColor(m_graphColor);
            linescatterStyle.setBrush(ScatterStylebrush);

            m_vctgraphObj[i].m_pgraph->setScatterStyle(linescatterStyle);

        }

//        //设置理论曲线风格
//        //散点
//        //设置显示理论曲线
////        m_vctgraphObj[i].m_pLgraph->setVisible(true);
//        //设置曲线风格:直线/散点
//        QCPGraph::LineStyle LGraphStyle;
//        //设置特殊显示----显示为直线
//        LGraphStyle=QCPGraph::lsLine;

//        if(LGraphStyle==QCPGraph::lsNone)//散点
//        {
//            LGraphStyle=QCPGraph::lsNone;

//            QCPScatterStyle LscatterscatterStyle;//设置点散点形式
//            //散点大小
//            LscatterscatterStyle.setSize(m_LgraphWidth);
//            //散点画笔
//            QPen LScatterStylepen;
//            LScatterStylepen.setColor(m_LgraphColor);
//            LScatterStylepen.setWidth(1);
//            LscatterscatterStyle.setPen(LScatterStylepen);
//            //散点画刷
//            QBrush LScatterStylebrush;
//            LScatterStylebrush.setColor(m_LgraphColor);
//            LscatterscatterStyle.setBrush(LScatterStylebrush);
//            //散点形状
//            QCPScatterStyle::ScatterShape LscatterShape;
//            LscatterShape=QCPScatterStyle::ssDiamond;
//            //散点形状:QCPScatterStyle::ssDiamond(宝石型)，
//            //scatterShape(散点形状)包括以下种类：
//            //     ssNone,ssDot,ssCross,ssPlus,ssCircle,ssDisc,ssSquare,ssDiamond,
//            //     ssStar,ssTriangle,,ssTriangleInverted,,ssCrossSquare,ssPlusSquare,
//            //     ssCrossCircle,ssPlusCircle,ssPeace,ssPixmap,ssCustom
//            LscatterscatterStyle.setShape(LscatterShape);

//            m_vctgraphObj[i].m_pLgraph->setScatterStyle(LscatterscatterStyle);
//        }
//        else//直线连线
//        {
//            //LineStyle:
//            //lsNone(点),lsLine(普通线),lsImpulse(以点画脉冲),
//            //lsStepLeft(点位于台阶左边的连线),
//            //lsStepRight(点位于台阶右边的连线),
//            //lsStepCenter(点位于台阶中间的连线),
//            LGraphStyle=QCPGraph::lsLine;
//            m_vctgraphObj[i].m_pLgraph->setLineStyle(LGraphStyle);

//            QCPScatterStyle LlinescatterStyle;//设置直线点形式
//            //直线点点大小
//            LlinescatterStyle.setSize(m_LgraphWidth);
//            //直线点点画笔
//            QPen LScatterStylepen;
//            LScatterStylepen.setColor(m_LgraphColor);
//            LScatterStylepen.setWidth(1);
//            LlinescatterStyle.setPen(LScatterStylepen);
//            //直线点点画刷
//            QBrush LScatterStylebrush;
//            LScatterStylebrush.setColor(m_LgraphColor);
//            LlinescatterStyle.setBrush(LScatterStylebrush);

//            m_vctgraphObj[i].m_pLgraph->setScatterStyle(LlinescatterStyle);

//        }

*/

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
    //        m_plot->axisRect()->axis(m_xAxistype, i)->setSelectedLabelFont();
    //        m_plot->axisRect()->axis(m_xAxistype, i)->setSelectedBasePen();
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

            m_vctgraphObj[i].m_CxLast.clear();
            m_vctgraphObj[i].m_CxLast.append(tx);
        }
        //把取到的全部y的值存入数组m_Cy
        foreach (double ty, y) {
            m_vctgraphObj[i].m_Cy.append(ty);//在容器m_Cy末尾添加ty

            m_vctgraphObj[i].m_CyLast.clear();
            m_vctgraphObj[i].m_CyLast.append(ty);
        }
        //把曲线的值设置为刚才的两个数组
        m_vctgraphObj[i].m_pgraph->setData(m_vctgraphObj[i].m_Cx, m_vctgraphObj[i].m_Cy);//键值，值
        m_vctgraphObj[i].m_pgraphLast->setData(m_vctgraphObj[i].m_CxLast, m_vctgraphObj[i].m_CyLast);

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
//            支持特殊显示(如:直线连线、钻石形刻度等)、曲线缓冲区;
//          (2) 目标增减、应用QTabWidget类实现多曲线配置中配置相关函数的添加
//          (3) 实现多曲线配置对话框的配置数据的封装（json生成），并实现向主类的数据传输;
//               及在主类的Json数据解析（json解析）,并赋值到到曲线类中。

/*****其他需求*******/
// staticgraph工程剩余需求：
//1 原始坐标体系。  (右键可选择点击：在实时模式进行了拖动、缩放之后，右键可以选择单击回复到一开始的定制画面。)
//2 自适应数据。   （右键可选择点击：找出x、y数据各自的最大值最小值，把这个作出的矩形放到画面中间）
//3 T0获取。      (T0获取并根据T0(靶弹，未定义，拦截弹）计算相对时弹道及清屏。)
//4 样式显示。     (曲线按照某种样式进行显示，如某个区间显示某种颜色，某种刻度，大小等。)(增加到属性中)
//5 显示模式。     (固定显示模式（在staticgraph工程）、滚动显示模式（在rollinggraph工程）。)
//6 界面修改。    （双击修改界面部分简单参数配置的功能。）


