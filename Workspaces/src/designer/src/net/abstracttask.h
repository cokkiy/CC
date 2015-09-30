#ifndef TASKINTERFACE_H
#define TASKINTERFACE_H
#include "net_global.h"
#include <QObject>
class NETSHARED_EXPORT TaskInterface : public QObject
{
    Q_OBJECT
public:
    TaskInterface(QObject *parent = 0);
    /*!
      获取当前任务的目录
      @return QString  当前任务的目录
    */
    virtual QString currentTaskPath() = 0;
    /*!
      获取当前任务的代号,
      @return QString  当前任务的代号
    */
    virtual QString currentTaskCode() = 0;
};

#endif // TASKINTERFACE_H
