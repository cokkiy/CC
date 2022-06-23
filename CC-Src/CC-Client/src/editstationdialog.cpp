#include "editstationdialog.h"
#include "filebrowserdialog.h"
#include "processdialog.h"
#include "ui_editstationdialog.h"
#include <QMessageBox>

using namespace std;

EditStationDialog::EditStationDialog(StationInfo *station,
                                     EditType editType /*= EditType::Edit*/,
                                     QWidget *parent /*= 0*/)
    : QDialog(parent) {
  ui = new Ui::EditStationDialog();
  ui->setupUi(this);

  //设置输入验证
  macRegExp.setPattern(
      QStringLiteral("((\\d|[a-f]|[A-F]){2}-){7}(\\d|[a-f]|[A-F]){2}"));
  ipRegExp.setPattern(
      QStringLiteral("(((?:(?:25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}"
                     "(?:25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))),)+"));
  macValidator.setRegExp(macRegExp);
  ipValidator.setRegExp(ipRegExp);
  ui->NIMACLineEdit->setValidator(&macValidator);
  ui->NIIPLineEdit->setValidator(&ipValidator);
  //设置标题
  if (editType == New) {
    this->setWindowTitle(QStringLiteral("新建工作站"));
  } else {
    this->setWindowTitle(QStringLiteral("编辑工作站-%1").arg(station->Name()));
  }
  this->station = station;

  //初始化显示内容
  initUI();
}

EditStationDialog::~EditStationDialog() { delete ui; }

void EditStationDialog::on_addNiPushButton_clicked() {
  AddNIConfigControl("", "");
}
//添加网卡配置控件
void EditStationDialog::AddNIConfigControl(QString mac, QString IPs) {
  QGroupBox *groupBox = new QGroupBox(QStringLiteral("网卡信息"));
  groupBox->setObjectName(QStringLiteral("NIGroupbox"));
  QHBoxLayout *hLayout1 = new QHBoxLayout();
  QHBoxLayout *hLayout2 = new QHBoxLayout();
  QLabel *label1 = new QLabel(QStringLiteral("MAC地址："));
  QLabel *label2 = new QLabel(QStringLiteral("IP（多个IP用逗号分隔)"));
  QLineEdit *macEdit = new QLineEdit();
  QLineEdit *ipEdit = new QLineEdit();
  macEdit->setText(mac);
  macEdit->setValidator(&macValidator);
  ipEdit->setText(IPs);
  ipEdit->setValidator(&ipValidator);
  hLayout1->addWidget(label1);
  hLayout1->addWidget(macEdit);
  hLayout2->addWidget(label2);
  hLayout2->addWidget(ipEdit);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addItem(hLayout1);
  layout->addItem(hLayout2);
  groupBox->setLayout(layout);
  ui->niVerticalLayout->addWidget(groupBox);
}

void EditStationDialog::on_removeNIPushButton_clicked() {
  int count = ui->niVerticalLayout->count();
  if (count > 3) {
    QLayoutItem *item = ui->niVerticalLayout->itemAt(count - 1);
    ui->niVerticalLayout->removeItem(item);
    item->widget()->setVisible(false);
    delete item;
  } else {
    QMessageBox::warning(NULL, QStringLiteral("错误"),
                         QStringLiteral("没有可以删除的网卡设置项"));
  }
}

//确定
void EditStationDialog::on_OKPushButton_clicked() {
  if (ui->nameLineEdit->text() == QStringLiteral("")) {
    QMessageBox::warning(NULL, QStringLiteral("错误"),
                         QStringLiteral("工作站名称不能为空"));
    return;
  }
  if (ui->NIMACLineEdit->text() == QStringLiteral("")) {
    QMessageBox::warning(NULL, QStringLiteral("错误"),
                         QStringLiteral("工作站MAC地址不能为空"));
    return;
  }
  //     if (ui->NIIPLineEdit->text() == QStringLiteral(""))
  //     {
  //         QMessageBox::warning(NULL, QStringLiteral("错误"),
  //         QStringLiteral("工作站IP地址不能为空\r\n至少要提供一个IP地址"));
  //         return;
  //     }
  //获取新值
  UpdateStation();
  this->done(QDialog::Accepted);
}
//取消
void EditStationDialog::on_CancelPushButton_clicked() {
  this->done(QDialog::Rejected);
}
//添加启动程序
void EditStationDialog::on_addStartAppPushButton_clicked() {
  if (ui->appNameLineEdit->text() == QStringLiteral("")) {
    QMessageBox::warning(NULL, QStringLiteral("错误"),
                         QStringLiteral("没有输入要启动程序名称."));
    return;
  }
  if (ui->appProcNameLineEdit->text() == QStringLiteral("")) {
    QString name = ui->appNameLineEdit->text();
    int index = name.indexOf(QRegExp("\\w:\\\\", Qt::CaseInsensitive));
    if (index != -1) {
      // win likes system
      int pos = name.lastIndexOf(QRegExp("\\\\\\w+"));
      if (pos != -1) {
        name = name.mid(pos + 1);
        if (name.endsWith(".exe", Qt::CaseInsensitive)) {
          name = name.mid(0, name.length() - 4);
        }
        ui->appProcNameLineEdit->setText(name);
      } else {
        ui->appProcNameLineEdit->setText(name);
      }
    } else {
      ui->appProcNameLineEdit->setText(ui->appNameLineEdit->text());
    }
  }
  int row = ui->startAppTableWidget->rowCount();
  ui->startAppTableWidget->setRowCount(row + 1);
  QTableWidgetItem *pathItem =
      new QTableWidgetItem(ui->appNameLineEdit->text());
  QTableWidgetItem *argItem =
      new QTableWidgetItem(ui->argumentLineEdit->text());
  QTableWidgetItem *procNameItem =
      new QTableWidgetItem(ui->appProcNameLineEdit->text());
  QTableWidgetItem *allowMultiInstanceItem = new QTableWidgetItem();
  allowMultiInstanceItem->setCheckState(
      ui->allowMultiInstanceCheckBox->checkState());
  ui->startAppTableWidget->setItem(row, 0, pathItem);
  ui->startAppTableWidget->setItem(row, 1, argItem);
  ui->startAppTableWidget->setItem(row, 2, procNameItem);
  ui->startAppTableWidget->setItem(row, 3, allowMultiInstanceItem);
  ui->appNameLineEdit->setText("");
  ui->argumentLineEdit->setText("");
  ui->appProcNameLineEdit->setText("");
}
//删除启动程序
void EditStationDialog::on_removeStartAppPushButton_clicked() {
  auto index = ui->startAppTableWidget->currentIndex();
  if (index.isValid()) {
    ui->startAppTableWidget->removeRow(index.row());
  } else {
    QMessageBox::warning(NULL, QStringLiteral("错误"),
                         QStringLiteral("没有选择要删除的启动程序."));
  }
}
//添加监视进程
void EditStationDialog::on_addMonitorProcPushButton_clicked() {
  if (ui->procNameLineEdit->text() == QStringLiteral("")) {
    QMessageBox::warning(NULL, QStringLiteral("错误"),
                         QStringLiteral("没有输入要监视的进程名称."));
    return;
  }
  ui->monitorListWidget->addItem(ui->procNameLineEdit->text());
  ui->procNameLineEdit->setText("");
}

//删除监视进程
void EditStationDialog::on_removeMonitorProcPushButton_clicked() {
  auto index = ui->monitorListWidget->currentIndex();
  if (index.isValid()) {
    ui->monitorListWidget->takeItem(index.row());
  } else {
    QMessageBox::warning(NULL, QStringLiteral("错误"),
                         QStringLiteral("没有选择要删除的监视进程."));
  }
}

//为监视进程选择当前运行进程
void EditStationDialog::on_selectViewProcPushButton_clicked() {
  ProcessDialog dlg(station);
  if (dlg.exec() == QDialog::Accepted) {
    ui->procNameLineEdit->setText(dlg.ProcessName());
  }
}
//为添加启动程序选择当前运行进程
void EditStationDialog::on_selectProcPushButton_clicked() {
  ProcessDialog dlg(station);
  if (dlg.exec() == QDialog::Accepted) {
    ui->appProcNameLineEdit->setText(dlg.ProcessName());
  }
}

//选择启动程序路径
void EditStationDialog::on_selecApptPushButton_clicked() {
  FileBrowserDialog dlg(station, false);
  if (dlg.exec() == QDialog::Accepted) {
    QString path = dlg.SelectedPath();
    if (path.endsWith(".sh")) {
      ui->appNameLineEdit->setText("/bin/sh");
      ui->argumentLineEdit->setText(path);
    } else {
      ui->appNameLineEdit->setText(path);
    }
  }
}

//更新工作站数据
void EditStationDialog::UpdateStation() {
  station->setName(ui->nameLineEdit->text());
  station->clearStartApp();
  for (int i = 0; i < ui->startAppTableWidget->rowCount(); i++) {
    station->addStartApp(ui->startAppTableWidget->item(i, 0)->text(),
                         ui->startAppTableWidget->item(i, 1)->text(),
                         ui->startAppTableWidget->item(i, 2)->text(),
                         ui->startAppTableWidget->item(i, 3)->checkState() ==
                             Qt::Checked);
  }
  station->clearStandAloneMonitorProc();
  for (int i = 0; i < ui->monitorListWidget->count(); i++) {
    station->addStandAloneMonitorProcess(
        ui->monitorListWidget->item(i)->text());
  }
  station->NetworkIntefaces.clear();
  QStringList ips =
      ui->NIIPLineEdit->text().split(",", QString::SkipEmptyParts);
  NetworkInterface ni(ui->NIMACLineEdit->text(), ips);
  station->NetworkIntefaces.push_back(ni);
  int count = ui->niVerticalLayout->count();
  if (count > 3) {
    for (int i = 3; i < count; i++) {
      QLayoutItem *item = ui->niVerticalLayout->itemAt(i);
      auto edits = item->widget()->findChildren<QLineEdit *>();
      if (edits[0]->text() != QStringLiteral("")) {
        ips = edits[1]->text().split(",", QString::SkipEmptyParts);
        NetworkInterface ni(edits[0]->text(), ips);
        station->NetworkIntefaces.push_back(ni);
      }
    }
  }
  station->UpdateStation();
}

//初始化UI
void EditStationDialog::initUI() {
  if (station != NULL) {
    ui->nameLineEdit->setText(station->Name());
    if (station->NetworkIntefaces.begin() != station->NetworkIntefaces.end()) {
      auto first = station->NetworkIntefaces.begin();
      ui->NIMACLineEdit->setText(first->getMAC());
      ui->NIIPLineEdit->setText(first->getAllIPString());
      first++;
      for (auto iter = first; iter != station->NetworkIntefaces.end(); iter++) {
        AddNIConfigControl(iter->getMAC(), iter->getAllIPString());
      }
    }
    auto appList = station->getStartAppNames();
    ui->startAppTableWidget->setRowCount(appList.size());
    int row = 0;
    for (auto &app : appList) {
      QTableWidgetItem *pathItem =
          new QTableWidgetItem(QString::fromStdWString(app.AppPath));
      ui->startAppTableWidget->setItem(row, 0, pathItem);
      QTableWidgetItem *argItem =
          new QTableWidgetItem(QString::fromStdString(app.Arguments));
      ui->startAppTableWidget->setItem(row, 1, argItem);
      QTableWidgetItem *procNameItem =
          new QTableWidgetItem(QString::fromStdString(app.ProcessName));
      ui->startAppTableWidget->setItem(row, 2, procNameItem);
      QTableWidgetItem *allowMultiInstanceItem = new QTableWidgetItem();
      allowMultiInstanceItem->setCheckState(
          app.AllowMultiInstance ? Qt::Checked : Qt::Unchecked);
      ui->startAppTableWidget->setItem(row, 3, allowMultiInstanceItem);
      row++;
    }

    ui->monitorListWidget->addItems(station->getStandAloneMonitorProcNames());
  }
}
