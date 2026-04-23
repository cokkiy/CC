#ifndef QTDISPATCHER_H
#define QTDISPATCHER_H

#include <QObject>
#include <Ice/Ice.h>

/*
 A Qt version ICE dispatcher  
 **/
class QtDispatcher : public QObject, public Ice::Dispatcher
{
    Q_OBJECT

public:
    /*!
    Create an instance of QtDispatcher
    @param bool debug in debug mode will display debug info
    @return 
    作者：cokkiy（张立民)
    创建时间：2015/12/15 21:03:52
    */
    QtDispatcher(bool debug = false);
    ~QtDispatcher();

    /*!
    From Ice::Dispatcher, this function called inside from Ice thread pool.
    If an instance of QApplication already exists, post a Qt Event to it
    The event contains dispatcher call and connection information.
    It will be received and processed by customeEvent().
    @param const DispatcherCallPtr & dispatcher call
    @param const ConnectionPtr & connection information.
    @return void
    作者：cokkiy（张立民)
    创建时间：2015/12/15 21:00:31
    */
    virtual void dispatch(const Ice::DispatcherCallPtr& d, const Ice::ConnectionPtr& c) override;
   
    /*!
    Stop dispatcher before exit!
    */
    void stop();

private:
    /*
     from QObject
     This event is called from inside the Qt main event loop (the Qt "GUI" thread)
     Processes events send by dispacth()
     **/
    virtual void customEvent(QEvent * e) override;
    /*
     debug mode or not
     **/
    bool _debug;

    /*
    is stop?
    **/
    bool isStop = false;

};

#endif // QTDISPATCHER_H
