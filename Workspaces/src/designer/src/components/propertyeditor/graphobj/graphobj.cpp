#include "graphobj.h"
#include "ui_graphobj.h"
#include "graphobjdlg.h"


#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>


QT_BEGIN_NAMESPACE

GraphObj::GraphObj()
{
    ui = new Ui::GraphObj;
    ui->setupUi(this);
    setWindowTitle(tr("多曲线配置"));

}

GraphObj::~GraphObj()
{
    delete ui;
}
//左移曲线
void GraphObj::on_pushButton_leftmove_clicked()
{
    //先做判断，至少已拥有2条及2条以上曲线
     qint32 numOfgraph = ui->tabWidget->count();//获取曲线总数
     if(numOfgraph>=2)
     {
         //第1步
         //先把当前曲线及前1条曲线的索引号存起来
         qint32 currentindex;
         currentindex= ui->tabWidget->currentIndex();//当前曲线索引号
         if(currentindex>=1)
         {
             //第2步
             //取到current窗口
             GraphObjDlg * t_current = (GraphObjDlg*)(ui->tabWidget->widget(currentindex));
             //标注添加曲线的tab名称
             QString strtabName;//标注添加曲线的tab名称---不带索引
             strtabName = (t_current->m_GraphName);
             //获取插入曲线的位置，在当前曲线相邻曲线的左侧
             qint32 insertgraphindexplace = currentindex-1;
             //添加tab
             ui->tabWidget->insertTab(insertgraphindexplace,t_current, strtabName);//插入曲线的tab标签名字
             //设置为当前tab
             ui->tabWidget->setCurrentWidget(t_current);
         }

     }

}


//右移曲线
void GraphObj::on_pushButton_rightmove_clicked()
{
    //先做判断，至少已拥有2条及2条以上曲线
    qint32 numOfgraph = ui->tabWidget->count();//获取曲线总数
    if(numOfgraph>=2)
    {
        //第1步
        //先把当前曲线及前1条曲线的索引号存起来
        qint32 currentindex;
        currentindex= ui->tabWidget->currentIndex();//当前曲线索引号
        //第2步
        //取到current窗口
        GraphObjDlg * t_current = (GraphObjDlg*)(ui->tabWidget->widget(currentindex));
        //标注添加曲线的tab名称
        QString strtabName;//标注添加曲线的tab名称---不带索引
        strtabName = (t_current->m_GraphName);
        //获取插入曲线的位置，在当前曲线相邻曲线的右侧
        qint32 insertgraphindexplace = currentindex+1;
        //添加tab
        ui->tabWidget->insertTab(insertgraphindexplace,t_current, strtabName);//插入曲线的tab标签名字
        //设置为当前tab
        ui->tabWidget->setCurrentWidget(t_current);

    }

}

//添加新曲线
void GraphObj::on_pushButton_AddNewObj_clicked()
{
    //将所有当前页面数据保存为一个Json对象，用于在每次插入tab时加载用以构造
    QJsonObject obj;

    //情况1：最原始的情况，什么曲线都没有的情况,一条新曲线赋初值
    //啥都不干，彻底为空，直接让Graphobjdlg的构造函数的jobj.isEmpty()部分的代码赋初值
    if(ui->tabWidget->count()==0)
    {}
    else  //情况2：此前就有存在的曲线，这里再插入一条新曲线;
    {
        //保存当前页面所有标签的数据
        //用GraphObjDlg * tt取到当前页面的指针（地址）(已配好的数据)
        GraphObjDlg * tt = (GraphObjDlg*)(ui->tabWidget->widget(ui->tabWidget->currentIndex()));

        //下面在该Json对象中插入各个要保存的属性
        obj.insert("Xmax",tt->m_Xmax);//第1
        obj.insert("Xmin",tt->m_Xmin);//第2
        obj.insert("Ymax",tt->m_Ymax);//第3
        obj.insert("Ymin",tt->m_Ymin);//第4
        obj.insert("XAxisLabel",tt->m_XAxisLabel);//第7
        obj.insert("YAxisLabel",tt->m_YAxisLabel);//第8
        obj.insert("XAxisdisplay",tt->m_XAxisdisplay);//第9
        obj.insert("YAxisdisplay",tt->m_YAxisdisplay);//第10
        obj.insert("Scaleplace_x",tt->m_Scaleplace_x);//第11
        obj.insert("Scaleplace_y",tt->m_Scaleplace_y);//第12
        obj.insert("OriginPlace",tt->m_OriginPlace);//第12
        obj.insert("GraphName",tt->m_GraphName);//第13
        obj.insert("GraphWidth",tt->m_GraphWidth);//第14
        obj.insert("strgraphColor",tt->m_strgraphColor);//第16
        obj.insert("LGraphWidth",tt->m_LGraphWidth);//第17
        obj.insert("strLgraphColor",tt->m_strLgraphColor);//第19
        obj.insert("xParam",tt->m_xParam);//第20
        obj.insert("yParam",tt->m_yParam);//第21
        obj.insert("Lgraphfile",tt->m_Lgraphfile);//第22
        obj.insert("XAxiswideth",tt->m_XAxiswideth);//第23
        obj.insert("chooseXAxisColor",tt->m_chooseXAxisColor);//第25
        obj.insert("chooseXAxislabeldisplay",tt->m_chooseXAxislabeldisplay);//第31
        obj.insert("chooseYAxislabeldisplay",tt->m_chooseYAxislabeldisplay);//第32
        obj.insert("chooseXAxisScalelabeldisplay",tt->m_chooseXAxisScalelabeldisplay);//第33
        obj.insert("chooseYAxisScalelabeldisplay",tt->m_chooseYAxisScalelabeldisplay);//第34
        obj.insert("chooseXAxisScaleTickdisplay",tt->m_chooseXAxisScaleTickdisplay);//第35
        obj.insert("chooseYAxisScaleTickdisplay",tt->m_chooseYAxisScaleTickdisplay);//第36
        obj.insert("XAxislabelFont",tt->m_XAxislabelFont);//第37
        obj.insert("XAxisScalelabelFont",tt->m_XAxisScalelabelFont);//第39
        obj.insert("YAxisScalelabelFont",tt->m_YAxisScalelabelFont);//第40
        obj.insert("numOfXAxisScale",tt->m_numOfXAxisScale);//第41
        obj.insert("XAxisScaleRuler",tt->m_XAxisScaleRuler);//第42
        obj.insert("numOfYAxisScale",tt->m_numOfYAxisScale);//第43
        obj.insert("YAxisScaleRuler",tt->m_YAxisScaleRuler);//第44
        obj.insert("XAxisScaleprecision",tt->m_XAxisScaleprecision);//第45
        obj.insert("YAxisScaleprecision",tt->m_YAxisScaleprecision);//第46
        obj.insert("XAxisScalelabeloffset_x",tt->m_XAxisScalelabeloffset_x);//第47
        obj.insert("XAxisScalelabeloffset_y",tt->m_XAxisScalelabeloffset_y);//第48
        obj.insert("YAxisScalelabeloffset_x",tt->m_YAxisScalelabeloffset_x);//第47
        obj.insert("YAxisScalelabeloffset_y",tt->m_YAxisScalelabeloffset_y);//第48
    }

    //使用配好的Json对象，进行新的页面的创建
    GraphObjDlg * t = new GraphObjDlg(obj, this);

    //标注添加曲线的tab名称
    QString strtabName;//标注添加曲线的tab名称---不带索引
    strtabName = (t->m_GraphName);


    qint32 currentgraphindex;
    currentgraphindex = ui->tabWidget->currentIndex();
    //获取插入曲线的位置，在当前曲线相邻的右侧
    qint32 insertgraphindexplace = currentgraphindex+1;
    //添加tab
    ui->tabWidget->insertTab(insertgraphindexplace,t, strtabName);//插入曲线的tab标签名字
    //设置为当前tab
    ui->tabWidget->setCurrentWidget(t);

    //获取曲线总数，填充到曲线统计文本框
    qint32 numOftab=ui->tabWidget->count();
    QString numOftabstr = QString::number(numOftab);
    ui->lineEdit_numofgraph->setText(numOftabstr);

    update();
}

//删除当前曲线
void GraphObj::on_pushButton_DelCurrentObj_clicked()
{
    int currentIndex = ui->tabWidget->currentIndex();
    ui->tabWidget->removeTab(currentIndex);

    //获取曲线总数，填充到曲线统计文本框
    qint32 numOftab=ui->tabWidget->count();
    QString numOftabstr = QString::number(numOftab);
    ui->lineEdit_numofgraph->setText(numOftabstr);
}

//输出json字符串
QString GraphObj::transText()
{
    return m_jsonStr;
}

//打开对话框
void GraphObj::Show()
{
    show();
    exec();
}

//以字符串初始化控件
void GraphObj::InitControl(QString str)
{

    //先删除所有标签
    for(int i=0; i<ui->tabWidget->count(); i++)
    {
        ui->tabWidget->removeTab(i);
    }

    //错误信息
    QJsonParseError jerr;

    //取得json文档
    QJsonDocument json_doc = QJsonDocument::fromJson(str.toUtf8(), &jerr);

    //如果没有发生错误，则开始解析
    if(jerr.error == QJsonParseError::NoError)
    {
        //空文档
        if(json_doc.isEmpty())
        {
            qDebug()<<"Empty json";
        }

        //数组文档(因为多目标，所以必须是数组)
        if(json_doc.isArray())
        {
            //取得数组
            QJsonArray array = json_doc.array();

            //数组的大小
            int size = array.size();

            //获取曲线总数，填充到曲线统计文本框
            qint32 numOftab=size;
            QString numOftabstr = QString::number(numOftab);
            ui->lineEdit_numofgraph->setText(numOftabstr);
            update();

            //循环获取
            for(int i=0;i<size;i++)
            {
                //取得一个数组的值
                QJsonValue value = array.at(i);

                //如果值是一个对象
                if(value.isObject())
                {
                    //转换成json对象
                    QJsonObject name = value.toObject();

                    //下面开始挨个解析

                    GraphObjDlg * t = new GraphObjDlg(name, this);

                    //目标名称
                    t->m_GraphName = name["GraphName"].toString();

                    //添加标签
                    qint32 index = ui->tabWidget->addTab(t, t->m_GraphName);

                    //设置属性分栏标题的文字
                    ui->tabWidget->setTabText(index, t->m_GraphName);


                }
            }
        }
    }
    else
    {
        qDebug()<<"error in GraphObj::InitControl : " + jerr.errorString();
    }
}






//点击 确定
void GraphObj::on_pushButton_SaveQuit_clicked()
{
    //Json数组
    QJsonArray stateArray;

    //保存所有标签的数据
    for(int i=0; i<ui->tabWidget->count(); i++)
    {
        GraphObjDlg * t = (GraphObjDlg*)(ui->tabWidget->widget(i));

        QJsonObject stateObject;

        //下面在json中插入各个要保存的属性

//        stateObject.insert("GraphName", t->m_GraphName);

        stateObject.insert("Xmax",t->m_Xmax);//第1
        stateObject.insert("Xmin",t->m_Xmin);//第2
        stateObject.insert("Ymax",t->m_Ymax);//第3
        stateObject.insert("Ymin",t->m_Ymin);//第4
        stateObject.insert("XAxisLabel",t->m_XAxisLabel);//第7
        stateObject.insert("YAxisLabel",t->m_YAxisLabel);//第8
        stateObject.insert("XAxisdisplay",t->m_XAxisdisplay);//第9
        stateObject.insert("YAxisdisplay",t->m_YAxisdisplay);//第10
        stateObject.insert("Scaleplace_x",t->m_Scaleplace_x);//第11
        stateObject.insert("Scaleplace_y",t->m_Scaleplace_y);//第12
        stateObject.insert("OriginPlace",t->m_OriginPlace);//第12
        stateObject.insert("GraphName",t->m_GraphName);//第13
        stateObject.insert("GraphWidth",t->m_GraphWidth);//第14
        stateObject.insert("strgraphColor",t->m_strgraphColor);//第16
        stateObject.insert("LGraphWidth",t->m_LGraphWidth);//第17
        stateObject.insert("strLgraphColor",t->m_strLgraphColor);//第19
        stateObject.insert("xParam",t->m_xParam);//第20
        stateObject.insert("yParam",t->m_yParam);//第21
        stateObject.insert("Lgraphfile",t->m_Lgraphfile);//第22
        stateObject.insert("XAxiswideth",t->m_XAxiswideth);//第23
        stateObject.insert("chooseXAxisColor",t->m_chooseXAxisColor);//第25
        stateObject.insert("chooseXAxislabeldisplay",t->m_chooseXAxislabeldisplay);//第31
        stateObject.insert("chooseYAxislabeldisplay",t->m_chooseYAxislabeldisplay);//第32
        stateObject.insert("chooseXAxisScalelabeldisplay",t->m_chooseXAxisScalelabeldisplay);//第33
        stateObject.insert("chooseYAxisScalelabeldisplay",t->m_chooseYAxisScalelabeldisplay);//第34
        stateObject.insert("chooseXAxisScaleTickdisplay",t->m_chooseXAxisScaleTickdisplay);//第35
        stateObject.insert("chooseYAxisScaleTickdisplay",t->m_chooseYAxisScaleTickdisplay);//第36

        stateObject.insert("XAxislabelFont",t->m_XAxislabelFont);//第37
        stateObject.insert("XAxisScalelabelFont",t->m_XAxisScalelabelFont);//第39
        stateObject.insert("YAxisScalelabelFont",t->m_YAxisScalelabelFont);//第40
        stateObject.insert("numOfXAxisScale",t->m_numOfXAxisScale);//第41
        stateObject.insert("numOfYAxisScale",t->m_numOfYAxisScale);//第42
        stateObject.insert("YAxisScaleRuler",t->m_YAxisScaleRuler);//第43
        stateObject.insert("XAxisScaleRuler",t->m_XAxisScaleRuler);//第44
        stateObject.insert("XAxisScaleprecision",t->m_XAxisScaleprecision);//第45
        stateObject.insert("YAxisScaleprecision",t->m_YAxisScaleprecision);//第46
        stateObject.insert("XAxisScalelabeloffset_x",t->m_XAxisScalelabeloffset_x);//第47
        stateObject.insert("XAxisScalelabeloffset_y",t->m_XAxisScalelabeloffset_y);//第48
        stateObject.insert("YAxisScalelabeloffset_x",t->m_YAxisScalelabeloffset_x);//第47
        stateObject.insert("YAxisScalelabeloffset_y",t->m_YAxisScalelabeloffset_y);//第48

        //将每一行Json字符串添加到Json数组
        stateArray.append(stateObject);
    }

    QJsonDocument Json_doc(stateArray);

    //将构建的Json数组赋值 使用紧凑模式
    m_jsonStr = Json_doc.toJson(QJsonDocument::Compact);

    this->accept();

}

//点击 取消
void GraphObj::on_pushButton_UnsaveQuit_clicked()
{
    this->reject();
}

QT_END_NAMESPACE
