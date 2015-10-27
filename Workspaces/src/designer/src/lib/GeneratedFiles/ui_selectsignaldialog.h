/********************************************************************************
** Form generated from reading UI file 'selectsignaldialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SELECTSIGNALDIALOG_H
#define UI_SELECTSIGNALDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SelectSignalDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QTreeWidget *signalList;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SelectSignalDialog)
    {
        if (SelectSignalDialog->objectName().isEmpty())
            SelectSignalDialog->setObjectName(QStringLiteral("SelectSignalDialog"));
        SelectSignalDialog->resize(514, 183);
        verticalLayout = new QVBoxLayout(SelectSignalDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        groupBox = new QGroupBox(SelectSignalDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        signalList = new QTreeWidget(groupBox);
        signalList->setObjectName(QStringLiteral("signalList"));
        signalList->setSortingEnabled(false);
        signalList->header()->setVisible(false);

        verticalLayout_2->addWidget(signalList);


        verticalLayout->addWidget(groupBox);

        buttonBox = new QDialogButtonBox(SelectSignalDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(SelectSignalDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SelectSignalDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SelectSignalDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(SelectSignalDialog);
    } // setupUi

    void retranslateUi(QDialog *SelectSignalDialog)
    {
        SelectSignalDialog->setWindowTitle(QApplication::translate("SelectSignalDialog", "Go to slot", 0));
        groupBox->setTitle(QApplication::translate("SelectSignalDialog", "Select signal", 0));
        QTreeWidgetItem *___qtreewidgetitem = signalList->headerItem();
        ___qtreewidgetitem->setText(1, QApplication::translate("SelectSignalDialog", "class", 0));
        ___qtreewidgetitem->setText(0, QApplication::translate("SelectSignalDialog", "signal", 0));
    } // retranslateUi

};

namespace Ui {
    class SelectSignalDialog: public Ui_SelectSignalDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SELECTSIGNALDIALOG_H
