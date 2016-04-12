/********************************************************************************
** Form generated from reading UI file 'screenimagedialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCREENIMAGEDIALOG_H
#define UI_SCREENIMAGEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_ScreenImageDialog
{
public:

    void setupUi(QDialog *ScreenImageDialog)
    {
        if (ScreenImageDialog->objectName().isEmpty())
            ScreenImageDialog->setObjectName(QStringLiteral("ScreenImageDialog"));
        ScreenImageDialog->setWindowModality(Qt::NonModal);
        ScreenImageDialog->resize(1024, 768);

        retranslateUi(ScreenImageDialog);

        QMetaObject::connectSlotsByName(ScreenImageDialog);
    } // setupUi

    void retranslateUi(QDialog *ScreenImageDialog)
    {
        ScreenImageDialog->setWindowTitle(QApplication::translate("ScreenImageDialog", "\345\261\217\345\271\225\345\277\253\347\205\247", 0));
    } // retranslateUi

};

namespace Ui {
    class ScreenImageDialog: public Ui_ScreenImageDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCREENIMAGEDIALOG_H
