#include "q2wmapstaticelementdlg.h"
#include "ui_q2wmapstaticelementdlg.h"
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include "q2wmapcircledlg.h"
#include "q2wmappolygondialog.h"
#include "q2wmaplabeldlg.h"
#include "q2wmapelementdlg.h"

Q2wmapStaticElementDlg::Q2wmapStaticElementDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Q2wmapStaticElementDlg)
{
    ui->setupUi(this);
}

Q2wmapStaticElementDlg::~Q2wmapStaticElementDlg()
{
    delete ui;
}

//输出json字符串
QString Q2wmapStaticElementDlg::transText()
{
    return m_jsonStr;
}

//以字符串初始化控件
void Q2wmapStaticElementDlg::InitControl(QString str)
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

                    //元素类型
                    QString strType = name["Type"].toString();


                    if(strType == "Circle")
                    {
                        Q2wmapCircleDlg * t = new Q2wmapCircleDlg(name);

                        //目标名称
                        t->m_strName = name["Name"].toString();

                        //添加标签
                        qint32 index = ui->tabWidget->addTab(t, t->m_strName);

                        //设置属性分栏标题的文字
                        ui->tabWidget->setTabText(index, t->m_strName);
                    }
                    else if(strType == "Polygon")
                    {
                        Q2wmapPolygonDialog * t = new Q2wmapPolygonDialog(name);

                        //目标名称
                        t->m_strName = name["Name"].toString();

                        //添加标签
                        qint32 index = ui->tabWidget->addTab(t, t->m_strName);

                        //设置属性分栏标题的文字
                        ui->tabWidget->setTabText(index, t->m_strName);
                    }
                    else if(strType == "Label")
                    {
                        Q2wmapLabelDlg * t = new Q2wmapLabelDlg(name);

                        //目标名称
                        t->m_strName = name["Name"].toString();

                        //添加标签
                        qint32 index = ui->tabWidget->addTab(t, t->m_strName);

                        //设置属性分栏标题的文字
                        ui->tabWidget->setTabText(index, t->m_strName);
                    }
                }
            }
        }
    }
    else
    {
        qDebug()<<"error in Q2wmapStaticElementDlg::InitControl : " + jerr.errorString();
    }
}

//打开对话框
void Q2wmapStaticElementDlg::Show()
{
    show();
}

//点击按钮-添加圆形区域
void Q2wmapStaticElementDlg::on_pushButtonCircle_clicked()
{
    QJsonObject obj;

    Q2wmapCircleDlg * t = new Q2wmapCircleDlg(obj);

    QString str = "新圆形区域";

    //添加tab
    ui->tabWidget->addTab(t, str);

    //设置为当前tab
    ui->tabWidget->setCurrentWidget(t);
}

//点击按钮-添加不规则区域
void Q2wmapStaticElementDlg::on_pushButtonPolygon_clicked()
{
    QJsonObject obj;

    Q2wmapPolygonDialog * t = new Q2wmapPolygonDialog(obj);

    QString str = "新不规则区域";

    //添加tab
    ui->tabWidget->addTab(t, str);

    //设置为当前tab
    ui->tabWidget->setCurrentWidget(t);
}

//点击按钮-添加名称标签
void Q2wmapStaticElementDlg::on_pushButtonLabel_clicked()
{
    QJsonObject obj;

    Q2wmapLabelDlg * t = new Q2wmapLabelDlg(obj);

    QString str = "新标签";

    //添加tab
    ui->tabWidget->addTab(t, str);

    //设置为当前tab
    ui->tabWidget->setCurrentWidget(t);
}

//点击按钮-删除当前元素
void Q2wmapStaticElementDlg::on_pushButtonDel_clicked()
{
    int currentIndex = ui->tabWidget->currentIndex();
    ui->tabWidget->removeTab(currentIndex);
}

//点击确定按钮
void Q2wmapStaticElementDlg::on_pushButtonOK_clicked()
{
    //Json数组
    QJsonArray stateArray;

    //保存所有标签的数据
    for(int i=0; i<ui->tabWidget->count(); i++)
    {
        Q2wmapElementDlg * t = (Q2wmapElementDlg*)(ui->tabWidget->widget(i));

        QJsonObject stateObject;

        if(t->m_strType == "Circle")
        {
            Q2wmapCircleDlg * tc = (Q2wmapCircleDlg*)t;

            //下面在json中插入各个要保存的属性

            //目标名称
            stateObject.insert("Name", tc->m_strName);
        }
        else if(t->m_strType == "Polygon")
        {
            Q2wmapPolygonDialog *tp = (Q2wmapPolygonDialog*)t;

            //下面在json中插入各个要保存的属性

            //目标名称
            stateObject.insert("Name", tp->m_strName);
        }
        else if(t->m_strType == "Lable")
        {
            Q2wmapLabelDlg * tl = (Q2wmapLabelDlg*)t;

            //下面在json中插入各个要保存的属性

            //目标名称
            stateObject.insert("Name", tl->m_strName);
        }

        //将每一行Json字符串添加到Json数组
        stateArray.append(stateObject);
    }

    QJsonDocument Json_doc(stateArray);

    //将构建的Json数组赋值 使用紧凑模式
    m_jsonStr = Json_doc.toJson(QJsonDocument::Compact);

    this->accept();
}

//点击取消按钮
void Q2wmapStaticElementDlg::on_pushButtonCancel_clicked()
{
    this->reject();
}
