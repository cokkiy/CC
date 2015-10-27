#include "abstractpage.h"
#include "qimage.h"
#include "qbitmap.h"
#include "qpicture.h"
class AbstractPagePrivate {
    //Q_DECLARE_PUBLIC(AbstractPage)
public:
    explicit AbstractPagePrivate();
    QString pagename;
    QPixmap  *pixmap;
    QBrush brush;
    QGradient gradient;
};
AbstractPagePrivate::AbstractPagePrivate():
    pagename("pagename"),pixmap(NULL),
    brush(Qt::blue)
{
}
//void AbstractPagePrivate::updateLabel()
//{
//    Q_Q(AbstractPage);
//    valid_hints = false;

//    if (isTextLabel) {
//        QSizePolicy policy = q->sizePolicy();
//        const bool wrap = align & Qt::TextWordWrap;
//        policy.setHeightForWidth(wrap);
//        if (policy != q->sizePolicy())  // ### should be replaced by WA_WState_OwnSizePolicy idiom
//            q->setSizePolicy(policy);
//        textLayoutDirty = true;
//    }
//    q->updateGeometry();
//    q->update(q->contentsRect());
//}
AbstractPage::AbstractPage(QWidget *parent) :
    QWidget(parent), d_ptr(new AbstractPagePrivate)
{
}
AbstractPage::~AbstractPage()
{
}
const QString AbstractPage::pagename() const
{
    Q_D(const AbstractPage);
    return d->pagename;
}

void AbstractPage::setPageName(const QString &pagename)
{
    Q_D(AbstractPage);
    d->pagename = pagename;
    update();
}
const QPixmap *AbstractPage::map() const
{
    Q_D(const AbstractPage);
    return d->pixmap;
}

void AbstractPage::setMap(const QPixmap &pixmap)
{
    Q_D(AbstractPage);
    if(!pixmap.isNull())
    {
        if (!d->pixmap || d->pixmap->cacheKey() != pixmap.cacheKey()) {
            d->pixmap = new QPixmap(pixmap);
        }
        if (d->pixmap->depth() == 1 && !d->pixmap->mask())
            d->pixmap->setMask(*((QBitmap *)d->pixmap));
    }
    else
    {
        if(d->pixmap)
        {
            delete d->pixmap;
            d->pixmap = NULL;
        }
    }
    update();
    //d->updateLabel();

}

const QBrush AbstractPage::brush() const
{
    Q_D(const AbstractPage);
    return d->brush;
}

void AbstractPage::setBrush(const QBrush &bkBrush)
{
    Q_D(AbstractPage);
    d->brush = bkBrush;
    update();
}

void AbstractPage::paintEvent(QPaintEvent *)
{
    Q_D(AbstractPage);
    QPainter painter(this);
    QRect cr = contentsRect();
    painter.save();
    if(d->pixmap)
    {
        painter.drawPixmap(cr, *(d->pixmap), d->pixmap->rect());
    }
    else
    {
        painter.fillRect(cr, brush());
    }
    painter.restore();
}
