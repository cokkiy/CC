#ifndef COMPASS_H
#define COMPASS_H

#include <QWidget>

class compass : public QWidget
{
    Q_OBJECT
//    Q_PROPERTY( QColor front_color READ frontColor WRITE frontColor)
public:
    compass(QWidget *parent = 0);
//    ~compass();
//public:
//    inline QColor frontColor()
//    {
//        return m_front_color;
//    };
//    inline void frontColor(QColor front_color)
//    {
//        m_front_color = front_color;
//    };
//protected slots:
//    void timerEvent(QTimerEvent *event);
//private :
//    int m_timer_id;
//    QColor m_front_color;
};

#endif
