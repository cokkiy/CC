#ifndef __PACKETANALYZEWINDOW_H__
#define __PACKETANALYZEWINDOW_H__


#include <QDialog>

namespace Ui {
class PacketAnalyzeWindow;
}
class QTimer;
class QProgressDialog;
class PacketGraph;
class PacketAnalyzeWindow : public QDialog {
  Q_OBJECT

public:
  explicit PacketAnalyzeWindow(QWidget *parent = nullptr,
                               Qt::WindowFlags f = Qt::WindowFlags());
  ~PacketAnalyzeWindow();

private:
  Ui::PacketAnalyzeWindow *ui;
  PacketGraph *packetgraph;
};



#endif // __PACKETANALYZEWINDOW_H__