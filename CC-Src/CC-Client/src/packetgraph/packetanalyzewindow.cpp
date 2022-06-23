#include "packetanalyzewindow.h"
#include "captureprogressdlg.h"
#include "packetgraph.h"
#include "ui_packetanalyzewindow.h"
#include <QProgressDialog>
#include <QTimer>

using namespace QtDataVisualization;

PacketAnalyzeWindow::PacketAnalyzeWindow(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f), ui(new Ui::PacketAnalyzeWindow) {
  ui->setupUi(this);

  Q3DBars *rainfall = new Q3DBars;
  if (!rainfall->hasContext()) {
    qWarning() << QStringLiteral("Couldn't initialize the OpenGL context.");
    return;
  }

  ui->barVerticalLayout->addWidget(QWidget::createWindowContainer(rainfall));

  packetgraph = new PacketGraph(rainfall);
  packetgraph->addDataSet();

  QTextStream stream;
  QFile dataFile(":/data/data.txt");
  if (dataFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    stream.setDevice(&dataFile);
    while (!stream.atEnd()) {
      QString line = stream.readLine();
      if (line.startsWith("#")) // Ignore comments
        continue;
      ui->listWidget->addItem(line);
    }
  }
}

PacketAnalyzeWindow::~PacketAnalyzeWindow() {
  delete ui;
  delete packetgraph;
}
