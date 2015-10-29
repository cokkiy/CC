#include "q2wmapcircledlg.h"
#include "ui_q2wmapcircledlg.h"
#include <QColorDialog>

Q2wmapCircleDlg::Q2wmapCircleDlg(QJsonObject jobj) : ui(new Ui::Q2wmapCircleDlg)
{
    ui->setupUi(this);

    m_strType = "Circle";

    if(jobj.isEmpty())
    {
        //区域名称
        m_strName = "新圆形区域";

        //圆心经度
        m_dbPosL = 0.0;

        //圆心纬度
        m_dbPosB = 0.0;

        //半径(Km)
        m_dbRadius = 0.0;

        //边线颜色
        m_strColor= "#0000ff";

        //边线粗细
        m_iWidth = 3;
    }
    else
    {
        //下面开始挨个解析

        //区域名称
        m_strName = jobj["Name"].toString();

        //圆心经度
        m_dbPosL = jobj["PosL"].toDouble();

        //圆心纬度
        m_dbPosB = jobj["PosB"].toDouble();

        //半径(Km)
        m_dbRadius = jobj["Radius"].toDouble();

        //边线颜色
        m_strColor= jobj["Color"].toString();

        //边线粗细
        m_iWidth = jobj["Width"].toInt();

    }

    // 设置控件
    setControls();
}

Q2wmapCircleDlg::~Q2wmapCircleDlg()
{
    delete ui;
}

//设置控件
void Q2wmapCircleDlg::setControls()
{
    //区域名称
    ui->lineEditAreaName->setText(m_strName);

    //圆心经度
    ui->lineEditPosL->setText(QString("%1").arg(m_dbPosL));

    //圆心纬度
    ui->lineEditPosB->setText(QString("%1").arg(m_dbPosB));

    //半径(Km)
    ui->lineEditRadius->setText(QString("%1").arg(m_dbRadius));

    //边线颜色
    ui->pushButton_SelectColor->setStyleSheet(QString("border:2px solid #555555; background-color: %1").arg(m_strColor));

    //边线粗细
    ui->lineEditWidth->setText(QString("%1").arg(m_iWidth));
}

//区域名称改变
void Q2wmapCircleDlg::on_lineEditAreaName_textChanged(const QString &arg1)
{
    m_strName = ui->lineEditAreaName->text();
}

//圆心经度改变
void Q2wmapCircleDlg::on_lineEditPosL_textChanged(const QString &arg1)
{
    m_dbPosL = ui->lineEditPosL->text().toDouble();
}

//圆心纬度改变
void Q2wmapCircleDlg::on_lineEditPosB_textChanged(const QString &arg1)
{
    m_dbPosB = ui->lineEditPosB->text().toDouble();
}

//半径(Km)改变
void Q2wmapCircleDlg::on_lineEditRadius_textChanged(const QString &arg1)
{
    m_dbRadius = ui->lineEditRadius->text().toDouble();
}

//边线粗细改变
void Q2wmapCircleDlg::on_lineEditWidth_textChanged(const QString &arg1)
{
    m_iWidth = ui->lineEditWidth->text().toInt();
}

//边线颜色改变
void Q2wmapCircleDlg::on_pushButton_SelectColor_clicked()
{
    QColorDialog dlg;

    //取到设置的颜色
    QColor c = dlg.getColor();

    //无效颜色不处理(例如用户取消了对话框)
    if(c == QColor::Invalid)
    {
        return;
    }

    m_strColor = c.name();

    //将按钮颜色设置为取到的颜色
    ui->pushButton_SelectColor->setStyleSheet(QString("border:2px solid #555555; background-color: %1").arg(c.name()));
}
