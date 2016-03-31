#ifndef STATIONBAR_H
#define STATIONBAR_H

#include <QWidget>
class QProgressBar;
class QLabel;

/************************************************************************/
/* ����վͼ�����ؼ�
/* ��ͼ��ͽ�����
/************************************************************************/
class StationBar : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString TipText READ TipText WRITE setTipText)
	Q_PROPERTY(size_t Percent READ Percent WRITE setPercent)
	Q_PROPERTY(QString StationName READ StationName WRITE setStationName)
	Q_PROPERTY(bool IsOnline READ IsOnline WRITE setIsOnline)

private:
	//��ͼ��Ĺ���վ����--����
	const QString stationName_on = QStringLiteral("<html><head / ><body align = \"center\"><p><img src=\":/Icons/ncom003.ico\"/></p><p>%1</p></body></html>");
	//��ͼ��Ĺ���վ����--������
	const QString stationName_off = QStringLiteral("<html><head / ><body align = \"center\"><p><img src=\":/Icons/ncom002.ico\"/></p><p>%1</p></body></html>");

private:
	bool m_IsOnline;
public:
	/*!
	��ȡ�Ƿ�����
	@return bool
	���ߣ�cokkiy��������
	����ʱ��:2016/3/23 15:56:22
	*/
	bool IsOnline();
	/*!
	�����Ƿ�����
	@param bool value �Ƿ�����
	@return void
	���ߣ�cokkiy��������
	����ʱ��:2016/3/23 15:56:34
	*/
	void setIsOnline(bool value);

private:
	QString m_StationName;
public:
	
	/*!
	��ȡ����վ����
	@return QString ����վ����
	���ߣ�cokkiy��������
	����ʱ��:2016/3/23 15:37:16
	*/
	QString StationName();
	/*!
	���ù���վ����
	@param QString value ����վ����
	@return void
	���ߣ�cokkiy��������
	����ʱ��:2016/3/23 15:37:41
	*/
	void setStationName(QString value);



private:
	size_t m_Percent;
public:
	/*!
	��ȡ��ǰ����
	@return int ��ǰ����
	���ߣ�cokkiy��������
	����ʱ��:2016/3/23 15:34:38
	*/
	size_t Percent();

	/*!
	���ý���
	@param int value ����
	@return void
	���ߣ�cokkiy��������
	����ʱ��:2016/3/23 15:34:58
	*/
	void setPercent(size_t value);


private:
	QString m_TipText;
public:
	/*!
	��ȡ��ǰ��ʾ��Ϣ
	@return QString ��ǰ��ʾ��Ϣ
	���ߣ�cokkiy��������
	����ʱ��:2016/3/23 15:35:14
	*/
	QString TipText();
	/*!
	���õ�ǰ��ʾ��Ϣ
	@param QString value Ҫ���õ���ʾ��Ϣ
	@return void
	���ߣ�cokkiy��������
	����ʱ��:2016/3/23 15:35:46
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
