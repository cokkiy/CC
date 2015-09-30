#include "button.h"
#include "buttonplugin.h"

#include <QtPlugin>

ButtonPlugin::ButtonPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void ButtonPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool ButtonPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *ButtonPlugin::createWidget(QWidget *parent)
{
    return new Button(parent);
}

QString ButtonPlugin::name() const
{
    return QLatin1String("Button");
}

QString ButtonPlugin::group() const
{
    return tr("button");
}

QIcon ButtonPlugin::icon() const
{
    return QIcon();
}

QString ButtonPlugin::domXml() const
{
    QString xmls = "<ui version =\"1.0\" displayname = \"";
    xmls += tr("Button");
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
    xmls += "       <width>100</width>\n";
    xmls += "       <height>50</height>\n";
    xmls += "       </rect>\n";
    xmls += "   </property>\n";
    xmls += "</widget>\n";
    xmls += "</ui>\n";
    return xmls;
}

QString ButtonPlugin::toolTip() const
{
    return QLatin1String("");
}

QString ButtonPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool ButtonPlugin::isContainer() const
{
    return false;
}

QString ButtonPlugin::includeFile() const
{
    return QLatin1String("button.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(buttonplugin, ButtonPlugin)
#endif // QT_VERSION < 0x050000
