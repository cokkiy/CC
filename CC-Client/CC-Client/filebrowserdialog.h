#ifndef FILEBROWSERDIALOG_H
#define FILEBROWSERDIALOG_H

#include <QDialog>
#include <QIcon>
namespace Ui {class FileBrowserDialog;};
class StationInfo;

//Զ���ļ�����Ի���
class FileBrowserDialog : public QDialog
{
	Q_OBJECT

public:
	/*!
	����Զ���ļ�����Ի���
	@param StationInfo * station ����վ
	@param bool Directory ѡ��Ŀ¼�����ļ�
	@param QString dir ��ǰĿ¼
	@param QWidget * parent
	@return 
	���ߣ�cokkiy��������
	����ʱ��:2016/4/14 8:40:05
	*/
	FileBrowserDialog(StationInfo* station, bool Directory = true,
		QString dir = QStringLiteral(""), QWidget *parent = 0);
	~FileBrowserDialog();


	/*!
	��ȡѡ����ļ���Ŀ¼·��
	@return QString ��ǰѡ����ļ���Ŀ¼·��
	���ߣ�cokkiy��������
	����ʱ��:2016/4/14 8:38:52
	*/
	QString SelectedPath();

private:
	Ui::FileBrowserDialog *ui;
	bool Directory;
	StationInfo* station;
	//��ǰĿ¼
	QString currentDir;
	//���ѡ���Ŀ¼���ļ�·��
	QString selectedPath;

	//ͼ��
	QIcon dirIcon, fileIcon;
	//��ȡ�ļ���Ϣ
	void getFileInfo(QString path);
	//���˫��
	void on_cellDoubleClicked(int row,int column);

	protected slots:
	void on_cancelPushButton_clicked();
	void on_okPushButton_clicked();
	void on_upPushButton_clicked();
	void on_downPushButton_clicked();
};

#endif // FILEBROWSERDIALOG_H
