#include "selectparamdialog.h"
#include "ui_selectsingleparamdialog.h"
#include <QtDesigner/QDesignerSettingsInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
QT_BEGIN_NAMESPACE

namespace qdesigner_internal {
QSelectParamDialog::QSelectParamDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectSingleParamDialog)
{
    ui->setupUi(this);
    m_iMinTableNo = 1;
    m_iMaxTableNo = 30000;
    m_iCurrentTableNo = 0;
    setWindowTitle(tr("Select Param"));
    connect(ui->m_AllSystemRadioButton,SIGNAL(toggled(bool)),this,SLOT(SelectAllSystemRadioButton(bool)));
    connect(ui->m_C3IRadioButton,SIGNAL(toggled(bool)),this,SLOT(SelectC3IRadioButton(bool)));
    connect(ui->m_DFZXRadioButton,SIGNAL(toggled(bool)),this,SLOT(SelectDFZXRadioButton(bool)));
    connect(ui->m_KELRadioButton,SIGNAL(toggled(bool)),this,SLOT(SelectKELRadioButton(bool)));
    connect(ui->m_CustomRadioButton,SIGNAL(toggled(bool)),this,SLOT(SelectCustomRadioButton(bool)));
    connect(ui->m_SelectParamButton,SIGNAL(clicked()),this,SLOT(SelectCustom()));
    ui->m_formTableWidget->setColumnCount(2);
    QStringList header1;
    header1<<tr("表号")<<tr("表名");
    ui->m_formTableWidget->setHorizontalHeaderLabels(header1);
    ui->m_formTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->m_formTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->m_formTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->m_formTableWidget->setColumnWidth(0,80);
    ui->m_formTableWidget->setColumnWidth(1,800);
    connect(ui->m_formTableWidget,SIGNAL(cellClicked(int,int)),this,SLOT(OnClickTableForm(int,int)));
    ui->m_paramTableWidget->setColumnCount(16);
    QStringList header2;
    header2<<tr("表号")<<tr("参数号")<<tr("名称")<<tr("代号")
            <<tr("传输格式")<<tr("数长")<<tr("转换格式")<<tr("下限")
            <<tr("上限")<<tr("量纲")<<tr("单位")<<tr("显示格式")
            <<tr("理论值")<<tr("范围")<<tr("索引")<<tr("备注");
     ui->m_paramTableWidget->setHorizontalHeaderLabels(header2);
     ui->m_paramTableWidget->resizeColumnsToContents();
     ui->m_paramTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
     ui->m_paramTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
     ui->m_paramTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
     connect(ui->m_paramTableWidget,SIGNAL(cellClicked(int,int)),this,SLOT(OnClickParamForm(int,int)));
     ui->m_paramTableWidget->setColumnWidth(0,80);
     net = NetComponents::getInforCenter();
}
//选择信息约定表参数表控件表格单表信息
void QSelectParamDialog::OnClickTableForm(int row,int)
{
    QString tableNoStr = ui->m_formTableWidget->item(row,0)->text();
    ushort itableNo = tableNoStr.toUShort();
    SingleTableParamVector ParamVector = net->getParamVector(itableNo);
    ui->m_paramTableWidget->setRowCount(0);
    for(int i = 0;i < ParamVector.size();i++ )
    {
        int row = ui->m_paramTableWidget->rowCount();
        ui->m_paramTableWidget->setRowCount(row+1);
        AbstractParam Param = ParamVector.at(i);
        ui->m_paramTableWidget->setItem(row,0,new QTableWidgetItem(QString::number(Param.GetTableNo())));
        ui->m_paramTableWidget->setItem(row,1,new QTableWidgetItem(QString::number(Param.GetParamNo())));
        ui->m_paramTableWidget->item(row,0)->setTextAlignment(Qt::AlignCenter);
        ui->m_paramTableWidget->setItem(row,2,new QTableWidgetItem(tr(Param.GetParamName().c_str())));
        ui->m_paramTableWidget->setItem(row,3,new QTableWidgetItem(tr(Param.GetParamCode().c_str())));
        ui->m_paramTableWidget->setItem(row,4,new QTableWidgetItem(tr(AbstractParam::GetTypeString(Param.GetParamTranType()).c_str())));
        ui->m_paramTableWidget->setItem(row,5,new QTableWidgetItem(QString::number(Param.GetParamDataLen())));
        ui->m_paramTableWidget->setItem(row,6,new QTableWidgetItem(tr(AbstractParam::GetTypeString(Param.GetParamConvType()).c_str())));
        ui->m_paramTableWidget->setItem(row,7,new QTableWidgetItem(QString::number(Param.GetParamLorlmt())));
        ui->m_paramTableWidget->setItem(row,8,new QTableWidgetItem(QString::number(Param.GetParamUprlmt())));
        ui->m_paramTableWidget->setItem(row,9,new QTableWidgetItem(QString::number(Param.GetParamQuotiety())));
        ui->m_paramTableWidget->setItem(row,10,new QTableWidgetItem(tr(Param.GetParamUnit().c_str())));
        ui->m_paramTableWidget->setItem(row,11,new QTableWidgetItem(tr(Param.GetParamShowType().c_str())));
        ui->m_paramTableWidget->setItem(row,12,new QTableWidgetItem(tr(Param.GetParamTheoryValue().c_str())));
        ui->m_paramTableWidget->setItem(row,13,new QTableWidgetItem(tr(Param.GetParamTheoryRange().c_str())));
        ui->m_paramTableWidget->setItem(row,14,new QTableWidgetItem(QString::number(Param.GetParamGroupIndex())));
        ui->m_paramTableWidget->setItem(row,15,new QTableWidgetItem(tr(Param.GetParamNote().c_str())));
    }
    ui->m_paramTableWidget->resizeColumnsToContents();
}
//选择信息约定表参数控件表格单参数信息
void QSelectParamDialog::OnClickParamForm(int row,int)
{
    QString tableNoStr = ui->m_paramTableWidget->item(row,0)->text();
    QString paramNoStr = ui->m_paramTableWidget->item(row,1)->text();
    ui->m_parameditor->setText(tr("[") + tableNoStr + tr(",") + paramNoStr + tr("]") );
}

void QSelectParamDialog::SelectAllSystemRadioButton(bool check)
{
    if(check)
    {
        m_iMaxTableNo = 30000;
        m_iMinTableNo = 1;
        ui->m_CustomMinParamEdit->setText(tr("1"));
        ui->m_CustomMinParamEdit->setEnabled(false);
        ui->m_CustomMaxParamEdit->setText(tr("30000"));
        ui->m_CustomMaxParamEdit->setEnabled(false);
        ui->m_SelectParamButton->setEnabled(false);
        initFormTable();
    }
}

void QSelectParamDialog::SelectC3IRadioButton(bool check)
{
    if(check)
    {
        m_iMinTableNo = 1;
        m_iMaxTableNo = 10000;
        ui->m_CustomMinParamEdit->setText(tr("1"));
        ui->m_CustomMinParamEdit->setEnabled(false);
        ui->m_CustomMaxParamEdit->setText(tr("10000"));
        ui->m_CustomMaxParamEdit->setEnabled(false);
        ui->m_SelectParamButton->setEnabled(false);
        initFormTable();
    }
}

void QSelectParamDialog::SelectKELRadioButton(bool check)
{
    if(check)
    {
        m_iMaxTableNo = 30000;
        m_iMinTableNo = 20001;
        ui->m_CustomMinParamEdit->setText(tr("20001"));
        ui->m_CustomMinParamEdit->setEnabled(false);
        ui->m_CustomMaxParamEdit->setText(tr("30000"));
        ui->m_CustomMaxParamEdit->setEnabled(false);
        ui->m_SelectParamButton->setEnabled(false);
        initFormTable();
    }
}

void QSelectParamDialog::SelectDFZXRadioButton(bool check)
{
    if(check)
    {
        m_iMaxTableNo = 20000;
        m_iMinTableNo = 10001;
        ui->m_CustomMinParamEdit->setText(tr("10001"));
        ui->m_CustomMinParamEdit->setEnabled(false);
        ui->m_CustomMaxParamEdit->setText(tr("20000"));
        ui->m_CustomMaxParamEdit->setEnabled(false);
        ui->m_SelectParamButton->setEnabled(false);
        initFormTable();
    }
}

void QSelectParamDialog::SelectCustomRadioButton(bool check)
{
    if(check)
    {
        ui->m_CustomMinParamEdit->setText(QString::number(m_iMinTableNo));
        ui->m_CustomMinParamEdit->setEnabled(true);
        ui->m_CustomMaxParamEdit->setText(QString::number(m_iMaxTableNo));
        ui->m_CustomMaxParamEdit->setEnabled(true);
        ui->m_SelectParamButton->setEnabled(true);
    }
}

void QSelectParamDialog::SelectCustom()
{
    m_iMinTableNo = ui->m_CustomMinParamEdit->text().toUShort();
    m_iMaxTableNo = ui->m_CustomMaxParamEdit->text().toUShort();
    initFormTable();
}
//初始化信息约定表表参数控件表格
bool QSelectParamDialog::initFormTable()
{
    QMap<unsigned short, QString > tablemap;
    tablemap = net->getTabList();
    ui->m_formTableWidget->setRowCount(0);
    QMap<unsigned short, QString >::iterator it = tablemap.begin();
    for(;it != tablemap.end();it++)
    {
        ushort tableno = it.key();
        if(tableno >= m_iMinTableNo && tableno <= m_iMaxTableNo)
        {
            int row = ui->m_formTableWidget->rowCount();
            ui->m_formTableWidget->setRowCount(row+1);
            ui->m_formTableWidget->setItem(row,0,new QTableWidgetItem(QString::number(it.key())));
            ui->m_formTableWidget->item(row,0)->setTextAlignment(Qt::AlignCenter);
            ui->m_formTableWidget->setItem(row,1,new QTableWidgetItem(it.value()));
        }
    }
    
    return true;
}


int QSelectParamDialog::showDialog()
{

    ui->m_parameditor->setFocus();
    return exec();
}

QString QSelectParamDialog::text() const
{
    return ui->m_parameditor->text();
}

void QSelectParamDialog::SetText(QString str)
{
    ui->m_parameditor->setText(str);
    if(str.length() > 4)
    {
        int pos = str.indexOf(tr(","));
        QString str1 = str.left(pos);
        str1 = str1.right(pos-1);
        m_iCurrentTableNo = str1.toUShort();
    }
    if(m_iCurrentTableNo >= 1  && m_iCurrentTableNo <= 10000)
    {
        m_iMinTableNo = 1;
        m_iMaxTableNo = 10000;
        ui->m_C3IRadioButton->setChecked(true);
    }
    else if(m_iCurrentTableNo >= 10001  && m_iCurrentTableNo <= 20000)
    {
        m_iMinTableNo = 10001;
        m_iMaxTableNo = 20000;
        ui->m_DFZXRadioButton->setChecked(true);
    }
    else if(m_iCurrentTableNo >= 20001  && m_iCurrentTableNo <= 30000)
    {
        m_iMinTableNo = 20001;
        m_iMaxTableNo = 30000;
        ui->m_KELRadioButton->setChecked(true);
    }
    else
    {
        m_iMinTableNo = 1;
        m_iMaxTableNo = 30000;
        ui->m_AllSystemRadioButton->setChecked(true);
    }
    initFormTable();
}
QSelectParamDialog::~QSelectParamDialog()
{
    delete ui;
}
} // namespace qdesigner_internal

QT_END_NAMESPACE









