#include "qtdispatcher.h"
#include <QApplication>
#include <Ice/Connection.h>
#include <iostream>
#include <QDebug>
using namespace std;

namespace
{
    const int CUSTOM_EVENT_TYPE = QEvent::User + 10000;
    /*
     Dispacther event
     **/
    class DispatchEvent :public QEvent
    {
    public:
        DispatchEvent(const Ice::DispatcherCallPtr& d, const Ice::ConnectionPtr& c)
            :QEvent(QEvent::Type(CUSTOM_EVENT_TYPE)),
            dispatcherCall(d),
            connection(c)
        {};
        /*
         Dispacther Call
         **/
        Ice::DispatcherCallPtr dispatcherCall;
        /*
         Connection
         **/
        Ice::ConnectionPtr connection;
    };
}

QtDispatcher::QtDispatcher(bool debug)
    : _debug(debug)
{
}

QtDispatcher::~QtDispatcher()
{

}

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
void QtDispatcher::dispatch(const Ice::DispatcherCallPtr& d, const Ice::ConnectionPtr& c)
{
    if (_debug)
    {
        if (c->getInfo()->incoming)
        {
            qDebug() << "QtDispatcher::" << __func__ << "() : incoming call" << endl;
        }
        else
        {
            qDebug() << "QtDispatcher::" << __func__ << "() : AMI call" << endl;
        }
    }
    if (qApp)
    {
        if (_debug)
        {
            qDebug() << "QtDispatcher::dispatch(): dispacthing event with qApp->postEvent()" << endl;
        }
        qApp->postEvent(this, (QEvent*)new DispatchEvent(d, c));
    }
    else
    {
        if (_debug)
            qDebug() << "QtDispatcher::dispatch(): executing remote call directly with d->run()" << endl;
        d->run(); // Does not throw, blocks until op complete
    }
}

/*
from QObject
This event is called from inside the Qt main event loop (the Qt "GUI" thread)
Processes events send by dispacth()
**/
void QtDispatcher::customEvent(QEvent * event)
{
    if (_debug)
    {
        qDebug() << "QtDispatcher::" << __func__ << "()" << endl;
    }
    assert (event->type() == CUSTOM_EVENT_TYPE);
    DispatchEvent* e = dynamic_cast<DispatchEvent*>(event);
    assert(e);

    try
    {
        e->dispatcherCall->run(); //Dose not throw, blocks until op completes
    }
    catch (const Ice::Exception& e)
    {
        qDebug() << "QtDispatcher::customEvent(): caught ex: " << e.what() << endl;
    }
}
