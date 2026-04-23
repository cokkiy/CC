#ifndef FILEBROWSERDIALOG_H
#define FILEBROWSERDIALOG_H

#include <QDialog>
#include <QIcon>
namespace Ui {class FileBrowserDialog;};
class StationInfo;

//远程文件浏览对话框
class FileBrowserDialog : public QDialog
{
	Q_OBJECT

public:
	/*!
	创建远程文件浏览对话框
	@param StationInfo * station 工作站
	@param bool Directory 选择目录还是文件
	@param QString dir 当前目录
	@param QWidget * parent
	@return 
	作者：cokkiy（张立民）
	创建时间:2016/4/14 8:40:05
	*/
	FileBrowserDialog(StationInfo* station, bool Directory = true,
		QString dir = QStringLiteral(""), QWidget *parent = 0);
	~FileBrowserDialog();


	/*!
	获取选择的文件或目录路径
	@return QString 当前选择的文件或目录路径
	作者：cokkiy（张立民）
	创建时间:2016/4/14 8:38:52
	*/
	QString SelectedPath();

private:
	Ui::FileBrowserDialog *ui;
	bool Directory;
	StationInfo* station;
	//当前目录
	QString currentDir;
	//最后选择的目录或文件路径
	QString selectedPath;

	//图标
	QIcon dirIcon, fileIcon;
	//获取文件信息
	void getFileInfo(QString path);
	//鼠标双击
	void on_cellDoubleClicked(int row,int column);

	protected slots:
	void on_cancelPushButton_clicked();
	void on_okPushButton_clicked();
	void on_upPushButton_clicked();
	void on_downPushButton_clicked();
};

#endif // FILEBROWSERDIALOG_H
