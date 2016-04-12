/********************************************************************************
** Form generated from reading UI file 'detaildialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DETAILDIALOG_H
#define UI_DETAILDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DetailDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *NameLineEdit;
    QLabel *label_7;
    QLineEdit *processCountLineEdit;
    QPushButton *powerOnPushButton;
    QPushButton *powerOffPushButton;
    QPushButton *rebootPushButton;
    QPushButton *screenCapturePushButton;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QLineEdit *IPLineEdit;
    QLabel *label_3;
    QLineEdit *MACLineEdit;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_4;
    QLineEdit *memorySizeLineEdit;
    QLabel *label_6;
    QLineEdit *CPURateLineEdit;
    QLabel *label_5;
    QLineEdit *memoryRateLineEdit;
    QSplitter *splitter;
    QFrame *plotFrame;
    QVBoxLayout *plotVerticalLayout;
    QFrame *processFrame;
    QVBoxLayout *verticalLayout_3;
    QTableWidget *processTableWidget;

    void setupUi(QDialog *DetailDialog)
    {
        if (DetailDialog->objectName().isEmpty())
            DetailDialog->setObjectName(QStringLiteral("DetailDialog"));
        DetailDialog->resize(937, 587);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(DetailDialog->sizePolicy().hasHeightForWidth());
        DetailDialog->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Icons/detail.png"), QSize(), QIcon::Normal, QIcon::Off);
        DetailDialog->setWindowIcon(icon);
        DetailDialog->setSizeGripEnabled(false);
        verticalLayout = new QVBoxLayout(DetailDialog);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label = new QLabel(DetailDialog);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_2->addWidget(label);

        NameLineEdit = new QLineEdit(DetailDialog);
        NameLineEdit->setObjectName(QStringLiteral("NameLineEdit"));
        NameLineEdit->setMinimumSize(QSize(300, 0));

        horizontalLayout_2->addWidget(NameLineEdit);

        label_7 = new QLabel(DetailDialog);
        label_7->setObjectName(QStringLiteral("label_7"));

        horizontalLayout_2->addWidget(label_7);

        processCountLineEdit = new QLineEdit(DetailDialog);
        processCountLineEdit->setObjectName(QStringLiteral("processCountLineEdit"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(processCountLineEdit->sizePolicy().hasHeightForWidth());
        processCountLineEdit->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(processCountLineEdit);

        powerOnPushButton = new QPushButton(DetailDialog);
        powerOnPushButton->setObjectName(QStringLiteral("powerOnPushButton"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/Icons/52design.com_jingying_059.png"), QSize(), QIcon::Normal, QIcon::Off);
        powerOnPushButton->setIcon(icon1);

        horizontalLayout_2->addWidget(powerOnPushButton);

        powerOffPushButton = new QPushButton(DetailDialog);
        powerOffPushButton->setObjectName(QStringLiteral("powerOffPushButton"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/Icons/52design.com_jingying_108.png"), QSize(), QIcon::Normal, QIcon::Off);
        powerOffPushButton->setIcon(icon2);

        horizontalLayout_2->addWidget(powerOffPushButton);

        rebootPushButton = new QPushButton(DetailDialog);
        rebootPushButton->setObjectName(QStringLiteral("rebootPushButton"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/Icons/2009724113238761.png"), QSize(), QIcon::Normal, QIcon::Off);
        rebootPushButton->setIcon(icon3);

        horizontalLayout_2->addWidget(rebootPushButton);

        screenCapturePushButton = new QPushButton(DetailDialog);
        screenCapturePushButton->setObjectName(QStringLiteral("screenCapturePushButton"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(screenCapturePushButton->sizePolicy().hasHeightForWidth());
        screenCapturePushButton->setSizePolicy(sizePolicy2);
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/Icons/screencapture.png"), QSize(), QIcon::Normal, QIcon::Off);
        screenCapturePushButton->setIcon(icon4);

        horizontalLayout_2->addWidget(screenCapturePushButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_2 = new QLabel(DetailDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout->addWidget(label_2);

        IPLineEdit = new QLineEdit(DetailDialog);
        IPLineEdit->setObjectName(QStringLiteral("IPLineEdit"));

        horizontalLayout->addWidget(IPLineEdit);

        label_3 = new QLabel(DetailDialog);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout->addWidget(label_3);

        MACLineEdit = new QLineEdit(DetailDialog);
        MACLineEdit->setObjectName(QStringLiteral("MACLineEdit"));

        horizontalLayout->addWidget(MACLineEdit);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_4 = new QLabel(DetailDialog);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_3->addWidget(label_4);

        memorySizeLineEdit = new QLineEdit(DetailDialog);
        memorySizeLineEdit->setObjectName(QStringLiteral("memorySizeLineEdit"));

        horizontalLayout_3->addWidget(memorySizeLineEdit);

        label_6 = new QLabel(DetailDialog);
        label_6->setObjectName(QStringLiteral("label_6"));

        horizontalLayout_3->addWidget(label_6);

        CPURateLineEdit = new QLineEdit(DetailDialog);
        CPURateLineEdit->setObjectName(QStringLiteral("CPURateLineEdit"));

        horizontalLayout_3->addWidget(CPURateLineEdit);

        label_5 = new QLabel(DetailDialog);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout_3->addWidget(label_5);

        memoryRateLineEdit = new QLineEdit(DetailDialog);
        memoryRateLineEdit->setObjectName(QStringLiteral("memoryRateLineEdit"));

        horizontalLayout_3->addWidget(memoryRateLineEdit);


        verticalLayout->addLayout(horizontalLayout_3);

        splitter = new QSplitter(DetailDialog);
        splitter->setObjectName(QStringLiteral("splitter"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(splitter->sizePolicy().hasHeightForWidth());
        splitter->setSizePolicy(sizePolicy3);
        splitter->setOrientation(Qt::Horizontal);
        splitter->setOpaqueResize(false);
        plotFrame = new QFrame(splitter);
        plotFrame->setObjectName(QStringLiteral("plotFrame"));
        plotFrame->setMinimumSize(QSize(300, 0));
        plotFrame->setFrameShape(QFrame::StyledPanel);
        plotFrame->setFrameShadow(QFrame::Raised);
        plotVerticalLayout = new QVBoxLayout(plotFrame);
        plotVerticalLayout->setSpacing(6);
        plotVerticalLayout->setContentsMargins(11, 11, 11, 11);
        plotVerticalLayout->setObjectName(QStringLiteral("plotVerticalLayout"));
        splitter->addWidget(plotFrame);
        processFrame = new QFrame(splitter);
        processFrame->setObjectName(QStringLiteral("processFrame"));
        processFrame->setFrameShape(QFrame::StyledPanel);
        processFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_3 = new QVBoxLayout(processFrame);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        processTableWidget = new QTableWidget(processFrame);
        if (processTableWidget->columnCount() < 8)
            processTableWidget->setColumnCount(8);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        processTableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        processTableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        processTableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        processTableWidget->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        processTableWidget->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        processTableWidget->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        processTableWidget->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        processTableWidget->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        processTableWidget->setObjectName(QStringLiteral("processTableWidget"));

        verticalLayout_3->addWidget(processTableWidget);

        splitter->addWidget(processFrame);

        verticalLayout->addWidget(splitter);


        retranslateUi(DetailDialog);

        QMetaObject::connectSlotsByName(DetailDialog);
    } // setupUi

    void retranslateUi(QDialog *DetailDialog)
    {
        DetailDialog->setWindowTitle(QApplication::translate("DetailDialog", "\345\267\245\344\275\234\347\253\231\350\257\246\347\273\206\344\277\241\346\201\257", 0));
        label->setText(QApplication::translate("DetailDialog", "\344\270\273\346\234\272\345\220\215\357\274\232", 0));
        label_7->setText(QApplication::translate("DetailDialog", "\350\277\233\347\250\213\346\200\273\346\225\260", 0));
        powerOnPushButton->setText(QApplication::translate("DetailDialog", "\345\274\200\346\234\272", 0));
        powerOffPushButton->setText(QApplication::translate("DetailDialog", "\345\205\263\346\234\272", 0));
        rebootPushButton->setText(QApplication::translate("DetailDialog", "\351\207\215\345\220\257", 0));
        screenCapturePushButton->setText(QApplication::translate("DetailDialog", "\345\261\217\345\271\225\345\277\253\347\205\247", 0));
        label_2->setText(QApplication::translate("DetailDialog", "IP\345\234\260\345\235\200\357\274\232", 0));
        label_3->setText(QApplication::translate("DetailDialog", "MAC\345\234\260\345\235\200\357\274\232", 0));
        label_4->setText(QApplication::translate("DetailDialog", "\345\206\205\345\255\230\345\244\247\345\260\217\357\274\210MB\357\274\211\357\274\232", 0));
        label_6->setText(QApplication::translate("DetailDialog", "CPU\345\215\240\347\224\250\347\216\207\357\274\210%\357\274\211", 0));
        label_5->setText(QApplication::translate("DetailDialog", "\345\206\205\345\255\230\345\215\240\347\224\250\347\216\207(%)", 0));
        QTableWidgetItem *___qtablewidgetitem = processTableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("DetailDialog", "\350\277\233\347\250\213ID", 0));
        QTableWidgetItem *___qtablewidgetitem1 = processTableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("DetailDialog", "\350\277\233\347\250\213\345\220\215", 0));
        QTableWidgetItem *___qtablewidgetitem2 = processTableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("DetailDialog", "\346\230\257\345\220\246\350\277\220\350\241\214", 0));
        QTableWidgetItem *___qtablewidgetitem3 = processTableWidget->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("DetailDialog", "CPU\345\215\240\347\224\250\347\216\207", 0));
        QTableWidgetItem *___qtablewidgetitem4 = processTableWidget->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QApplication::translate("DetailDialog", "\345\206\205\345\255\230\345\215\240\347\224\250\347\216\207", 0));
        QTableWidgetItem *___qtablewidgetitem5 = processTableWidget->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QApplication::translate("DetailDialog", "\347\272\277\347\250\213\346\225\260", 0));
        QTableWidgetItem *___qtablewidgetitem6 = processTableWidget->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QApplication::translate("DetailDialog", "\345\220\257\345\212\250\350\267\257\345\276\204", 0));
        QTableWidgetItem *___qtablewidgetitem7 = processTableWidget->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QApplication::translate("DetailDialog", "\345\217\202\346\225\260", 0));
    } // retranslateUi

};

namespace Ui {
    class DetailDialog: public Ui_DetailDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DETAILDIALOG_H
