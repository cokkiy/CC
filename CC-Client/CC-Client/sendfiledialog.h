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
	���������ļ��Ի���
	@param StationList * pStations ȫ������վ�б�
	@param const QModelIndexList & selectedIndexs ѡ���Ĺ���������
	@param bool allStations �Ƿ���ȫ������վ
	@param QWidget * parent
	@return 
	���ߣ�cokkiy��������
	����ʱ��:2016/3/23 10:59:55
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
	
	//��������
	void createLayout(StationList* pStations, const QModelIndexList& selectedIndexs, bool allStations);

	//�źŴ�����
	void on_failToSendFile(StationInfo* station, QString fileName, QString message);
	void on_newFileSize(StationInfo* station, QString fileName, size_t size);
	void on_sendFileCompleted(StationInfo* station, QString fileName);
	void on_fileSended(StationInfo* station, QString fileName, size_t size);
	void on_allCompleted(StationInfo* station, int total);

	//�ۺ���
	private slots:
	//���Ҫ���͵��ļ�
	void on_browserPushButton_clicked();
	//���Ͱ�ť����
	void on_sendPushButton_clicked();
	//ѡ��Ŀ���ļ���
	void on_selectDestPushButton_clicked();	
	
};

#endif // SENDFILEDIALOG_H
