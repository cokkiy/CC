#include "graphobjdlg.h"
#include "ui_graphobjdlg.h"


#include <QColorDialog>
#include "paramselect/selectparamdialog.h"
#include <QFileDialog>
#include <QFontDialog>
#include <QFont>


GraphObjDlg::GraphObjDlg(QJsonObject jobj, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GraphObjDlg)
{
    ui->setupUi(this);

    if(jobj.isEmpty())
    {

        //给曲线赋默认值
        m_Xmax = 80 ;
        m_Xmin = 0 ;
        m_Ymax = 80 ;
        m_Ymin = 0 ;
        m_XAxisLabel = QString("X");
        m_YAxisLabel = QString("Y");
        m_XAxisdisplay = true;
        m_YAxisdisplay = true;
        m_Scaleplace_x = QString(tr("上"));//X的刻度位置（上中下）
        m_Scaleplace_y = QString(tr("右")); //Y的刻度位置（左中右）
        m_OriginPlace = QString(tr("左下")); //原点的位置：（左下、左上、右下、右上、上中、下中、正中）
        m_GraphName  = QString(tr("X-Y曲线"));
        m_GraphWidth = 3;
        m_LGraphWidth = 3;
        //曲线颜色
        m_strgraphColor = "#ff0000";  //红色
        //理论曲线颜色
        m_strLgraphColor = "#ffaa00"; //金色
//        //曲线颜色
//        m_strgraphColor = "#0000ff";  //蓝色
//        //理论曲线颜色
//        m_strLgraphColor = "#555555"; //灰色
        m_xParam = QString("[10001,2]");
        m_yParam = QString("[10001,2]");
        m_Lgraphfile = QString("");//理论曲线文件(路径)
        m_XAxiswideth = 2;
//        m_YAxiswideth = 2;
        m_chooseXAxisColor ="#ffaa00"; //金色,白色--"#ffffff"
        m_chooseXAxislabeldisplay = true;
        m_chooseYAxislabeldisplay = true;
        m_chooseXAxisScalelabeldisplay = true;
        m_chooseYAxisScalelabeldisplay = true;
        m_chooseXAxisScaleTickdisplay = true;
        m_chooseYAxisScaleTickdisplay = true;
        //取到设置的字体
        m_XAxislabelFont = "Sans Serif,28,-1,5,50,0,0,0,0,0";
        m_XAxisScalelabelFont = "Sans Serif,10,-1,5,50,0,0,0,0,0";
        m_YAxisScalelabelFont = "Sans Serif,10,-1,5,50,0,0,0,0,0";
        m_numOfXAxisScale = 4;
        m_numOfYAxisScale = 4;
        //刻度小线段长度的设置
        m_XAxisScaleRuler =10;
        m_YAxisScaleRuler =10;
        m_XAxisScaleprecision = 2;
        m_YAxisScaleprecision = 2;
        m_XAxisScalelabeloffset_x = "-15";
        m_XAxisScalelabeloffset_y = "0";
        m_YAxisScalelabeloffset_x = "0";
        m_YAxisScalelabeloffset_y = "0";


        //设置控件
        setControls();
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
        //x轴标签
        m_XAxisLabel =jobj["XAxisLabel"].toString();
        //y轴标签
        m_YAxisLabel =jobj["YAxisLabel"].toString();
        //x轴刻度位置
        m_Scaleplace_x =jobj["Scaleplace_x"].toString();
        //y轴刻度位置
        m_Scaleplace_y =jobj["Scaleplace_y"].toString();
        //原点的位置：左下、左上、右下、右上、上中、下中、正中
        m_OriginPlace =jobj["OriginPlace"].toString();
        //是否显示x轴
        m_XAxisdisplay =jobj["XAxisdisplay"].toBool();
        //是否显示y轴
        m_YAxisdisplay =jobj["YAxisdisplay"].toBool();
        //实时曲线名称
        m_GraphName = jobj["GraphName"].toString();
        //实时曲线宽度
        m_GraphWidth =jobj["GraphWidth"].toInt();
        //理论曲线颜色
        m_strgraphColor =jobj["strgraphColor"].toString();
        //理论曲线名称
//        m_LGraphName = jobj["GraphName"].toString();
        //理论曲线宽度
        m_LGraphWidth = jobj["LGraphWidth"].toInt();
        //理论曲线颜色
        m_strLgraphColor = jobj["strLgraphColor"].toString();
        //实时曲线x参数
        m_xParam = jobj["xParam"].toString();
        //实时曲线y参数
        m_yParam = jobj["yParam"].toString();
        //理论曲线路径
        m_Lgraphfile = jobj["Lgraphfile"].toString();
        m_XAxiswideth = jobj["XAxiswideth"].toInt();
        m_chooseXAxisColor = jobj["chooseXAxisColor"].toString();
        m_chooseXAxislabeldisplay = jobj["chooseXAxislabeldisplay"].toBool();
        m_chooseYAxislabeldisplay = jobj["chooseYAxislabeldisplay"].toBool();
        m_chooseXAxisScalelabeldisplay = jobj["chooseXAxisScalelabeldisplay"].toBool();
        m_chooseYAxisScalelabeldisplay = jobj["chooseYAxisScalelabeldisplay"].toBool();
        m_chooseXAxisScaleTickdisplay = jobj["chooseXAxisScaleTickdisplay"].toBool();
        m_chooseYAxisScaleTickdisplay = jobj["chooseYAxisScaleTickdisplay"].toBool();
        m_XAxislabelFont= jobj["XAxislabelFont"].toString();
        m_XAxisScalelabelFont= jobj["XAxisScalelabelFont"].toString();
        m_YAxisScalelabelFont= jobj["YAxisScalelabelFont"].toString();
        m_numOfXAxisScale = jobj["numOfXAxisScale"].toInt();
        m_XAxisScaleRuler = jobj["XAxisScaleRuler"].toInt();
        m_numOfYAxisScale = jobj["numOfYAxisScale"].toInt();
        m_YAxisScaleRuler = jobj["YAxisScaleRuler"].toInt();

        m_XAxisScaleprecision = jobj["XAxisScaleprecision"].toInt();
        m_YAxisScaleprecision = jobj["YAxisScaleprecision"].toInt();

        m_XAxisScalelabeloffset_x = jobj["XAxisScalelabeloffset_x"].toString();
        m_XAxisScalelabeloffset_y = jobj["XAxisScalelabeloffset_y"].toString();
        m_YAxisScalelabeloffset_x = jobj["YAxisScalelabeloffset_x"].toString();
        m_YAxisScalelabeloffset_y = jobj["YAxisScalelabeloffset_y"].toString();

        //设置控件
        setControls();
    }



}

GraphObjDlg::~GraphObjDlg()
{
    delete ui;
}



//设置控件
void GraphObjDlg::setControls()
{

    ui->lineEdit_Xmax_tab->setText(QString::number(m_Xmax));
    ui->lineEdit_Xmin_tab->setText(QString::number(m_Xmin));
    ui->lineEdit_Ymax_tab->setText(QString::number(m_Ymax));
    ui->lineEdit_Ymin_tab->setText(QString::number(m_Ymin));
    ui->lineEdit_XAxisLabel_tab->setText(m_XAxisLabel);
    ui->lineEdit_YAxisLabel_tab->setText(m_YAxisLabel);
    ui->checkBox_chooseAxisdisplay_x->setChecked(m_XAxisdisplay);
    ui->checkBox_chooseAxisdisplay_y->setChecked(m_YAxisdisplay);
    ui->comboBox_Scaleplace_x->setCurrentText(m_Scaleplace_x);
    ui->comboBox_Scaleplace_y->setCurrentText(m_Scaleplace_y);
    //原点的位置：左下、左上、右下、右上、上中、下中、正中
    ui->comboBox_OriginPlace->setCurrentText(m_OriginPlace);
    ui->lineEdit_GraphName_tab->setText(m_GraphName);
    ui->lineEdit_GraphWidth_tab->setText(QString::number(m_GraphWidth));
    ui->pushButton_chooseGraphColor_tab->setStyleSheet(QString("background-color: %1").arg(m_strgraphColor));
    ui->lineEdit_LGraphWidth_tab->setText(QString::number(m_LGraphWidth));
    ui->pushButton_chooseLGraphColor_tab->setStyleSheet(QString("background-color: %1").arg(m_strLgraphColor));
    ui->lineEdit_inputXparam_tab->setText(m_xParam);
    ui->lineEdit_inputYparam_tab->setText(m_yParam);
    ui->lineEdit_chooseGraphbasemap_tab->setText(m_Lgraphfile);
    ui->lineEdit_XAxiswideth_tab->setText(QString::number(m_XAxiswideth));
    ui->lineEdit_numOfXAxisScale_tab->setText(QString::number(m_numOfXAxisScale));
    ui->lineEdit_XAxisScaleRuler_tab->setText(QString::number(m_XAxisScaleRuler));
    ui->lineEdit_numOfYAxisScale_tab->setText(QString::number(m_numOfYAxisScale));
    ui->lineEdit_YAxisScaleRuler_tab->setText(QString::number(m_YAxisScaleRuler));
    ui->lineEdit_XAxisScaleprecision_tab->setText(QString::number(m_XAxisScaleprecision));
    ui->lineEdit_YAxisScaleprecision_tab->setText(QString::number(m_YAxisScaleprecision));
    ui->pushButton_chooseXAxisColor_tab->setStyleSheet(QString("background-color: %1").arg(m_chooseXAxisColor));
    ui->checkBox_chooseXAxislabeldisplay_tab->setChecked(m_chooseXAxislabeldisplay);
    ui->checkBox_chooseYAxislabeldisplay_tab->setChecked(m_chooseYAxislabeldisplay);
    ui->checkBox_chooseXAxisScalelabeldisplay_tab->setChecked(m_chooseXAxisScalelabeldisplay);
    ui->checkBox_chooseYAxisScalelabeldisplay_tab->setChecked(m_chooseYAxisScalelabeldisplay);
    ui->checkBox_chooseXAxisScaleTickdisplay_tab->setChecked(m_chooseXAxisScaleTickdisplay);
    ui->checkBox_chooseYAxisScaleTickdisplay_tab->setChecked(m_chooseYAxisScaleTickdisplay);
    //准备好向界面的填充文字---字的大小
    //XAxislabelFont
    QString m_XAxislabelFontstr = m_XAxislabelFont;
    QStringList m_XAxislabelFontwords = m_XAxislabelFontstr.split(",");
    qint32 indexofXAxislabelFontsize = 1;
    QString m_XAxislabelFont_ui = m_XAxislabelFontwords.at(indexofXAxislabelFontsize);

    QString m_XAxisScalelabelFontstr = m_XAxisScalelabelFont;
    QStringList m_XAxisScalelabelFontwords = m_XAxisScalelabelFontstr.split(",");
    qint32 indexofXAxisScalelabelFontsize = 1;
    QString m_XAxisScalelabelFont_ui = m_XAxisScalelabelFontwords.at(indexofXAxisScalelabelFontsize);
    //YAxisScalelabelFont
    QString m_YAxisScalelabelFontstr = m_YAxisScalelabelFont;
    QStringList m_YAxisScalelabelFontwords = m_YAxisScalelabelFontstr.split(",");
    qint32 indexofYAxisScalelabelFontsize = 1;
    QString m_YAxisScalelabelFont_ui = m_YAxisScalelabelFontwords.at(indexofYAxisScalelabelFontsize);
    //返回字体字符串到空行中
    ui->lineEdit_XAxislabelFont_tab->setText(m_XAxislabelFont_ui);

    ui->lineEdit_XAxisScalelabelFont_tab->setText(m_XAxisScalelabelFont_ui);
    ui->lineEdit_YAxisScalelabelFont_tab->setText(m_YAxisScalelabelFont_ui);


    ui->lineEdit_XAxisScalelabeloffset_x_tab->setText(m_XAxisScalelabeloffset_x);
    ui->lineEdit_XAxisScalelabeloffset_y_tab->setText(m_XAxisScalelabeloffset_y);
    ui->lineEdit_YAxisScalelabeloffset_x_tab->setText(m_YAxisScalelabeloffset_x);
    ui->lineEdit_YAxisScalelabeloffset_y_tab->setText(m_YAxisScalelabeloffset_y);

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


//选择实时曲线颜色
void GraphObjDlg::on_pushButton_chooseGraphColor_tab_clicked()
{


    QColorDialog graphcolordialog;

    //取到设置的颜色
    QColor tempgraphcolor = graphcolordialog.getColor();

    //无效颜色不处理(例如用户取消了对话框)
    if(tempgraphcolor == QColor::Invalid)
    {
        setControls();
        return;
    }
    m_strgraphColor= tempgraphcolor.name();
    //将按钮颜色设置为取到的颜色
    //最爽的还是样式表
    //background-color: rgb(255, 0, 0);
    //ui->label_chooseGraphColor->setStyleSheet("background-color: rgb(255, 0, 0)");
    ui->pushButton_chooseGraphColor_tab->setStyleSheet(QString("background-color: %1").arg(tempgraphcolor.name()));

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


//选择理论曲线颜色
void GraphObjDlg::on_pushButton_chooseLGraphColor_tab_clicked()
{

    QColorDialog graphcolordialog;
//    QColor m_graphcolor;

    //取到设置的颜色
    QColor tempLgraphcolor = graphcolordialog.getColor();


    //无效颜色不处理(例如用户取消了对话框)
    if(tempLgraphcolor == QColor::Invalid)
    {
       return;
    }
    m_strLgraphColor = tempLgraphcolor.name();
    //将按钮颜色设置为取到的颜色
    //最爽的还是样式表
    //background-color: rgb(255, 0, 0);
    //ui->label_chooseGraphColor->setStyleSheet("background-color: rgb(255, 0, 0)");
    ui->pushButton_chooseLGraphColor_tab->setStyleSheet(QString("background-color: %1").arg(tempLgraphcolor.name()));


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




void GraphObjDlg::on_lineEdit_XAxiswideth_tab_textChanged(const QString &arg1)
{
    m_XAxiswideth = arg1.toInt();

}

void GraphObjDlg::on_pushButton_chooseXAxisColor_tab_clicked()
{
    QColorDialog XAxiscolordialog;

    //取到设置的颜色
    QColor tempXAxiscolor = XAxiscolordialog.getColor();

    //无效颜色不处理(例如用户取消了对话框)
    if(tempXAxiscolor == QColor::Invalid)
    {
        setControls();
        return;
    }
    m_chooseXAxisColor= tempXAxiscolor.name();
    //将按钮颜色设置为取到的颜色
    //最爽的还是样式表
     ui->pushButton_chooseXAxisColor_tab->setStyleSheet(QString("background-color: %1").arg(tempXAxiscolor.name()));

}




void GraphObjDlg::on_checkBox_chooseXAxislabeldisplay_tab_clicked(bool checked)
{
    if(checked)
    {
        m_chooseXAxislabeldisplay=true;
    }
    else
    {
        m_chooseXAxislabeldisplay=false;
    }

}



void GraphObjDlg::on_checkBox_chooseYAxislabeldisplay_tab_clicked(bool checked)
{
    if(checked)
    {
        m_chooseYAxislabeldisplay=true;
    }
    else
    {
        m_chooseYAxislabeldisplay=false;
    }

}




void GraphObjDlg::on_lineEdit_numOfXAxisScale_tab_textChanged(const QString &arg1)
{
    m_numOfXAxisScale = arg1.toInt();
}

void GraphObjDlg::on_lineEdit_XAxisScaleRuler_tab_textChanged(const QString &arg1)
{
    m_XAxisScaleRuler = arg1.toInt();
}



void GraphObjDlg::on_lineEdit_XAxisScaleprecision_tab_textChanged(const QString &arg1)
{
    m_XAxisScaleprecision = arg1.toInt();
}

void GraphObjDlg::on_checkBox_chooseXAxisScalelabeldisplay_tab_clicked(bool checked)
{
    if(checked)
    {
        m_chooseXAxisScalelabeldisplay=true;
    }
    else
    {
        m_chooseXAxisScalelabeldisplay=false;
    }

}



void GraphObjDlg::on_lineEdit_numOfYAxisScale_tab_textChanged(const QString &arg1)
{
    m_numOfYAxisScale = arg1.toDouble();
}

void GraphObjDlg::on_lineEdit_YAxisScaleRuler_tab_textChanged(const QString &arg1)
{
    m_YAxisScaleRuler = arg1.toInt();
}




void GraphObjDlg::on_lineEdit_YAxisScaleprecision_tab_textChanged(const QString &arg1)
{
    m_YAxisScaleprecision = arg1.toInt();
}

void GraphObjDlg::on_checkBox_chooseYAxisScalelabeldisplay_tab_clicked(bool checked)
{
    if(checked)
    {
        m_chooseYAxisScalelabeldisplay=true;
    }
    else
    {
        m_chooseYAxisScalelabeldisplay=false;
    }

}


void GraphObjDlg::on_pushButton_XAxislabelFont_tab_clicked()
{
//    bool ok;
//    QFontDialog XAxislabelfontDialog;
//    QFont XAxislabelfont = XAxislabelfontDialog.getFont(&ok,this);

    bool ok;
    //保存用户选中的字体，以便每次作为默认字体载入，且显示在默认栏选中状态
    bool tempok;
    QFont firstfont ;
    tempok = firstfont.fromString(m_XAxislabelFont);
    //    QFont XAxislabelfont = QFontDialog::getFont(&ok, QFont("Times", 28), this);
    QFont XAxislabelfont = QFontDialog::getFont(&ok, firstfont, this);

    if (ok) {
        // 点击"ok",字体设为用户选择的字体
    } else {
        // 点击“cancel”,字体设置为初始化的默认字体：Times,28
        // 取到设置的字体
        // m_XAxislabelFont = "Sans Serif,28,-1,5,50,0,0,0,0,0";
        //setControls();
    }

    //转化为字符串描述
    m_XAxislabelFont= XAxislabelfont.toString();
    //准备好向界面的填充文字---字的大小
    QString m_fontstr = XAxislabelfont.toString();
    QStringList m_fontwords = m_fontstr.split(",");
    qint32 indexoffontsize = 1;
    QString m_XAxislabelFont_ui = m_fontwords.at(indexoffontsize);

    //返回字体字符串到空行中
    ui->lineEdit_XAxislabelFont_tab->setText(m_XAxislabelFont_ui);
    //    for (int i = 0; i < m_fontwords.size(); ++i)
    //         cout << m_fontwords.at(i).toLocal8Bit().constData() << endl;
}


void GraphObjDlg::on_pushButton_XAxisScalelabelFont_tab_clicked()
{
    bool ok;
    //保存用户选中的字体，以便每次作为默认字体载入，且显示在默认栏选中状态
    bool tempok;
    QFont firstfont ;
    tempok = firstfont.fromString(m_XAxisScalelabelFont);
    QFont XAxisScalelabelfont = QFontDialog::getFont(&ok, firstfont, this);

    if (ok) {
        // 点击"ok",字体设为用户选择的字体
    } else {
        // 点击“cancel”,字体设置为初始化的默认字体：firstfont:Times,28
        //m_YAxislabelFont = "Sans Serif,28,-1,5,50,0,0,0,0,0";
    }

    //转化为字符串描述
    m_XAxisScalelabelFont= XAxisScalelabelfont.toString();
    //准备好向界面的填充文字---字的大小
    QString m_fontstr = XAxisScalelabelfont.toString();
    QStringList m_fontwords = m_fontstr.split(",");
    qint32 indexoffontsize = 1;
    QString m_XAxisScalelabelFont_ui = m_fontwords.at(indexoffontsize);
    //返回字体字符串到空行中
    ui->lineEdit_XAxisScalelabelFont_tab->setText(m_XAxisScalelabelFont_ui);

}

void GraphObjDlg::on_pushButton_YAxisScalelabelFont_tab_clicked()
{
    bool ok;
    //保存用户选中的字体，以便每次作为默认字体载入，且显示在默认栏选中状态
    bool tempok;
    QFont firstfont ;
    tempok = firstfont.fromString(m_YAxisScalelabelFont);
    QFont YAxisScalelabelfont = QFontDialog::getFont(&ok, firstfont, this);

    if (ok) {
        // 点击"ok",字体设为用户选择的字体
    } else {
        // 点击“cancel”,字体设置为初始化的默认字体：firstfont:Times,28
        //m_YAxislabelFont = "Sans Serif,28,-1,5,50,0,0,0,0,0";
    }


    //转化为字符串描述
    m_YAxisScalelabelFont= YAxisScalelabelfont.toString();
    //准备好向界面的填充文字---字的大小
    QString m_fontstr = YAxisScalelabelfont.toString();
    QStringList m_fontwords = m_fontstr.split(",");
    qint32 indexoffontsize = 1;
    QString m_YAxisScalelabelFont_ui = m_fontwords.at(indexoffontsize);
    //返回字体字符串到空行中
    ui->lineEdit_YAxisScalelabelFont_tab->setText(m_YAxisScalelabelFont_ui);

}

void GraphObjDlg::on_lineEdit_XAxisScalelabeloffset_x_tab_textChanged(const QString &arg1)
{
    m_XAxisScalelabeloffset_x = arg1;
}
void GraphObjDlg::on_lineEdit_XAxisScalelabeloffset_y_tab_textChanged(const QString &arg1)
{
    m_XAxisScalelabeloffset_y = arg1;
}
void GraphObjDlg::on_lineEdit_YAxisScalelabeloffset_x_tab_textChanged(const QString &arg1)
{
    m_YAxisScalelabeloffset_x = arg1;
}
void GraphObjDlg::on_lineEdit_YAxisScalelabeloffset_y_tab_textChanged(const QString &arg1)
{
    m_YAxisScalelabeloffset_y = arg1;
}

void GraphObjDlg::on_checkBox_chooseXAxisScaleTickdisplay_tab_clicked(bool checked)
{
    if(checked)
    {
        m_chooseXAxisScaleTickdisplay=true;
    }
    else
    {
        m_chooseXAxisScaleTickdisplay=false;
    }
    
}

void GraphObjDlg::on_checkBox_chooseYAxisScaleTickdisplay_tab_clicked(bool checked)
{
    if(checked)
    {
        m_chooseYAxisScaleTickdisplay=true;
    }
    else
    {
        m_chooseYAxisScaleTickdisplay=false;
    }

}


void GraphObjDlg::on_comboBox_OriginPlace_currentTextChanged(const QString &arg1)
{
      m_OriginPlace = arg1;

}


