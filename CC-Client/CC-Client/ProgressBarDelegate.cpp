#include "ProgressBarDelegate.h"
#include <QPainter>
#include <QTextOption>

ProgressBarDelegate::ProgressBarDelegate(QObject *parent)
{
	barWidth = 100;
	barHeight = 24;
}

void ProgressBarDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	
	if (index.isValid() && (index.column() == 3 || index.column() == 4))
	{
		if (option.state & QStyle::State_Selected)
			painter->fillRect(option.rect, option.palette.highlight());

		QTextOption txtOption;
		txtOption.setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
		float progressValue = index.model()->data(index, Qt::DisplayRole).toFloat();
		QString text = index.data().toString();
		if (!qIsNaN(progressValue))
		{
			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);
			painter->setPen(Qt::NoPen);
			if (progressValue < 75)
				painter->setBrush(Qt::green);
			else if (progressValue < 85)
			{
				painter->setBrush(QColor(qRgb(0xfd, 0x90, 0x00)));
			}
			else {
				painter->setBrush(Qt::red);
			}
			float width = (option.rect.width() - 2) / 100.0*progressValue;
			painter->drawRect(QRectF(option.rect.x(), option.rect.top() + 2, width, option.rect.height() - 2));
			painter->restore();

			text.append("%");
		}
		painter->drawText(option.rect, text, txtOption);
	}
}

QSize ProgressBarDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	return QSize(barWidth, barHeight);
}

void ProgressBarDelegate::setBarSize(int barHeight, int barWidth)
{
	this->barHeight = barHeight;
	this->barWidth = barWidth;
}
