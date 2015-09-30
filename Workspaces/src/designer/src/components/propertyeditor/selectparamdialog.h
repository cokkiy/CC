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

class QPlainTextEdit;
class QDesignerFormEditorInterface;

namespace qdesigner_internal {

class QT_PROPERTYEDITOR_EXPORT QSelectParamDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QSelectParamDialog(QWidget *parent = 0);
    ~QSelectParamDialog();
    int showDialog();
    QString text() const;
    void SetText(QString );
private:
    InformationInterface* net;
    bool initFormTable();
private slots:
    void OnClickTableForm(int,int);
    void OnClickParamForm(int,int);
    void SelectAllSystemRadioButton(bool);
    void SelectKELRadioButton(bool);
    void SelectDFZXRadioButton(bool);
    void SelectCustomRadioButton(bool);
    void SelectC3IRadioButton(bool );
    void SelectCustom();
private:
//    Net m_form;
//    QDesignerFormEditorInterface *m_core;
    //Top Layout
    QHBoxLayout *m_topLayoutLeft;
    QHBoxLayout *m_topLayoutRight;
    QLabel *m_ParamLabel;//
    QLineEdit *m_parameditor;
    //Left Layout
    QGridLayout *m_leftLayout;
    QLabel *m_formNameLabel;
    QTableWidget *m_formTableWidget;
    //Right Layout
    QGridLayout *m_rightLayout;
    QTableWidget *m_paramTableWidget;
    QLabel *m_paramNameLabel;
    //Bottom Layout
    QGridLayout *m_bottomLayout;
    QDialogButtonBox *buttonBox;


    QLineEdit* m_CustomMinParamEdit;
    QLineEdit* m_CustomMaxParamEdit;
    QPushButton* m_SelectParamButton;

    QRadioButton* m_AllSystemRadioButton;
    QRadioButton* m_C3IRadioButton;
    QRadioButton* m_DFZXRadioButton;
    QRadioButton* m_KELRadioButton;

private:
    ushort m_iMaxTableNo;
    ushort m_iMinTableNo;
    int m_iCurrentTableNo;

};
} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // SELECTPARAMDIALOG_H
