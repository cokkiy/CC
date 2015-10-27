#include "q2wmapobjdlg.h"
#include "ui_q2wmapobjdlg.h"

#include <QColorDialog>
#include "paramselect/selectparamdialog.h"
#include <QFileDialog>

Q2wmapObjDlg::Q2wmapObjDlg(QJsonObject jobj, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Q2wmapObjDlg)
{
    ui->setupUi(this);

    if(jobj.isEmpty())
    {
        //目标名称
        m_strName = "新目标";

        //是否主目标
        m_bMainObj = false;

        //曲线颜色
        m_strCColor = "#0000ff";

        //曲线粗细
        m_iCWidth = 3;

        //理论曲线颜色
        m_strLColor = "#555555";

        //理论曲线粗细
        m_iLWidth = 3;

        //X轴参数
        m_strXParam = "";

        //Y轴参数
        m_strYParam = "";

        //理论曲线路径文件
        m_strLFile = "";
    }
    else
    {
        //下面开始挨个解析

        //目标名称
        m_strName = jobj["Name"].toString();

        //是否主目标
        m_bMainObj = jobj["MainObj"].toBool();

        //曲线颜色
        m_strCColor = jobj["CColor"].toString();

        //曲线粗细
        m_iCWidth = jobj["CWidth"].toInt();

        //理论曲线颜色
        m_strLColor = jobj["LColor"].toString();

        //理论曲线粗细
        m_iLWidth = jobj["LWidth"].toInt();

        //X轴参数
        m_strXParam = jobj["XParam"].toString();

        //Y轴参数
        m_strYParam = jobj["YParam"].toString();

        //理论曲线路径文件
        m_strLFile = jobj["LFile"].toString();
    }

    //设置控件
    setControls();
}

Q2wmapObjDlg::~Q2wmapObjDlg()
{
    delete ui;
}

//设置控件
void Q2wmapObjDlg::setControls()
{
    //目标名称
    ui->textEdit_targetName->setText(m_strName);

    //是否主目标
    ui->checkBox_MainObj->setChecked(m_bMainObj);

    //曲线颜色
    ui->pushButton_SelectCColor->setStyleSheet(QString("border:2px solid #555555; background-color: %1").arg(m_strCColor));

    //理论曲线颜色
    ui->pushButton_SelectLColor->setStyleSheet(QString("border:2px solid #555555; background-color: %1").arg(m_strLColor));

    //曲线粗细
    ui->textEdit_CWidth->setText(QString("%1").arg(m_iCWidth));

    //理论曲线粗细
    ui->textEdit_LWidth->setText(QString("%1").arg(m_iLWidth));

    //x轴参数
    ui->textEdit_xParam->setText(m_strXParam);

    //y轴参数
    ui->textEdit_yParam->setText(m_strYParam);

    //理论曲线路径文件
    ui->textEdit_LFile->setText(m_strLFile);
}

//选择曲线颜色按钮
void Q2wmapObjDlg::on_pushButton_SelectCColor_clicked()
{
    QColorDialog dlg;

    //取到设置的颜色
    QColor c = dlg.getColor();

    //无效颜色不处理(例如用户取消了对话框)
    if(c == QColor::Invalid)
    {
        setControls();
        return;
    }

    m_strCColor = c.name();

    //将按钮颜色设置为取到的颜色
    ui->pushButton_SelectCColor->setStyleSheet(QString("border:2px solid #555555; background-color: %1").arg(c.name()));
}

//选择理论曲线颜色按钮
void Q2wmapObjDlg::on_pushButton_SelectLColor_clicked()
{
    QColorDialog dlg;

    //取到设置的颜色
    QColor c = dlg.getColor();

    //无效颜色不处理(例如用户取消了对话框)
    if(c == QColor::Invalid)
    {
        return;
    }

    m_strLColor = c.name();

    //将按钮颜色设置为取到的颜色
    ui->pushButton_SelectLColor->setStyleSheet(QString("border:2px solid #555555; background-color: %1").arg(c.name()));
}


//选择X轴参数
void Q2wmapObjDlg::on_pushButton_SelectXParam_clicked()
{
    // 单参数选择对话框
    qdesigner_internal::QSelectParamDialog dlg;

    //未选择参数，退出
    if (dlg.showDialog() != QDialog::Accepted )
    {
        return;
    }

    m_strXParam = dlg.text();

    ui->textEdit_xParam->setText(m_strXParam);
}

//选择Y轴参数
void Q2wmapObjDlg::on_pushButton_SelectYParam_clicked()
{
    // 单参数选择对话框
    qdesigner_internal::QSelectParamDialog dlg;

    //未选择参数，退出
    if (dlg.showDialog() != QDialog::Accepted )
    {
        return;
    }

    m_strYParam = dlg.text();

    ui->textEdit_yParam->setText(m_strYParam);
}

//选择理论曲线文件
void Q2wmapObjDlg::on_pushButton_SelectLFile_clicked()
{
    m_strLFile = QFileDialog::getOpenFileName(this, tr("选择理论曲线"), "", tr("txt Files(*.txt)"));
    ui->textEdit_LFile->setText(m_strLFile);
}

//勾选/取消 是否主目标勾选框
void Q2wmapObjDlg::on_checkBox_MainObj_toggled(bool checked)
{
    if(checked == true)
    {
        m_bMainObj = true;
    }
    else
    {
        m_bMainObj = false;
    }
}

//目标名称改变
void Q2wmapObjDlg::on_textEdit_targetName_textChanged()
{
    m_strName = ui->textEdit_targetName->toPlainText();
}

//理论曲线粗细改变
void Q2wmapObjDlg::on_textEdit_LWidth_textChanged()
{
    m_iLWidth = ui->textEdit_LWidth->toPlainText().toInt();
}

//曲线粗细改变
void Q2wmapObjDlg::on_textEdit_CWidth_textChanged()
{
    m_iCWidth = ui->textEdit_CWidth->toPlainText().toInt();
}

//理论曲线文件路径改变
void Q2wmapObjDlg::on_textEdit_LFile_textChanged()
{
    m_strLFile = ui->textEdit_LFile->toPlainText();
}


