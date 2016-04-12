#include "floatingmenu.h"
#include "ui_floatingmenu.h"
#include <QPainter>
#include <QPen>
#include <QParallelAnimationGroup>
#include <QtMath>
#include <QScreen>
#include <QMainWindow>
//初始化为空指针
FloatingMenu* FloatingMenu::instance = nullptr;

//修正显示位置
void FloatingMenu::AmendPosition(int &posX, int &posY)
{
    QPoint point = mapToGlobal(QPoint(posX, posY));
    QSize screenSize = QGuiApplication::primaryScreen()->availableSize();

    if (point.x() - size() / 2 < 0)
    {
        //左边
        posX += size() / 2 - point.x();
    }
    else if (point.x() + size() / 2 > screenSize.width())
    {
        //右边
        posX = screenSize.width() - size() / 2;
    }
    if (point.y() - size() / 2 < 0)
    {
        //上边
        posY += size() / 2 - point.y();
    }
    else if (point.y() + size() / 2 > screenSize.height())
    {
        //下边
        posY = screenSize.height() - size() / 2;
    }
}

#ifdef NeoLinux
void FloatingMenu::AmendPositionForNeoLinx(int &posX, int &posY)
{
    QMainWindow* parent=(QMainWindow*)this->parent();
    if (posX - size() / 2 < 0)
    {
        //左边
        posX += size() / 2 - posX;
    }
    else if (posX + size() / 2 > parent->width())
    {
        //右边
        posX = parent->width() - size() / 2;
    }
    if (posY - size() / 2 < 0)
    {
        //上边
        posY += size() / 2 - posY;
    }
    else if (posY + size() / 2 > parent->height())
    {
        //下边
        posY = parent->height() - size() / 2;
    }
}

#endif

//关闭菜单
void FloatingMenu::closeMe()
{
    close();
}

//初始化
FloatingMenu::FloatingMenu(QWidget *parent)
    : QDialog(parent), borderPen(QBrush(QColor(255, 170, 0)), 3, Qt::DotLine), background(QColor(170, 255, 127, 20))
{
    ui = new Ui::FloatingMenu();
    ui->setupUi(this);

#ifdef NeoLinux
    this->setWindowFlags(Qt::FramelessWindowHint);
#else
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
#endif
    //没有标题栏,不在任务栏显示
    this->setAttribute(Qt::WA_TranslucentBackground);

    //启动退出计时器
    exitTimer = new QTimer();
    connect(exitTimer, SIGNAL(timeout()), this, SLOT(close()));
}

FloatingMenu::~FloatingMenu()
{
    delete ui;
    delete exitTimer;
    for (QToolButton* button : buttons)
    {
        delete button;
    }
}
//鼠标进入
void FloatingMenu::enterEvent(QEvent * event)
{
    exitTimer->stop();
}

//鼠标离开
void FloatingMenu::leaveEvent(QEvent *event)
{
    exitTimer->start(500); //500ms后退出
}

/*!
显示悬浮式菜单
@param int posX 显示位置（中心)
@param int posY 显示位置（中心)
@return void
@remarks 设置好所有选项,按钮后调用show显示
作者：cokkiy（张立民)
创建时间：2015/11/09 21:07:00
*/
void FloatingMenu::show(int posX, int posY)
{
#ifdef NeoLinux
    QMainWindow* parent=(QMainWindow*)this->parent();
    posX=posX-parent->geometry().left();
    posY=posY-parent->geometry().top();
#endif
    if (instance != nullptr)
    {
        instance->close();
        delete instance;
    }
    instance = this;

#ifdef NeoLinux
    AmendPositionForNeoLinx(posX,posY);
#else
    //修正坐标,不要超出屏幕边距
    AmendPosition(posX, posY);
#endif

    //动画组
    QAnimationGroup* animationGroup = new QParallelAnimationGroup();
    //设置所有按钮动画属性
    int index = 0;
    float angle = 2 * 3.1415 / buttons.count(); //弧度值
    float r = (size() - buttonSize) / 2.0;
    for (QToolButton* button : buttons)
    {
        QPropertyAnimation* animation = new QPropertyAnimation(button, "geometry");
        animation->setDuration(300); //300ms
        //从中心开始变大到外边,大小从10开始
        float x = r*qSin(angle*index) + size() / 2.0 - buttonSize / 2.0;
        float y = -r*qCos(angle*index) + size() / 2.0 - buttonSize / 2.0;
        animation->setStartValue(QRectF(0, 0, buttonSize * 10.0 / size(), buttonSize * 10.0 / size()));
        animation->setEndValue(QRectF(x, y, buttonSize, buttonSize)); //最后到54
        animationGroup->addAnimation(animation);
        index++;
    }

    QPropertyAnimation* animation=new QPropertyAnimation(this, "geometry");
    animation->setDuration(300);
    animation->setStartValue(QRectF(posX-10/2,posY-10/2,10,10));
    animation->setEndValue(QRectF(posX - size() / 2.0, posY - size() / 2.0, size(), size()));
    animationGroup->addAnimation(animation);
    animationGroup->start(QAbstractAnimation::DeleteWhenStopped);
    
    //1秒后退出
    exitTimer->start(1000);
    QDialog::show();
}

/*绘制圆形底图*/
void FloatingMenu::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    painter.save();
    painter.setBrush(background);
    painter.setPen(borderPen/*Qt::PenStyle::NoPen*/);
    painter.translate(width() / 2.0, height() / 2.0);
    //底图离外边buttonSize/2.0
    painter.drawEllipse(QPointF(0, 0), (width() - buttonSize / 2.0) / 2.0, (height() - buttonSize / 2.0) / 2.0);
    painter.restore();
}

/*!
设置大小
@param int size 大小
@return void
作者：cokkiy（张立民)
创建时间：2015/11/09 16:37:20
*/
void FloatingMenu::setSize(int size /*= 250*/)
{
    this->m_size = size;
}

/*!
获取菜单大小
@return int 悬浮菜单大小
作者：cokkiy（张立民)
创建时间：2015/11/09 16:37:20
*/
int FloatingMenu::size()
{
    return m_size;
}

/*!
设置边框
@param const QPen & borderPen
@return void
作者：cokkiy（张立民)
创建时间：2015/11/09 16:48:01
*/
void FloatingMenu::setBorder(const QPen& borderPen)
{
    this->borderPen = borderPen;
}

/*!
设置背景
@param const QBrush & backBrush
@return void
作者：cokkiy（张立民)
创建时间：2015/11/09 16:48:17
*/
void FloatingMenu::setBackground(const QBrush& backBrush)
{
    this->background = backBrush;
}


/*!
设置按钮大小
@param int buttonSize 按钮大小，默认54
@return void
作者：cokkiy（张立民)
创建时间：2015/11/09 21:50:00
*/
void FloatingMenu::setButtonSize(int buttonSize /*= 54*/)
{
    this->buttonSize = buttonSize;
}

/*!
添加一个按钮
@param const QString& iconName 按钮图标文件
@param const QString& text 按钮文字
@param const QObject * receiver 处理函数所在对象
@param const char * member 处理函数名称
@param int iconSize 图标大小，默认为32
@param bool enabled 是否使能
@return void
作者：cokkiy（张立民)
创建时间：2015/11/09 17:16:58
*/
void FloatingMenu::addButton(const QString& iconName, const QString& text,
    const QObject* receiver, const char* member, int iconSize, bool enabled)
{
    QToolButton* button = new QToolButton(this);
    QIcon icon;
    icon.addFile(iconName, QSize(), QIcon::Normal, QIcon::Off);
    button->setIcon(icon);
    button->setIconSize(QSize(iconSize, iconSize));
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setText(text);
    button->setEnabled(enabled);
    connect(button, SIGNAL(clicked()), this, SLOT(closeMe()));
    if (receiver != NULL&&member != NULL)
        connect(button, SIGNAL(clicked()), receiver, member);
    buttons.push_back(button);
}

/*!
添加一个禁用的按钮(为了美观)
@param const QString & iconName 按钮图标名称
@param const QString & text 按钮文字
@return void
作者：cokkiy（张立民)
创建时间：2015/11/10 10:17:44
*/
void FloatingMenu::addButton(const QString& iconName, const QString& text)
{
    addButton(iconName, text, NULL, NULL, 32, false);
}

