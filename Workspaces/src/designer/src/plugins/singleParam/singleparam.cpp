#include "singleparam.h"
#include <stdlib.h>
SingleParam::SingleParam(QWidget *parent) :
    QLabel(parent)
{
    m_timer_id = startTimer(500);
}
SingleParam::~SingleParam()
{
    killTimer(m_timer_id);
}

void SingleParam::timerEvent(QTimerEvent *event)
{
    int singledata = rand();
    QString txt = QString("%1").arg(singledata);
    setText(txt);    
    update();
}
