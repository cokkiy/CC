#ifndef QPAGENUMDIALOG_H
#define QPAGENUMDIALOG_H

#include <QDialog>
namespace Ui {class QPageNumDialog;};

class QPageNumDialog : public QDialog
{
	Q_OBJECT

public:
	QPageNumDialog(QWidget *parent = 0);
	~QPageNumDialog();
	//获取选定的页面编号
	int value();

private:
	Ui::QPageNumDialog *ui;

	private slots:
	void on_OKPushButton_clicked();
};

#endif // QPAGENUMDIALOG_H
