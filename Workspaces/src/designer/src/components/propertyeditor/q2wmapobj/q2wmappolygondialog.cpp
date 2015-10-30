#include "q2wmappolygondialog.h"
#include "ui_q2wmappolygondialog.h"
#include <QColorDialog>

Q2wmapPolygonDialog::Q2wmapPolygonDialog(QJsonObject jobj) : ui(new Ui::Q2wmapPolygonDialog)
{
    ui->setupUi(this);

    m_strType = "Polygon";

    if(jobj.isEmpty())
    {
        //图形名称
        m_strName = "新定制图形";

        //边线颜色
        m_strColor= "#0000ff";

        //边线粗细
        m_iWidth = 3;
    }
    else
    {
        //下面开始挨个解析

        //图形名称
        m_strName = jobj["Name"].toString();

        //边线颜色
        m_strColor= jobj["Color"].toString();

        //边线粗细
        m_iWidth = jobj["Width"].toInt();

        //组成定制图形的点
        m_strPoly = jobj["Poly"].toString();

    }

    // 设置控件
    setControls();
}

//设置控件
void Q2wmapPolygonDialog::setControls()
{
    //图形名称
    ui->lineEditName->setText(m_strName);

    //边线颜色
    ui->pushButton_SelectColor->setStyleSheet(QString("border:2px solid #555555; background-color: %1").arg(m_strColor));

    //边线粗细
    ui->lineEditWidth->setText(QString("%1").arg(m_iWidth));

    //组成定制图形的点
    ui->textEdit->setText(m_strPoly);
}


Q2wmapPolygonDialog::~Q2wmapPolygonDialog()
{
    delete ui;
}

//改变名称
void Q2wmapPolygonDialog::on_lineEditName_textChanged(const QString &arg1)
{
    m_strName = ui->lineEditName->text();
}

//改变边线粗细
void Q2wmapPolygonDialog::on_lineEditWidth_textChanged(const QString &arg1)
{
    m_iWidth = ui->lineEditWidth->text().toInt();
}

//改变颜色
void Q2wmapPolygonDialog::on_pushButton_SelectColor_clicked()
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

//改变多行文本
void Q2wmapPolygonDialog::on_textEdit_textChanged()
{
    m_strPoly = ui->textEdit->toPlainText();
}
