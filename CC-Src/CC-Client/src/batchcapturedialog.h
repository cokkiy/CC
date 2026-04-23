#ifndef BATCHCAPTUREDIALOG_H
#define BATCHCAPTUREDIALOG_H

#include <QDialog>
#include <QAbstractItemModel>
#include <list>
namespace Ui {class BatchCaptureDialog;};
class StationList;
class StationInfo;
class BatchCaptureDialog : public QDialog
{
	Q_OBJECT

public:
	BatchCaptureDialog(StationList* pStations, const QModelIndexList& selectedIndexs, QWidget *parent = 0);
	~BatchCaptureDialog();

private:
	Ui::BatchCaptureDialog *ui;
	std::list<StationInfo*> stations;
	int captured = 0;
	private slots:
	void on_startPushButton_clicked();
	void on_selectDirPushButton_clicked();
	void On_ScreenCaptured(StationInfo* s, bool result, QString message);
};

#endif // BATCHCAPTUREDIALOG_H
