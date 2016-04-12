#include "screenimagedialog.h"
#include "ui_screenimagedialog.h"
#include "StationInfo.h"
#include <Ice/Exception.h>
#include <QPainter>
#include <QPen>
#include <QImage>


ScreenImageDialog::ScreenImageDialog(StationInfo* station, QWidget *parent)
	: QDialog(parent)
{
	ui = new Ui::ScreenImageDialog();
	ui->setupUi(this);
    this->setWindowFlags(Qt::Window| Qt::WindowMaximizeButtonHint|Qt::WindowFullscreenButtonHint|Qt::WindowCloseButtonHint);
	this->setWindowTitle(station->Name() + QStringLiteral(" - 屏幕快照"));
	Ice::Long position = 0;
	getCaptureData(station, position);

}

void ScreenImageDialog::getCaptureData(StationInfo* station, Ice::Long position)
{
	station->controlProxy->begin_captureScreen(position,
		[=](bool eof, int length, const CC::ByteArray& data)
	{
		buffer.insert(buffer.end(), data.begin(), data.end());
		if (!eof)
		{
			getCaptureData(station, position + length);
		}
		else
		{
			finished = true;
			this->update();
		}
	},
		[=](const Ice::Exception& ex)
	{
		errorOccured = true;
		const CC::FileTransException* e = dynamic_cast<const CC::FileTransException*>(&ex);
		if (e != NULL)
		{
			message = QString::fromStdWString(e->Message);
		}
		else
		{
			message = ex.what();
		}
	});
}

ScreenImageDialog::~ScreenImageDialog()
{
	delete ui;
}

//实现绘制
void ScreenImageDialog::paintEvent(QPaintEvent * e)
{
	QPainter painter(this);
	if (errorOccured)
	{
		QBrush bk(Qt::black);
		painter.setBrush(bk);
		painter.drawRect(this->rect());
		QPen pen(Qt::red);
		painter.setPen(pen);
		painter.drawText(this->rect(),QStringLiteral("发生错误：%1").arg(message));
		
	}
	else if (finished)
	{
		QImage image;
		image.loadFromData(&buffer[0], buffer.size());
		painter.drawImage(this->rect(), image);
	}
}

