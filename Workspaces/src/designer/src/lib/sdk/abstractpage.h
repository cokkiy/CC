#ifndef ABSTRACTPAGE_H
#define ABSTRACTPAGE_H
#include <QtDesigner/sdk_global.h>
#include <QWidget>

QT_BEGIN_NAMESPACE

class AbstractPagePrivate;
class QDESIGNER_SDK_EXPORT AbstractPage : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString pagename READ pagename WRITE setPageName)
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry)
    Q_PROPERTY(QPixmap map READ map WRITE setMap)
    Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
public:
    explicit AbstractPage(QWidget *parent = 0);
    virtual ~AbstractPage();
    const QString pagename() const;
    const QPixmap *map() const;
    const QBrush brush() const;
    void paintEvent(QPaintEvent *);
signals:

public slots:    
    void setPageName(const QString &);
    void setMap(const QPixmap &);
    void setBrush(const QBrush&);
private:
    QScopedPointer<AbstractPagePrivate> d_ptr;
    Q_DECLARE_PRIVATE(AbstractPage)
    Q_DISABLE_COPY(AbstractPage)
};
QT_END_NAMESPACE
#endif // ABSTRACTPAGE_H
