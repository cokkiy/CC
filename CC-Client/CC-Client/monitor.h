#ifndef MONITOR_H
#define MONITOR_H

#include <QThread>

class Monitor : public QThread
{
    Q_OBJECT

public:
    Monitor(QObject *parent);
    ~Monitor();

private:
    
};

#endif // MONITOR_H
