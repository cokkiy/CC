#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
/**************************
\class Option 选项
 **************************/
class Option : public QObject
{
    Q_OBJECT

public:
    Option(QObject *parent=NULL);
    ~Option();

private:
    
};

#endif // CONFIG_H
