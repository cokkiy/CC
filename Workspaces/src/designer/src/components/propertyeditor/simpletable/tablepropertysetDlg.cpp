#include <QtDesigner/QDesignerFormEditorInterface>
#include "tablepropertysetDlg.h"
#include "tableitemset.h"
#include "ui_tablepropertysetdialog.h"
#include "../paramselect/selectmultiparamdlg.h"

QT_BEGIN_NAMESPACE
namespace qdesigner_internal {
QTablePropertySetDlg::QTablePropertySetDlg(QWidget *parent,QString tablepropertystr) :
    QDialog(parent),
    ui(new Ui::TablePropertySetDialog)
{
    m_bAddDelMenuVisible = false;
    m_bColumnContextMenuVisible = false;
    m_bItemFormatVisible = false;
    m_bRCHandleMenuVisible = false;
    m_bSetParamMenuVisible = false;
    m_bEditState = false;
    m_iCurrentRow = -1;
    m_iCurrentCol = -1;
    m_iCurrentEditRow = -1;
    m_iCurrentEditCol = -1;
    m_TableProperty.setTableVector(tablepropertystr);
    m_iRowNum = m_TableProperty.getRowNum();
    m_iColNum = m_TableProperty.getColNum();
    MainMenu = new QMenu(this);
    ui->setupUi(this);
    this->setFixedSize(geometry().width(),geometry().height());
    initMenu();
    setWindowTitle(tr("表格属性设置"));
    setTableWidgetProperty();
    resetRowColumnSizeFrame(0,0,20);
    resetTableSetFrame();
}
QTablePropertySetDlg::~QTablePropertySetDlg()
{
    delete ui;
}
//显示配置对话框
int QTablePropertySetDlg::showDialog()
{
    return exec();
}
//设置表格属性，关联信号槽函数
void QTablePropertySetDlg::setTableWidgetProperty()
{
    resetDialog();
    connect(ui->m_TableWidget->horizontalHeader(),SIGNAL(sectionResized(int,int,int )),this,SLOT(onHSectionResized(int,int,int)));
    connect(ui->m_TableWidget->verticalHeader(),SIGNAL(sectionResized(int,int,int )),this,SLOT(onVSectionResized(int,int,int)));
    connect(ui->m_TableWidget,SIGNAL(cellPressed(int,int)),this,SLOT(onCellClicked(int,int)));
    connect(ui->m_TableWidget,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),this,SLOT(onItemEdit(QTableWidgetItem *)));
    connect(ui->m_TableWidget,SIGNAL(itemSelectionChanged()),this,SLOT(onItemSelectionChanged()));
    ui->m_TableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
}
//获取表格某行高或某列高数据显示
void QTablePropertySetDlg::resetTableSetFrame()
{
    ui->TableRowEdit->setText(QString::number(m_iRowNum));
    ui->TableColumnEdit->setText(QString::number(m_iColNum));
}
//获取表格宽度和高度大小显示
void QTablePropertySetDlg::resetTableSizeFrame()
{
    QSize size = m_TableProperty.size();
    ui->m_TableWidthEdit->setText(QString::number(size.width()));
    ui->m_TableHeightEdit->setText(QString::number(size.height()));
}
//获取表格某行高或某列高数据显示
void QTablePropertySetDlg::resetRowColumnSizeFrame(int index,int number,int size)
{
    ui->m_RowColumnomboBox->setCurrentIndex(index);
    ui->m_CellNumberEdit->setText(QString::number(number + 1));
    ui->m_CellSizeEdit->setText(QString::number(size));
}
//重新显示对话框控件的数据和属性
void QTablePropertySetDlg::resetDialog()
{
    ui->m_TableWidget->reset();
    ui->m_TableWidget->setRowCount(m_iRowNum);
    ui->m_TableWidget->setColumnCount(m_iColNum);
    for(int i = 0; i < m_iRowNum; i++)
    {
        QTableCell Cell = m_TableProperty.getTableVector()->at(i * m_iColNum);
        ui->m_TableWidget->setRowHeight(i,Cell.getHeight());
        for(int j = 0;j< m_iColNum; j++)
        {
            QTableWidgetItem *item = new QTableWidgetItem;
            QTableCell TableCell = m_TableProperty.getTableVector()->at(i * m_iColNum + j);
            ui->m_TableWidget->setColumnWidth(j,TableCell.getWidth());
            item->setFont(TableCell.getTextFont());
            item->setTextAlignment(TableCell.getTextHAlignment()|TableCell.getTextVAlignment());
            item->setTextColor(TableCell.getTextColor());
            item->setBackgroundColor(TableCell.getBackgroundColor());
            item->setText(TableCell.getConfigModeContent());
            ui->m_TableWidget->setItem(i,j,item);
        }
    }
    resetTableSizeFrame();
    resetTableSetFrame();
}
//鼠标从当前编辑的单元格移开事件
void QTablePropertySetDlg::onItemSelectionChanged()
{
    if(m_bEditState)
    {
       QTableWidgetItem *item = ui->m_TableWidget->item(m_iCurrentEditRow,m_iCurrentEditCol);
       QTableCell TableCell = m_TableProperty.getTableVector()->at(m_iCurrentEditRow * m_iColNum + m_iCurrentEditCol);
       QString str = item->text();
       TableCell.setText(str);
       m_TableProperty.getTableVector()->replace(m_iCurrentEditRow * m_iColNum + m_iCurrentEditCol,TableCell);
       resetDialog();
    }
    m_bEditState = false;
}
//通过移动横向表头某项改变表格某列宽度
void QTablePropertySetDlg::onHSectionResized(int logicalIndex, int , int newSize)
{
    resetRowColumnSizeFrame(1,logicalIndex, newSize);
    QSize size = m_TableProperty.size();
    QTableCell TableCell = m_TableProperty.getTableVector()->at(logicalIndex);
    ui->m_TableWidthEdit->setText(QString::number(size.width()- TableCell.getWidth() + newSize));
}
//通过移动纵向表头某项改变表格某行高度
void QTablePropertySetDlg::onVSectionResized(int logicalIndex, int , int newSize)
{
    resetRowColumnSizeFrame(0,logicalIndex, newSize);
    QSize size = m_TableProperty.size();
    QTableCell TableCell = m_TableProperty.getTableVector()->at(logicalIndex*m_iColNum);
    ui->m_TableHeightEdit->setText(QString::number(size.height() - TableCell.getHeight() + newSize));
}
//通过表头点击事件选择列
void QTablePropertySetDlg::onSectionPressed(int )
{
    m_bColumnContextMenuVisible = true;
}
//编辑单元格文本
void QTablePropertySetDlg::onItemEdit(QTableWidgetItem * item)
{
    setTableItemSize();
    m_iCurrentEditRow = item->row();
    m_iCurrentEditCol  = item->column();
    QTableCell TableCell = m_TableProperty.getTableVector()->at(m_iCurrentEditRow * m_iColNum + m_iCurrentEditCol);
    item->setText(TableCell.getText());
    ui->m_TableWidget->setItem(m_iCurrentEditRow,m_iCurrentEditCol,item);
    m_bEditState = true;
}
//点击单元格事件
void QTablePropertySetDlg::onCellClicked(int row,int col)
{
    m_iCurrentRow = row;
    m_iCurrentCol = col;
	m_bRCHandleMenuVisible = true;
}
//获取右键菜单可用状态，初始化右键各菜单项可见状态
bool QTablePropertySetDlg::getMenuState(QCursor cur)
{
    //鼠标坐标转换，由屏幕坐标转换为窗口坐标
	QPoint point = mapFromGlobal(cur.pos());
    QRect rect = ui->m_TableWidget->geometry();
    if(!((point.y()<rect.bottom())&&
        (point.y()>rect.top())&&
        (point.x()<rect.right())&&
        (point.x()>rect.left())))
        return false;
	//处理行和列操作菜单可见时关联的菜单状态
    if(m_bRCHandleMenuVisible)
	{
		m_bAddDelMenuVisible = true;
        QList<QTableWidgetItem* > items = ui->m_TableWidget->selectedItems();
		if(items.count() > 0)
			m_bItemFormatVisible = true;
	}

    //设置关联参数菜单项状态
    QList<QTableWidgetItem* > items = ui->m_TableWidget->selectedItems();
    for(int i = 0;i < items.count();i++)
    {
        if(items.at(i)->column() != 0)
        {
            m_bSetParamMenuVisible = false;
            break;
        }
        m_bSetParamMenuVisible = true;
    }

    if(items.count() > 0)
        m_bColumnContextMenuVisible = true;

    if(m_bColumnContextMenuVisible)
    {
        m_bItemFormatVisible = true;
        m_bAddDelMenuVisible = true;
    }

    m_bAddDelMenuVisible = true;
    if(m_iRowNum == 0 && m_iColNum == 0)
    {
        m_bAddDelMenuVisible = false;
    }

    if(m_bColumnContextMenuVisible||m_bAddDelMenuVisible||
        m_bRCHandleMenuVisible||m_bItemFormatVisible||
        m_bSetParamMenuVisible)
		return true;
    else
        return false;
}
//键盘事件
void QTablePropertySetDlg::keyPressEvent(QKeyEvent* event)
{
    int keyValue = event->key();
	//键盘Delete按键事件
    if(keyValue == Qt::Key_Delete)
    {
        QList<QTableWidgetItem* > items = ui->m_TableWidget->selectedItems();
        if(items.count() > 0)
        {
            for(int i = 0;i<items.count();i++)
            {
                int col = items.at(i)->column();
                int row = items.at(i)->row();
                QTableCell TableCell = m_TableProperty.getTableVector()->at(row * m_iColNum + col);
                TableCell.setText(tr(""));
                m_TableProperty.getTableVector()->replace(row * m_iColNum + col,TableCell);
                items.at(i)->setFont(TableCell.getTextFont());
                items.at(i)->setTextAlignment(TableCell.getTextHAlignment()|TableCell.getTextVAlignment());
                items.at(i)->setTextColor(TableCell.getTextColor());
                items.at(i)->setBackgroundColor(TableCell.getBackgroundColor());
                items.at(i)->setText(TableCell.getConfigModeContent());
                ui->m_TableWidget->setItem(row,col,items.at(i));
            }
        }
    }
}
//右键菜单事件
void QTablePropertySetDlg::contextMenuEvent(QContextMenuEvent*)
{
    QCursor cur=this->cursor();
    if(getMenuState(cur))
    {
        setMenuState();
		QCursor cur=this->cursor();
        MainMenu->exec(cur.pos());
        m_iCurrentRow = -1;
        m_iCurrentCol = -1;
    }
    m_bAddDelMenuVisible = false;
    m_bColumnContextMenuVisible = false;
    m_bItemFormatVisible = false;
    m_bRCHandleMenuVisible = false;
    m_bSetParamMenuVisible = false;
}
//设置单元格显示内容
void QTablePropertySetDlg::setTableItemTextType(qint8 m_TextType)
{
    setTableItemSize();
    QList<QTableWidgetItem* > items = ui->m_TableWidget->selectedItems();
    for(int i = 0;i<items.count();i++)
    {
        int col = items.at(i)->column();
        int row = items.at(i)->row();
        QTableCell TableCell = m_TableProperty.getTableVector()->at(row * m_iColNum + col);
        TableCell.setTextType(m_TextType);
        m_TableProperty.getTableVector()->replace(row * m_iColNum + col,TableCell);
    }
    resetDialog();
}

//关联参数
void QTablePropertySetDlg::setParam()
{
    setTableItemSize();
    QVector<QString> paramVector;
    paramVector.clear();
    QVector<QString> formulaVector;
    formulaVector.clear();
    QList<QTableWidgetItem* > items = ui->m_TableWidget->selectedItems();
    for(int i = 0;i < items.count(); i++)
    {
        QTableCell TableCell = m_TableProperty.getTableVector()->at(items.at(i)->row()*m_iColNum);
        formulaVector.push_back(TableCell.getFormulaStr());
    }
    QSelectMultiParamDlg dlg;
    dlg.setParamList(formulaVector);
    if(dlg.exec() == QDialog::Accepted)
    {
        dlg.getParamList(paramVector,formulaVector);
        QList<QTableWidgetItem* > items = ui->m_TableWidget->selectedItems();
        //初始化单元格关联的参数和公式
        for(int i = 0;i < items.count(); i++)
        {
            for(int j = 0;j < m_iColNum;j++)
            {
                QTableCell TableCell = m_TableProperty.getTableVector()->at(items.at(i)->row() * m_iColNum + j);
                TableCell.setFormulaStr(tr(""));
                TableCell.setParamStr(tr(""));
                m_TableProperty.getTableVector()->replace(items.at(i)->row() * m_iColNum + j,TableCell);
            }
        }
        int num = 0;
        if(paramVector.count() > items.count())
            num = items.count();
        else
            num = paramVector.count();
		//设置单元格关联的参数和公式
        for(int i = 0;i < num; i++)
        {
            for(int j = 0;j < m_iColNum;j++)
            {
                QTableCell TableCell = m_TableProperty.getTableVector()->at(items.at(i)->row() * m_iColNum + j);
                TableCell.setParamStr(paramVector.at(0));
                TableCell.setFormulaStr(formulaVector.at(0));
                m_TableProperty.getTableVector()->replace(items.at(i)->row() * m_iColNum + j,TableCell);
            }
            paramVector.pop_front();
            formulaVector.pop_front();
        }
        setTableWidgetProperty();
    }
}

//初始化右键菜单可见状态
void QTablePropertySetDlg::setMenuState()
{
    act_SetParam->setVisible(m_bSetParamMenuVisible);

    act_AddRow->setVisible(m_bAddDelMenuVisible);
    act_AddColumn->setVisible(m_bAddDelMenuVisible);

    act_DelRow->setVisible(m_bRCHandleMenuVisible);
    act_InsertRow->setVisible(m_bRCHandleMenuVisible);
    act_DelColumn->setVisible(m_bRCHandleMenuVisible);
    act_InsertColumn->setVisible(m_bRCHandleMenuVisible);

    act_ItemFormat->setVisible(m_bItemFormatVisible);

    ColumnContextMenu->setEnabled(m_bColumnContextMenuVisible);
    act_ShowDefault->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaName->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaCode->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaNumber->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaTransType->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaConvType->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaLowerLimite->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaUpperLimite->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaDimension->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaUnit->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaDisplayType->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaTheoryValue->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaTheoryValueRange->setVisible(m_bColumnContextMenuVisible);
    act_ShowParaRemark->setVisible(m_bColumnContextMenuVisible);
}
//确认按钮事件
void QTablePropertySetDlg::on_buttonBox_accepted()
{
    setTableItemSize();
    this->accept();
}
//初始化右键菜单
void QTablePropertySetDlg::initMenu()
{
    //关联参数
	act_SetParam = new QAction(tr("关联参数"),this);
    connect(act_SetParam,SIGNAL(triggered(bool)),this,SLOT(setParam()));
    MainMenu->addAction(act_SetParam);

    MainMenu->addSeparator();

    act_ItemFormat = new QAction(tr("格式"),this);
    connect(act_ItemFormat,SIGNAL(triggered(bool)),this,SLOT(setItemFormat()));
    MainMenu->addAction(act_ItemFormat);

    MainMenu->addSeparator();
	//表格设置
    act_AddRow = new QAction(tr("增加行"),this);
    connect(act_AddRow,SIGNAL(triggered(bool)),this,SLOT(addRow()));
    MainMenu->addAction(act_AddRow);

    act_InsertRow = new QAction(tr("插入行"),this);
    connect(act_InsertRow,SIGNAL(triggered(bool)),this,SLOT(insertRow()));
    MainMenu->addAction(act_InsertRow);

    act_DelRow = new QAction(tr("删除行"),this);
    connect(act_DelRow,SIGNAL(triggered(bool)),this,SLOT(delRow()));
    MainMenu->addAction(act_DelRow);

    MainMenu->addSeparator();
    
    act_AddColumn = new QAction(tr("增加列"),this);
    connect(act_AddColumn,SIGNAL(triggered(bool)),this,SLOT(addColumn()));
    MainMenu->addAction(act_AddColumn);

    act_InsertColumn = new QAction(tr("插入列"),this);
    connect(act_InsertColumn,SIGNAL(triggered(bool)),this,SLOT(insertColumn()));
    MainMenu->addAction(act_InsertColumn);

    act_DelColumn = new QAction(tr("删除列"),this);
    connect(act_DelColumn,SIGNAL(triggered(bool)),this,SLOT(delColumn()));
    MainMenu->addAction(act_DelColumn);

    MainMenu->addSeparator();
    ColumnContextMenu = MainMenu->addMenu(tr("列内容"));
	act_ShowDefault = new QAction(tr("值"),this);
    connect(act_ShowDefault,SIGNAL(triggered(bool)),this,SLOT(setShowDefault()));
    ColumnContextMenu->addAction(act_ShowDefault);

	act_ShowParaName = new QAction(tr("名称"),this);
    connect(act_ShowParaName,SIGNAL(triggered(bool)),this,SLOT(setShowParaName()));
    ColumnContextMenu->addAction(act_ShowParaName);

	act_ShowParaNumber = new QAction(tr("编号"),this);
    connect(act_ShowParaNumber,SIGNAL(triggered(bool)),this,SLOT(setShowParaNumber()));
    ColumnContextMenu->addAction(act_ShowParaNumber);

	act_ShowParaCode = new QAction(tr("代号"),this);
    connect(act_ShowParaCode,SIGNAL(triggered(bool)),this,SLOT(setShowParaCode()));
    ColumnContextMenu->addAction(act_ShowParaCode);
	
	act_ShowParaDimension = new QAction(tr("量纲"),this);
    connect(act_ShowParaDimension,SIGNAL(triggered(bool)),this,SLOT(setShowParaDimension()));
    ColumnContextMenu->addAction(act_ShowParaDimension);
	
	act_ShowParaUnit = new QAction(tr("单位"),this);
    connect(act_ShowParaUnit,SIGNAL(triggered(bool)),this,SLOT(setShowParaUnit()));
    ColumnContextMenu->addAction(act_ShowParaUnit);

    act_ShowParaUpperLimite = new QAction(tr("上限"),this);
    connect(act_ShowParaUpperLimite,SIGNAL(triggered(bool)),this,SLOT(setShowParaUpperLimite()));
    ColumnContextMenu->addAction(act_ShowParaUpperLimite);

    act_ShowParaLowerLimite = new QAction(tr("下限"),this);
    connect(act_ShowParaLowerLimite,SIGNAL(triggered(bool)),this,SLOT(setShowParaLowerLimite()));
    ColumnContextMenu->addAction(act_ShowParaLowerLimite);
	
	act_ShowParaDisplayType = new QAction(tr("显示格式"),this);
    connect(act_ShowParaDisplayType,SIGNAL(triggered(bool)),this,SLOT(setShowParaDisplayType()));
    ColumnContextMenu->addAction(act_ShowParaDisplayType);

	act_ShowParaTheoryValue = new QAction(tr("理论值"),this);
    connect(act_ShowParaTheoryValue,SIGNAL(triggered(bool)),this,SLOT(setShowParaTheoryValue()));
    ColumnContextMenu->addAction(act_ShowParaTheoryValue);
	
	act_ShowParaTheoryValueRange = new QAction(tr("范围"),this);
    connect(act_ShowParaTheoryValueRange,SIGNAL(triggered(bool)),this,SLOT(setShowParaTheoryValueRange()));
    ColumnContextMenu->addAction(act_ShowParaTheoryValueRange);
		
	act_ShowParaTransType = new QAction(tr("传输类型"),this);
    connect(act_ShowParaTransType,SIGNAL(triggered(bool)),this,SLOT(setShowParaTransType()));
    ColumnContextMenu->addAction(act_ShowParaTransType);

	act_ShowParaConvType = new QAction(tr("转换类型"),this);
    connect(act_ShowParaConvType,SIGNAL(triggered(bool)),this,SLOT(setShowParaConvType()));
    ColumnContextMenu->addAction(act_ShowParaConvType);


    act_ShowParaRemark = new QAction(tr("备注"),this);
    connect(act_ShowParaRemark,SIGNAL(triggered(bool)),this,SLOT(setShowParaRemark()));
    ColumnContextMenu->addAction(act_ShowParaRemark);

	//不显示任何数据
	act_ShowNull = new QAction(tr("null"),this);
    connect(act_ShowNull,SIGNAL(triggered(bool)),this,SLOT(setShowNull()));
    ColumnContextMenu->addAction(act_ShowNull);


}
//处理表格行列操作事件
void QTablePropertySetDlg::onOperateTableEvent(int type)
{
    setTableItemSize();
    switch (type) {
    case tp_addRow:
        m_TableProperty.addRow();
        break;
    case tp_insertRow:
        m_TableProperty.insertRow(m_iCurrentRow);
        break;
    case tp_delRow:
        m_TableProperty.delRow(m_iCurrentRow);
        break;
    case tp_addColumn:
        m_TableProperty.addColumn();
        break;
    case tp_insertColumn:
        m_TableProperty.insertColumn(m_iCurrentCol);
        break;
    case tp_delColumn:
        m_TableProperty.delColumn(m_iCurrentCol);
        break;
    default:
        break;
    }
    m_iRowNum = m_TableProperty.getRowNum();
    m_iColNum = m_TableProperty.getColNum();
    resetDialog();
}
//追加行
void QTablePropertySetDlg::addRow()
{
    onOperateTableEvent(tp_addRow);
}
//插入行，插入行位于当前行的上一行
void QTablePropertySetDlg::insertRow()
{
    onOperateTableEvent(tp_insertRow);
}
//删除当前行
void QTablePropertySetDlg::delRow()
{
    onOperateTableEvent(tp_delRow);
}
//追加列
void QTablePropertySetDlg::addColumn()
{
    onOperateTableEvent(tp_addColumn);
}
//插入列，插入列位于当前列的上一列
void QTablePropertySetDlg::insertColumn()
{
    onOperateTableEvent(tp_insertColumn);
}
//删除当前列
void QTablePropertySetDlg::delColumn()
{
    onOperateTableEvent(tp_delColumn);
}
//获取表格控件个单元格的大小后对表格属性数据进行设置
void QTablePropertySetDlg::setTableItemSize()
{
    for(int i = 0; i < m_iRowNum; i++)
    {
        int h = ui->m_TableWidget->rowHeight(i);
        for(int j = 0;j < m_iColNum; j++)
        {
            int w = ui->m_TableWidget->columnWidth(j);
            QTableCell TableCell = m_TableProperty.getTableVector()->at(i * m_iColNum + j);
            TableCell.setWidth(w);
            TableCell.setHeight(h);
            m_TableProperty.getTableVector()->replace(i * m_iColNum + j,TableCell);
        }
     }
}
//显示默认内容
void QTablePropertySetDlg::setShowDefault()
{
    setTableItemTextType(param_Value);
}
//显示参数名称
void QTablePropertySetDlg::setShowParaName()
{
    setTableItemTextType(param_Name);
}
void QTablePropertySetDlg::setShowParaNumber()
{
    setTableItemTextType(param_No);
}
//显示参数代号
void QTablePropertySetDlg::setShowParaCode()
{
    setTableItemTextType(param_Code);
}
//显示参数传输类型
void QTablePropertySetDlg::setShowParaTransType()
{
    setTableItemTextType(param_TransType);
}
//显示参数转换类型
void QTablePropertySetDlg::setShowParaConvType()
{
    setTableItemTextType(param_ConvType);
}
//显示参数下限
void QTablePropertySetDlg::setShowParaLowerLimite()
{
    setTableItemTextType(param_LowerLimite);
}
//显示参数上限
void QTablePropertySetDlg::setShowParaUpperLimite()
{
    setTableItemTextType(param_UpperLimite);
}
//显示参数量纲
void QTablePropertySetDlg::setShowParaDimension()
{
    setTableItemTextType(param_Dimension);
}
//显示参数单位
void QTablePropertySetDlg::setShowParaUnit()
{
    setTableItemTextType(param_Unit);
}
//显示参数显示格式
void QTablePropertySetDlg::setShowParaDisplayType()
{
    setTableItemTextType(param_DisplayType);
}
//显示参数理论值
void QTablePropertySetDlg::setShowParaTheoryValue()
{
    setTableItemTextType(param_TheoryValue);
}
//显示参数理论值范围
void QTablePropertySetDlg::setShowParaTheoryValueRange()
{
    setTableItemTextType(param_TheoryValueRange);
}
//显示参数备注
void QTablePropertySetDlg::setShowParaRemark()
{
    setTableItemTextType(param_Remark);
}
// 不显示任何数据
void QTablePropertySetDlg::setShowNull()
{
	setTableItemTextType(param_NULL);
}
//单元格格式设置
void QTablePropertySetDlg::setItemFormat()
{
    setTableItemSize();
    QList<QTableWidgetItem* > items = ui->m_TableWidget->selectedItems();
    int col = items.at(0)->column();
    int row = items.at(0)->row();
    QTableCell TableCell = m_TableProperty.getTableVector()->at(row * m_iColNum + col);
    tableitemset set(TableCell);
    if(set.exec() == QDialog::Accepted)
    {
        TableCell = set.getTableCell();
        for(int i = 0;i<items.count();i++)
        {
            int col = items.at(i)->column();
            int row = items.at(i)->row();
            QTableCell TableCell1 = m_TableProperty.getTableVector()->at(row * m_iColNum + col);
            TableCell1.setTextColor(TableCell.getTextColor());
            TableCell1.setTextFont(TableCell.getTextFont());
            TableCell1.setTextHAlignment(TableCell.getTextHAlignment());
            TableCell1.setTextVAlignment(TableCell.getTextVAlignment());
            TableCell1.setBackgroundColor(TableCell.getBackgroundColor());
            TableCell1.setShowType(TableCell.getShowType());
            TableCell1.setLeftEdgeVisible(TableCell.getLeftEdgeVisible());
            TableCell1.setTopEdgeVisible(TableCell.getTopEdgeVisible());
            TableCell1.setLeftMargin(TableCell.getLeftMargin());
            TableCell1.setRightMargin(TableCell.getRightMargin());
            TableCell1.setTopMargin(TableCell.getTopMargin());
            TableCell1.setBottomMargin(TableCell.getBottomMargin());
            m_TableProperty.getTableVector()->replace(row * m_iColNum + col,TableCell1);
        }
        resetDialog();
    }
}
//设置表格大小确认按钮事件
void QTablePropertySetDlg::on_m_TableSizeSetBotton_clicked()
{
    m_TableProperty.setSize(ui->m_TableWidthEdit->text().toInt(),ui->m_TableHeightEdit->text().toInt());
    resetDialog();
}
//设置表格行高或列宽确认按钮事件
void QTablePropertySetDlg::on_m_CellSizeSetBotton_clicked()
{
    if(ui->m_RowColumnomboBox->currentIndex() == 0 )
        m_TableProperty.setRowHeight(ui->m_CellNumberEdit->text().toInt(),ui->m_CellSizeEdit->text().toInt());
    else
        m_TableProperty.setColumnWidth(ui->m_CellNumberEdit->text().toInt(),ui->m_CellSizeEdit->text().toInt());
    resetDialog();
}
//设置表格行列数确认按钮事件
void QTablePropertySetDlg::on_TableSetBotton_clicked()
{
    int row = ui->TableRowEdit->text().toInt();
    int col = ui->TableColumnEdit->text().toInt();
	//异常处理
    if(col <= 0 || row <= 0)
    {
        m_iRowNum = 0;
        m_iColNum = 0;
        m_TableProperty.setRowNum(m_iRowNum);
        m_TableProperty.setColNum(m_iColNum);
        m_TableProperty.getTableVector()->clear();
        setTableWidgetProperty();
        return;
    }
    if((m_iRowNum <= 0)||(m_iColNum <= 0))
    {
        m_iRowNum = row;
        m_iColNum = col;
        m_TableProperty.setRowNum(m_iRowNum);
        m_TableProperty.setColNum(m_iColNum);
        m_TableProperty.setTableVector(tr(""));
        setTableWidgetProperty();
        return;
    }
    int count = 0;
	//行数调整
    if(row > m_iRowNum)
    {
        count = row - m_iRowNum;
        for(int i = 0; i < count; i++)
            addRow();
    }
    if(row < m_iRowNum)
    {
        count = m_iRowNum - row;
        for(int i = 0; i < count; i++)
        {
            m_iCurrentRow = m_iRowNum - 1;
            delRow();
        }
    }
	//列数调整
    if(col > m_iColNum)
    {
        count = col - m_iColNum;
        for(int i = 0; i < count; i++)
            addColumn();
    }
    if(col < m_iColNum)
    {
        count = m_iColNum - col;
        for(int i = 0; i < count; i++)
        {
            m_iCurrentCol = m_iColNum - 1;
            delColumn();
        }
    }
}
} // namespace qdesigner_internal

QT_END_NAMESPACE








