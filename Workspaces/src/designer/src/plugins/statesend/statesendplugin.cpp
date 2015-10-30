#include "statesend.h"
#include "statesendplugin.h"

#include <QtPlugin>

StateSendPlugin::StateSendPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void StateSendPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool StateSendPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *StateSendPlugin::createWidget(QWidget *parent)
{
    return new StateSend(parent);
}

QString StateSendPlugin::name() const
{
    return QLatin1String("StateSend");
}

QString StateSendPlugin::group() const
{
    return tr("command");
}

QIcon StateSendPlugin::icon() const
{
    return QIcon("://button_icon.png");
}

QString StateSendPlugin::toolTip() const
{
    return QLatin1String("");
}

QString StateSendPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool StateSendPlugin::isContainer() const
{
    return false;
}

QString StateSendPlugin::domXml() const
{
    QString xmls = "<ui version =\"1.0\" displayname = \"";
    xmls += tr("StateSend");
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

QString StateSendPlugin::includeFile() const
{
    return QLatin1String("statesend.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(statesendplugin, StateSendPlugin)
#endif // QT_VERSION < 0x050000
