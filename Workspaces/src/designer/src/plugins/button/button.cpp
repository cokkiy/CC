#include "button.h"

Button::Button(QWidget *parent) :
    QPushButton(parent)
{
    m_normalImage = QUrl::fromUserInput(":/resource/button_normal.png");
    m_hoverImage = QUrl::fromUserInput(":/resource/button_hover.png");
    m_pressedImage = QUrl::fromUserInput(":/resource/button_pressed.png");
    m_disabledImage = QUrl::fromUserInput("://resource/button_disabled.png");
    m_focusImage = QUrl::fromUserInput(":/resource/button_focused.png");

    connect(this, SIGNAL(textColorChanged()), this, SLOT(update()));
    connect(this, SIGNAL(imageChanged()), this, SLOT(update()));
}

void Button::setTextColor(const QColor &color)
{
    if (m_textColor != color) {
        m_textColor = color;
        emit textColorChanged();
    }
}

void Button::setNormalImage(const QUrl &newUrl)
{
    QUrl tempUrl;
    QRegExp re = QRegExp("^qrc.*");
    if (re.exactMatch(newUrl.toString())) {
        tempUrl = QUrl::fromUserInput("file::" + newUrl.path());
    }
    else {
        tempUrl = newUrl;
    }
    if (m_normalImage != tempUrl) {
        m_normalImage = tempUrl;
        emit imageChanged();
    }
}

void Button::setHoverImage(const QUrl &newUrl)
{
    QUrl tempUrl;
    QRegExp re = QRegExp("^qrc.*");
    if (re.exactMatch(newUrl.toString())) {
        tempUrl = QUrl::fromUserInput("file::" + newUrl.path());
    }
    else {
        tempUrl = newUrl;
    }
    if (m_hoverImage != tempUrl) {
        m_hoverImage = tempUrl;
        emit imageChanged();
    }
}

void Button::setPressedImage(const QUrl &newUrl)
{
    QUrl tempUrl;
    QRegExp re = QRegExp("^qrc.*");
    if (re.exactMatch(newUrl.toString())) {
        tempUrl = QUrl::fromUserInput("file::" + newUrl.path());
    }
    else {
        tempUrl = newUrl;
    }
    if (m_pressedImage != tempUrl) {
        m_pressedImage = tempUrl;
        emit imageChanged();
    }
}

void Button::setDisabledImage(const QUrl &newUrl)
{
    QUrl tempUrl;
    QRegExp re = QRegExp("^qrc.*");
    if (re.exactMatch(newUrl.toString())) {
        tempUrl = QUrl::fromUserInput("file::" + newUrl.path());
    }
    else {
        tempUrl = newUrl;
    }
    if (m_disabledImage != tempUrl) {
        m_disabledImage = tempUrl;
        emit imageChanged();
    }
}

void Button::setFocusImage(const QUrl &newUrl)
{
    QUrl tempUrl;
    QRegExp re = QRegExp("^qrc.*");
    if (re.exactMatch(newUrl.toString())) {
        tempUrl = QUrl::fromUserInput("file::" + newUrl.path());
    }
    else {
        tempUrl = newUrl;
    }
    if (m_focusImage != tempUrl) {
        m_focusImage = tempUrl;
        emit imageChanged();
    }
}

//更新样式表，包括更新字体颜色和背景图片
void Button::stylesheetUpdate()
{
    QString strColor = QString("rgb(%1, %2, %3)").arg(QString::number(textColor().red()),
                                                      QString::number(textColor().green()),
                                                      QString::number(textColor().blue()));
    QString styleSheet = QString("QPushButton{border-image: url(%1);}").arg(m_normalImage.path()) +
                         QString("QPushButton:hover{border-image: url(%1);}").arg(m_hoverImage.path()) +
                         QString("QPushButton:pressed{border-image: url(%1);}").arg(m_pressedImage.path()) +
                         QString("QPushButton:!enabled{border-image: url(%1);}").arg(m_disabledImage.path()) +
                         QString("QPushButton:focus:!hover:!pressed{border-image: url(%1);}").arg(m_focusImage.path()) +
                         QString("QPushButton{color: %1}").arg(strColor);
    this->setStyleSheet(styleSheet);
}

void Button::paintEvent(QPaintEvent *event)
{
    stylesheetUpdate();
    QPushButton::paintEvent(event);
}
