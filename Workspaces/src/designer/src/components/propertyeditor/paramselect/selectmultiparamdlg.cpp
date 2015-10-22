#include "selectmultiparamdlg.h"
#include "ui_selectmultiparamdlg.h"

QSelectMultiParamDlg::QSelectMultiParamDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSelectMultiParamDlg)
{
    m_iMinTableNo = 1;
    m_iMaxTableNo = 30000;
    m_iCurrentItemNo = -1;
    m_iSystemType = 4;
    m_SelectingVector.clear();
    m_SelectedVector.clear();
    m_FormulaVector.clear();
    ui->setupUi(this);
    initParamFormMap();
    initParamFormWidget();
    setParamFormWidgetItem();
    setRadioState();
    initParamWidget();
    initSelectedWidget();
    setButtonState(false);
}

void QSelectMultiParamDlg::setButtonState(bool able)
{
    ui->m_AddButton->setEnabled(able);
    ui->m_ReplaceButton->setEnabled(able);
    ui->m_DelButton->setEnabled(able);
    ui->m_UpButton->setEnabled(able);
    ui->m_DownButton->setEnabled(able);
    ui->m_TopButton->setEnabled(able);
    ui->m_BottomButton->setEnabled(able);
}
void QSelectMultiParamDlg::setRadioState()
{
    ui->m_MinlineEdit->setText(QString::number(m_iMinTableNo));
    ui->m_MaxlineEdit->setText(QString::number(m_iMaxTableNo));
    ui->m_MinlineEdit->setEnabled(m_iSystemType == 4);
    ui->m_MaxlineEdit->setEnabled(m_iSystemType == 4);
    ui->label->setEnabled(m_iSystemType == 4);
    ui->label_2->setEnabled(m_iSystemType == 4);
    ui->m_SetCustom->setEnabled(m_iSystemType == 4);
}
QSelectMultiParamDlg::~QSelectMultiParamDlg()
{
    delete ui;

}
void QSelectMultiParamDlg::initParamFormWidget()
{
    QStringList header1;
    header1<<tr("表号")<<tr("表名"); 
    ui->m_FormTableWidget->setColumnCount(2);
    ui->m_FormTableWidget->setHorizontalHeaderLabels(header1);
    ui->m_FormTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->m_FormTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->m_FormTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->m_FormTableWidget->setColumnWidth(0,80);
    ui->m_FormTableWidget->setColumnWidth(1,800);
}

void QSelectMultiParamDlg::initParamWidget()
{
    ui->m_ParamTableWidget->setColumnCount(16);
    QStringList header2;
    header2<<tr("表号")<<tr("参数号")<<tr("名称")<<tr("代号")
           <<tr("传输格式")<<tr("数长")<<tr("转换格式")<<tr("下限")
           <<tr("上限")<<tr("量纲")<<tr("单位")<<tr("显示格式")
           <<tr("理论值")<<tr("范围")<<tr("索引")<<tr("备注");
    ui->m_ParamTableWidget->setHorizontalHeaderLabels(header2);
    //ui->m_ParamTableWidget->resizeColumnsToContents();
    ui->m_ParamTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->m_ParamTableWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->m_ParamTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void QSelectMultiParamDlg::initSelectedWidget()
{
    ui->m_SelectedtableWidget->setColumnCount(4);
    QStringList header2;
    header2<<tr("表号")<<tr("参数号")<<tr("名称")<<tr("公式");
    ui->m_SelectedtableWidget->setHorizontalHeaderLabels(header2);
    ui->m_SelectedtableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->m_SelectedtableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->m_SelectedtableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void QSelectMultiParamDlg::setParamFormWidgetItem()
{
    ui->m_FormTableWidget->setRowCount(0);
    QMap<unsigned short, QString >::iterator it = tablemap.begin();
    for(;it != tablemap.end();it++)
    {
        ushort tableno = it.key();
        if(tableno >= m_iMinTableNo && tableno <= m_iMaxTableNo)
        {
            int row = ui->m_FormTableWidget->rowCount();
            ui->m_FormTableWidget->setRowCount(row+1);
            ui->m_FormTableWidget->setRowHeight(row,25);
            ui->m_FormTableWidget->setItem(row,0,new QTableWidgetItem(QString::number(it.key())));
            ui->m_FormTableWidget->item(row,0)->setTextAlignment(Qt::AlignCenter);
            ui->m_FormTableWidget->setItem(row,1,new QTableWidgetItem(it.value()));
        }
    }
}

bool QSelectMultiParamDlg::initParamFormMap()
{
    net = NetComponents::getInforCenter();
    tablemap.clear();
    tablemap = net->getTabList();
    if(tablemap.isEmpty())
        return false;
    else
        return true;
}

void QSelectMultiParamDlg::setSelectedWidgetItem()
{
    ui->m_SelectedtableWidget->setRowCount(0);
    for(int j = 0;j < m_SelectedVector.count();j++)
    {
        QString str = m_SelectedVector.at(j);
        ui->m_SelectedtableWidget->setRowHeight(j,25);
        int r = ui->m_SelectedtableWidget->rowCount();
        ui->m_SelectedtableWidget->setRowCount(r+1);
        str = str.right(str.length() - 1);
        int pos = str.indexOf(',');
        QString text = str.left(pos);
        ui->m_SelectedtableWidget->setItem(j,0,new QTableWidgetItem(text));
        ui->m_SelectedtableWidget->item(j,0)->setTextAlignment(Qt::AlignCenter);
        str = str.right(str.length()-pos-1);
        pos = str.indexOf(']');
        text = str.left(pos);
        ui->m_SelectedtableWidget->setItem(j,1,new QTableWidgetItem(text));
        //ui->m_SelectedtableWidget->item(j,1)->setTextAlignment(Qt::AlignCenter);
        str = str.right(str.length()-pos-1);
        ui->m_SelectedtableWidget->setItem(j,2,new QTableWidgetItem(str));
        QLineEdit * lineEdit = new QLineEdit;
        lineEdit->setFixedHeight(30);
        ui->m_SelectedtableWidget->setCellWidget(j,3,lineEdit);
        ((QLineEdit *)ui->m_SelectedtableWidget->cellWidget(j,3))->setAlignment(Qt::AlignCenter);
        ((QLineEdit *)ui->m_SelectedtableWidget->cellWidget(j,3))->setText(m_FormulaVector.at(j));
        //ui->m_SelectedtableWidget->item(j,2)->setTextAlignment(Qt::AlignCenter);
    }
    ui->m_SelectedtableWidget->resizeColumnsToContents();
    setButtonState(false);
}

void QSelectMultiParamDlg::getLineEditText()
{
    if(m_FormulaVector.count() == 0)
        return;
    m_FormulaVector.clear();
    for(int j = 0;j < m_SelectedVector.count();j++)
    {
        QString str = ((QLineEdit *)ui->m_SelectedtableWidget->cellWidget(j,3))->text();
        m_FormulaVector.push_back(str);
    }
}

void QSelectMultiParamDlg::on_m_AddButton_clicked()
{
    getLineEditText();
    bool ok = false;
    for(int i = 0;i < m_SelectingVector.count();i++)
    {
        for(int j = 0;j < m_SelectedVector.count();j++)
        {
            if(m_SelectedVector.at(j) == m_SelectingVector.at(i))
                ok = true;
        }
        if(!ok)
        {
            m_SelectedVector.push_back(m_SelectingVector.at(i));
            QString str = m_SelectingVector.at(i);
            int pos = str.indexOf(']');
            str = str.left(pos + 1);
            m_FormulaVector.push_back(str);
        }
        ok = false;
    }
    setSelectedWidgetItem();
    QList<QTableWidgetItem* > items = ui->m_ParamTableWidget->selectedItems();
    for(int i = 0;i<items.count();i++)
        ui->m_ParamTableWidget->setItemSelected(items.at(i),false);
    ui->m_AddButton->setEnabled(false);
}

void QSelectMultiParamDlg::on_m_DFZXRadio_clicked()
{
    m_iSystemType = 2;
    m_iMinTableNo = 10001;
    m_iMaxTableNo = 20000;
    setRadioState();
    setParamFormWidgetItem();
}

void QSelectMultiParamDlg::on_m_C3IRadio_clicked()
{
    m_iMinTableNo = 1;
    m_iMaxTableNo = 10000;
    m_iSystemType = 1;
    setRadioState();
    setParamFormWidgetItem();
}

void QSelectMultiParamDlg::on_m_KELRadio_clicked()
{
    m_iSystemType = 3;
    m_iMinTableNo = 20001;
    m_iMaxTableNo = 30000;
    setRadioState();
    setParamFormWidgetItem();
}

void QSelectMultiParamDlg::on_m_CustomRadio_clicked()
{
    m_iSystemType = 4;
    setRadioState();
}

void QSelectMultiParamDlg::on_m_SetCustom_clicked()
{
    m_iMinTableNo = ui->m_MinlineEdit->text().toInt();
    m_iMaxTableNo = ui->m_MaxlineEdit->text().toInt();
    setParamFormWidgetItem();
}

void QSelectMultiParamDlg::on_m_FormTableWidget_cellClicked(int row, int )
{
    QString tableNoStr = ui->m_FormTableWidget->item(row,0)->text();
    ushort itableNo = tableNoStr.toUShort();
    SingleTableParamVector ParamVector = net->getParamVector(itableNo);
    ui->m_ParamTableWidget->setRowCount(0);
    for(int i = 0;i < ParamVector.size();i++ )
    {
        int row = ui->m_ParamTableWidget->rowCount();
        ui->m_ParamTableWidget->setRowHeight(row,25);
        ui->m_ParamTableWidget->setRowCount(row+1);
        AbstractParam Param = ParamVector.at(i);
        ui->m_ParamTableWidget->setItem(row,0,new QTableWidgetItem(QString::number(Param.GetTableNo())));
        ui->m_ParamTableWidget->setItem(row,1,new QTableWidgetItem(QString::number(Param.GetParamNo())));
        ui->m_ParamTableWidget->item(row,0)->setTextAlignment(Qt::AlignCenter);
        ui->m_ParamTableWidget->setItem(row,2,new QTableWidgetItem(tr(Param.GetParamName().c_str())));
        ui->m_ParamTableWidget->setItem(row,3,new QTableWidgetItem(tr(Param.GetParamCode().c_str())));
        ui->m_ParamTableWidget->setItem(row,4,new QTableWidgetItem(AbstractParam::GetTypeString(Param.GetParamTranType()).c_str()));
        ui->m_ParamTableWidget->setItem(row,5,new QTableWidgetItem(QString::number(Param.GetParamDataLen())));
        ui->m_ParamTableWidget->setItem(row,6,new QTableWidgetItem(AbstractParam::GetTypeString(Param.GetParamConvType()).c_str()));
        ui->m_ParamTableWidget->setItem(row,7,new QTableWidgetItem(QString::number(Param.GetParamLorlmt())));
        ui->m_ParamTableWidget->setItem(row,8,new QTableWidgetItem(QString::number(Param.GetParamUprlmt())));
        ui->m_ParamTableWidget->setItem(row,9,new QTableWidgetItem(QString::number(Param.GetParamQuotiety())));
        ui->m_ParamTableWidget->setItem(row,10,new QTableWidgetItem(tr(Param.GetParamUnit().c_str())));
        ui->m_ParamTableWidget->setItem(row,11,new QTableWidgetItem(tr(Param.GetParamShowType().c_str())));
        ui->m_ParamTableWidget->setItem(row,12,new QTableWidgetItem(tr(Param.GetParamTheoryValue().c_str())));
        ui->m_ParamTableWidget->setItem(row,13,new QTableWidgetItem(tr(Param.GetParamTheoryRange().c_str())));
        ui->m_ParamTableWidget->setItem(row,14,new QTableWidgetItem(QString::number(Param.GetParamGroupIndex())));
        ui->m_ParamTableWidget->setItem(row,15,new QTableWidgetItem(tr(Param.GetParamNote().c_str())));
    }
    ui->m_ParamTableWidget->resizeColumnsToContents();
}

void QSelectMultiParamDlg::on_m_ParamTableWidget_cellClicked(int , int )
{
    ui->m_AddButton->setEnabled(false);
    m_SelectingVector.clear();
    QList<QTableWidgetItem* > items = ui->m_ParamTableWidget->selectedItems();
    for(int i = 0;i < items.count();i = i + 16)
    {
        int row = items.at(i)->row();
        QString str = tr("[");
        str += ui->m_ParamTableWidget->item(row,0)->text();
        str = str + tr(",");
        str =  str + ui->m_ParamTableWidget->item(row,1)->text();
        str = str + tr("]");
        str =  str + ui->m_ParamTableWidget->item(row,2)->text();
        m_SelectingVector.push_back(str);
        ui->m_AddButton->setEnabled(true);
    }
}

void QSelectMultiParamDlg::on_m_DelButton_clicked()
{
    getLineEditText();
    if(m_iCurrentItemNo != -1)
    {
        m_SelectedVector.removeAt(m_iCurrentItemNo);
        m_FormulaVector.removeAt(m_iCurrentItemNo);
    }
    setSelectedWidgetItem();
    m_iCurrentItemNo = -1;
}

void QSelectMultiParamDlg::on_m_TopButton_clicked()
{
    getLineEditText();
    if(m_iCurrentItemNo != -1)
    {
        QString str = m_SelectedVector.at(m_iCurrentItemNo);
        m_SelectedVector.removeAt(m_iCurrentItemNo);
        m_SelectedVector.insert(0,str);
        str = m_FormulaVector.at(m_iCurrentItemNo);
        m_FormulaVector.removeAt(m_iCurrentItemNo);
        m_FormulaVector.insert(0,str);
        setSelectedWidgetItem();
        m_iCurrentItemNo = -1;
    }
    m_iCurrentItemNo = 0;
    ui->m_SelectedtableWidget->selectRow(0);
    setButtonState(true);
}

void QSelectMultiParamDlg::on_m_BottomButton_clicked()
{
    getLineEditText();
    if(m_iCurrentItemNo != -1)
    {
        QString str = m_SelectedVector.at(m_iCurrentItemNo);
        m_SelectedVector.removeAt(m_iCurrentItemNo);
        m_SelectedVector.push_back(str);
        str = m_FormulaVector.at(m_iCurrentItemNo);
        m_FormulaVector.removeAt(m_iCurrentItemNo);
        m_FormulaVector.push_back(str);
        setSelectedWidgetItem();
        m_iCurrentItemNo = -1;
    }
    m_iCurrentItemNo = m_SelectedVector.count()-1;
    ui->m_SelectedtableWidget->selectRow(m_SelectedVector.count()-1);
    setButtonState(true);
}

void QSelectMultiParamDlg::on_m_UpButton_clicked()
{
    getLineEditText();
    if(m_iCurrentItemNo != -1)
    {
        QString str1 = m_SelectedVector.at(m_iCurrentItemNo);
        QString str2 = m_FormulaVector.at(m_iCurrentItemNo);
        if(m_iCurrentItemNo != 0)
        {

            m_SelectedVector.replace(m_iCurrentItemNo,m_SelectedVector.at(m_iCurrentItemNo-1));
            m_SelectedVector.replace(m_iCurrentItemNo-1,str1);
            m_FormulaVector.replace(m_iCurrentItemNo,m_FormulaVector.at(m_iCurrentItemNo-1));
            m_FormulaVector.replace(m_iCurrentItemNo-1,str2);
            m_iCurrentItemNo = m_iCurrentItemNo - 1;
        }
        else
        {
            m_SelectedVector.pop_front();
            m_SelectedVector.push_back(str1);
            m_FormulaVector.pop_front();
            m_FormulaVector.push_back(str2);
            m_iCurrentItemNo = m_SelectedVector.count() - 1;
        }
    }
    setSelectedWidgetItem();
    ui->m_SelectedtableWidget->selectRow(m_iCurrentItemNo);
    setButtonState(true);
}

void QSelectMultiParamDlg::on_m_DownButton_clicked()
{
    getLineEditText();
    if(m_iCurrentItemNo != -1)
    {
        QString str1 = m_SelectedVector.at(m_iCurrentItemNo);
        QString str2 = m_FormulaVector.at(m_iCurrentItemNo);
        if(m_iCurrentItemNo != m_SelectedVector.count() - 1)
        {
            m_SelectedVector.replace(m_iCurrentItemNo,m_SelectedVector.at(m_iCurrentItemNo+1));
            m_SelectedVector.replace(m_iCurrentItemNo+1,str1);
            m_FormulaVector.replace(m_iCurrentItemNo,m_FormulaVector.at(m_iCurrentItemNo+1));
            m_FormulaVector.replace(m_iCurrentItemNo+1,str2);
            m_iCurrentItemNo = m_iCurrentItemNo + 1;
        }
        else
        {
            m_SelectedVector.pop_back();
            m_SelectedVector.push_front(str1);
            m_FormulaVector.pop_back();
            m_FormulaVector.push_front(str2);
            m_iCurrentItemNo = 0;
        }
    }
    setSelectedWidgetItem();
    ui->m_SelectedtableWidget->selectRow(m_iCurrentItemNo);
    setButtonState(true);
}

void QSelectMultiParamDlg::on_m_SelectedtableWidget_cellClicked(int row, int )
{
    setButtonState(false);
    QList<QTableWidgetItem* > items = ui->m_SelectedtableWidget->selectedItems();
    m_iCurrentItemNo = row;
    if(items.count() > 0)
        setButtonState(true);
}
void QSelectMultiParamDlg::getParamList(QVector<QString>& paramVector,QVector<QString>& formulaVector)
{
    getLineEditText();
    paramVector.clear();
    formulaVector.clear();
    paramVector = m_SelectedVector;
    formulaVector = m_FormulaVector;
}

void QSelectMultiParamDlg::setMultiParamFormulaStr(const QString inputStr)
{
    m_FormulaVector.clear();
    QString formulaStr = inputStr;
    while (formulaStr.length() > 5)
    {
        int pos = 0;
        formulaStr = formulaStr.right(formulaStr.length() - 1);
        pos = formulaStr.indexOf('}');
        QString str = formulaStr.left(pos);
        formulaStr = formulaStr.right(formulaStr.length() - pos - 1);
        m_FormulaVector.push_back(str);
    }
    for(int i = 0;i < m_FormulaVector.count();i++)
    {
        AbstractParam *param = net->getParam(m_FormulaVector.at(i));
        if(param)
        {
            QString str = tr("[");
            str = str + QString::number(param->GetTableNo());
            str = str + tr(",");
            str = str + QString::number(param->GetParamNo());
            str = str + tr("]");
            str = str + tr(param->GetParamName().c_str());
            m_SelectedVector.push_back(str);
        }
    }
    setSelectedWidgetItem();
}

QString QSelectMultiParamDlg::getMultiParamFormulaStr()
{
    getLineEditText();
    QString str = tr("");
    for(int i = 0;i < m_FormulaVector.count();i++)
    {
        str = str + tr("{");
        str = str + m_FormulaVector.at(i);
        str = str + tr("}");
    }
    return str;
}

void QSelectMultiParamDlg::setParamList(QVector<QString> formulaVector)
{
    m_FormulaVector = formulaVector;
    for(int i = 0;i < m_FormulaVector.count();i++)
    {
        AbstractParam *param = net->getParam(m_FormulaVector.at(i));
        if(param)
        {
            QString str = tr("[");
            str = str + QString::number(param->GetTableNo());
            str = str + tr(",");
            str = str + QString::number(param->GetParamNo());
            str = str + tr("]");
            str = str + tr(param->GetParamName().c_str());
            m_SelectedVector.push_back(str);
        }
    }
    for(int i = 0;i < m_FormulaVector.count();i++)
    {
        if(!m_FormulaVector.at(i).isEmpty())
        {
            setSelectedWidgetItem();
            return;
        }
    }
    m_SelectedVector.clear();
    m_FormulaVector.clear();
}

void QSelectMultiParamDlg::on_m_ReplaceButton_clicked()
{
    getLineEditText();
    QList<QTableWidgetItem* > items = ui->m_ParamTableWidget->selectedItems();
    if(items.count() == 0)
        return;
    items = ui->m_SelectedtableWidget->selectedItems();
    int row = items.at(0)->row();
    if(m_SelectedVector.at(row) == m_SelectingVector.at(0))
            return;
    m_SelectedVector.replace(row,m_SelectingVector.at(0));
    QString str = m_SelectingVector.at(0);
    int pos = str.indexOf(']');
    str = str.left(pos + 1);
    m_FormulaVector.replace(row,str);
    setSelectedWidgetItem();
    items = ui->m_ParamTableWidget->selectedItems();
    for(int i = 0;i<items.count();i++)
        ui->m_ParamTableWidget->setItemSelected(items.at(i),false);

}

void QSelectMultiParamDlg::on_selectAllParamButton_clicked()
{
    int row = ui->m_ParamTableWidget->rowCount();
    for(int i = 0;i < row;i++)
        ui->m_ParamTableWidget->selectRow(i);
    if(row > 0)
        on_m_ParamTableWidget_cellClicked(0,0);
}
