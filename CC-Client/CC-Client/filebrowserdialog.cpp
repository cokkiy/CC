#include "filebrowserdialog.h"
#include "ui_filebrowserdialog.h"
#include "StationInfo.h"
#include <QDir>
#include <QSysInfo>
#include <QMessageBox>

FileBrowserDialog::FileBrowserDialog(StationInfo* station, bool Directory, QString dir, QWidget *parent)
	: station(station), QDialog(parent), Directory(Directory)
{
	ui = new Ui::FileBrowserDialog();
	ui->setupUi(this);
	getFileInfo(dir);
	dirIcon.addFile(QStringLiteral(":/Icons/dir.png"));
	fileIcon.addFile(QStringLiteral(":/Icons/file.png"));	
	connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &FileBrowserDialog::on_cellDoubleClicked);
}

FileBrowserDialog::~FileBrowserDialog()
{
	delete ui;
}

/*!
获取选择的文件或目录路径
@return QString 当前选择的文件或目录路径
作者：cokkiy（张立民）
创建时间:2016/4/14 8:38:52
*/
QString FileBrowserDialog::SelectedPath()
{
	return selectedPath;
}

void FileBrowserDialog::getFileInfo(QString path)
{
	if (station != NULL&&station->controlProxy != NULL)
	{
		CC::Path dir;
		dir.Name = path.toStdWString();
		station->controlProxy->begin_getFileInfo(dir,
			[=](const CC::FileInfos& infos)
		{	
			//正常
			int rowCount = ui->tableWidget->rowCount();
			for (int row = 0; row < rowCount; row++)
			{
				for (int coloum = 0; coloum < 4; coloum++)
				{
					QTableWidgetItem* item = ui->tableWidget->takeItem(row, coloum);
					if (item != NULL)
						delete item;
				}
			}
			ui->tableWidget->setRowCount(1);
			int row = 0;

			QTableWidgetItem* item = new QTableWidgetItem(dirIcon, QStringLiteral(".."));
			ui->tableWidget->setItem(row, 0, item);
			item = new QTableWidgetItem(QStringLiteral("目录"));
			ui->tableWidget->setItem(row, 1, item);
			item = new QTableWidgetItem(QStringLiteral(""));
			ui->tableWidget->setItem(row, 2, item);
			item = new QTableWidgetItem(QStringLiteral(""));
			ui->tableWidget->setItem(row, 3, item);
			row++;
			
			for (CC::FileInfo info : infos)
			{
				currentDir = QString::fromStdWString(info.Parent);
				ui->currentLabel->setText(QStringLiteral("当前位置：%1").arg(currentDir));
				ui->tableWidget->setRowCount(row + 1);
				QTableWidgetItem* item = new QTableWidgetItem(info.isDirectory ? dirIcon : fileIcon, QString::fromStdWString(info.Path));
				ui->tableWidget->setItem(row, 0, item);
				item = new QTableWidgetItem(info.isDirectory?QStringLiteral("目录"):QStringLiteral("文件"));
				ui->tableWidget->setItem(row, 1, item);
				item = new QTableWidgetItem(QString::fromStdWString(info.LastModify));
				ui->tableWidget->setItem(row, 2, item);
				item = new QTableWidgetItem(QString::fromStdWString(info.CreationTime));
				ui->tableWidget->setItem(row, 3, item);
				row++;
			}
		},
			[=](const Ice::Exception& ex)
		{
			//异常
			const CC::DirectoryAccessError* error = dynamic_cast<const CC::DirectoryAccessError*>(&ex);
			if (error != NULL)
			{
				ui->currentLabel->setText(QStringLiteral("%1:%2")
					.arg(QString::fromStdWString(error->Reason))
					.arg(QString::fromStdWString(error->DirName)));
			}
			else
			{
				ui->currentLabel->setText(ex.what());
			}
		});
	}
}

void FileBrowserDialog::on_cellDoubleClicked(int row, int column)
{
	QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();
	if (items.size() > 0 && items[1]->text() == QStringLiteral("目录"))
	{
		if (items[0]->text() == QStringLiteral(".."))
		{
			on_upPushButton_clicked();
		}
		else
		{
			on_downPushButton_clicked();
		}
	}
}

void FileBrowserDialog::on_cancelPushButton_clicked()
{
	this->reject();
}

void FileBrowserDialog::on_okPushButton_clicked()
{
	QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();
	if (items.size() > 0)
	{
		if (Directory)
		{
			if (items[1]->text() == QStringLiteral("目录") && items[0]->text() != QStringLiteral("..") )
			{
				if (!currentDir.endsWith("/") && !currentDir.endsWith("\\"))
				{
					currentDir = currentDir + "/";
				}
				selectedPath = currentDir + items[0]->text();
				
			}
			else
			{
				selectedPath = currentDir;
			}
			this->accept();
		}
		else
		{
			if (items[1]->text() == QStringLiteral("文件"))
			{
				if (!currentDir.endsWith("/") && !currentDir.endsWith("\\"))
				{
					currentDir = currentDir + "/";
				}
				selectedPath = currentDir + items[0]->text();
				this->accept();
			}
			else
			{
				QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("只能选择文件。"));
			}
		}
	}
	else
	{
		if (Directory)
		{
			selectedPath = currentDir;
			this->accept();
		}
		else
		{
			QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("必须选择一个文件或目录。"));
		}
	}
}

void FileBrowserDialog::on_upPushButton_clicked()
{
	if (currentDir.startsWith("/"))
	{
		if (currentDir != "/")
		{
			if (currentDir.endsWith("/"))
			{
				currentDir = currentDir.left(currentDir.length() - 1);
			}
			//Linux liked system
			int pos = currentDir.lastIndexOf("/");
			if (pos == 0)
			{
				//已经是最后一级了
				QString path = "/";
				getFileInfo(path);

			}
			else
			{
				QString path = currentDir.left(pos);
				getFileInfo(path);
			}
		}
	}
	else
	{
		// Windows liked system
		int pos = currentDir.lastIndexOf("\\");
		if (pos == currentDir.length() - 1)
		{
			//去掉最后的"\\"
			currentDir = currentDir.left(currentDir.length() - 1);
			pos = currentDir.lastIndexOf("\\");
		}
		if (pos != -1)
		{
			currentDir = currentDir.left(pos + 1);
			getFileInfo(currentDir);
		}
		else
		{
			//已经到磁盘盘符了
			currentDir = "/";
			ui->currentLabel->setText(QStringLiteral("当前位置：%1").arg(currentDir));
			getFileInfo(currentDir);
		}
	}
}

void FileBrowserDialog::on_downPushButton_clicked()
{
	QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();
	if (items.size() != 0)
	{
		if (items[1]->text() == QStringLiteral("目录"))
		{
			QString selectedPath = items[0]->text();
			if (currentDir.startsWith("/") && !selectedPath.endsWith(":\\"))
			{
				//Linux liked system
				if (!currentDir.endsWith("/"))
				{
					currentDir += "/";
				}
				QString path = currentDir + items[0]->text();
				currentDir = path;
				ui->currentLabel->setText(QStringLiteral("当前位置：%1").arg(currentDir));
				getFileInfo(path);
			}
			else
			{
				//windows system
				if (selectedPath.endsWith(":\\"))
				{
					currentDir = selectedPath;
					ui->currentLabel->setText(QStringLiteral("当前位置：%1").arg(currentDir));
					getFileInfo(currentDir);
				}
				else
				{
					if (!currentDir.endsWith("\\"))
					{
						currentDir += "\\";
					}
					QString path = currentDir + items[0]->text();
					currentDir = path;
					ui->currentLabel->setText(QStringLiteral("当前位置：%1").arg(currentDir));
					getFileInfo(path);
				}
			}
			
		}
	}
}

