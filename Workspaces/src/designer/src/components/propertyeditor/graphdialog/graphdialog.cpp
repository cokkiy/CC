#include "../propertyeditor/graphdialog/graphdialog.h"
#include "ui_graphdialog.h"
#include "selectparamdialog.h"


#include <QDesignerFormEditorInterface>

#include <QColorDialog>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QString>
#include <qpainter.h>
#include <QVariant>



#include <QApplication>
#include <QStandardItemModel>
#include <QTableView>
#include <QLineEdit>
#include<QtDebug>
#include<QGridLayout>
#include <QMessageBox>

#include <QJsonDocument>
#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>
#include <QDesignerFormEditorInterface>


QT_BEGIN_NAMESPACE
GraphDialog::GraphDialog(QDesignerFormEditorInterface *core,QWidget *parent) :
    QDialog(parent),
    m_core(core),
    ui(new Ui::GraphDialog)
{

    ui->setupUi(this);
//    QMetaObject::connectSlotsByName(this);

    setWindowTitle(tr("Select Param"));

    numOfgraph=1;

    net = NetComponents::getInforCenter();

}
GraphDialog::~GraphDialog()
{
    delete ui;
}


//打开对话框
int GraphDialog::showDialog()
{
    show();
    return exec();
}
//尼玛，神马都是浮云，取currentIndex，亦即currentgraphindex搞好曲线配置项配置即可！！！！
//每次输入完，获取一次索引号，将配好的数据加到对应索引号的容器（数组）中即可。
//增加新曲线（实际是插入新曲线）
void GraphDialog::on_pushButton_insertnewGraph_target_clicked()
{
    numOfgraph=ui->tabWidget->count();//获取曲线总数

    tab_2 = new QWidget();
    tab_2->setObjectName(QStringLiteral("tab_2"));

    ui->tabWidget->addTab(tab_2, QString());

    qint32 graphindex;
    graphindex = ui->tabWidget->currentIndex()+1;//每次插入的都是下一条曲线:即当前曲线索引号加1
    QString strGraphindexName;
    strGraphindexName=QString("%1%2").arg(tr("曲线")).arg(graphindex);
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(tab_2), strGraphindexName);//插入曲线的名字

    //插入之后的其余的曲线改名为：依次加1
    qint32 graphindexChanged;
    qint32 graphindexChangedCount;//需要改名的曲线计数
    graphindexChangedCount = numOfgraph-graphindex;

    for(qint32 i=0;i<graphindexChangedCount;i++)//其余的每条曲线都要改名
    {
      graphindexChanged = graphindex+i+1;
      QString strGraphindexChangedName;
      strGraphindexChangedName=QString("%1%2").arg(tr("曲线")).arg(graphindexChanged);
      ui->tabWidget->setTabText(ui->tabWidget->indexOf(tab_2), strGraphindexChangedName);
    }

    ui->tabWidget->setCurrentIndex(graphindex);
    graphindex++;
}

//删除当前选中曲线
void GraphDialog::on_pushButton_delchooseGraph_target_clicked()
{
    numOfgraph=ui->tabWidget->count();//获取曲线总数

    //先把当前曲线的索引号存起来
    qint32 graphindex;
    graphindex= ui->tabWidget->currentIndex();

    //删除当前曲线
    ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabWidget->currentWidget()));
}


//左移曲线
void GraphDialog::on_pushButton_leftmovegraph_target_clicked()
{

    //思路：把当前2个窗口存起来，然后用setTabOrder()进行交换
    //第一步
    //先把当前曲线及前1条曲线的索引号存起来
    qint32 graphindex;
    graphindex= ui->tabWidget->currentIndex()-1;
    //第二步
    //取到左边窗口和右边窗口
    QWidget *widget_right = ui->tabWidget->currentWidget();//right widget
    ui->tabWidget->setCurrentIndex(graphindex);
    QWidget *widget_left = ui->tabWidget->currentWidget();//left widget

    ui->tabWidget->setTabOrder(widget_right,widget_left);

}


//右移曲线
void GraphDialog::on_pushButton_rightmovegraph_target_clicked()
{
    //第一步
    //先把当前曲线及后1条曲线的索引号存起来
    qint32 graphindex;
    graphindex= ui->tabWidget->currentIndex()+1;
    //第二步
    //取到左边窗口和右边窗口
    QWidget *widget_left = ui->tabWidget->currentWidget();//left widget
    ui->tabWidget->setCurrentIndex(graphindex);
    QWidget *widget_right = ui->tabWidget->currentWidget();//right widget

    ui->tabWidget->setTabOrder(widget_right,widget_left);

}




//X轴最大值
void GraphDialog::on_lineEdit_Xmax_tab_textChanged(const QString &arg1)
{
//   QString m_Xmax = arg1;
   m_Xmax = arg1;
   currentgraphindex=ui->tabWidget->currentIndex();//获取当前曲线索引号


}

//X轴最小值
void GraphDialog::on_lineEdit_Xmin_tab_textChanged(const QString &arg1)
{
//    QString m_Xmin = arg1;
    m_Xmin = arg1;
    currentgraphindex=ui->tabWidget->currentIndex();//获取当前曲线索引号

}


//Y轴最大值
void GraphDialog::on_lineEdit_Ymax_textChanged(const QString &arg1)
{
//   QString m_Ymax = arg1;
   m_Ymax = arg1;
   currentgraphindex=ui->tabWidget->currentIndex();//获取当前曲线索引号
}

//Y轴最小值
void GraphDialog::on_lineEdit_Ymin_textChanged(const QString &arg1)
{
//    QString m_Ymin = arg1;
    m_Ymin = arg1;
    currentgraphindex=ui->tabWidget->currentIndex();//获取当前曲线索引号

}

//X轴文本偏移量
void GraphDialog::on_lineEdit_wenbenpianyi_x_textChanged(const QString &arg1)
{
//    QString m_Xoffset = arg1;
    m_Xoffset = arg1;
    currentgraphindex=ui->tabWidget->currentIndex();//获取当前曲线索引号

}

//Y轴文本偏移量
void GraphDialog::on_lineEdit_wenbenpianyi_y_textChanged(const QString &arg1)
{
//    QString m_Yoffset = arg1;
    m_Yoffset = arg1;
    currentgraphindex=ui->tabWidget->currentIndex();//获取当前曲线索引号
}


//X轴轴标签
void GraphDialog::on_lineEdit_inputAxisLabel_x_textChanged(const QString &arg1)
{
//    QString m_XAxisLabel = arg1;
    m_XAxisLabel = arg1;
    currentgraphindex=ui->tabWidget->currentIndex();//获取当前曲线索引号
}

//Y轴轴标签
void GraphDialog::on_lineEdit_inputAxisLabel_y_textChanged(const QString &arg1)
{
//    QString m_YAxisLabel = arg1;
    m_YAxisLabel = arg1;
    currentgraphindex=ui->tabWidget->currentIndex();//获取当前曲线索引号

}

//曲线名称（图例名称）
void GraphDialog::on_lineEdit_inputGraphName_tab_textChanged(const QString &arg1)
{
//    QString m_GraphName = arg1;
    m_GraphName = arg1;
    currentgraphindex=ui->tabWidget->currentIndex();//获取当前曲线索引号

}

//曲线宽度
void GraphDialog::on_lineEdit_GraphWidth_tab_textChanged(const QString &arg1)
{
//    QString m_GraphWidth = arg1;
    m_GraphWidth = arg1;
    currentgraphindex=ui->tabWidget->currentIndex();//获取当前曲线索引号

}

//曲线缓冲区大小
void GraphDialog::on_lineEdit_GraphBuffer_tab_textChanged(const QString &arg1)
{
//    QString m_GraphBuffer = arg1;
    m_GraphBuffer = arg1;
    currentgraphindex=ui->tabWidget->currentIndex();//获取当前曲线索引号

}



//是否选中显示X轴
void GraphDialog::on_checkBox_chooseAxisdisplay_x_clicked(bool checked)
{
//    qint32 m_XAxisdisplay;
    if(checked)
    {
        m_XAxisdisplay=1;
    }
    else
    {
        m_XAxisdisplay=0;
    }

}

//是否选中显示Y轴
void GraphDialog::on_checkBox_chooseAxisdisplay_y_clicked(bool checked)
{
//    qint32 m_YAxisdisplay;
    if(checked)
    {
        m_YAxisdisplay=1;
    }
    else
    {
        m_YAxisdisplay=0;
    }
}


//选择曲线颜色
void GraphDialog::on_pushButton_chooseGraphColor_tab_clicked()
{

    QColorDialog graphcolordialog;
//    QColor m_graphcolor;

    //取到设置的颜色
    QColor m_graphcolor = graphcolordialog.getColor();
//    Data->m_graphcolor=graphcolordialog.getColor();
    //将按钮颜色设置为取到的颜色
    //最爽的还是样式表
    //background-color: rgb(255, 0, 0);
    //ui->label_chooseGraphColor->setStyleSheet("background-color: rgb(255, 0, 0)");
    QString m_strgraphColor = QString("rgb(%1, %2, %3)").arg(QString::number(m_graphcolor.red()),
                                                      QString::number(m_graphcolor.green()),
                                                      QString::number(m_graphcolor.blue()));
    m_strgraphColor_RED = m_graphcolor.red();
    m_strgraphColor_GREEN = m_graphcolor.green();
    m_strgraphColor_BLUE = m_graphcolor.green();

    ui->label_chooseGraphColor->setStyleSheet(QString("background-color: %1").arg(m_strgraphColor));

    currentgraphindex=ui->tabWidget->currentIndex();//获取当前曲线索引号

}


//X的刻度位置（上中下）
void GraphDialog::on_comboBox_keduweizhi_x_currentTextChanged(const QString &arg1)
{
    m_keduweizhi_x = arg1;

}

//Y的刻度位置（左中右）
void GraphDialog::on_comboBox_keduweizhi_y_currentTextChanged(const QString &arg1)
{
    m_keduweizhi_y = arg1;

}

//选择弹道底图（理论弹道）
void GraphDialog::on_pushButton_chooseGraphbasemap_tab_clicked()
{
    QFileDialog m_lgraphOpenDialog;
//    QDir m_lgraphdirectory;
    QString m_lgraphfile;//理论曲线文件(路径)
    //文件父指针，文件打开对话框标题，文件路径（自选），文件过滤器（自选）
    m_lgraphfile = m_lgraphOpenDialog.getOpenFileName(this,tr("装载理论弹道"),"",tr("All Files(*.*)"));

//    QString m_strTFile=m_lgraphfile;
//    if(m_lgraphfile.isNull())
//    {
//        return false;
//    }
//    QFile f(m_lgraphfile);



     /*
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

  */



}



//选择X参数
void GraphDialog::on_pushButton_chooseXparam_tab_clicked()
{

    // 单参数选择对话框
    qdesigner_internal::QSelectParamDialog dlg;
//    dlg.showDialog();

    if (dlg.showDialog() != QDialog::Accepted )
        return;
    ui->lineEdit_inputXparam_tab->setText(dlg.text());
}

//选择Y参数
void GraphDialog::on_pushButton_chooseYparam_tab_clicked()
{
    // 单参数选择对话框
    qdesigner_internal::QSelectParamDialog dlg;
//    dlg.showDialog();

    if (dlg.showDialog() != QDialog::Accepted )
        return;
    ui->lineEdit_inputYparam_tab->setText(dlg.text());

}




//传回Json字符串
QString GraphDialog::TransStrStateJson() const
{
    return strStateJson;
}

//确认按钮槽函数,读取配置数据Json字符串生成,执行完关闭窗口
//按下确定键，向主类传送Json
void GraphDialog::on_buttonBox_OKorCancel_target_accepted()
{


    QJsonArray stateArray;//Json数组
    QJsonObject stateObject;

//    QString m_Xmax = arg1;
//    QString m_Xmin = arg1;
//    QString m_Ymax = arg1;
//    QString m_Ymin = arg1;
//    QString m_Xoffset = arg1;
//    QString m_Yoffset = arg1;
//    QString m_XAxisLabel = arg1;
//    QString m_YAxisLabel = arg1;
//    QString m_GraphName = arg1;
//    QString m_GraphWidth = arg1;
//    QString m_GraphBuffer = arg1;
//    qint32 m_XAxisdisplay;
//    qint32 m_YAxisdisplay;
//    qint32 m_strgraphColor_RED ;
//    qint32 m_strgraphColor_GREEN;
//    qint32 m_strgraphColor_BLUE;
//    QString m_keduweizhi_x;
//    QString m_keduweizhi_y;

    stateObject.insert("Xmax",m_Xmax);//第1
    stateObject.insert("Xmin",m_Xmin);//第2
    stateObject.insert("Ymax",m_Ymax);//第3
    stateObject.insert("Ymin",m_Ymin);//第4
    stateObject.insert("Xoffset",m_Xoffset);//第5
    stateObject.insert("Yoffset",m_Yoffset);//第6
    stateObject.insert("XAxisLabel",m_XAxisLabel);//第7
    stateObject.insert("YAxisLabel",m_YAxisLabel);//第8
    stateObject.insert("GraphName",m_GraphName);//第9
    stateObject.insert("GraphWidth",m_GraphWidth);//第10
    stateObject.insert("GraphBuffer",m_GraphBuffer);//第11
    stateObject.insert("XAxisdisplay",m_XAxisdisplay);//第12
    stateObject.insert("YAxisdisplay",m_YAxisdisplay);//第13
    stateObject.insert("strgraphColor_RED",m_strgraphColor_RED);//第14
    stateObject.insert("strgraphColor_GREEN",m_strgraphColor_GREEN);//第14
    stateObject.insert("strgraphColor_BLUE",m_strgraphColor_BLUE);//第14
    stateObject.insert("keduweizhi_x",m_keduweizhi_x);//第15
    stateObject.insert("keduweizhi_y",m_keduweizhi_y);//第15

//    QString str = QString(tr("12345"));
//    stateObject.insert("number",str);//第1列
//    QString str = QString(tr("12345"));
//    stateObject["0"] = str;//第1列



    stateArray.append(stateObject);//将每一行Json字符串添加到Json数组

    QJsonDocument doc2(stateArray);
    QByteArray byte_array=doc2.toJson();
    strStateJson=byte_array;//将构建的Json数组赋值给接口接收字符串
    this->accept();//执行确定完毕关闭当前窗口

}


//放弃按钮槽函数
//按下取消键
void GraphDialog::on_buttonBox_OKorCancel_target_rejected()
{
    if(!(QMessageBox::information(this,tr("提示"),tr("您确定要关闭么？"),tr("是"),tr("否"))))
    {
        this->reject();
    }

}

/*
//确认按钮槽函数,读取配置数据Json字符串生成,执行完关闭窗口
void GraphDialog::on_okButton_clicked()
{
    int columns=model->columnCount();//总列
     int rows=model->rowCount();//总行
     QJsonArray stateArray;//Json数组
     for (int row=0;row<rows;++row)
         {
         QJsonObject stateObject;
         for (int column=0;column<columns;++column)
             {
                  QModelIndex indexx=model->index(row,column);
                  QString str=indexx.data().toString();//遍历的单元格数据
                  switch(column)
                  {
                  case 0:
                      stateObject["0"] = str.toDouble();//第1列单元格
                      break;
                  case 1:
                      stateObject["1"] = str.toDouble();//第2列单元格
                      break;
                  case 2:
                      stateObject["2"] = str;//第3列单元格
                      break;
                  }
         }
         stateArray.append(stateObject);//将每一行Json字符串添加到Json数组
     }
     QJsonDocument doc2(stateArray);
     QByteArray byte_array=doc2.toJson();
     strStateJson=byte_array;//将构建的Json数组赋值给接口接收字符串
     this->accept();//执行确定完毕关闭当前窗口
}

//放弃按钮槽函数
void GraphDialog::on_quitPushButton_clicked()
{
    if(!(QMessageBox::information(this,tr("提示"),tr("您确定要关闭么？"),tr("是"),tr("否"))))
    {
        this->reject();
    }
}
*/


QT_END_NAMESPACE












