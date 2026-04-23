#ifndef VIEWLOGDIALOG_H
#define VIEWLOGDIALOG_H

#include <QDialog>
#include <list>
namespace Ui {class ViewLogDialog;};

class ViewLogDialog : public QDialog
{
	Q_OBJECT

public:
	ViewLogDialog(std::list<QString> logs, QWidget *parent = 0);
	~ViewLogDialog();

private:
	Ui::ViewLogDialog *ui;
	std::list<QString> logs;
	void saveLog(bool checked);
};

#endif // VIEWLOGDIALOG_H
