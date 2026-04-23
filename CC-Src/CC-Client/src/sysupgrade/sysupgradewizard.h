#ifndef SYSUPGRADEWIZARD_H
#define SYSUPGRADEWIZARD_H

#include <QWizard>
#include <QObject>

class SysUpgradeWizard : public QWizard
{
public:
    explicit SysUpgradeWizard(QWidget *parent=nullptr);
};

#endif // SYSUPGRADEWIZARD_H
