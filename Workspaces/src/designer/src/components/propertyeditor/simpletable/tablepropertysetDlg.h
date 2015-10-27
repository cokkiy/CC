#ifndef TABLEPROPERTYSETDLG_H
#define TABLEPROPERTYSETDLG_H
#include <QtWidgets/QTableWidget>
#include <QDialog>
#include <QMenu>
#include "propertyeditor_global.h"
#include <Net/NetComponents>
#include "tablecell.h"
#include "tableproperty.h"

namespace Ui 
{
	class TablePropertySetDialog;
}

QT_BEGIN_NAMESPACE
namespace qdesigner_internal {

class QT_PROPERTYEDITOR_EXPORT QTablePropertySetDlg : public QDialog
{
    Q_OBJECT
private:
    Ui::TablePropertySetDialog *ui;
public:
    explicit QTablePropertySetDlg(QWidget *parent = 0,QString str =tr(""));
    //显示配置对话框
	int showDialog();
    ~QTablePropertySetDlg();
    QTableProperty getTableProperty() const {return m_TableProperty;}
private:
	//获取表格控件个单元格的大小后对表格属性数据进行设置
    void setTableItemSize();
	//设置表格控件属性，进行信号槽设置
    void setTableWidgetProperty();
	//重新显示对话框控件的数据
    void resetDialog();
    //获取表格某行高或某列高数据显示，可设置
    void resetTableSizeFrame();
    //获取表格行数和列数显示，可设置
    void resetTableSetFrame();
    //获取表格宽度和高度大小显示，可设置
    void resetRowColumnSizeFrame(int index,int number,int size);
private:
	//表格行数
    int m_iRowNum;
	//表格列数
    int m_iColNum;
	//编辑状态
    bool m_bEditState;
	//鼠标点击表格控件时，获取的鼠标在表格定位的行数和列数编号
    int m_iCurrentRow;
    int m_iCurrentCol;
	//当表格为编辑状态时单元格行数和列数编号
    int m_iCurrentEditRow;
    int m_iCurrentEditCol;
	//表格属性
    QTableProperty m_TableProperty;
private slots:
	//通过移动横向表头某项改变表格某列宽度
    void onHSectionResized(int logicalIndex, int oldSize, int newSize);
	//通过移动纵向表头某项改变表格某行高度
    void onVSectionResized(int logicalIndex, int oldSize, int newSize);
	//鼠标从当前编辑的单元格移开事件
    void onItemSelectionChanged();
	//通过表头点击事件选择列
    void onSectionPressed(int );
	//点击单元格事件
    void onCellClicked(int ,int );
	//编辑单元格文本
    void onItemEdit(QTableWidgetItem *);
	//设置单元格显示内容
    void setTableItemTextType(qint8 m_TextType);
	//关联参数,只有选择表格的第1列的单元格时该功能有效
    void setParam();
	//追加行
    void addRow();
	//插入行，插入行位于当前行的上一行
    void insertRow();
	//删除当前行
    void delRow();
	//追加列
    void addColumn();
	//插入列，插入列位于当前列的上一列
    void insertColumn();
	//删除当前列
    void delColumn();
	//显示默认内容
    void setShowDefault();
	//显示参数名称
    void setShowParaName();
	 //显示参数代号
    void setShowParaCode();
	//参数编号
    void setShowParaNumber();
	//显示参数传输类型
    void setShowParaTransType();
	//显示参数转换类型
    void setShowParaConvType();
	//显示参数下限
    void setShowParaLowerLimite();
	//显示参数上限
    void setShowParaUpperLimite();
	//显示参数量纲
    void setShowParaDimension();
	//显示参数单位
    void setShowParaUnit();
	//显示参数显示格式
    void setShowParaDisplayType();
	//显示参数理论值
    void setShowParaTheoryValue();
	//显示参数理论值范围
    void setShowParaTheoryValueRange();
	//显示参数备注
    void setShowParaRemark();
	//不显示任何数据
	void setShowNull();
    //单元格格式
    void setItemFormat();
	//确认按钮事件
    void on_buttonBox_accepted();
	//设置表格大小确认按钮事件
    void on_m_TableSizeSetBotton_clicked();
	//设置表格行高或列宽确认按钮事件
    void on_m_CellSizeSetBotton_clicked();
	//设置表格行列数确认按钮事件
    void on_TableSetBotton_clicked();
private:
	//键盘事件
    void keyPressEvent(QKeyEvent*);
    //右键菜单事件
    void contextMenuEvent(QContextMenuEvent* );
	//处理右键菜单
    bool getMenuState(QCursor );
	//右键菜单
    QMenu* MainMenu;
	//初始化右键菜单
    void initMenu();
	//获取右键菜单可用状态，初始化右键各菜单项可见状态
    void setMenuState();
	//处理表格行列操作事件
    void onOperateTableEvent(int type);
	//关联参数
    QAction* act_SetParam;
    bool m_bSetParamMenuVisible;
    //增加行
    QAction* act_AddRow;
    //增加列
    QAction* act_AddColumn;
    //增加行或列菜单是否可见
    bool m_bAddDelMenuVisible;

    //插入行
    QAction* act_InsertRow;
    //删除行
    QAction* act_DelRow;
    //插入列
    QAction* act_InsertColumn;
    //删除列
    QAction* act_DelColumn;
    //行和列操作菜单是否可见
    bool m_bRCHandleMenuVisible;

	//单元格格式
    QAction* act_ItemFormat;
    //单元格格式菜单是否可见
    bool m_bItemFormatVisible;

	//列内容
    QMenu* ColumnContextMenu;
    //默认显示
    QAction* act_ShowDefault;
    //显示参数名称
    QAction* act_ShowParaName;
	 //显示参数代号
    QAction* act_ShowParaCode;
	//参数编号
    QAction* act_ShowParaNumber;
	//显示参数传输类型
    QAction* act_ShowParaTransType;
	//显示参数转换类型
    QAction* act_ShowParaConvType;
	//显示参数下限
    QAction* act_ShowParaLowerLimite;
	//显示参数上限
    QAction* act_ShowParaUpperLimite;
	//显示参数量纲
    QAction* act_ShowParaDimension;
	//显示参数单位
    QAction* act_ShowParaUnit;
	//显示参数显示格式
    QAction* act_ShowParaDisplayType;
	//显示参数理论值
    QAction* act_ShowParaTheoryValue;
	//显示参数理论值范围
    QAction* act_ShowParaTheoryValueRange;
	//显示参数备注
    QAction* act_ShowParaRemark;
	//不显示任何数据
    QAction* act_ShowNull;
    //列内容菜单是否可见
    bool m_bColumnContextMenuVisible;
};
}
QT_END_NAMESPACE

#endif // TABLEPROPERTYSET_H
