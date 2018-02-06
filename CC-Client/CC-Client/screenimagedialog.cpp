#include "screenimagedialog.h"
#include "ui_screenimagedialog.h"
#include "StationInfo.h"
#include <Ice/Exception.h>
#include <QPainter>
#include <QPen>
#include <QImage>
#include <QFileDialog>
#include <QMessageBox>


ScreenImageDialog::ScreenImageDialog(StationInfo* station, QWidget *parent)
	: QDialog(parent), station(station)
{
	ui = new Ui::ScreenImageDialog();
	ui->setupUi(this);
	this->setWindowFlags(Qt::Window | Qt::WindowMaximizeButtonHint | Qt::WindowFullscreenButtonHint | Qt::WindowCloseButtonHint);
	this->setWindowTitle(station->Name() + QStringLiteral(" - 屏幕快照"));
	ui->captureToolButton->setEnabled(false);
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
		this->update();
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
		ui->captureToolButton->setEnabled(true);
	}
	else if (finished)
	{
		QImage image;
		image.loadFromData(&buffer[0], buffer.size());
		
		//绘制图像
		painter.drawImage(this->rect(), image);

		//绘制一个红边
		QPen pen(Qt::red);
		pen.setWidth(3);
		painter.setPen(pen);
		painter.drawRect(this->rect());
		ui->captureToolButton->setEnabled(true);
	}
}

void ScreenImageDialog::mouseDoubleClickEvent(QMouseEvent * event)
{
	ui->frame->setVisible(!ui->frame->isVisible());
}

void ScreenImageDialog::on_saveToolButton_clicked()
{
	if (finished)
	{
		QFileDialog dlg;
		dlg.setDirectory(QDir::home());
		dlg.setNameFilter(tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
		dlg.setAcceptMode(QFileDialog::AcceptSave);
		dlg.setConfirmOverwrite(true);
		dlg.setFileMode(QFileDialog::AnyFile);
		if (dlg.exec() == QDialog::Accepted)
		{
			QImage image;
			image.loadFromData(&buffer[0], buffer.size());
			QString fileName = dlg.selectedFiles()[0];
			if (fileName.endsWith(".jpg")|| fileName.endsWith(".jpeg"))
			{
				image.save(dlg.selectedFiles()[0], "png");
			}
			else if (fileName.endsWith(".bmp"))
			{
				image.save(dlg.selectedFiles()[0], "bmp");
			}
			else
			{
				image.save(dlg.selectedFiles()[0], "png");
			}	
		}
	}
	else
	{
		QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("没有可以保存的图像。"));
	}
}

void ScreenImageDialog::on_captureToolButton_clicked()
{
//	QPainter painter(this);
//	QBrush bk(Qt::black);
//	painter.setBrush(bk);
//	painter.drawRect(this->rect());
//	QPen pen(Qt::red);
//	painter.setPen(pen);
//	painter.drawText(this->rect(), QStringLiteral("开始重新捕获屏幕..."));
	ui->captureToolButton->setEnabled(false);
	Ice::Long position = 0;
	finished = false;
	errorOccured = false;
	buffer.clear();
	getCaptureData(station, position);
}

