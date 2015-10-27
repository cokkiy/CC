#ifndef SELECTPARAMDIALOG_H
#define SELECTPARAMDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QRadioButton>
#include <QWidget>
#include <QtGui>
#include <QHBoxLayout>
#include <QGridLayout>
#include "propertyeditor_global.h"
#include <Net/NetComponents>

QT_BEGIN_NAMESPACE

class QDesignerFormEditorInterface;
namespace Ui {
class SelectSingleParamDialog;
}

namespace qdesigner_internal {

class QT_PROPERTYEDITOR_EXPORT QSelectParamDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QSelectParamDialog(QWidget *parent = 0);
    ~QSelectParamDialog();
    //显示窗口
    int showDialog();
    //获取所选择的参数字符串（包含标号、参数号）或公式字符串
    QString text() const;
    //将单参数控件中的字符串传入对话框
    void SetText(QString );
private:
    //ui 界面
    Ui::SelectSingleParamDialog *ui;
private:
    //信息约定表参数接口
    InformationInterface* net;
    //初始化信息约定表表参数控件表格
    bool initFormTable();
private slots:
    //选择信息约定表参数表控件表格单表信息
    void OnClickTableForm(int,int);
    //选择信息约定表参数控件表格单参数信息
    void OnClickParamForm(int,int);
    void SelectAllSystemRadioButton(bool);
    void SelectKELRadioButton(bool);
    void SelectDFZXRadioButton(bool);
    void SelectCustomRadioButton(bool);
    void SelectC3IRadioButton(bool );
    void SelectCustom();
private:
    ushort m_iMaxTableNo;
    ushort m_iMinTableNo;
    int m_iCurrentTableNo;
};
} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // SELECTPARAMDIALOG_H
