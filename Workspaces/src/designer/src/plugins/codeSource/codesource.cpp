#include "codesource.h"
#include <QtMath>
#include<QMessageBox>
#include<QScrollBar>
#include <QDebug>
#include<QPalette>

codeSource::codeSource(QWidget *parent) :
    QTextEdit(parent)
{
    ////init
    setWindowTitle(tr("codeSource"));
    setData("0");//初始数据

    setBlankNum(1);// blank number
    setTextColor(Qt::black); //设置文本颜色
    m_txt = center;//初始字体位置为居中对齐
    m_bgcolor.setRgb(210,210,255);//背景颜色

    ////设置一个500ms定时器
    m_timer_id = startTimer(500);
    m_dc = NetComponents::getDataCenter();
    //设置为只读模式，不能输入，解决空格键不能全屏显示的问题,andrew,20150919
    setReadOnly(true);

    stylesheetUpdate();
    connect(this, SIGNAL(backgroundColorChanged(QColor)), this, SLOT(stylesheetUpdate()));
}

codeSource::~codeSource()
{
    killTimer(m_timer_id);
    if(m_dc)
    {
        delete m_dc;  m_dc = NULL;
    }
}

void codeSource::stylesheetUpdate()
{
    QString strColor = QString("rgb(%1, %2, %3)").arg(QString::number(backgroundColor().red()),
                                                      QString::number(backgroundColor().green()),
                                                      QString::number(backgroundColor().blue()));
    this->setStyleSheet( QString("QTextEdit{background-color: %1}").arg(strColor));
}

void codeSource::setData(const QString newdata)
{
    m_data = newdata;
    update();
}

void codeSource::settxtalignment(const textAlignment newtxt)
{
    m_txt = newtxt;
    emit txtalignmentChanged();
    update();
}

void codeSource::setBackgroundColor(const QColor newbgcolor)
{
    m_bgcolor = newbgcolor;
    emit backgroundColorChanged(m_bgcolor);
    update();
}

void codeSource::setBlankNum(const int blanknum)
{
    blankNum = blanknum;
    update();
    updateGeometry();
}

void codeSource::handleData()
{
    //disData数据为QString
    if(disData.length()%2==1)
        disData = "0"+disData;

    QString t_str = disData;
    QString t_dest;

    //生成满足要求的QString，内容为blankNum个空格
    QString str = " ";
    if(blankNum>=0&&blankNum<=5)
    {
        str = str.leftJustified(blankNum,' ');
    }
    else//message
    {
        QMessageBox msgBox;
        msgBox.setText("不在其数值范围，可选范围为[1,5]!");
        msgBox.exec();
        setBlankNum(1);
    }
    //将每两个源码间加blankNum个空格
    for(int pos = 0;pos<t_str.size();pos++)
    {
        t_dest += t_str.at(pos);
        t_dest += t_str.at(++pos);
        t_dest += str;
    }
    disData = t_dest;

    setText(disData);

    //设置对齐方式
    switch (m_txt) {
    case left:
        setAlignment(Qt::AlignLeft);
        break;
    case center:
        setAlignment(Qt::AlignCenter);
        break;
    case right:
        setAlignment(Qt::AlignRight);
        break;
    }
}

//定时器事件
void codeSource::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    //////**************使用数据处理中心的接口方法 QByteArray 转换不对
    //    QVariant ret;
    //    if(m_dc->getValue(m_data,ret))
    //    {
    //        if(ret.type()==QVariant::ByteArray)
    //        {
    //            QByteArray rr = ret.value<QByteArray>();
    //            QByteArray rr1 = rr.toHex();//to #16
    //            // #16 QByteArray to #16 QString
    //            foreach (char c, rr1) {
    //                disData += QString(c);
    //            }
    //        }
    //    }

    ////*******通过数据类型转换实现的十六进制源码，数据长度受限，目前只能显示8位源码
    QByteArray rr,rr1;
    QString strData = m_data;
    bool ok;
    qint64 value = strData.toUInt(&ok,10);
    strData = strData.setNum(value,16);
    rr = strData.toLocal8Bit();
    rr1 = QByteArray::fromHex(rr);
    //#16 QByteArray to #16 QString
    disData = rr1.toHex().toUpper();
    //********************************end

    //get scrollbar position
    int scrl = this->verticalScrollBar()->sliderPosition();
    handleData();//数据处理并写入文本
    //set scrollbar position
    this->verticalScrollBar()->setValue(scrl);
    update();
}
void codeSource::paintEvent(QPaintEvent *e)
{
    //解决背景色在实时状态下不能设置的问题,andrew,20150919
    stylesheetUpdate();
    QTextEdit::paintEvent(e);
}


