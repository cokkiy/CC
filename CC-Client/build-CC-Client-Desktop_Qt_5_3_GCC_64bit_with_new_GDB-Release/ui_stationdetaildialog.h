/********************************************************************************
** Form generated from reading UI file 'stationdetaildialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STATIONDETAILDIALOG_H
#define UI_STATIONDETAILDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qwt_plot.h"
#include "qwt_thermo.h"

QT_BEGIN_NAMESPACE

class Ui_StationDetailDialog
{
public:
    QHBoxLayout *horizontalLayout;
    QSplitter *splitter;
    QWidget *widget;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *nameLineEdit;
    QLabel *label_2;
    QLineEdit *IPLineEdit;
    QGroupBox *groupBox_6;
    QVBoxLayout *verticalLayout_6;
    QTableWidget *processTableWidget;
    QCheckBox *viewAllProcessesCheckBox;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_5;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout;
    QwtThermo *cpuThermo;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_3;
    QwtPlot *cpuQwtPlot;
    QHBoxLayout *horizontalLayout_6;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_2;
    QwtThermo *memoryThermo;
    QGroupBox *groupBox_5;
    QHBoxLayout *horizontalLayout_4;
    QwtPlot *memoryQwtPlot;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_5;
    QTextBrowser *stateTextBrowser;

    void setupUi(QDialog *StationDetailDialog)
    {
        if (StationDetailDialog->objectName().isEmpty())
            StationDetailDialog->setObjectName(QStringLiteral("StationDetailDialog"));
        StationDetailDialog->resize(1071, 732);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Icons/startApp1.png"), QSize(), QIcon::Normal, QIcon::Off);
        StationDetailDialog->setWindowIcon(icon);
        StationDetailDialog->setSizeGripEnabled(false);
        StationDetailDialog->setModal(false);
        horizontalLayout = new QHBoxLayout(StationDetailDialog);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        splitter = new QSplitter(StationDetailDialog);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        widget = new QWidget(splitter);
        widget->setObjectName(QStringLiteral("widget"));
        verticalLayout_4 = new QVBoxLayout(widget);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label = new QLabel(widget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_2->addWidget(label);

        nameLineEdit = new QLineEdit(widget);
        nameLineEdit->setObjectName(QStringLiteral("nameLineEdit"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(nameLineEdit->sizePolicy().hasHeightForWidth());
        nameLineEdit->setSizePolicy(sizePolicy);
        nameLineEdit->setReadOnly(true);

        horizontalLayout_2->addWidget(nameLineEdit);

        label_2 = new QLabel(widget);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        IPLineEdit = new QLineEdit(widget);
        IPLineEdit->setObjectName(QStringLiteral("IPLineEdit"));
        IPLineEdit->setReadOnly(true);

        horizontalLayout_2->addWidget(IPLineEdit);


        verticalLayout_4->addLayout(horizontalLayout_2);

        groupBox_6 = new QGroupBox(widget);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        groupBox_6->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        verticalLayout_6 = new QVBoxLayout(groupBox_6);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        processTableWidget = new QTableWidget(groupBox_6);
        if (processTableWidget->columnCount() < 9)
            processTableWidget->setColumnCount(9);
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
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        processTableWidget->setHorizontalHeaderItem(8, __qtablewidgetitem8);
        processTableWidget->setObjectName(QStringLiteral("processTableWidget"));
        processTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        processTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        processTableWidget->setShowGrid(false);
        processTableWidget->setSortingEnabled(true);

        verticalLayout_6->addWidget(processTableWidget);

        viewAllProcessesCheckBox = new QCheckBox(groupBox_6);
        viewAllProcessesCheckBox->setObjectName(QStringLiteral("viewAllProcessesCheckBox"));

        verticalLayout_6->addWidget(viewAllProcessesCheckBox);


        verticalLayout_4->addWidget(groupBox_6);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        groupBox_2 = new QGroupBox(widget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy1);
        groupBox_2->setMaximumSize(QSize(16777215, 180));
        groupBox_2->setAlignment(Qt::AlignCenter);
        verticalLayout = new QVBoxLayout(groupBox_2);
        verticalLayout->setSpacing(2);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(2, 2, 2, 2);
        cpuThermo = new QwtThermo(groupBox_2);
        cpuThermo->setObjectName(QStringLiteral("cpuThermo"));
        QSizePolicy sizePolicy2(QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(cpuThermo->sizePolicy().hasHeightForWidth());
        cpuThermo->setSizePolicy(sizePolicy2);
        cpuThermo->setScaleMaxMinor(100);
        cpuThermo->setScalePosition(QwtThermo::NoScale);
        cpuThermo->setAlarmLevel(85);
        cpuThermo->setSpacing(1);
        cpuThermo->setPipeWidth(60);

        verticalLayout->addWidget(cpuThermo);


        horizontalLayout_5->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(widget);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy3);
        groupBox_3->setMaximumSize(QSize(16777215, 180));
        horizontalLayout_3 = new QHBoxLayout(groupBox_3);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        cpuQwtPlot = new QwtPlot(groupBox_3);
        cpuQwtPlot->setObjectName(QStringLiteral("cpuQwtPlot"));
        QSizePolicy sizePolicy4(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(cpuQwtPlot->sizePolicy().hasHeightForWidth());
        cpuQwtPlot->setSizePolicy(sizePolicy4);

        horizontalLayout_3->addWidget(cpuQwtPlot);


        horizontalLayout_5->addWidget(groupBox_3);


        verticalLayout_3->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        groupBox_4 = new QGroupBox(widget);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        sizePolicy1.setHeightForWidth(groupBox_4->sizePolicy().hasHeightForWidth());
        groupBox_4->setSizePolicy(sizePolicy1);
        groupBox_4->setMaximumSize(QSize(16777215, 180));
        groupBox_4->setAlignment(Qt::AlignCenter);
        verticalLayout_2 = new QVBoxLayout(groupBox_4);
        verticalLayout_2->setSpacing(2);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(2, 2, 2, 2);
        memoryThermo = new QwtThermo(groupBox_4);
        memoryThermo->setObjectName(QStringLiteral("memoryThermo"));
        sizePolicy2.setHeightForWidth(memoryThermo->sizePolicy().hasHeightForWidth());
        memoryThermo->setSizePolicy(sizePolicy2);
        memoryThermo->setScaleMaxMinor(100);
        memoryThermo->setScalePosition(QwtThermo::NoScale);
        memoryThermo->setAlarmLevel(85);
        memoryThermo->setSpacing(1);
        memoryThermo->setPipeWidth(60);

        verticalLayout_2->addWidget(memoryThermo);


        horizontalLayout_6->addWidget(groupBox_4);

        groupBox_5 = new QGroupBox(widget);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        sizePolicy3.setHeightForWidth(groupBox_5->sizePolicy().hasHeightForWidth());
        groupBox_5->setSizePolicy(sizePolicy3);
        groupBox_5->setMaximumSize(QSize(16777215, 180));
        horizontalLayout_4 = new QHBoxLayout(groupBox_5);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        memoryQwtPlot = new QwtPlot(groupBox_5);
        memoryQwtPlot->setObjectName(QStringLiteral("memoryQwtPlot"));
        sizePolicy4.setHeightForWidth(memoryQwtPlot->sizePolicy().hasHeightForWidth());
        memoryQwtPlot->setSizePolicy(sizePolicy4);

        horizontalLayout_4->addWidget(memoryQwtPlot);


        horizontalLayout_6->addWidget(groupBox_5);


        verticalLayout_3->addLayout(horizontalLayout_6);


        verticalLayout_4->addLayout(verticalLayout_3);

        splitter->addWidget(widget);
        groupBox = new QGroupBox(splitter);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout_5 = new QVBoxLayout(groupBox);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        stateTextBrowser = new QTextBrowser(groupBox);
        stateTextBrowser->setObjectName(QStringLiteral("stateTextBrowser"));

        verticalLayout_5->addWidget(stateTextBrowser);

        splitter->addWidget(groupBox);

        horizontalLayout->addWidget(splitter);


        retranslateUi(StationDetailDialog);

        QMetaObject::connectSlotsByName(StationDetailDialog);
    } // setupUi

    void retranslateUi(QDialog *StationDetailDialog)
    {
        StationDetailDialog->setWindowTitle(QApplication::translate("StationDetailDialog", "%1\350\257\246\347\273\206\344\277\241\346\201\257", 0));
        label->setText(QApplication::translate("StationDetailDialog", "\345\220\215\347\247\260\357\274\232", 0));
        label_2->setText(QApplication::translate("StationDetailDialog", "IP:", 0));
        groupBox_6->setTitle(QApplication::translate("StationDetailDialog", "\350\277\233\347\250\213\344\277\241\346\201\257", 0));
        QTableWidgetItem *___qtablewidgetitem = processTableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("StationDetailDialog", "\345\220\215\347\247\260\357\274\210\350\277\233\347\250\213)", 0));
        QTableWidgetItem *___qtablewidgetitem1 = processTableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("StationDetailDialog", "PID", 0));
        QTableWidgetItem *___qtablewidgetitem2 = processTableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("StationDetailDialog", "CPU\357\274\210%)", 0));
        QTableWidgetItem *___qtablewidgetitem3 = processTableWidget->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("StationDetailDialog", "\345\206\205\345\255\230\357\274\210%)", 0));
        QTableWidgetItem *___qtablewidgetitem4 = processTableWidget->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QApplication::translate("StationDetailDialog", "\345\206\205\345\255\230\357\274\210M)", 0));
        QTableWidgetItem *___qtablewidgetitem5 = processTableWidget->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QApplication::translate("StationDetailDialog", "\347\272\277\347\250\213", 0));
        QTableWidgetItem *___qtablewidgetitem6 = processTableWidget->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QApplication::translate("StationDetailDialog", "\347\224\250\346\210\267\345\220\215", 0));
        QTableWidgetItem *___qtablewidgetitem7 = processTableWidget->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QApplication::translate("StationDetailDialog", "\346\217\217\350\277\260", 0));
        QTableWidgetItem *___qtablewidgetitem8 = processTableWidget->horizontalHeaderItem(8);
        ___qtablewidgetitem8->setText(QApplication::translate("StationDetailDialog", "\347\212\266\346\200\201", 0));
#ifndef QT_NO_TOOLTIP
        viewAllProcessesCheckBox->setToolTip(QApplication::translate("StationDetailDialog", "\344\270\215\351\200\211\345\217\252\346\230\276\347\244\272\347\233\221\350\247\206\350\277\233\347\250\213,\345\220\246\345\210\231\346\230\276\347\244\272\345\205\250\351\203\250\350\277\220\350\241\214\344\270\255\350\277\233\347\250\213\344\277\241\346\201\257", 0));
#endif // QT_NO_TOOLTIP
        viewAllProcessesCheckBox->setText(QApplication::translate("StationDetailDialog", "\346\230\276\347\244\272\345\205\250\351\203\250\350\277\233\347\250\213", 0));
        groupBox_2->setTitle(QApplication::translate("StationDetailDialog", "CPU\344\275\277\347\224\250\347\216\207", 0));
        groupBox_3->setTitle(QApplication::translate("StationDetailDialog", "CPU\344\275\277\347\224\250\350\256\260\345\275\225", 0));
        groupBox_4->setTitle(QApplication::translate("StationDetailDialog", "\345\206\205\345\255\230\344\275\277\347\224\250\347\216\207", 0));
        groupBox_5->setTitle(QApplication::translate("StationDetailDialog", "\345\206\205\345\255\230\344\275\277\347\224\250\350\256\260\345\275\225", 0));
        groupBox->setTitle(QApplication::translate("StationDetailDialog", "\347\212\266\346\200\201\345\217\230\345\214\226\350\256\260\345\275\225", 0));
    } // retranslateUi

};

namespace Ui {
    class StationDetailDialog: public Ui_StationDetailDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STATIONDETAILDIALOG_H
