#include "rollinggraph.h"
#include "rollinggraphplugin.h"

#include <QtPlugin>

rollinggraphPlugin::rollinggraphPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void rollinggraphPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool rollinggraphPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *rollinggraphPlugin::createWidget(QWidget *parent)
{
    return new rollinggraph(parent);
}

QString rollinggraphPlugin::name() const
{
    return QLatin1String("rollinggraph");
}

/*
QString rollinggraphPlugin::group() const
{
    return QLatin1String("");
}
*/
QString rollinggraphPlugin::group() const
{
    return tr("graph");
}

QIcon rollinggraphPlugin::icon() const
{
    return QIcon();
}

QString rollinggraphPlugin::toolTip() const
{
    return QLatin1String("");
}

QString rollinggraphPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool rollinggraphPlugin::isContainer() const
{
    return false;
}

/*
QString rollinggraphPlugin::domXml() const
{
    return QLatin1String("<widget class=\"rollinggraph\" name=\"rollinggraph\">\n</widget>\n");
}
*/
QString rollinggraphPlugin::domXml() const
{
    QString xmls = "<ui version =\"1.0\" displayname = \"";
    xmls += tr("rollinggraph");
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
    xmls += "       <width>800</width>\n";
    xmls += "       <height>494</height>\n";
    xmls += "       </rect>\n";
    xmls += "   </property>\n";
    xmls += "</widget>\n";
    xmls += "</ui>\n";
    return xmls;
}

QString rollinggraphPlugin::includeFile() const
{
    return QLatin1String("rollinggraph.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(rollinggraphplugin, rollinggraphPlugin)
#endif // QT_VERSION < 0x050000
