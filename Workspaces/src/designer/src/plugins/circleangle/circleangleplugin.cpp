#include "circleangle.h"
#include "circleangleplugin.h"

#include <QtPlugin>

circleanglePlugin::circleanglePlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void circleanglePlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool circleanglePlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *circleanglePlugin::createWidget(QWidget *parent)
{
    return new circleangle(parent);
}

QString circleanglePlugin::name() const
{
    return QLatin1String("circleangle");
}

QString circleanglePlugin::group() const
{
    return tr("static");
}

QIcon circleanglePlugin::icon() const
{
    return QIcon(QStringLiteral("://images/circle.png"));
}

QString circleanglePlugin::domXml() const
{
    QString xmls = "<ui version =\"1.0\" displayname = \"";
    xmls += tr("circleangle");
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
    xmls += "       <height>100</height>\n";
    xmls += "       </rect>\n";
    xmls += "   </property>\n";
    xmls += "</widget>\n";
    xmls += "</ui>\n";
    return xmls;
}

QString circleanglePlugin::toolTip() const
{
    return QLatin1String("circle");
}

QString circleanglePlugin::whatsThis() const
{
    return QLatin1String("");
}

bool circleanglePlugin::isContainer() const
{
    return false;
}

QString circleanglePlugin::includeFile() const
{
    return QLatin1String("circleangle.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(circleangleplugin, circleanglePlugin)
#endif // QT_VERSION < 0x050000
