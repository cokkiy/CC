#pragma once
#include <QAbstractItemDelegate>
#include <QTextOption>
class ProgressBarDelegate:public QAbstractItemDelegate
{
	Q_OBJECT	
public:
	ProgressBarDelegate(QObject *parent = 0);

	void paint(QPainter *painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const override;

	QSize sizeHint(const QStyleOptionViewItem &option,
		const QModelIndex &index) const override;

	public slots:
	void setBarSize(int barHeight,int barWidth);

private:
	int barHeight;
	int barWidth;
};

