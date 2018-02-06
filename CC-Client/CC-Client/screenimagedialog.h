#ifndef SCREENIMAGEDIALOG_H
#define SCREENIMAGEDIALOG_H

#include <QDialog>
#include <vector>
#include <Ice/Config.h>
#include <QString>

#ifdef Q_OS_LINUX
using byte = unsigned char;
#endif

namespace Ui {class ScreenImageDialog;}
class StationInfo;
class ScreenImageDialog : public QDialog
{
	Q_OBJECT

public:
	ScreenImageDialog(StationInfo* station,QWidget *parent = 0);

	void getCaptureData(StationInfo* station,Ice::Long position);

	~ScreenImageDialog();
	//绘制事件
	virtual void paintEvent(QPaintEvent *) override;

private:
	Ui::ScreenImageDialog *ui;
	//数据
	std::vector<byte> buffer;
	//错误是否发生
	bool errorOccured = false;
	bool finished = false;
	QString message;

	StationInfo* station;

protected:
	void mouseDoubleClickEvent(QMouseEvent * event) override;

	protected slots:
	//保存快照
	void on_saveToolButton_clicked();
	//再次捕获快照
	void on_captureToolButton_clicked();
};

#endif // SCREENIMAGEDIALOG_H
