#include "graphObj.h"
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

//添加新曲线
void GraphObj::on_pushButton_AddNewObj_clicked()
{
    QJsonObject obj;

    GraphObjDlg * t = new GraphObjDlg(obj, this);

    //标注添加曲线的tab名称
    QString strtabName;
    strtabName = (t->m_GraphName);

    //添加tab
    ui->tabWidget->addTab(t, strtabName);

    //设置为当前tab
    ui->tabWidget->setCurrentWidget(t);
}

//删除当前曲线
void GraphObj::on_pushButton_DelCurrentObj_clicked()
{
    int currentIndex = ui->tabWidget->currentIndex();
    ui->tabWidget->removeTab(currentIndex);
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


////左移曲线
//void GraphDialog::on_pushButton_leftmovegraph_target_clicked()
//{

//    //思路：把当前2个窗口存起来，然后用setTabOrder()进行交换
//    //第一步
//    //先把当前曲线及前1条曲线的索引号存起来
//    qint32 graphindex;
//    graphindex= ui->tabWidget->currentIndex()-1;
//    //第二步
//    //取到左边窗口和右边窗口
//    QWidget *widget_right = ui->tabWidget->currentWidget();//right widget
//    ui->tabWidget->setCurrentIndex(graphindex);
//    QWidget *widget_left = ui->tabWidget->currentWidget();//left widget

//    ui->tabWidget->setTabOrder(widget_right,widget_left);

//    numOfgraph = ui->tabWidget->count();//获取曲线总数

//}


////右移曲线
//void GraphDialog::on_pushButton_rightmovegraph_target_clicked()
//{
//    //第一步
//    //先把当前曲线及后1条曲线的索引号存起来
//    qint32 graphindex;
//    graphindex= ui->tabWidget->currentIndex()+1;
//    //第二步
//    //取到左边窗口和右边窗口
//    QWidget *widget_left = ui->tabWidget->currentWidget();//left widget
//    ui->tabWidget->setCurrentIndex(graphindex);
//    QWidget *widget_right = ui->tabWidget->currentWidget();//right widget

//    ui->tabWidget->setTabOrder(widget_right,widget_left);

//    numOfgraph = ui->tabWidget->count();//获取曲线总数

//}

//void on_pushButton_leftmove_clicked()
//{

//}

//void on_pushButton_rightmove_clicked()
//{

//}


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
        stateObject.insert("Xoffset",t->m_Xoffset);//第5
        stateObject.insert("Yoffset",t->m_Yoffset);//第6
        stateObject.insert("XAxisLabel",t->m_XAxisLabel);//第7
        stateObject.insert("YAxisLabel",t->m_YAxisLabel);//第8
        stateObject.insert("XAxisdisplay",t->m_XAxisdisplay);//第9
        stateObject.insert("YAxisdisplay",t->m_YAxisdisplay);//第10
        stateObject.insert("Scaleplace_x",t->m_Scaleplace_x);//第11
        stateObject.insert("Scaleplace_y",t->m_Scaleplace_y);//第12

        stateObject.insert("GraphName",t->m_GraphName);//第13
        stateObject.insert("GraphWidth",t->m_GraphWidth);//第14
        stateObject.insert("GraphBuffer",t->m_GraphBuffer);//第15
        stateObject.insert("strgraphColor",t->m_strgraphColor);//第16

        stateObject.insert("LGraphWidth",t->m_LGraphWidth);//第17
        stateObject.insert("LGraphBuffer",t->m_LGraphBuffer);//第18
        stateObject.insert("strLgraphColor",t->m_strLgraphColor);//第19

        stateObject.insert("xParam",t->m_xParam);//第20
        stateObject.insert("yParam",t->m_yParam);//第21
        stateObject.insert("Lgraphfile",t->m_Lgraphfile);//第22


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
