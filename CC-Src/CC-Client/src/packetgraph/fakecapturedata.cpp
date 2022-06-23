#include "fakecapturedata.h"

FakeCaptureData::FakeCaptureData(QObject *parent) : QThread(parent) {}

void FakeCaptureData::run() {
  for (int i = 0; i < 100; i++) {
    msleep(10);
    emit progress(i);
  }
  emit progress(100);
}