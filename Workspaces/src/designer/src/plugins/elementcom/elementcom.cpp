#include "elementcom.h"
#include <stdlib.h>
// sdk
#include<QDebug>
Elementcom::Elementcom(QWidget *parent) :
    QLabel(parent)
{
    m_bVisible = true;
}
Elementcom::~Elementcom()
{
}
void Elementcom::visible()
{
    m_bVisible = !m_bVisible;
    if(!m_bVisible){
        hide();
    }
    else
    {
        show();
    }
}
