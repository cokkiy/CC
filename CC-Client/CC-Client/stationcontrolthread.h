#ifndef STATIONCONTROLTHREAD_H
#define STATIONCONTROLTHREAD_H

#include <QObject>
#include "StationInfo.h"
#include "AppControlParameter.h"

class StationControlThread : public QObject
{
    Q_OBJECT

public slots:
    void startApp(StationInfo* s);

signals:
    void resultReady(const CC::AppStartingResults& results);    
    
};

#endif // STATIONCONTROLTHREAD_H
