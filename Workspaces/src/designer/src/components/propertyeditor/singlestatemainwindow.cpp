#include "singlestatemainwindow.h"
#include "singlestatelineeditdelegate.h"
#include "singlestatespindelegate.h"

#include <QApplication>
#include <QStandardItemModel>
#include <QTableView>
#include <QLineEdit>
#include<QGridLayout>
#include<QSplitter>
#include<QVBoxLayout>
#include <QFont>

#include <QJsonDocument>
#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>

SingleStateMainWindow::SingleStateMainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle(QObject::tr("状态编辑"));
    resize(QSize(700,600));        //设置窗口初始大小
    rowValue=2;      //初始化行数

    //初始化模型
    model = new QStandardItemModel(rowValue,3,this);// 设置模型中行数与列数
    model->setHeaderData(0,Qt::Horizontal,tr("下限值"));
    model->setHeaderData(1,Qt::Horizontal,tr("上限值"));
    model->setHeaderData(2,Qt::Horizontal,tr("双击单元格打开图片"));

     //初始化表格视图
    QTableView *tableView = new QTableView(this);
    tableView->setModel(model);
    SingleStateSpinDelegate *spinDelegate=new SingleStateSpinDelegate();
    tableView->setItemDelegateForColumn(0,spinDelegate);       //第一列下限值    数字显示框
    tableView->setItemDelegateForColumn(1,spinDelegate);       //第二列上限值    数字显示框
    SingleStateLineEditDelegate *lineeditdelegate=new SingleStateLineEditDelegate();
    tableView->setItemDelegateForColumn(2,lineeditdelegate);   //第三列打开图片     路径
    tableView->setColumnWidth(2,450);        //设置第三列初始宽度
    tableView->resize(QSize(650,500));        //设置窗口初始大小

    //顶部增加1行按钮控件
    addRowButton=new QPushButton(this);
    addRowButton->setText(tr("增加行"));//确定行数按钮
    addRowButton->setFont(QFont("微软雅黑",10,QFont::DemiBold));
    addRowButton->setFixedSize(90,30);

    //顶部删除1行按钮控件
    delRowButton=new QPushButton(this);
    delRowButton->setText(tr("减少行"));//确定行数按钮
    delRowButton->setFont(QFont("微软雅黑",10,QFont::DemiBold));
    delRowButton->setFixedSize(90,30);

    //顶部删除选中行按钮控件
    delCurrentRowButton=new QPushButton(this);
    delCurrentRowButton->setText(tr("删除指定行"));//确定行数按钮
    delCurrentRowButton->setFont(QFont("微软雅黑",10,QFont::DemiBold));
    delCurrentRowButton->setFixedSize(90,30);
    lineedit=new QLineEdit(this);
    lineedit->setFixedSize(90,30);

    //设置底部按钮控件
    okButton=new QPushButton(this);
    QPushButton *cancelButton=new QPushButton(this);
    okButton->setText(tr("确定"));
    cancelButton->setText(tr("放弃"));
    okButton->setFixedSize(90,30);
    okButton->setFont(QFont("微软雅黑",12,QFont::Bold));//设置确定按钮字体、大小、粗细
    cancelButton->setFont(QFont("微软雅黑",10,QFont::Normal));

    //设置窗口布局
    widget=new QWidget();       // 新建一个widget实例
    this->setCentralWidget(widget);    // 将这个实例设置为setCentralWidget

    //顶部增加行、删除行按钮布局
    TopAddDelLayout=new QHBoxLayout;    //新建一个从左向右方式插入控件的顶部子布局
    TopAddDelLayout->setSpacing(15);    //控件之间间距
    TopAddDelLayout->addWidget(addRowButton);//向布局中添加控件
    TopAddDelLayout->addWidget(delRowButton);

    //顶部删除指定行按钮布局
    TopDelButtonLayout=new QHBoxLayout;
    TopDelButtonLayout->addStretch(0);          //在按钮前插入一个占位符
    TopDelButtonLayout->setSpacing(15);    //控件之间间距
    TopDelButtonLayout->addWidget(delCurrentRowButton);
    TopDelButtonLayout->addWidget(lineedit);

    //顶部布局
    TopLayout=new QHBoxLayout;
    TopLayout->addLayout(TopAddDelLayout);
    TopLayout->addLayout(TopDelButtonLayout);

    //底部按钮布局
    BottomLayout=new QHBoxLayout;    //新建一个从左向右方式插入控件的底部子布局
    BottomLayout->addStretch(0);          //在按钮前插入一个占位符
    BottomLayout->addWidget(okButton);
    BottomLayout->addWidget(cancelButton);

    //主布局
    mainLayout=new QVBoxLayout;    //新建一个从上到下方式插入控件的主布局
    mainLayout->setMargin(15);        //布局内部对话框的边距
    mainLayout->setSpacing(15);      //控件之间间距
    mainLayout->addLayout(TopLayout);   //向主布局中加入顶部子布局
    mainLayout->addWidget(tableView);
    mainLayout->addLayout(BottomLayout);   //向主布局中加入底部子布局
    widget->setLayout(mainLayout);   //将widget的布局设置为mainLayout

    //增加1行按钮响应
    connect(addRowButton,SIGNAL(clicked()),this,SLOT(showTableAddRow()));

    //删除1行按钮响应
    connect(delRowButton,SIGNAL(clicked()),this,SLOT(showTableDelRow()));

    //删除指定选行按钮响应
    connect(delCurrentRowButton,SIGNAL(clicked()),this,SLOT(showTableDelCurrentRow()));

    //底部确定按钮响应
    connect(okButton,SIGNAL(clicked()),this,SLOT(showTableOk()));

    //底部取消按钮响应
    connect(cancelButton,SIGNAL(clicked()),this,SLOT(quitWindow()));
}

//关闭当前对话框槽函数
void   SingleStateMainWindow::quitWindow()
{
    if(!(QMessageBox::information(this,tr("提示"),tr("您确定要放弃当前对话框么？"),tr("是"),tr("否"))))
    {
        this->close();
    }
}

//增加一行槽函数
void   SingleStateMainWindow::showTableAddRow()
{
   rowValue=model->rowCount();
   model->insertRow(rowValue);
}

//删除一行槽函数
void   SingleStateMainWindow::showTableDelRow()
{
    int rowValueNum=1;//每次删除的行数
    rowValue=model->rowCount();//减少之前的行数
    int addrowValue=rowValue-rowValueNum;//删除后的现有行数
    if(addrowValue>=0)
    {
        model->setRowCount(addrowValue);
    }
    else
    {
        QMessageBox::information(this,tr("提示"),tr("您都删光了，没法再删了！"));
    }
}

//删除指定行槽函数
void   SingleStateMainWindow::showTableDelCurrentRow()
{
    bool ook;
    rowValue=model->rowCount();
    QString rowStrValue=lineedit->text();
    int lineEditRow=rowStrValue.toInt(&ook);
    int delRowValue=lineEditRow-1;//输入删除的行

    if(rowStrValue.isEmpty() )//如果输入框空
    {
        QMessageBox::information(this,tr("提示"),tr("您请在输入框输入要删除的行！"));
    }
   else if(lineEditRow==0)//如果输入框空
    {
        QMessageBox::information(this,tr("提示"),tr("您输入的没有呀，没法删！"));
    }
    else if(delRowValue>=0 && delRowValue<rowValue)//如果输入框不空
    {
        QString delRowValueToString=QString("您确定删除第%1行吗？").arg(delRowValue+1);
        int ok = QMessageBox::warning(this,tr("提示"),delRowValueToString,QMessageBox::Yes,QMessageBox::No);
        if(ok == QMessageBox::No)
        {
            model->revert(); //如果不删除，则撤销
        }
        else model->takeRow(delRowValue); //否则提交，删除该行
    }
    else if( delRowValue+1>rowValue)
    {
        QMessageBox::information(this,tr("提示"),tr("您指定的行没有呀，没法删！"));
    }
}

//配置完成确定槽函数
void   SingleStateMainWindow::showTableOk()
{
    int columns=model->columnCount();
    int rows=model->rowCount();
    QJsonArray stateArray;
    for (int row=0;row<rows;++row)
        {
        QJsonObject stateObject;
        for (int column=0;column<columns;++column)
            {
                 QModelIndex indexx=model->index(row,column);
                 QString str=indexx.data().toString();

                 switch(column)
                 {
                 case 0:
                     stateObject["0"] = str;
                     break;
                 case 1:
                     stateObject["1"] = str;
                     break;
                 case 2:
                     stateObject["2"] = str;
                     break;
                 }
        }
        stateArray.append(stateObject);
    }

    QJsonObject state_total;
    state_total["状态"] = stateArray;
    QJsonDocument doc2(state_total);

    QByteArray byte_array=doc2.toJson();
    qDebug()<<byte_array;
}



