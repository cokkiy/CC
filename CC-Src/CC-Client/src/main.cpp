#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include "qtdispatcher.h"


class CCApplication : public Ice::Application
{
public:
    CCApplication() {}
    virtual int run(int, char*[]);
};
int CCApplication::run(int argc, char* argv[])
{
    //     argc = 0;
    //     Ice::InitializationData initData;
    //     initData.properties = Ice::createProperties();
    //     //加载配置文件
    //     initData.properties->load("Config.Server");
    //     //装载Qt版本Dispatcher
    //     initData.dispatcher = new QtDispatcher(true);
    //     Ice::CommunicatorPtr communicator = Ice::initialize(argc, 0, initData);
    //     Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapter("StateReceiver");
    //     //StationList *pStations = new StationList();
    //    // auto StationStateReceiverPtr = new StationStateReceiver(pStations);
    //     adapter->add(StationStateReceiverPtr, communicator->stringToIdentity("stateReceiver"));
    //     adapter->activate();
    // 
    // 
    //     QApplication a(argc, argv);
    // 
    //     QTranslator qtTranslator;
    //     qtTranslator.load("qt_" + QLocale::system().name(),
    //         QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    //     a.installTranslator(&qtTranslator);
    // 
    //     QTranslator myappTranslator;
    //     myappTranslator.load("cc-client_" + QLocale::system().name());
    //     a.installTranslator(&myappTranslator);
    // 
    // 
    //     MainWindow w;
    //     //w.pStationList = pStations;
    //     w.show();
    // 
    //     return a.exec();
    return 0;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
        QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load("cc-client_" + QLocale::system().name());
    a.installTranslator(&myappTranslator);


    MainWindow w;
    w.show();

    return a.exec();
}
