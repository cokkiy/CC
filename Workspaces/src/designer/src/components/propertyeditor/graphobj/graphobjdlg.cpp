#include "graphobjdlg.h"
#include "ui_graphobjdlg.h"

#include <QColorDialog>
#include "selectparamdialog.h"
#include <QFileDialog>

GraphObjDlg::GraphObjDlg(QJsonObject jobj, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GraphObjDlg)
{
    ui->setupUi(this);

    if(jobj.isEmpty())
    {

        //给曲线赋默认值
        m_Xmax = 100 ;
        m_Xmin = 0 ;
        m_Ymax = 100 ;
        m_Ymin = 0 ;
        m_Xoffset = 1;
        m_Yoffset = 1;
        m_XAxisLabel = QString("X");
        m_YAxisLabel = QString("Y");
        m_XAxisdisplay = true;
        m_YAxisdisplay = true;
        m_Scaleplace_x = QString(tr("上"));//X的刻度位置（上中下）
        m_Scaleplace_y = QString(tr("右")); //Y的刻度位置（左中右）

        m_GraphName  = QString(tr("X-Y曲线"));
        m_GraphWidth = 3;
        m_GraphBuffer = 3000;
        m_strgraphColor  = QString("rgb(255, 0, 0)");
        m_LGraphWidth = 3;
        m_LGraphBuffer = 3000;
        m_strLgraphColor = QString("rgb(223, 223, 223)");

        m_xParam = QString("[10001,2]");
        m_yParam = QString("[10001,2]");

        m_Lgraphfile = QString("");//理论曲线文件(路径)
    }
    else
    {
        //下面开始挨个解析

        //x轴最大值
        m_Xmax = jobj["Xmax"].toDouble();
        //x轴最小值
        m_Xmin = jobj["Xmin"].toDouble();
        //y轴最大值
        m_Ymax = jobj["Ymax"].toDouble();
        //y轴最小值
        m_Ymin = jobj["Xmin"].toDouble();
        //x轴标签文本偏移
        m_Xoffset =jobj["Xoffset"].toInt();
        //y轴标签文本偏移
        m_Yoffset =jobj["Yoffset"].toInt();
        //x轴标签
        m_XAxisLabel =jobj["XAxisLabel"].toString();
        //y轴标签
        m_YAxisLabel =jobj["YAxisLabel"].toString();
        //x轴刻度位置
        m_Scaleplace_x =jobj["Scaleplace_x"].toString();
        //y轴刻度位置
        m_Scaleplace_y =jobj["Scaleplace_y"].toString();
        //是否显示x轴
        m_XAxisdisplay =jobj["XAxisdisplay"].toBool();
        //是否显示y轴
        m_YAxisdisplay =jobj["YAxisdisplay"].toBool();

        //实时曲线名称
        m_GraphName = jobj["GraphName"].toString();
        //实时曲线宽度
        m_GraphWidth =jobj["GraphWidth"].toInt();
        //实时曲线缓冲区大小
        m_GraphBuffer = jobj["GraphBuffer"].toDouble();
        //理论曲线颜色
        m_strgraphColor =jobj["strgraphColor"].toString();

        //理论曲线名称
//        m_LGraphName = jobj["GraphName"].toString();
        //理论曲线宽度
        m_LGraphWidth = jobj["LGraphWidth"].toInt();
        //理论曲线缓冲区大小
        m_LGraphBuffer = jobj["LGraphBuffer"].toDouble();
        //理论曲线颜色
        m_strLgraphColor = jobj["strLgraphColor"].toString();

        //实时曲线x参数
        m_xParam = jobj["xParam"].toString();
        //实时曲线y参数
        m_yParam = jobj["yParam"].toString();

        //理论曲线路径
        m_Lgraphfile = jobj["Lgraphfile"].toString();
    }

    //设置控件
    setControls();
}

GraphObjDlg::~GraphObjDlg()
{
    delete ui;
}

//设置控件
void GraphObjDlg::setControls()
{
//    //目标名称
//    ui->textEdit_targetName->setText(m_strName);

//    //曲线颜色
//    ui->pushButton_SelectCColor->setStyleSheet(QString("border:2px solid #555555; background-color: %1").arg(m_strCColor));

//    //理论曲线颜色
//    ui->pushButton_SelectLColor->setStyleSheet(QString("border:2px solid #555555; background-color: %1").arg(m_strLColor));

    //tab名称----目标名称


    ui->lineEdit_Xmax_tab->setText(QString::number(m_Xmax));
    ui->lineEdit_Xmin_tab->setText(QString::number(m_Xmin));
    ui->lineEdit_Ymax_tab->setText(QString::number(m_Ymax));
    ui->lineEdit_Ymin_tab->setText(QString::number(m_Ymin));
    ui->lineEdit_Xoffset_tab->setText(QString::number(m_Xoffset));
    ui->lineEdit_Yoffset_tab->setText(QString::number(m_Yoffset));
    ui->lineEdit_XAxisLabel_tab->setText(m_XAxisLabel);
    ui->lineEdit_YAxisLabel_tab->setText(m_YAxisLabel);
    ui->checkBox_chooseAxisdisplay_x->setChecked(m_XAxisdisplay);
    ui->checkBox_chooseAxisdisplay_y->setChecked(m_YAxisdisplay);
    ui->comboBox_Scaleplace_x->setCurrentText(m_Scaleplace_x);
    ui->comboBox_Scaleplace_y->setCurrentText(m_Scaleplace_y);

    ui->lineEdit_GraphName_tab->setText(m_GraphName);
    ui->lineEdit_GraphWidth_tab->setText(QString::number(m_GraphWidth));
    ui->lineEdit_GraphBuffer_tab->setText(QString::number(m_GraphBuffer));
    ui->label_chooseGraphColor->setStyleSheet(QString("background-color: %1").arg(m_strgraphColor));

    ui->lineEdit_LGraphWidth_tab->setText(QString::number(m_LGraphWidth));
    ui->lineEdit_LGraphBuffer_tab->setText(QString::number(m_LGraphBuffer));
    ui->label_chooseLGraphColor->setStyleSheet(QString("background-color: %1").arg(m_strLgraphColor));


    ui->lineEdit_inputXparam_tab->setText(m_xParam);
    ui->lineEdit_inputYparam_tab->setText(m_yParam);
    ui->lineEdit_chooseGraphbasemap_tab->setText(m_Lgraphfile);
}


////选择理论曲线文件
//void GraphObjDlg::on_pushButton_SelectLFile_clicked()
//{
//    m_strLFile = QFileDialog::getOpenFileName(this, tr("选择理论曲线"), "", tr("txt Files(*.txt)"));
//    ui->textEdit_LFile->setText(m_strLFile);
//}

////理论曲线文件路径改变
//void GraphObjDlg::on_textEdit_LFile_textChanged()
//{
//    m_strLFile = ui->textEdit_LFile->toPlainText();
//}




//zjb add new fuction is underside:


//X轴最大值
void GraphObjDlg::on_lineEdit_Xmax_tab_textChanged(const QString &arg1)
{

   m_Xmax = arg1.toDouble();

}

//X轴最小值
void GraphObjDlg::on_lineEdit_Xmin_tab_textChanged(const QString &arg1)
{

    m_Xmin = arg1.toDouble();
}


//Y轴最大值
void GraphObjDlg::on_lineEdit_Ymax_tab_textChanged(const QString &arg1)
{
   m_Ymax = arg1.toDouble();
}

//Y轴最小值
void GraphObjDlg::on_lineEdit_Ymin_tab_textChanged(const QString &arg1)
{

    m_Ymin = arg1.toDouble();

}

//X轴文本偏移量
void GraphObjDlg::on_lineEdit_Xoffset_tab_textChanged(const QString &arg1)
{
    m_Xoffset = arg1.toInt();
}

//Y轴文本偏移量
void GraphObjDlg::on_lineEdit_Yoffset_tab_textChanged(const QString &arg1)
{

    m_Yoffset = arg1.toInt();
}


//X轴轴标签
void GraphObjDlg::on_lineEdit_XAxisLabel_tab_textChanged(const QString &arg1)
{
    m_XAxisLabel = arg1;
}

//Y轴轴标签
void GraphObjDlg::on_lineEdit_YAxisLabel_tab_textChanged(const QString &arg1)
{

    m_YAxisLabel = arg1;

}

//是否选中显示X轴
void GraphObjDlg::on_checkBox_chooseAxisdisplay_x_clicked(bool checked)
{

    if(checked)
    {
        m_XAxisdisplay=true;
    }
    else
    {
        m_XAxisdisplay=false;
    }

}

//是否选中显示Y轴
void GraphObjDlg::on_checkBox_chooseAxisdisplay_y_clicked(bool checked)
{
    if(checked)
    {
        m_YAxisdisplay=true;
    }
    else
    {
        m_YAxisdisplay=false;
    }
}

//X的刻度位置（上中下）
void GraphObjDlg::on_comboBox_Scaleplace_x_currentTextChanged(const QString &arg1)
{

    m_Scaleplace_x = arg1;

}

//Y的刻度位置（左中右）
void GraphObjDlg::on_comboBox_Scaleplace_y_currentTextChanged(const QString &arg1)
{

    m_Scaleplace_y = arg1;

}


//曲线名称（图例名称）
void GraphObjDlg::on_lineEdit_GraphName_tab_textChanged(const QString &arg1)
{
    m_GraphName = arg1;

}


//实时曲线宽度
void GraphObjDlg::on_lineEdit_GraphWidth_tab_textChanged(const QString &arg1)
{
    m_GraphWidth = arg1.toInt();
}

//实时曲线缓冲区大小
void GraphObjDlg::on_lineEdit_GraphBuffer_tab_textChanged(const QString &arg1)
{

    m_GraphBuffer = arg1.toDouble();

}

//选择实时曲线颜色
void GraphObjDlg::on_pushButton_chooseGraphColor_tab_clicked()
{


    QColorDialog graphcolordialog;

    //取到设置的颜色
    QColor m_graphcolor = graphcolordialog.getColor();


    //无效颜色不处理(例如用户取消了对话框)
    if(m_graphcolor == QColor::Invalid)
    {
        setControls();
        return;
    }
    m_strgraphColor= m_graphcolor.name();
    //将按钮颜色设置为取到的颜色
    //最爽的还是样式表
    //background-color: rgb(255, 0, 0);
    //ui->label_chooseGraphColor->setStyleSheet("background-color: rgb(255, 0, 0)");
    ui->label_chooseGraphColor->setStyleSheet(QString("background-color: %1").arg(m_graphcolor.name()));

}

//选择实时曲线样式
void GraphObjDlg::on_pushButton_Graphmode_tab_clicked()
{

}

//理论曲线宽度
void GraphObjDlg::on_lineEdit_LGraphWidth_tab_textChanged(const QString &arg1)
{
    m_LGraphWidth = arg1.toInt();
}

//理论曲线缓冲区大小
void GraphObjDlg::on_lineEdit_LGraphBuffer_tab_textChanged(const QString &arg1)
{
    m_LGraphBuffer = arg1.toDouble();

}

//选择理论曲线颜色
void GraphObjDlg::on_pushButton_chooseLGraphColor_tab_clicked()
{

    QColorDialog graphcolordialog;
//    QColor m_graphcolor;

    //取到设置的颜色
    QColor m_Lgraphcolor = graphcolordialog.getColor();


    //无效颜色不处理(例如用户取消了对话框)
    if(m_Lgraphcolor == QColor::Invalid)
    {
        setControls();
        return;
    }
    m_strLgraphColor = m_Lgraphcolor.name();
    //将按钮颜色设置为取到的颜色
    //最爽的还是样式表
    //background-color: rgb(255, 0, 0);
    //ui->label_chooseGraphColor->setStyleSheet("background-color: rgb(255, 0, 0)");
    ui->label_chooseLGraphColor->setStyleSheet(QString("background-color: %1").arg(m_Lgraphcolor.name()));


}





//选择弹道底图（理论弹道）
void GraphObjDlg::on_pushButton_chooseGraphbasemap_tab_clicked()
{

    QFileDialog m_LgraphOpenDialog;

    //文件父指针，文件打开对话框标题，文件路径（自选），文件过滤器（自选）
    m_Lgraphfile = m_LgraphOpenDialog.getOpenFileName(this,tr("装载理论弹道"),"",tr("All Files(*.*)"));

    ui->lineEdit_chooseGraphbasemap_tab->setText(m_Lgraphfile);

}



//选择X参数
void GraphObjDlg::on_pushButton_chooseXparam_tab_clicked()
{


    // 单参数选择对话框
    qdesigner_internal::QSelectParamDialog dlg;
    //未选择参数，退出
    if (dlg.showDialog() != QDialog::Accepted )
        return;

    m_xParam = dlg.text();

    ui->lineEdit_inputXparam_tab->setText(dlg.text());
}

//选择Y参数
void GraphObjDlg::on_pushButton_chooseYparam_tab_clicked()
{
    // 单参数选择对话框
    qdesigner_internal::QSelectParamDialog dlg;
    //未选择参数，退出
    if (dlg.showDialog() != QDialog::Accepted )
        return;

    m_yParam = dlg.text();

    ui->lineEdit_inputYparam_tab->setText(dlg.text());

}


