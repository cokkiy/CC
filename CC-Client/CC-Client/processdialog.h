#ifndef PROCESSDIALOG_H
#define PROCESSDIALOG_H

#include <QDialog>
#include <QString>
namespace Ui {class ProcessDialog;};
class StationInfo;

//查看/选择远程工作站进程信息对话框
class ProcessDialog : public QDialog
{
	Q_OBJECT

public:
	ProcessDialog(StationInfo* station,QWidget *parent = 0);
	~ProcessDialog();

	/*!
	获取选择的进程Id
	@return int 进程Id
	作者：cokkiy（张立民）
	创建时间:2016/4/14 11:14:31
	*/
	int ProcessId();

	/*!
	获取选择的远程进程名称
	@return QString 远程进程名称
	作者：cokkiy（张立民）
	创建时间:2016/4/14 11:15:03
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
