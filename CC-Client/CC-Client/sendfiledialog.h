#ifndef SENDFILEDIALOG_H
#define SENDFILEDIALOG_H

#include <QDialog>
#include <QAbstractItemModel>
#include <Ice/Communicator.h>
#include <list>
namespace Ui {class SendFileDialog;};
class StationList;
class StationInfo;
class StationBar;
class SendFileDialog : public QDialog
{
	Q_OBJECT

public:	
	/*!
	创建发送文件对话框
	@param StationList * pStations 全部工作站列表
	@param const QModelIndexList & selectedIndexs 选定的工作扎索引
	@param bool allStations 是否是全部工作站
	@param QWidget * parent
	@return 
	作者：cokkiy（张立民）
	创建时间:2016/3/23 10:59:55
	*/
	SendFileDialog(StationList* pStations,const QModelIndexList& selectedIndexs,
		bool allStations, Ice::CommunicatorPtr communicator, QWidget *parent = 0);
	~SendFileDialog();

private:
	Ui::SendFileDialog *ui;
	//StationList* pStations;
	//bool allStations;
	//QModelIndexList selectedIndex;
	Ice::CommunicatorPtr communicator;
	QStringList fileNames;
	std::list<StationInfo*> stations;
	std::map<StationInfo*, StationBar*> station_bar;
	
	//创建布局
	void createLayout(StationList* pStations, const QModelIndexList& selectedIndexs, bool allStations);

	//信号处理函数
	void on_failToSendFile(StationInfo* station, QString fileName, QString message);
	void on_newFileSize(StationInfo* station, QString fileName, size_t size);
	void on_sendFileCompleted(StationInfo* station, QString fileName);
	void on_fileSended(StationInfo* station, QString fileName, size_t size);
	void on_allCompleted(StationInfo* station, int total);

	//槽函数
	private slots:
	//浏览要发送的文件
	void on_browserPushButton_clicked();
	//发送按钮按下
	void on_sendPushButton_clicked();
	//选择目标文件夹
	void on_selectDestPushButton_clicked();	
	
};

#endif // SENDFILEDIALOG_H
