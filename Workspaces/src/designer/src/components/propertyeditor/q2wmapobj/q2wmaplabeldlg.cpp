#include "q2wmaplabeldlg.h"
#include "ui_q2wmaplabeldlg.h"
#include <QColorDialog>
#include <QFontDialog>

Q2wmapLabelDlg::Q2wmapLabelDlg(QJsonObject jobj) :  ui(new Ui::Q2wmapLabelDlg)
{
    ui->setupUi(this);

    m_strType = "Label";

    if(jobj.isEmpty())
    {
        //标签名称
        m_strName = "新标签";

        //位置经度
        m_dbPosL = 0.0;

        //位置纬度
        m_dbPosB = 0.0;

        //边线颜色
        m_strColor= "#0000ff";

        //字体
        m_strFont = "";
    }
    else
    {
        //下面开始挨个解析

        //标签名称
        m_strName = jobj["Name"].toString();

        //位置经度
        m_dbPosL = jobj["PosL"].toDouble();

        //位置纬度
        m_dbPosB = jobj["PosB"].toDouble();

        //边线颜色
        m_strColor= jobj["Color"].toString();

        //字体
        m_strFont = jobj["Font"].toString();
    }

    // 设置控件
    setControls();
}

Q2wmapLabelDlg::~Q2wmapLabelDlg()
{
    delete ui;
}

//设置控件
void Q2wmapLabelDlg::setControls()
{
    //区域名称
    ui->lineEditLabelName->setText(m_strName);

    //位置经度
    ui->lineEditPosL->setText(QString("%1").arg(m_dbPosL));

    //位置纬度
    ui->lineEditPosB->setText(QString("%1").arg(m_dbPosB));

    //边线颜色
    ui->pushButton_SelectColor->setStyleSheet(QString("border:2px solid #555555; background-color: %1").arg(m_strColor));

    //字体
    ui->pushButton_Font->setText(m_strFont);
}

//改变标签名称
void Q2wmapLabelDlg::on_lineEditLabelName_textChanged(const QString &arg1)
{
    m_strName = ui->lineEditLabelName->text();
}

//改变位置经度
void Q2wmapLabelDlg::on_lineEditPosL_textChanged(const QString &arg1)
{
    m_dbPosL = ui->lineEditPosL->text().toDouble();
}

//改变位置纬度
void Q2wmapLabelDlg::on_lineEditPosB_textChanged(const QString &arg1)
{
    m_dbPosB = ui->lineEditPosB->text().toDouble();
}

//改变标签颜色
void Q2wmapLabelDlg::on_pushButton_SelectColor_clicked()
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

//改变标签字体
void Q2wmapLabelDlg::on_pushButton_Font_clicked()
{
    QFontDialog dlg;

    bool ok = false;

    //初始字体
    QFont fontInit;

    fontInit.fromString(m_strFont);

    //取到设置的字体
    QFont font = dlg.getFont(&ok, fontInit);

    //无效字体不处理(例如用户取消了对话框)
    if(ok == false)
    {
        return;
    }

    m_strFont = font.toString();

    //将按钮文字设置为取到的字体
    ui->pushButton_Font->setText(m_strFont);
}
