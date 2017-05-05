#include "stationbar.h"
#include "StationInfo.h"
#include <QFrame>
#include <QBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

bool StationBar::IsOnline()
{
	return m_IsOnline;
}

void StationBar::setIsOnline(bool value)
{
	if (m_IsOnline != value)
	{
		m_IsOnline = value;
		if (m_IsOnline)
		{
			iconLabel->setText(stationName_on.arg(m_StationName));
		}
		else
		{
			iconLabel->setText(stationName_off.arg(m_StationName));
		}
	}
}

QString StationBar::StationName()
{
	return m_StationName;
}

void StationBar::setStationName(QString value)
{
	if (m_StationName != value)
	{
		m_StationName = value;
		if(m_IsOnline)
			iconLabel->setText(stationName_on.arg(m_StationName));
		else
		{
			iconLabel->setText(stationName_off.arg(m_StationName));
		}
	}
}

size_t StationBar::Percent()
{
	return m_Percent;
}

void StationBar::setPercent(size_t value)
{
	if (m_Percent != value)
	{
		m_Percent = value;
		int percent = value / (double)maxPercent * 100;
		progressBar->setValue(percent);
	}
}

QString StationBar::TipText()
{
	return m_TipText;
}

void StationBar::setTipText(QString value, bool error/* = false*/, bool showLogButton/*=false*/)
{
	if (m_TipText != value)
	{
		m_TipText = value;
		infoLabel->setText(value);
		if (error)
		{
			infoLabel->setStyleSheet(QStringLiteral("color: rgb(255, 0, 0);"));
		}
		else
		{
			infoLabel->setStyleSheet("");
		}

		if (showLogButton)
		{
			if (error)
			{
				logButton->setText(QStringLiteral("发生错误，查看日志"));
				logButton->setStyleSheet(QStringLiteral("color: rgb(255, 0, 0);"));
			}
			else
			{
				logButton->setText(QStringLiteral("查看发送日志"));
				logButton->setStyleSheet(QStringLiteral(""));
			}
		}
		else
		{
			logButton->setText("");
		}
	}
}

StationBar::StationBar(StationInfo* station, QWidget *parent)
	: QWidget(parent), maxPercent(100), station(station)
{
	QHBoxLayout* horizontalLayout = new QHBoxLayout(this);
	horizontalLayout->setSpacing(6);
	horizontalLayout->setContentsMargins(11, 11, 11, 11);
	//horizontalLayout->setObjectName(QStringLiteral("horizontalLayout_2"));
	iconLabel = new QLabel(this);
	//label->setObjectName(QStringLiteral("label_2"));
	//iconLabel->setText("<html><head/><body align=\"center\"><p><img src=\":/Icons/ncom003.ico\"/></p><p>\345\267\245\344\275\234\347\253\2311\345\257\271\345\257\271\345\257\271</p></body></html>");

	iconLabel->setTextFormat(Qt::AutoText);
	iconLabel->setWordWrap(true);

	iconLabel->setMaximumWidth(80);
	iconLabel->setMinimumWidth(80);

	horizontalLayout->addWidget(iconLabel);

	QVBoxLayout* verticalLayout = new QVBoxLayout();
	verticalLayout->setSpacing(6);
	//verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));

	QHBoxLayout* innerHLayout = new QHBoxLayout();
	infoLabel = new QLabel(this);
	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(infoLabel->sizePolicy().hasHeightForWidth());
	infoLabel->setSizePolicy(sizePolicy);
	innerHLayout->addWidget(infoLabel);

	logButton = new QPushButton(this);
	logButton->setFlat(true);
	logButton->setToolTip(QStringLiteral("查看文件发送日志信息"));
	connect(logButton, &QPushButton::clicked, this, &StationBar::on_logButtonClicked);
	innerHLayout->addWidget(logButton);
	verticalLayout->addLayout(innerHLayout);

	progressBar = new QProgressBar(this);
	progressBar->setMinimum(0);
	progressBar->setMaximum(100);
	progressBar->setValue(0);

	verticalLayout->addWidget(progressBar);

	horizontalLayout->addLayout(verticalLayout);
}

StationBar::~StationBar()
{
	delete iconLabel;
	delete infoLabel;
	delete progressBar;
}

void StationBar::setMaxPercent(size_t size)
{
	maxPercent = size;
}

void StationBar::on_logButtonClicked(bool checked)
{
	emit ViewLog(station);
}
