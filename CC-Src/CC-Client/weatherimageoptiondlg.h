#ifndef WEATHERIMAGEOPTIONDLG_H
#define WEATHERIMAGEOPTIONDLG_H

#include <QDialog>
#include "option.h"
namespace Ui {class WeatherImageOptionDlg;};

class WeatherImageOptionDlg : public QDialog
{
	Q_OBJECT

public:
	WeatherImageOptionDlg(Option::WeatherImageDownloadOption* option, QWidget *parent = 0);
	~WeatherImageOptionDlg();

private:
	Ui::WeatherImageOptionDlg *ui;
	Option::WeatherImageDownloadOption* option;
	private slots:
	void on_okPushButton_clicked();
	void on_cancelPushButton_clicked();
};

#endif // WEATHERIMAGEOPTIONDLG_H
