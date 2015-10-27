#include "dialogoutdataconfig.h"
#include "ui_dialogoutdataconfig.h"
#include "../components/propertyeditor/paramselect/selectmultiparamdlg.h"
#include <QFileDialog>
#include <Net/NetComponents>
#include <QMessageBox>

DialogOutDataConfig::DialogOutDataConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOutDataConfig_UI())
{
    ui->setupUi(this);

    //导成一个文件还是多个文件
    m_bMultiOutFile = true;

    ui->radioButtonMutiFile->setChecked(true);
}

DialogOutDataConfig::~DialogOutDataConfig()
{
    delete ui;
}

//点击按钮-选择导出目录
void DialogOutDataConfig::on_pushButtonSelectDir_clicked()
{
    QFileDialog* fdlg = new QFileDialog(this, "请选择目录", "file");

    m_strFilePath = QFileDialog::getExistingDirectory(this, tr("请选择目录"),
                                                    "",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);

    ui->lineEditDir->setText(m_strFilePath);
}

//点击按钮-选择导出参数
void DialogOutDataConfig::on_pushButtonSelectParams_clicked()
{
    // 多参数选择对话框
    QSelectMultiParamDlg dlg(NULL);

    dlg.setMultiParamFormulaStr(m_strSelectedParams);

    if (dlg.exec() != QDialog::Accepted)
    {
        return;
    }

    //取得参数序列
    dlg.getParamList(m_v1, m_v2);

    m_strSelectedParams = dlg.getMultiParamFormulaStr();

    ui->lineEditSelectedParams->setText(m_strSelectedParams);
}

//点击导出按钮
void DialogOutDataConfig::on_pushButtonOutData_clicked()
{
    if(m_strFilePath.isEmpty())
    {
        QMessageBox::information(this,"提示","导出目录不能为空!");
        return;
    }

    //导出失败的文件个数
    qint32 iFailCount = 0;

    //导出参数个数
    int i = 0;

    //每个参数的值的数量
    int j = 0;

    //导出的文件
    QFile f;

    //导出文件名
    QString strFileName;

    //单文件模式
    if(!m_bMultiOutFile)
    {
        //导出文件名
        strFileName = m_strFilePath + "/" + "OutData.txt";

        f.setFileName(strFileName);

        if(!f.open(QIODevice::ReadWrite))
        {
            QMessageBox::information(this,"提示","创建导出文件失败，请检查是否有可用的磁盘空间!");
            return;
        }
    }

    //v2.size()就是要导出的参数的个数
    for(i =0; i<m_v2.size(); i++)
    {
        //多文件模式
        if(m_bMultiOutFile)
        {
            //导出文件名
            strFileName = m_strFilePath + "/" + m_v2[i] + ".txt";

            f.setFileName(strFileName);

            //创建并打开导出文件
            if(!f.open(QIODevice::ReadWrite))
            {

                iFailCount++;
                continue;
            }
        }

        //取得数据接口
        DataCenterInterface * datacenter = NetComponents::getDataCenter();

        //将要导出的数据
        QVector<double> outdata;

        //取历史数据
        datacenter->getHistoryData(m_v2[i],outdata);

        //把导出的数据写入导出文件
        for(j=0;j<outdata.size();j++)
        {
            QTextStream xin(&f);

            QString strWrite = QString("%1\r\n").arg(outdata[j]);

            xin<<strWrite;
        }

        delete datacenter;
    }

    //多文件模式
    if(m_bMultiOutFile)
    {
        if(iFailCount>0)
        {
            QMessageBox::information(this,"提示",QString("共导出%1个文件，其中失败%2个!").arg(i).arg(iFailCount));
        }
        else
        {
            QMessageBox::information(this,"提示",QString("导出%1个文件成功!").arg(i));
        }
    }
    else//单文件模式
    {
        QMessageBox::information(this,"提示","导出文件成功!");
    }

    this->accept();
}

//点击取消按钮
void DialogOutDataConfig::on_pushButtonCancel_clicked()
{
    this->accept();
}

//点击选择框-每个参数一个文件
void DialogOutDataConfig::on_radioButtonMutiFile_clicked()
{
    m_bMultiOutFile = true;
}

//点击选择框-全部参数一个文件
void DialogOutDataConfig::on_radioButtonSingleFile_clicked()
{
    m_bMultiOutFile = false;
}
