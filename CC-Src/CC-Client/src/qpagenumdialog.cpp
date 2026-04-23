#include "qpagenumdialog.h"
#include "ui_qpagenumdialog.h"
#include <QMessageBox>

QPageNumDialog::QPageNumDialog(QWidget *parent)
	: QDialog(parent)
{
	ui = new Ui::QPageNumDialog();
	ui->setupUi(this);
}

QPageNumDialog::~QPageNumDialog()
{
	delete ui;
}

int QPageNumDialog::value()
{
	return ui->pageNumSpinBox->value();
}

void QPageNumDialog::on_OKPushButton_clicked()
{
	if (ui->pageNumSpinBox->value() < 0 || ui->pageNumSpinBox->value() > 999)
	{
		QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("画面编号必须介于0-999之间。"));
		return;
	}
	accept();
}

