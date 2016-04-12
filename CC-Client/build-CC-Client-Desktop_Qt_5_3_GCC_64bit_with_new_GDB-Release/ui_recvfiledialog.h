/********************************************************************************
** Form generated from reading UI file 'recvfiledialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RECVFILEDIALOG_H
#define UI_RECVFILEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RecvFileDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *srcFileLineEdit;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *destLineEdit;
    QPushButton *browsePushButton;
    QPushButton *recvPushButton;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;

    void setupUi(QDialog *RecvFileDialog)
    {
        if (RecvFileDialog->objectName().isEmpty())
            RecvFileDialog->setObjectName(QStringLiteral("RecvFileDialog"));
        RecvFileDialog->resize(747, 465);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Icons/receiveFile.png"), QSize(), QIcon::Normal, QIcon::Off);
        RecvFileDialog->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(RecvFileDialog);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(RecvFileDialog);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        srcFileLineEdit = new QLineEdit(RecvFileDialog);
        srcFileLineEdit->setObjectName(QStringLiteral("srcFileLineEdit"));

        horizontalLayout->addWidget(srcFileLineEdit);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(RecvFileDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        destLineEdit = new QLineEdit(RecvFileDialog);
        destLineEdit->setObjectName(QStringLiteral("destLineEdit"));

        horizontalLayout_2->addWidget(destLineEdit);

        browsePushButton = new QPushButton(RecvFileDialog);
        browsePushButton->setObjectName(QStringLiteral("browsePushButton"));

        horizontalLayout_2->addWidget(browsePushButton);

        recvPushButton = new QPushButton(RecvFileDialog);
        recvPushButton->setObjectName(QStringLiteral("recvPushButton"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/Icons/download.jpg"), QSize(), QIcon::Normal, QIcon::Off);
        recvPushButton->setIcon(icon1);

        horizontalLayout_2->addWidget(recvPushButton);


        verticalLayout->addLayout(horizontalLayout_2);

        scrollArea = new QScrollArea(RecvFileDialog);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 727, 385));
        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollArea);


        retranslateUi(RecvFileDialog);

        QMetaObject::connectSlotsByName(RecvFileDialog);
    } // setupUi

    void retranslateUi(QDialog *RecvFileDialog)
    {
        RecvFileDialog->setWindowTitle(QApplication::translate("RecvFileDialog", "\346\216\245\346\224\266\346\226\207\344\273\266\345\257\271\350\257\235\346\241\206", 0));
        label->setText(QApplication::translate("RecvFileDialog", "\350\246\201\346\216\245\346\224\266\347\232\204\346\226\207\344\273\266", 0));
        srcFileLineEdit->setPlaceholderText(QApplication::translate("RecvFileDialog", "\350\276\223\345\205\245\350\246\201\346\216\245\346\224\266\347\232\204\346\226\207\344\273\266\350\267\257\345\276\204\345\222\214\345\220\215\347\247\260", 0));
        label_2->setText(QApplication::translate("RecvFileDialog", "\345\255\230\346\224\276\344\275\215\347\275\256\357\274\232", 0));
        browsePushButton->setText(QApplication::translate("RecvFileDialog", "\351\200\211\346\213\251\344\275\215\347\275\256", 0));
        recvPushButton->setText(QApplication::translate("RecvFileDialog", "\345\274\200\345\247\213\346\216\245\346\224\266", 0));
    } // retranslateUi

};

namespace Ui {
    class RecvFileDialog: public Ui_RecvFileDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RECVFILEDIALOG_H
