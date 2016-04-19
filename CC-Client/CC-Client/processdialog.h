#ifndef PROCESSDIALOG_H
#define PROCESSDIALOG_H

#include <QDialog>
#include <QString>
namespace Ui {class ProcessDialog;};
class StationInfo;

//�鿴/ѡ��Զ�̹���վ������Ϣ�Ի���
class ProcessDialog : public QDialog
{
	Q_OBJECT

public:
	ProcessDialog(StationInfo* station,QWidget *parent = 0);
	~ProcessDialog();

	/*!
	��ȡѡ��Ľ���Id
	@return int ����Id
	���ߣ�cokkiy��������
	����ʱ��:2016/4/14 11:14:31
	*/
	int ProcessId();

	/*!
	��ȡѡ���Զ�̽�������
	@return QString Զ�̽�������
	���ߣ�cokkiy��������
	����ʱ��:2016/4/14 11:15:03
	*/
	QString ProcessName();

private:
	Ui::ProcessDialog *ui;
	StationInfo* station;
	int processId;
	QString processName;

	void getProcessInfo();

	protected slots:
	void on_okPushButton_clicked();
	void on_cancelPushButton_clicked();
};

#endif // PROCESSDIALOG_H
