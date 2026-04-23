#include "sysupgradewizard.h"
#include "selectupgrademethoddlg.h"
#include "sendupgradefiledialog.h"
#include "sysupgradedialog.h"

SysUpgradeWizard::SysUpgradeWizard(QWidget *parent):QWizard(parent) {
  addPage(new SelectUpgradeMethodDlg);
  addPage(new SendUpgradeFileDialog);
  addPage(new SysUpgradeDialog);
  setWizardStyle(QWizard::ModernStyle);
}
