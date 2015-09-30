/****************************************************************************
 ** 文件名：rectangleplugin.cpp
 ** Rectangle插件设计器支持类实现，实现在设计器上设计时显示Rectangle外观，属性
 ** 版本：v1.0
 ** 作者：张立民
 ** 2015-08-13
****************************************************************************/

#include "rectangle.h"
#include "rectangleplugin.h"

#include <QtPlugin>

RectanglePlugin::RectanglePlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void RectanglePlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool RectanglePlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *RectanglePlugin::createWidget(QWidget *parent)
{
    return new Rectangle(parent);
}

QString RectanglePlugin::name() const
{
    return QLatin1String("Rectangle");
}

QString RectanglePlugin::group() const
{
    return tr("static");
}

QIcon RectanglePlugin::icon() const
{
    return QIcon();
}

QString RectanglePlugin::domXml() const
{
    QString xmls = "<ui version =\"1.0\" displayname = \"";
    xmls += tr("Rectangle");
    xmls += "\">\n";
    xmls += "<widget class=\"";
    xmls += name();
    xmls += "\" name=\"";
    xmls += name();
    xmls += "\">\n";
    xmls += "   <property name=\"geometry\">\n";
    xmls += "       <rect>\n";
    xmls += "       <x>0</x>\n";
    xmls += "       <y>0</y>\n";
    xmls += "       <width>150</width>\n";
    xmls += "       <height>100</height>\n";
    xmls += "       </rect>\n";
    xmls += "   </property>\n";
    xmls += "</widget>\n";
    xmls += "</ui>\n";
    return xmls;
}

QString RectanglePlugin::toolTip() const
{
    return QLatin1String("绘制一个可以填充的矩形");
}

QString RectanglePlugin::whatsThis() const
{
    return QLatin1String("绘制一个可以填充的矩形");
}

bool RectanglePlugin::isContainer() const
{
    return false;
}

QString RectanglePlugin::includeFile() const
{
    return QLatin1String("rectangle.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Rectangle, RectanglePlugin)
#endif // QT_VERSION < 0x050000
