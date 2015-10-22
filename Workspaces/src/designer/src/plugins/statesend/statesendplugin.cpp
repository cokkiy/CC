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
    return QLatin1String("");
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
    return QLatin1String("<widget class=\"StateSend\" name=\"stateSend\">\n</widget>\n");
}

QString StateSendPlugin::includeFile() const
{
    return QLatin1String("statesend.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(statesendplugin, StateSendPlugin)
#endif // QT_VERSION < 0x050000
