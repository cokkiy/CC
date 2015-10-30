#include "q2wmapobj.h"
#include "ui_q2wmapobj.h"
#include "q2wmapobjdlg.h"
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

QT_BEGIN_NAMESPACE

Q2wmapObj::Q2wmapObj()
{
    ui = new Ui::Q2wmapObj;
    ui->setupUi(this);
}

Q2wmapObj::~Q2wmapObj()
{
    delete ui;
}

//添加新目标
void Q2wmapObj::on_pushButton_AddNewObj_clicked()
{
    QJsonObject obj;

    Q2wmapObjDlg * t = new Q2wmapObjDlg(obj, this);

    QString str = "新目标";

    //第一个目标默认为主目标
    if(ui->tabWidget->count() == 0)
    {
        t->m_bMainObj = true;

        //为了勾选主目标的选框
        t->setControls();
    }

    //添加tab
    ui->tabWidget->addTab(t, str);

    //设置为当前tab
    ui->tabWidget->setCurrentWidget(t);
}

//删除当前目标
void Q2wmapObj::on_pushButton_DelCurrentObj_clicked()
{
    int currentIndex = ui->tabWidget->currentIndex();
    ui->tabWidget->removeTab(currentIndex);
}

//输出json字符串
QString Q2wmapObj::transText()
{
    return m_jsonStr;
}

//打开对话框
void Q2wmapObj::Show()
{
    show();
    exec();
}

//以字符串初始化控件
void Q2wmapObj::InitControl(QString str)
{
    //先删除所有标签
    for(int i=0; i<ui->tabWidget->count(); i++)
    {
        ui->tabWidget->removeTab(i);
    }

    //空字符串不再解析
    if(str.isEmpty())
    {
        return;
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

                    Q2wmapObjDlg * t = new Q2wmapObjDlg(name, this);

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
    else
    {
        qDebug()<<"error in Q2wmapObj::InitControl : " + jerr.errorString();
    }
}


//点击 确定
void Q2wmapObj::on_pushButton_SaveQuit_clicked()
{
    //Json数组
    QJsonArray stateArray;

    //主目标的个数
    qint32 iMainObjCount = 0;

    //保存所有标签的数据
    for(int i=0; i<ui->tabWidget->count(); i++)
    {
        Q2wmapObjDlg * t = (Q2wmapObjDlg*)(ui->tabWidget->widget(i));

        QJsonObject stateObject;

        //下面在json中插入各个要保存的属性

        //目标名称
        stateObject.insert("Name", t->m_strName);

        //是否主目标
        stateObject.insert("MainObj", t->m_bMainObj);

        if(t->m_bMainObj == true)
        {
            iMainObjCount++;
        }

        //曲线颜色
        stateObject.insert("CColor", t->m_strCColor);

        //曲线粗细
        stateObject.insert("CWidth", t->m_iCWidth);

        //理论曲线颜色
        stateObject.insert("LColor", t->m_strLColor);

        //理论曲线粗细
        stateObject.insert("LWidth", t->m_iLWidth);

        //X轴参数
        stateObject.insert("XParam", t->m_strXParam);

        //Y轴参数
        stateObject.insert("YParam", t->m_strYParam);

        //理论曲线路径文件
        stateObject.insert("LFile", t->m_strLFile);

        //将每一行Json字符串添加到Json数组
        stateArray.append(stateObject);
    }

    if(stateArray.size()>0 && iMainObjCount != 1)
    {
        QMessageBox::warning(this,                          //warning是警告样式，更多样式参考文档。this是父窗口指针
                             tr("提示信息"),              //MSGBOX的标题
                             tr("主目标的个数不合格，必须有且只有一个主目标！"),//MSGBOX的内容
                             QMessageBox::Ok,//MSGBOX上的一堆按钮，当然可以只要1个。风格样式参考文档
                             QMessageBox::Ok);//MSGBOX的默认按钮，就是按空格时系统认为你按的那个按钮
    }
    else
    {
        QJsonDocument Json_doc(stateArray);

        //将构建的Json数组赋值 使用紧凑模式
        m_jsonStr = Json_doc.toJson(QJsonDocument::Compact);

        this->accept();
    }
}

//点击 取消
void Q2wmapObj::on_pushButton_UnsaveQuit_clicked()
{
    this->reject();
}

QT_END_NAMESPACE
