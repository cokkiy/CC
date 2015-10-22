﻿#ifndef SELECTPARAMDIALOG_H
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
	//��ʾ����
    int showDialog();
	//��ȡ��ѡ��Ĳ����ַ�����������š������ţ���ʽ�ַ���
    QString text() const;
	//���������ؼ��е��ַ�������Ի���
    void SetText(QString );
private:
	//ui ����
    Ui::SelectSingleParamDialog *ui;
private:
	//��ϢԼ��������ӿ�
    InformationInterface* net;
	//��ʼ����ϢԼ���������ؼ����
    bool initFormTable();
private slots:
	//ѡ����ϢԼ���������ؼ���񵥱���Ϣ
    void OnClickTableForm(int,int);
	//ѡ����ϢԼ��������ؼ���񵥲�����Ϣ
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
