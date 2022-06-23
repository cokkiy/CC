#ifndef FAKECAPTUREDATA_H
#define FAKECAPTUREDATA_H

#include <QThread>

class FakeCaptureData : public QThread
{
    Q_OBJECT
public:
  FakeCaptureData(QObject *parent = nullptr);
  virtual void run() override;
signals:
  void progress(int percent);
};

#endif // FAKECAPTUREDATA_H
