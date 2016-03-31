#ifndef STATIONBAR_H
#define STATIONBAR_H

#include <QWidget>
class QProgressBar;
class QLabel;

/************************************************************************/
/* 工作站图标条控件
/* 带图标和进度条
/************************************************************************/
class StationBar : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString TipText READ TipText WRITE setTipText)
	Q_PROPERTY(size_t Percent READ Percent WRITE setPercent)
	Q_PROPERTY(QString StationName READ StationName WRITE setStationName)
	Q_PROPERTY(bool IsOnline READ IsOnline WRITE setIsOnline)

private:
	//带图标的工作站名称--在线
	const QString stationName_on = QStringLiteral("<html><head / ><body align = \"center\"><p><img src=\":/Icons/ncom003.ico\"/></p><p>%1</p></body></html>");
	//带图标的工作站名称--不在线
	const QString stationName_off = QStringLiteral("<html><head / ><body align = \"center\"><p><img src=\":/Icons/ncom002.ico\"/></p><p>%1</p></body></html>");

private:
	bool m_IsOnline;
public:
	/*!
	获取是否在线
	@return bool
	作者：cokkiy（张立民）
	创建时间:2016/3/23 15:56:22
	*/
	bool IsOnline();
	/*!
	设置是否在线
	@param bool value 是否在线
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/3/23 15:56:34
	*/
	void setIsOnline(bool value);

private:
	QString m_StationName;
public:
	
	/*!
	获取工作站名称
	@return QString 工作站名称
	作者：cokkiy（张立民）
	创建时间:2016/3/23 15:37:16
	*/
	QString StationName();
	/*!
	设置工作站名称
	@param QString value 工作站名称
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/3/23 15:37:41
	*/
	void setStationName(QString value);



private:
	size_t m_Percent;
public:
	/*!
	获取当前进度
	@return int 当前进度
	作者：cokkiy（张立民）
	创建时间:2016/3/23 15:34:38
	*/
	size_t Percent();

	/*!
	设置进度
	@param int value 进度
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/3/23 15:34:58
	*/
	void setPercent(size_t value);


private:
	QString m_TipText;
public:
	/*!
	获取当前提示信息
	@return QString 当前提示信息
	作者：cokkiy（张立民）
	创建时间:2016/3/23 15:35:14
	*/
	QString TipText();
	/*!
	设置当前提示信息
	@param QString value 要设置的提示信息
	@return void
	作者：cokkiy（张立民）
	创建时间:2016/3/23 15:35:46
	*/
	void setTipText(QString value);

public:
	StationBar(QWidget *parent);
	~StationBar();
	void setMaxPercent(size_t size);
private:
	QLabel* iconLabel;
	QLabel* infoLabel;
	QProgressBar* progressBar;
	size_t maxPercent;
};

#endif // STATIONBAR_H
