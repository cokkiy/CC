#ifndef TASKINSTANCE_H
#define TASKINSTANCE_H
#include "net_global.h"
#include "abstracttask.h"
#include <QString>
class NETSHARED_EXPORT TaskInstance : public TaskInterface
{
    Q_OBJECT
public:
    TaskInstance();
    virtual QString currentTaskPath();
    virtual QString currentTaskCode();
    virtual int load(QString dir);
private:
    QString m_currentTaskPath;
    QString m_currentTaskCode;
};

#endif // TASKINSTANCE_H
