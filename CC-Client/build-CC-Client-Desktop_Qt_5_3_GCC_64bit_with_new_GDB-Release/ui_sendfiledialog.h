/********************************************************************************
** Form generated from reading UI file 'sendfiledialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SENDFILEDIALOG_H
#define UI_SENDFILEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SendFileDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QPlainTextEdit *sendFileNamesPlainTextEdit;
    QPushButton *browserPushButton;
    QPushButton *sendPushButton;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *destLineEdit;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;

    void setupUi(QDialog *SendFileDialog)
    {
        if (SendFileDialog->objectName().isEmpty())
            SendFileDialog->setObjectName(QStringLiteral("SendFileDialog"));
        SendFileDialog->resize(732, 465);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Icons/sendFile.png"), QSize(), QIcon::Normal, QIcon::Off);
        SendFileDialog->setWindowIcon(icon);
        SendFileDialog->setSizeGripEnabled(true);
        verticalLayout = new QVBoxLayout(SendFileDialog);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(SendFileDialog);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        sendFileNamesPlainTextEdit = new QPlainTextEdit(SendFileDialog);
        sendFileNamesPlainTextEdit->setObjectName(QStringLiteral("sendFileNamesPlainTextEdit"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(sendFileNamesPlainTextEdit->sizePolicy().hasHeightForWidth());
        sendFileNamesPlainTextEdit->setSizePolicy(sizePolicy);
        sendFileNamesPlainTextEdit->setMaximumSize(QSize(16777215, 24));
        sendFileNamesPlainTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        sendFileNamesPlainTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        sendFileNamesPlainTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
        sendFileNamesPlainTextEdit->setReadOnly(true);

        horizontalLayout->addWidget(sendFileNamesPlainTextEdit);

        browserPushButton = new QPushButton(SendFileDialog);
        browserPushButton->setObjectName(QStringLiteral("browserPushButton"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/Icons/search.ico"), QSize(), QIcon::Normal, QIcon::Off);
        browserPushButton->setIcon(icon1);

        horizontalLayout->addWidget(browserPushButton);

        sendPushButton = new QPushButton(SendFileDialog);
        sendPushButton->setObjectName(QStringLiteral("sendPushButton"));
        sendPushButton->setEnabled(false);
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/Icons/52design.com_jingying_059.png"), QSize(), QIcon::Normal, QIcon::Off);
        sendPushButton->setIcon(icon2);

        horizontalLayout->addWidget(sendPushButton);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(SendFileDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy1);
        label_2->setMinimumSize(QSize(95, 0));

        horizontalLayout_2->addWidget(label_2);

        destLineEdit = new QLineEdit(SendFileDialog);
        destLineEdit->setObjectName(QStringLiteral("destLineEdit"));

        horizontalLayout_2->addWidget(destLineEdit);


        verticalLayout->addLayout(horizontalLayout_2);

        scrollArea = new QScrollArea(SendFileDialog);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 712, 385));
        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollArea);


        retranslateUi(SendFileDialog);

        QMetaObject::connectSlotsByName(SendFileDialog);
    } // setupUi

    void retranslateUi(QDialog *SendFileDialog)
    {
        SendFileDialog->setWindowTitle(QApplication::translate("SendFileDialog", "\345\217\221\351\200\201\346\226\207\344\273\266\345\257\271\350\257\235\346\241\206", 0));
        label->setText(QApplication::translate("SendFileDialog", "<html><head/><body><p>\351\200\211\346\213\251\350\246\201\345\217\221\351\200\201\347\232\204\346\226\207\344\273\266</p></body></html>", 0));
        browserPushButton->setText(QApplication::translate("SendFileDialog", "\351\200\211\346\213\251\346\226\207\344\273\266...", 0));
        sendPushButton->setText(QApplication::translate("SendFileDialog", "\345\217\221\351\200\201", 0));
        label_2->setText(QApplication::translate("SendFileDialog", "\347\233\256\346\240\207\346\226\207\344\273\266\344\275\215\347\275\256\357\274\232", 0));
        destLineEdit->setPlaceholderText(QApplication::translate("SendFileDialog", "\350\276\223\345\205\245\346\226\207\344\273\266\345\234\250\345\267\245\344\275\234\347\253\231\344\270\212\347\232\204\344\277\235\345\255\230\344\275\215\347\275\256\357\274\214\344\270\215\350\203\275\344\270\272\347\251\272", 0));
    } // retranslateUi

};

namespace Ui {
    class SendFileDialog: public Ui_SendFileDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SENDFILEDIALOG_H
