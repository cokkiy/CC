#include "singlestatedialog.h"
#include "ui_singlestatedialog.h"
#include "singlestatelineeditdelegate.h"
#include "singlestatespindelegate.h"

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

SingleStateDialog::SingleStateDialog(QDesignerFormEditorInterface *core,QWidget *parent) :
    QDialog(parent),
    m_core(core),
    ui(new Ui::SingleStateDialog)
{
    ui->setupUi(this);
    //初始化模型
    model = new QStandardItemModel(1,3,this);// 设置模型中行数与列数
    model->setHeaderData(0,Qt::Horizontal,tr("下限值"));
    model->setHeaderData(1,Qt::Horizontal,tr("上限值"));
    model->setHeaderData(2,Qt::Horizontal,tr("双击单元格打开图片"));

     //初始化表格视图
    ui->tableView->setModel(model);
    SingleStateSpinDelegate *spinDelegate=new SingleStateSpinDelegate();
    ui->tableView->setItemDelegateForColumn(0,spinDelegate);       //第一列下限值    数字显示框
    ui->tableView->setItemDelegateForColumn(1,spinDelegate);       //第二列上限值    数字显示框
    SingleStateLineEditDelegate *lineeditdelegate=new SingleStateLineEditDelegate();
    ui->tableView->setItemDelegateForColumn(2,lineeditdelegate);   //第三列打开图片     路径
    ui->tableView->setColumnWidth(2,450);        //设置第三列初始宽度
}

SingleStateDialog::~SingleStateDialog()
{
    delete ui;
}

//增加行按钮
void SingleStateDialog::on_addRowButton_clicked()
{
    rowValue=model->rowCount();
    model->insertRow(rowValue);
}

//删除行按钮
void SingleStateDialog::on_delRowButton_clicked()
{
    int rowValueNum=1;//每次删除的行数
     rowValue=model->rowCount();//减少之前的行数
     int addrowValue=rowValue-rowValueNum;//删除后的现有行数
       if(addrowValue>=0)
       { model->setRowCount(addrowValue);    }
     else
       {   QMessageBox::information(this,tr("提示"),tr("您都删光了，没法再删了！"));}
}

//删除指定行按钮
void SingleStateDialog::on_delCurrentRowButton_clicked()
{
    bool ook;
    rowValue=model->rowCount();
    QString rowStrValue=ui->lineedit->text();
    int delRowValue=rowStrValue.toInt(&ook)-1;//输入删除的行
    if(rowStrValue.isEmpty())//如果输入框空
    {
        QMessageBox::information(this,tr("提示"),tr("请在输入框输入要删除的行！"));
    }
    else if(delRowValue>=0 && delRowValue<rowValue)//如果输入框不空
    {
        QString delRowValueToString=QString("你确定删除第%1行吗？").arg(delRowValue+1);
        int ok = QMessageBox::warning(this,tr("提示"),delRowValueToString,QMessageBox::Yes,QMessageBox::No);
        if(ok == QMessageBox::No)
        {
            model->revert(); //如果不删除，则撤销
        }
        else model->takeRow(delRowValue); //否则提交，删除该行
    }
    else if( delRowValue+1>rowValue)
    {
        QMessageBox::information(this,tr("提示"),tr("您都删光了，没法再删了！"));
    }
}

//打开对话框
int SingleStateDialog::showDialog()
{
    show();
    return exec();
}

//传回Json字符串
QString SingleStateDialog::TransStrStateJson() const
{
    return strStateJson;
}

//确认按钮槽函数,读取TableView表格配置数据Json字符串生成,执行完关闭窗口
void SingleStateDialog::on_okButton_clicked()
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
void SingleStateDialog::on_quitPushButton_clicked()
{
    if(!(QMessageBox::information(this,tr("提示"),tr("您确定要关闭么？"),tr("是"),tr("否"))))
    {
        this->reject();
    }
}
