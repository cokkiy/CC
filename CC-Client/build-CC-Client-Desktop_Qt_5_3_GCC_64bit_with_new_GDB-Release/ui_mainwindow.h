/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qwt_plot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionAllPower_On;
    QAction *actionAll_Power_On;
    QAction *actionShutdown;
    QAction *actionAll_Shutdown;
    QAction *actionExit;
    QAction *actionStartAllApp;
    QAction *actionExitAllZXApp;
    QAction *actionCPU;
    QAction *actionMemory;
    QAction *actionAppCPU;
    QAction *actionAppMemory;
    QAction *actionProcCount;
    QAction *actionAppThreadCount;
    QAction *actionPowerOn;
    QAction *actionPoweroff;
    QAction *actionReboot;
    QAction *actionStartApp;
    QAction *actionExitApp;
    QAction *actionRestartApp;
    QAction *actionViewCPU;
    QAction *actionViewMemory;
    QAction *actionViewDetail;
    QAction *actionViewMessage;
    QAction *actionLineCPU;
    QAction *actionPieCPU;
    QAction *actionRectCPU;
    QAction *actionLineMemory;
    QAction *actionViewLineMemory;
    QAction *actionPieMemory;
    QAction *actionRectMemory;
    QAction *actionRebootAll;
    QAction *actionRestartAll;
    QAction *actionRemove;
    QAction *actionEdit;
    QAction *actionAllowAutoRefreshList;
    QAction *actionSetInterval;
    QAction *actionNewStation;
    QAction *actionSetDefaultAppAndProc;
    QAction *actionViewStationDetail;
    QAction *actionSendAll;
    QAction *actionSendSelection;
    QAction *actionReceiveAll;
    QAction *actionReceiveSelection;
    QAction *actionScreenCapture;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout_3;
    QSplitter *splitter;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *filterLineEdit;
    QToolButton *filterToolButton;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_3;
    QComboBox *displayModeComboBox;
    QSpacerItem *horizontalSpacer;
    QLabel *label_2;
    QComboBox *sortComboBox;
    QListView *listView;
    QTableView *tableView;
    QFrame *rightFrame;
    QVBoxLayout *verticalLayout_4;
    QSplitter *splitter_2;
    QwtPlot *qwtPlotCPU;
    QwtPlot *qwtPlotMemory;
    QFrame *detailFrame;
    QHBoxLayout *horizontalLayout_5;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_4;
    QLineEdit *procCountlineEdit;
    QListWidget *msgListWidget;
    QMenuBar *menuBar;
    QMenu *menuControl;
    QMenu *menuOperation;
    QMenu *menuView;
    QMenu *menuOption;
    QMenu *menuFileTranslation;
    QMenu *menuSend;
    QMenu *menuReceive;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1024, 768);
        MainWindow->setMinimumSize(QSize(800, 600));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(12);
        MainWindow->setFont(font);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Icons/ncom003.ico"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        MainWindow->setStyleSheet(QStringLiteral(""));
        actionAllPower_On = new QAction(MainWindow);
        actionAllPower_On->setObjectName(QStringLiteral("actionAllPower_On"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/Icons/52design.com_jingying_108.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAllPower_On->setIcon(icon1);
        actionAll_Power_On = new QAction(MainWindow);
        actionAll_Power_On->setObjectName(QStringLiteral("actionAll_Power_On"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/Icons/52design.com_jingying_059.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAll_Power_On->setIcon(icon2);
        actionShutdown = new QAction(MainWindow);
        actionShutdown->setObjectName(QStringLiteral("actionShutdown"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/Icons/52design.com_jingying_098.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionShutdown->setIcon(icon3);
        actionAll_Shutdown = new QAction(MainWindow);
        actionAll_Shutdown->setObjectName(QStringLiteral("actionAll_Shutdown"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/Icons/App12.jpg"), QSize(), QIcon::Normal, QIcon::Off);
        actionAll_Shutdown->setIcon(icon4);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/Icons/52design.com_3d_08.png"), QSize(), QIcon::Normal, QIcon::On);
        actionExit->setIcon(icon5);
        actionStartAllApp = new QAction(MainWindow);
        actionStartAllApp->setObjectName(QStringLiteral("actionStartAllApp"));
        actionStartAllApp->setIcon(icon2);
        actionExitAllZXApp = new QAction(MainWindow);
        actionExitAllZXApp->setObjectName(QStringLiteral("actionExitAllZXApp"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/Icons/52design.com_alth_171.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionExitAllZXApp->setIcon(icon6);
        actionCPU = new QAction(MainWindow);
        actionCPU->setObjectName(QStringLiteral("actionCPU"));
        actionCPU->setCheckable(true);
        actionCPU->setChecked(true);
        actionMemory = new QAction(MainWindow);
        actionMemory->setObjectName(QStringLiteral("actionMemory"));
        actionMemory->setCheckable(true);
        actionMemory->setChecked(true);
        actionAppCPU = new QAction(MainWindow);
        actionAppCPU->setObjectName(QStringLiteral("actionAppCPU"));
        actionAppCPU->setCheckable(true);
        actionAppCPU->setChecked(true);
        actionAppMemory = new QAction(MainWindow);
        actionAppMemory->setObjectName(QStringLiteral("actionAppMemory"));
        actionAppMemory->setCheckable(true);
        actionAppMemory->setChecked(true);
        actionProcCount = new QAction(MainWindow);
        actionProcCount->setObjectName(QStringLiteral("actionProcCount"));
        actionProcCount->setCheckable(true);
        actionAppThreadCount = new QAction(MainWindow);
        actionAppThreadCount->setObjectName(QStringLiteral("actionAppThreadCount"));
        actionAppThreadCount->setCheckable(true);
        actionPowerOn = new QAction(MainWindow);
        actionPowerOn->setObjectName(QStringLiteral("actionPowerOn"));
        actionPowerOn->setIcon(icon1);
        actionPoweroff = new QAction(MainWindow);
        actionPoweroff->setObjectName(QStringLiteral("actionPoweroff"));
        actionPoweroff->setIcon(icon3);
        actionReboot = new QAction(MainWindow);
        actionReboot->setObjectName(QStringLiteral("actionReboot"));
        actionStartApp = new QAction(MainWindow);
        actionStartApp->setObjectName(QStringLiteral("actionStartApp"));
        actionStartApp->setIcon(icon2);
        actionExitApp = new QAction(MainWindow);
        actionExitApp->setObjectName(QStringLiteral("actionExitApp"));
        actionExitApp->setIcon(icon6);
        actionRestartApp = new QAction(MainWindow);
        actionRestartApp->setObjectName(QStringLiteral("actionRestartApp"));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/Icons/2009724113238761.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionRestartApp->setIcon(icon7);
        actionViewCPU = new QAction(MainWindow);
        actionViewCPU->setObjectName(QStringLiteral("actionViewCPU"));
        actionViewCPU->setCheckable(true);
        actionViewCPU->setChecked(true);
        actionViewMemory = new QAction(MainWindow);
        actionViewMemory->setObjectName(QStringLiteral("actionViewMemory"));
        actionViewMemory->setCheckable(true);
        actionViewMemory->setChecked(true);
        actionViewDetail = new QAction(MainWindow);
        actionViewDetail->setObjectName(QStringLiteral("actionViewDetail"));
        actionViewDetail->setCheckable(true);
        actionViewDetail->setChecked(true);
        actionViewMessage = new QAction(MainWindow);
        actionViewMessage->setObjectName(QStringLiteral("actionViewMessage"));
        actionViewMessage->setCheckable(true);
        actionViewMessage->setChecked(true);
        actionLineCPU = new QAction(MainWindow);
        actionLineCPU->setObjectName(QStringLiteral("actionLineCPU"));
        actionLineCPU->setCheckable(true);
        actionLineCPU->setChecked(true);
        actionPieCPU = new QAction(MainWindow);
        actionPieCPU->setObjectName(QStringLiteral("actionPieCPU"));
        actionPieCPU->setCheckable(true);
        actionRectCPU = new QAction(MainWindow);
        actionRectCPU->setObjectName(QStringLiteral("actionRectCPU"));
        actionRectCPU->setCheckable(true);
        actionLineMemory = new QAction(MainWindow);
        actionLineMemory->setObjectName(QStringLiteral("actionLineMemory"));
        actionViewLineMemory = new QAction(MainWindow);
        actionViewLineMemory->setObjectName(QStringLiteral("actionViewLineMemory"));
        actionViewLineMemory->setCheckable(true);
        actionViewLineMemory->setChecked(true);
        actionPieMemory = new QAction(MainWindow);
        actionPieMemory->setObjectName(QStringLiteral("actionPieMemory"));
        actionPieMemory->setCheckable(true);
        actionRectMemory = new QAction(MainWindow);
        actionRectMemory->setObjectName(QStringLiteral("actionRectMemory"));
        actionRectMemory->setCheckable(true);
        actionRebootAll = new QAction(MainWindow);
        actionRebootAll->setObjectName(QStringLiteral("actionRebootAll"));
        actionRestartAll = new QAction(MainWindow);
        actionRestartAll->setObjectName(QStringLiteral("actionRestartAll"));
        actionRestartAll->setIcon(icon7);
        actionRemove = new QAction(MainWindow);
        actionRemove->setObjectName(QStringLiteral("actionRemove"));
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/Icons/52design.com_3d_08.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionRemove->setIcon(icon8);
        actionEdit = new QAction(MainWindow);
        actionEdit->setObjectName(QStringLiteral("actionEdit"));
        QIcon icon9;
        icon9.addFile(QStringLiteral(":/Icons/TOOLS.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionEdit->setIcon(icon9);
        actionAllowAutoRefreshList = new QAction(MainWindow);
        actionAllowAutoRefreshList->setObjectName(QStringLiteral("actionAllowAutoRefreshList"));
        actionAllowAutoRefreshList->setCheckable(true);
        actionAllowAutoRefreshList->setChecked(true);
        actionSetInterval = new QAction(MainWindow);
        actionSetInterval->setObjectName(QStringLiteral("actionSetInterval"));
        QIcon icon10;
        icon10.addFile(QStringLiteral(":/Icons/52design.com_alth_023.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSetInterval->setIcon(icon10);
        actionNewStation = new QAction(MainWindow);
        actionNewStation->setObjectName(QStringLiteral("actionNewStation"));
        QIcon icon11;
        icon11.addFile(QStringLiteral(":/Icons/people_officee_o.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionNewStation->setIcon(icon11);
        actionSetDefaultAppAndProc = new QAction(MainWindow);
        actionSetDefaultAppAndProc->setObjectName(QStringLiteral("actionSetDefaultAppAndProc"));
        QIcon icon12;
        icon12.addFile(QStringLiteral(":/Icons/cc-client.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionSetDefaultAppAndProc->setIcon(icon12);
        actionViewStationDetail = new QAction(MainWindow);
        actionViewStationDetail->setObjectName(QStringLiteral("actionViewStationDetail"));
        QIcon icon13;
        icon13.addFile(QStringLiteral(":/Icons/52design.com_file_036.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionViewStationDetail->setIcon(icon13);
        actionSendAll = new QAction(MainWindow);
        actionSendAll->setObjectName(QStringLiteral("actionSendAll"));
        actionSendSelection = new QAction(MainWindow);
        actionSendSelection->setObjectName(QStringLiteral("actionSendSelection"));
        actionReceiveAll = new QAction(MainWindow);
        actionReceiveAll->setObjectName(QStringLiteral("actionReceiveAll"));
        actionReceiveSelection = new QAction(MainWindow);
        actionReceiveSelection->setObjectName(QStringLiteral("actionReceiveSelection"));
        actionScreenCapture = new QAction(MainWindow);
        actionScreenCapture->setObjectName(QStringLiteral("actionScreenCapture"));
        QIcon icon14;
        icon14.addFile(QStringLiteral(":/Icons/screencapture.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionScreenCapture->setIcon(icon14);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout_3 = new QVBoxLayout(centralWidget);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        splitter = new QSplitter(centralWidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setFrameShape(QFrame::NoFrame);
        splitter->setFrameShadow(QFrame::Plain);
        splitter->setOrientation(Qt::Horizontal);
        splitter->setOpaqueResize(true);
        layoutWidget = new QWidget(splitter);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        frame = new QFrame(layoutWidget);
        frame->setObjectName(QStringLiteral("frame"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(frame);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setFont(font);

        horizontalLayout->addWidget(label);

        filterLineEdit = new QLineEdit(frame);
        filterLineEdit->setObjectName(QStringLiteral("filterLineEdit"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(filterLineEdit->sizePolicy().hasHeightForWidth());
        filterLineEdit->setSizePolicy(sizePolicy1);
        filterLineEdit->setMinimumSize(QSize(0, 0));

        horizontalLayout->addWidget(filterLineEdit);

        filterToolButton = new QToolButton(frame);
        filterToolButton->setObjectName(QStringLiteral("filterToolButton"));
        filterToolButton->setEnabled(false);

        horizontalLayout->addWidget(filterToolButton);

        horizontalSpacer_2 = new QSpacerItem(18, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        label_3 = new QLabel(frame);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout->addWidget(label_3);

        displayModeComboBox = new QComboBox(frame);
        displayModeComboBox->setObjectName(QStringLiteral("displayModeComboBox"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(displayModeComboBox->sizePolicy().hasHeightForWidth());
        displayModeComboBox->setSizePolicy(sizePolicy2);

        horizontalLayout->addWidget(displayModeComboBox);

        horizontalSpacer = new QSpacerItem(13, 18, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        label_2 = new QLabel(frame);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout->addWidget(label_2);

        sortComboBox = new QComboBox(frame);
        sortComboBox->setObjectName(QStringLiteral("sortComboBox"));
        sizePolicy2.setHeightForWidth(sortComboBox->sizePolicy().hasHeightForWidth());
        sortComboBox->setSizePolicy(sizePolicy2);
        sortComboBox->setMinimumSize(QSize(150, 0));

        horizontalLayout->addWidget(sortComboBox);


        verticalLayout_2->addLayout(horizontalLayout);

        listView = new QListView(frame);
        listView->setObjectName(QStringLiteral("listView"));
        listView->setContextMenuPolicy(Qt::CustomContextMenu);
        listView->setDragEnabled(true);
        listView->setDragDropMode(QAbstractItemView::DragDrop);
        listView->setDefaultDropAction(Qt::MoveAction);
        listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
        listView->setIconSize(QSize(16, 16));
        listView->setTextElideMode(Qt::ElideNone);
        listView->setViewMode(QListView::ListMode);

        verticalLayout_2->addWidget(listView);

        tableView = new QTableView(frame);
        tableView->setObjectName(QStringLiteral("tableView"));
        tableView->setContextMenuPolicy(Qt::CustomContextMenu);
        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableView->horizontalHeader()->setProperty("showSortIndicator", QVariant(true));

        verticalLayout_2->addWidget(tableView);


        verticalLayout->addWidget(frame);

        splitter->addWidget(layoutWidget);
        rightFrame = new QFrame(splitter);
        rightFrame->setObjectName(QStringLiteral("rightFrame"));
        QSizePolicy sizePolicy3(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(rightFrame->sizePolicy().hasHeightForWidth());
        rightFrame->setSizePolicy(sizePolicy3);
        rightFrame->setMinimumSize(QSize(300, 0));
        rightFrame->setStyleSheet(QStringLiteral("QFrame#rightFrame {background-color: rgb(170, 255, 127);}"));
        rightFrame->setFrameShape(QFrame::StyledPanel);
        rightFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_4 = new QVBoxLayout(rightFrame);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        splitter_2 = new QSplitter(rightFrame);
        splitter_2->setObjectName(QStringLiteral("splitter_2"));
        splitter_2->setOrientation(Qt::Vertical);
        splitter_2->setOpaqueResize(true);
        qwtPlotCPU = new QwtPlot(splitter_2);
        qwtPlotCPU->setObjectName(QStringLiteral("qwtPlotCPU"));
        splitter_2->addWidget(qwtPlotCPU);
        qwtPlotMemory = new QwtPlot(splitter_2);
        qwtPlotMemory->setObjectName(QStringLiteral("qwtPlotMemory"));
        splitter_2->addWidget(qwtPlotMemory);
        detailFrame = new QFrame(splitter_2);
        detailFrame->setObjectName(QStringLiteral("detailFrame"));
        detailFrame->setFrameShape(QFrame::StyledPanel);
        detailFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_5 = new QHBoxLayout(detailFrame);
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_4 = new QLabel(detailFrame);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_2->addWidget(label_4);

        procCountlineEdit = new QLineEdit(detailFrame);
        procCountlineEdit->setObjectName(QStringLiteral("procCountlineEdit"));

        horizontalLayout_2->addWidget(procCountlineEdit);


        horizontalLayout_5->addLayout(horizontalLayout_2);

        splitter_2->addWidget(detailFrame);
        msgListWidget = new QListWidget(splitter_2);
        msgListWidget->setObjectName(QStringLiteral("msgListWidget"));
        msgListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
        splitter_2->addWidget(msgListWidget);

        verticalLayout_4->addWidget(splitter_2);

        splitter->addWidget(rightFrame);

        verticalLayout_3->addWidget(splitter);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1024, 33));
        menuControl = new QMenu(menuBar);
        menuControl->setObjectName(QStringLiteral("menuControl"));
        menuOperation = new QMenu(menuBar);
        menuOperation->setObjectName(QStringLiteral("menuOperation"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QStringLiteral("menuView"));
        menuOption = new QMenu(menuBar);
        menuOption->setObjectName(QStringLiteral("menuOption"));
        menuFileTranslation = new QMenu(menuBar);
        menuFileTranslation->setObjectName(QStringLiteral("menuFileTranslation"));
        menuSend = new QMenu(menuFileTranslation);
        menuSend->setObjectName(QStringLiteral("menuSend"));
        QIcon icon15;
        icon15.addFile(QStringLiteral(":/Icons/sendFile.png"), QSize(), QIcon::Normal, QIcon::Off);
        menuSend->setIcon(icon15);
        menuReceive = new QMenu(menuFileTranslation);
        menuReceive->setObjectName(QStringLiteral("menuReceive"));
        QIcon icon16;
        icon16.addFile(QStringLiteral(":/Icons/receiveFile.png"), QSize(), QIcon::Normal, QIcon::Off);
        menuReceive->setIcon(icon16);
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuControl->menuAction());
        menuBar->addAction(menuOperation->menuAction());
        menuBar->addAction(menuFileTranslation->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuOption->menuAction());
        menuControl->addAction(actionAllPower_On);
        menuControl->addAction(actionStartAllApp);
        menuControl->addSeparator();
        menuControl->addAction(actionRebootAll);
        menuControl->addAction(actionRestartAll);
        menuControl->addSeparator();
        menuControl->addAction(actionExitAllZXApp);
        menuControl->addAction(actionAll_Shutdown);
        menuControl->addSeparator();
        menuControl->addAction(actionExit);
        menuOperation->addAction(actionPowerOn);
        menuOperation->addAction(actionPoweroff);
        menuOperation->addAction(actionReboot);
        menuOperation->addSeparator();
        menuOperation->addAction(actionStartApp);
        menuOperation->addAction(actionRestartApp);
        menuOperation->addAction(actionExitApp);
        menuOperation->addSeparator();
        menuOperation->addAction(actionScreenCapture);
        menuOperation->addAction(actionViewStationDetail);
        menuOperation->addSeparator();
        menuOperation->addAction(actionRemove);
        menuOperation->addAction(actionEdit);
        menuOperation->addAction(actionNewStation);
        menuView->addAction(actionViewCPU);
        menuView->addAction(actionViewMemory);
        menuView->addAction(actionViewDetail);
        menuView->addAction(actionViewMessage);
        menuOption->addAction(actionAllowAutoRefreshList);
        menuOption->addSeparator();
        menuOption->addAction(actionSetInterval);
        menuOption->addAction(actionSetDefaultAppAndProc);
        menuFileTranslation->addAction(menuSend->menuAction());
        menuFileTranslation->addAction(menuReceive->menuAction());
        menuSend->addAction(actionSendAll);
        menuSend->addAction(actionSendSelection);
        menuReceive->addAction(actionReceiveAll);
        menuReceive->addAction(actionReceiveSelection);

        retranslateUi(MainWindow);
        QObject::connect(actionExit, SIGNAL(triggered()), MainWindow, SLOT(close()));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "\344\270\255\346\216\247\346\216\247\345\210\266\345\217\260", 0));
        actionAllPower_On->setText(QApplication::translate("MainWindow", "\345\205\250\351\203\250\345\274\200\346\234\272...", 0));
#ifndef QT_NO_TOOLTIP
        actionAllPower_On->setToolTip(QApplication::translate("MainWindow", "\346\211\223\345\274\200\346\214\207\345\256\232\350\256\241\347\256\227\346\234\272", 0));
#endif // QT_NO_TOOLTIP
        actionAll_Power_On->setText(QApplication::translate("MainWindow", "\345\205\250\351\203\250\345\274\200\346\234\272", 0));
#ifndef QT_NO_TOOLTIP
        actionAll_Power_On->setToolTip(QApplication::translate("MainWindow", "\345\274\200\345\220\257\345\205\250\351\203\250\350\256\241\347\256\227\346\234\272", 0));
#endif // QT_NO_TOOLTIP
        actionShutdown->setText(QApplication::translate("MainWindow", "\345\205\263\346\234\272...", 0));
#ifndef QT_NO_TOOLTIP
        actionShutdown->setToolTip(QApplication::translate("MainWindow", "\345\205\263\351\227\255\346\214\207\345\256\232\350\256\241\347\256\227\346\234\272", 0));
#endif // QT_NO_TOOLTIP
        actionAll_Shutdown->setText(QApplication::translate("MainWindow", "\345\205\250\351\203\250\345\205\263\346\234\272", 0));
#ifndef QT_NO_TOOLTIP
        actionAll_Shutdown->setToolTip(QApplication::translate("MainWindow", "\345\205\263\351\227\255\345\205\250\351\203\250\350\256\241\347\256\227\346\234\272", 0));
#endif // QT_NO_TOOLTIP
        actionExit->setText(QApplication::translate("MainWindow", "\351\200\200\345\207\272", 0));
#ifndef QT_NO_TOOLTIP
        actionExit->setToolTip(QApplication::translate("MainWindow", "\351\200\200\345\207\272\346\216\247\345\210\266\345\217\260", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionExit->setStatusTip(QApplication::translate("MainWindow", "\351\200\200\345\207\272\346\216\247\345\210\266\345\217\260", 0));
#endif // QT_NO_STATUSTIP
        actionStartAllApp->setText(QApplication::translate("MainWindow", "\345\205\250\351\203\250\345\220\257\345\212\250\347\250\213\345\272\217...", 0));
#ifndef QT_NO_TOOLTIP
        actionStartAllApp->setToolTip(QApplication::translate("MainWindow", "\345\220\257\345\212\250\345\205\250\351\203\250\345\267\245\344\275\234\347\253\231\345\272\224\347\224\250\350\275\257\344\273\266", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionStartAllApp->setStatusTip(QApplication::translate("MainWindow", "\345\220\257\345\212\250\345\205\250\351\203\250\345\267\245\344\275\234\347\253\231\345\272\224\347\224\250\350\275\257\344\273\266", 0));
#endif // QT_NO_STATUSTIP
        actionExitAllZXApp->setText(QApplication::translate("MainWindow", "\345\205\250\351\203\250\351\200\200\345\207\272\347\250\213\345\272\217", 0));
#ifndef QT_NO_TOOLTIP
        actionExitAllZXApp->setToolTip(QApplication::translate("MainWindow", "\351\200\200\345\207\272\345\205\250\351\203\250\345\267\245\344\275\234\347\253\231\345\272\224\347\224\250\347\250\213\345\272\217", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionExitAllZXApp->setStatusTip(QApplication::translate("MainWindow", "\351\200\200\345\207\272\345\205\250\351\203\250\345\267\245\344\275\234\347\253\231\345\272\224\347\224\250\347\250\213\345\272\217", 0));
#endif // QT_NO_STATUSTIP
        actionCPU->setText(QApplication::translate("MainWindow", "CPU\345\215\240\347\224\250\347\216\207", 0));
        actionMemory->setText(QApplication::translate("MainWindow", "\345\206\205\345\255\230\345\215\240\347\224\250\347\216\207", 0));
        actionAppCPU->setText(QApplication::translate("MainWindow", "\346\214\207\346\230\276CPU\345\215\240\347\224\250\347\216\207", 0));
        actionAppMemory->setText(QApplication::translate("MainWindow", "\346\214\207\346\230\276\345\206\205\345\255\230\345\215\240\347\224\250\347\216\207", 0));
        actionProcCount->setText(QApplication::translate("MainWindow", "\350\277\233\347\250\213\346\225\260\351\207\217", 0));
        actionAppThreadCount->setText(QApplication::translate("MainWindow", "\346\214\207\346\230\276\347\272\277\347\250\213\346\225\260\351\207\217", 0));
        actionPowerOn->setText(QApplication::translate("MainWindow", "\345\274\200\346\234\272", 0));
#ifndef QT_NO_TOOLTIP
        actionPowerOn->setToolTip(QApplication::translate("MainWindow", "\346\211\223\345\274\200\351\200\211\345\256\232\347\232\204\350\256\241\347\256\227\346\234\272", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionPowerOn->setStatusTip(QApplication::translate("MainWindow", "\346\211\223\345\274\200\351\200\211\345\256\232\347\232\204\350\256\241\347\256\227\346\234\272", 0));
#endif // QT_NO_STATUSTIP
        actionPoweroff->setText(QApplication::translate("MainWindow", "\345\205\263\346\234\272", 0));
#ifndef QT_NO_TOOLTIP
        actionPoweroff->setToolTip(QApplication::translate("MainWindow", "\345\205\263\351\227\255\351\200\211\345\256\232\347\232\204\350\256\241\347\256\227\346\234\272", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionPoweroff->setStatusTip(QApplication::translate("MainWindow", "\345\205\263\351\227\255\351\200\211\345\256\232\347\232\204\350\256\241\347\256\227\346\234\272", 0));
#endif // QT_NO_STATUSTIP
        actionReboot->setText(QApplication::translate("MainWindow", "\351\207\215\345\220\257", 0));
#ifndef QT_NO_STATUSTIP
        actionReboot->setStatusTip(QApplication::translate("MainWindow", "\351\207\215\346\226\260\345\220\257\345\212\250\351\200\211\345\256\232\347\232\204\350\256\241\347\256\227\346\234\272", 0));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_WHATSTHIS
        actionReboot->setWhatsThis(QApplication::translate("MainWindow", "\351\207\215\346\226\260\345\220\257\345\212\250\351\200\211\345\256\232\347\232\204\350\256\241\347\256\227\346\234\272", 0));
#endif // QT_NO_WHATSTHIS
        actionStartApp->setText(QApplication::translate("MainWindow", "\345\220\257\345\212\250\347\250\213\345\272\217", 0));
#ifndef QT_NO_STATUSTIP
        actionStartApp->setStatusTip(QApplication::translate("MainWindow", "\345\220\257\345\212\250\351\200\211\345\256\232\345\267\245\344\275\234\347\253\231\347\232\204\347\250\213\345\272\217", 0));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_WHATSTHIS
        actionStartApp->setWhatsThis(QApplication::translate("MainWindow", "\345\220\257\345\212\250\351\200\211\345\256\232\345\267\245\344\275\234\347\253\231\347\232\204\347\250\213\345\272\217", 0));
#endif // QT_NO_WHATSTHIS
        actionExitApp->setText(QApplication::translate("MainWindow", "\351\200\200\345\207\272\347\250\213\345\272\217", 0));
#ifndef QT_NO_STATUSTIP
        actionExitApp->setStatusTip(QApplication::translate("MainWindow", "\351\200\200\345\207\272\351\200\211\345\256\232\345\267\245\344\275\234\347\253\231\347\232\204\345\272\224\347\224\250\347\250\213\345\272\217", 0));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_WHATSTHIS
        actionExitApp->setWhatsThis(QApplication::translate("MainWindow", "\351\200\200\345\207\272\351\200\211\345\256\232\345\267\245\344\275\234\347\253\231\347\232\204\345\272\224\347\224\250\347\250\213\345\272\217", 0));
#endif // QT_NO_WHATSTHIS
        actionRestartApp->setText(QApplication::translate("MainWindow", "\351\207\215\345\220\257\347\250\213\345\272\217", 0));
#ifndef QT_NO_STATUSTIP
        actionRestartApp->setStatusTip(QApplication::translate("MainWindow", "\351\207\215\346\226\260\345\220\257\345\212\250\351\200\211\345\256\232\345\267\245\344\275\234\347\253\231\347\232\204\345\272\224\347\224\250\347\250\213\345\272\217", 0));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_WHATSTHIS
        actionRestartApp->setWhatsThis(QApplication::translate("MainWindow", "\351\207\215\346\226\260\345\220\257\345\212\250\351\200\211\345\256\232\345\267\245\344\275\234\347\253\231\347\232\204\345\272\224\347\224\250\347\250\213\345\272\217", 0));
#endif // QT_NO_WHATSTHIS
        actionViewCPU->setText(QApplication::translate("MainWindow", "CPU\345\215\240\347\224\250\347\216\207", 0));
#ifndef QT_NO_STATUSTIP
        actionViewCPU->setStatusTip(QApplication::translate("MainWindow", "\346\237\245\347\234\213CPU\345\215\240\347\224\250\347\216\207", 0));
#endif // QT_NO_STATUSTIP
        actionViewMemory->setText(QApplication::translate("MainWindow", "\345\206\205\345\255\230\345\215\240\347\224\250\347\216\207", 0));
#ifndef QT_NO_TOOLTIP
        actionViewMemory->setToolTip(QApplication::translate("MainWindow", "\345\206\205\345\255\230\345\215\240\347\224\250\347\216\207", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionViewMemory->setStatusTip(QApplication::translate("MainWindow", "\346\237\245\347\234\213\345\206\205\345\255\230\345\215\240\347\224\250\347\216\207", 0));
#endif // QT_NO_STATUSTIP
        actionViewDetail->setText(QApplication::translate("MainWindow", "\350\277\233\347\250\213\346\225\260", 0));
#ifndef QT_NO_TOOLTIP
        actionViewDetail->setToolTip(QApplication::translate("MainWindow", "\346\237\245\347\234\213\346\210\226\345\205\263\351\227\255\350\277\233\347\250\213\346\225\260", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionViewDetail->setStatusTip(QApplication::translate("MainWindow", "\346\237\245\347\234\213\346\210\226\345\205\263\351\227\255\345\267\245\344\275\234\347\253\231\350\277\233\347\250\213\346\225\260\351\207\217\344\277\241\346\201\257", 0));
#endif // QT_NO_STATUSTIP
        actionViewMessage->setText(QApplication::translate("MainWindow", "\346\266\210\346\201\257", 0));
#ifndef QT_NO_TOOLTIP
        actionViewMessage->setToolTip(QApplication::translate("MainWindow", "\346\237\245\347\234\213\346\266\210\346\201\257", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionViewMessage->setStatusTip(QApplication::translate("MainWindow", "\346\237\245\347\234\213\346\266\210\346\201\257", 0));
#endif // QT_NO_STATUSTIP
        actionLineCPU->setText(QApplication::translate("MainWindow", "\347\272\277\346\200\247\345\233\276", 0));
        actionPieCPU->setText(QApplication::translate("MainWindow", "\351\245\274\345\233\276", 0));
        actionRectCPU->setText(QApplication::translate("MainWindow", "\346\237\261\347\212\266\345\233\276", 0));
        actionLineMemory->setText(QApplication::translate("MainWindow", "lineMemory", 0));
        actionViewLineMemory->setText(QApplication::translate("MainWindow", "\347\272\277\346\200\247\345\233\276", 0));
        actionPieMemory->setText(QApplication::translate("MainWindow", "\351\245\274\345\233\276", 0));
        actionRectMemory->setText(QApplication::translate("MainWindow", "\346\237\261\347\212\266\345\233\276", 0));
        actionRebootAll->setText(QApplication::translate("MainWindow", "\345\205\250\351\203\250\351\207\215\345\220\257\347\263\273\347\273\237", 0));
#ifndef QT_NO_STATUSTIP
        actionRebootAll->setStatusTip(QApplication::translate("MainWindow", "\351\207\215\346\226\260\345\220\257\345\212\250\345\205\250\351\203\250\345\267\245\344\275\234\347\253\231", 0));
#endif // QT_NO_STATUSTIP
        actionRestartAll->setText(QApplication::translate("MainWindow", "\345\205\250\351\203\250\351\207\215\345\220\257\345\272\224\347\224\250", 0));
#ifndef QT_NO_STATUSTIP
        actionRestartAll->setStatusTip(QApplication::translate("MainWindow", "\351\207\215\346\226\260\345\220\257\345\212\250\345\205\250\351\203\250\345\267\245\344\275\234\347\253\231\345\272\224\347\224\250\347\250\213\345\272\217", 0));
#endif // QT_NO_STATUSTIP
        actionRemove->setText(QApplication::translate("MainWindow", "\345\210\240\351\231\244\345\267\245\344\275\234\347\253\231", 0));
#ifndef QT_NO_TOOLTIP
        actionRemove->setToolTip(QApplication::translate("MainWindow", "\345\210\240\351\231\244\351\200\211\345\256\232\347\232\204\345\267\245\344\275\234\347\253\231", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionRemove->setStatusTip(QApplication::translate("MainWindow", "\344\273\216\345\210\227\350\241\250\344\270\255\345\210\240\351\231\244\351\200\211\345\256\232\347\232\204\345\267\245\344\275\234\347\253\231", 0));
#endif // QT_NO_STATUSTIP
        actionEdit->setText(QApplication::translate("MainWindow", "\347\274\226\350\276\221\345\267\245\344\275\234\347\253\231", 0));
#ifndef QT_NO_TOOLTIP
        actionEdit->setToolTip(QApplication::translate("MainWindow", "\347\274\226\350\276\221\345\267\245\344\275\234\347\253\231\344\277\241\346\201\257", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionEdit->setStatusTip(QApplication::translate("MainWindow", "\347\274\226\350\276\221\351\200\211\345\256\232\345\267\245\344\275\234\347\253\231\344\277\241\346\201\257", 0));
#endif // QT_NO_STATUSTIP
        actionAllowAutoRefreshList->setText(QApplication::translate("MainWindow", "\350\207\252\345\212\250\346\233\264\346\226\260\345\267\245\344\275\234\347\253\231\345\210\227\350\241\250", 0));
        actionAllowAutoRefreshList->setIconText(QApplication::translate("MainWindow", "\345\205\201\350\256\270\350\207\252\345\212\250\346\233\264\346\226\260\345\267\245\344\275\234\347\253\231\345\210\227\350\241\250", 0));
#ifndef QT_NO_TOOLTIP
        actionAllowAutoRefreshList->setToolTip(QApplication::translate("MainWindow", "\345\205\201\350\256\270\350\207\252\345\212\250\346\233\264\346\226\260\345\267\245\344\275\234\347\253\231\345\210\227\350\241\250", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionAllowAutoRefreshList->setStatusTip(QApplication::translate("MainWindow", "\345\205\201\350\256\270\350\207\252\345\212\250\346\233\264\346\226\260\345\267\245\344\275\234\347\253\231\345\210\227\350\241\250", 0));
#endif // QT_NO_STATUSTIP
        actionSetInterval->setText(QApplication::translate("MainWindow", "\350\256\276\347\275\256\347\233\221\350\247\206\351\227\264\351\232\224...", 0));
#ifndef QT_NO_TOOLTIP
        actionSetInterval->setToolTip(QApplication::translate("MainWindow", "\350\256\276\347\275\256\345\267\245\344\275\234\347\253\231\347\233\221\350\247\206\351\227\264\351\232\224", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionSetInterval->setStatusTip(QApplication::translate("MainWindow", "\350\256\276\347\275\256\345\267\245\344\275\234\347\253\231\347\233\221\350\247\206\351\227\264\351\232\224", 0));
#endif // QT_NO_STATUSTIP
        actionNewStation->setText(QApplication::translate("MainWindow", "\346\226\260\345\273\272\345\267\245\344\275\234\347\253\231", 0));
#ifndef QT_NO_TOOLTIP
        actionNewStation->setToolTip(QApplication::translate("MainWindow", "\346\226\260\345\273\272\344\270\200\344\270\252\345\267\245\344\275\234\347\253\231\351\205\215\347\275\256\351\241\271", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionNewStation->setStatusTip(QApplication::translate("MainWindow", "\346\226\260\345\273\272\344\270\200\344\270\252\345\267\245\344\275\234\347\253\231\351\205\215\347\275\256\351\241\271", 0));
#endif // QT_NO_STATUSTIP
        actionSetDefaultAppAndProc->setText(QApplication::translate("MainWindow", "\350\256\276\347\275\256\351\273\230\350\256\244\347\250\213\345\272\217\345\222\214\350\277\233\347\250\213...", 0));
#ifndef QT_NO_TOOLTIP
        actionSetDefaultAppAndProc->setToolTip(QApplication::translate("MainWindow", "\350\256\276\347\275\256\346\226\260\345\273\272\345\267\245\344\275\234\347\253\231\351\273\230\350\256\244\345\220\257\345\212\250\347\250\213\345\272\217\345\222\214\347\233\221\350\247\206\350\277\233\347\250\213", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionSetDefaultAppAndProc->setStatusTip(QApplication::translate("MainWindow", "\350\256\276\347\275\256\346\226\260\345\273\272\345\267\245\344\275\234\347\253\231\351\273\230\350\256\244\345\220\257\345\212\250\347\250\213\345\272\217\345\222\214\347\233\221\350\247\206\350\277\233\347\250\213", 0));
#endif // QT_NO_STATUSTIP
        actionViewStationDetail->setText(QApplication::translate("MainWindow", "\350\257\246\347\273\206\344\277\241\346\201\257", 0));
#ifndef QT_NO_TOOLTIP
        actionViewStationDetail->setToolTip(QApplication::translate("MainWindow", "\346\237\245\347\234\213\345\267\245\344\275\234\347\253\231\350\257\246\347\273\206\344\277\241\346\201\257", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionViewStationDetail->setStatusTip(QApplication::translate("MainWindow", "\346\237\245\347\234\213\345\267\245\344\275\234\347\253\231\350\257\246\347\273\206\344\277\241\346\201\257", 0));
#endif // QT_NO_STATUSTIP
        actionSendAll->setText(QApplication::translate("MainWindow", "\345\205\250\351\203\250\345\267\245\344\275\234\347\253\231...", 0));
        actionSendSelection->setText(QApplication::translate("MainWindow", "\351\200\211\345\256\232\345\267\245\344\275\234\347\253\231...", 0));
        actionReceiveAll->setText(QApplication::translate("MainWindow", "\345\205\250\351\203\250\345\267\245\344\275\234\347\253\231...", 0));
        actionReceiveSelection->setText(QApplication::translate("MainWindow", "\351\200\211\345\256\232\345\267\245\344\275\234\347\253\231...", 0));
        actionScreenCapture->setText(QApplication::translate("MainWindow", "\345\261\217\345\271\225\345\277\253\347\205\247", 0));
#ifndef QT_NO_TOOLTIP
        actionScreenCapture->setToolTip(QApplication::translate("MainWindow", "\346\215\225\350\216\267\345\267\245\344\275\234\347\253\231\345\261\217\345\271\225\345\277\253\347\205\247", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionScreenCapture->setStatusTip(QApplication::translate("MainWindow", "\346\215\225\350\216\267\351\200\211\345\256\232\345\267\245\344\275\234\347\253\231\345\261\217\345\271\225\345\277\253\347\205\247", 0));
#endif // QT_NO_STATUSTIP
        label->setText(QApplication::translate("MainWindow", "\350\277\207\346\273\244\357\274\232", 0));
#ifndef QT_NO_TOOLTIP
        filterLineEdit->setToolTip(QApplication::translate("MainWindow", "\350\276\223\345\205\245\350\277\207\346\273\244\346\235\241\344\273\266\357\274\214\347\204\266\345\220\216\346\214\211\345\233\236\350\275\246\351\224\256\350\277\207\346\273\244", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        filterLineEdit->setStatusTip(QApplication::translate("MainWindow", "\350\276\223\345\205\245\350\277\207\346\273\244\346\235\241\344\273\266\357\274\214\347\204\266\345\220\216\346\214\211\345\233\236\350\275\246\351\224\256\350\277\207\346\273\244", 0));
#endif // QT_NO_STATUSTIP
        filterLineEdit->setPlaceholderText(QApplication::translate("MainWindow", "\350\276\223\345\205\245\345\220\215\347\247\260\346\210\226IP", 0));
#ifndef QT_NO_TOOLTIP
        filterToolButton->setToolTip(QApplication::translate("MainWindow", "\347\274\226\350\276\221\350\277\207\346\273\244\346\235\241\344\273\266", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        filterToolButton->setStatusTip(QApplication::translate("MainWindow", "\347\274\226\350\276\221\350\277\207\346\273\244\346\235\241\344\273\266", 0));
#endif // QT_NO_STATUSTIP
        filterToolButton->setText(QApplication::translate("MainWindow", "\342\226\274", 0));
        label_3->setText(QApplication::translate("MainWindow", "\346\230\276\347\244\272\346\226\271\345\274\217\357\274\232", 0));
        displayModeComboBox->clear();
        displayModeComboBox->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Small Icons", 0)
         << QApplication::translate("MainWindow", "Larger Icons", 0)
         << QApplication::translate("MainWindow", "List", 0)
         << QApplication::translate("MainWindow", "Detail", 0)
        );
#ifndef QT_NO_TOOLTIP
        displayModeComboBox->setToolTip(QApplication::translate("MainWindow", "\351\200\211\346\213\251\346\230\276\347\244\272\346\226\271\345\274\217", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        displayModeComboBox->setStatusTip(QApplication::translate("MainWindow", "\351\200\211\346\213\251\346\230\276\347\244\272\346\226\271\345\274\217", 0));
#endif // QT_NO_STATUSTIP
        label_2->setText(QApplication::translate("MainWindow", "\346\216\222\345\272\217\357\274\232", 0));
        sortComboBox->clear();
        sortComboBox->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "IP", 0)
         << QApplication::translate("MainWindow", "IP DESC", 0)
         << QApplication::translate("MainWindow", "State", 0)
         << QApplication::translate("MainWindow", "State DESC", 0)
         << QApplication::translate("MainWindow", "Name", 0)
         << QApplication::translate("MainWindow", "Name DESC", 0)
        );
#ifndef QT_NO_TOOLTIP
        sortComboBox->setToolTip(QApplication::translate("MainWindow", "\351\200\211\346\213\251\346\216\222\345\272\217\346\226\271\345\274\217", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        sortComboBox->setStatusTip(QApplication::translate("MainWindow", "\351\200\211\346\213\251\346\216\222\345\272\217\346\226\271\345\274\217", 0));
#endif // QT_NO_STATUSTIP
        label_4->setText(QApplication::translate("MainWindow", "\350\277\233\347\250\213\346\225\260\357\274\232", 0));
        menuControl->setTitle(QApplication::translate("MainWindow", "\346\216\247\345\210\266(&C)", 0));
        menuOperation->setTitle(QApplication::translate("MainWindow", "\346\223\215\344\275\234", 0));
        menuView->setTitle(QApplication::translate("MainWindow", "\346\237\245\347\234\213", 0));
        menuOption->setTitle(QApplication::translate("MainWindow", "\351\200\211\351\241\271", 0));
        menuFileTranslation->setTitle(QApplication::translate("MainWindow", "\346\226\207\344\273\266\344\274\240\350\276\223", 0));
        menuSend->setTitle(QApplication::translate("MainWindow", "\345\217\221\351\200\201", 0));
        menuReceive->setTitle(QApplication::translate("MainWindow", "\346\216\245\346\224\266", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
