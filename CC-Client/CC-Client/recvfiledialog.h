#ifndef RECVFILEDIALOG_H
#define RECVFILEDIALOG_H

#include <QDialog>
#include <QAbstractItemModel>
#include <Ice/Communicator.h>
namespace Ui {class RecvFileDialog;};
class StationList;
class StationBar;
class StationInfo;
class RecvFileDialog : public QDialog
{
	Q_OBJECT

public:
	/*!
	创建接收文件对话框
	@param StationList * pStations
	@param const QModelIndexList & selectedIndexs
	@param bool allStations 是否是全部工作站
	@param Ice::CommunicatorPtr communicator
	@param QWidget * parent
	@return 
	作者：cokkiy（张立民）
	创建时间:2016/3/28 15:39:43
	*/
	RecvFileDialog(StationList* pStations, const QModelIndexList& selectedIndexs,
		bool allStations, Ice::CommunicatorPtr communicator, QWidget *parent = 0);
	~RecvFileDialog();

protected slots:
	void on_browsePushButton_clicked();
	void on_recvPushButton_clicked();
	void on_selectFilePushButton_clicked();

	//文件大小处理事件
	void on_notifyFileSize(StationInfo* station, long long size);
	//收取数据错误事件
	void on_getDataError(StationInfo* station, QString message);
	//创建文件错误事件
	void on_createFileError(StationInfo* station, QString fileName, QString message);
	//文件传输完毕
	void on_transFileComplete(StationInfo *station);
	//文件已获取大小事件
	void on_fileGeted(StationInfo* station, long long size);
private:
	Ui::RecvFileDialog *ui;
	QString destDir;
	Ice::CommunicatorPtr communicator;
	std::list<StationInfo*> stations;
	std::map<StationInfo*, StationBar*> station_bar;

	//创建布局
	void createLayout(StationList* pStations, const QModelIndexList& selectedIndexs, bool allStations);
	
};

#endif // RECVFILEDIALOG_H
