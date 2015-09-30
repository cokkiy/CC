#include "q2wmap.h"
#include "q2wmapplugin.h"

#include <QtPlugin>

Q2wmapPlugin::Q2wmapPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void Q2wmapPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool Q2wmapPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *Q2wmapPlugin::createWidget(QWidget *parent)
{
    return new Q2wmap(parent);
}

QString Q2wmapPlugin::name() const
{
    return QLatin1String("Q2wmap");
}

QString Q2wmapPlugin::group() const
{
    return QLatin1String("map");
}

QIcon Q2wmapPlugin::icon() const
{
    return QIcon(QStringLiteral("://images/q2wmap.png"));
}

QString Q2wmapPlugin::toolTip() const
{
    return "二维地图图元";
}

QString Q2wmapPlugin::whatsThis() const
{
    return "二维地图图元";
}

bool Q2wmapPlugin::isContainer() const
{
    return false;
}

QString Q2wmapPlugin::domXml() const
{
    return QLatin1String("<widget class=\"Q2wmap\" name=\"Q2wmap\">\n</widget>\n");
}

QString Q2wmapPlugin::includeFile() const
{
    return QLatin1String("q2wmap.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Q2wmapPlugin, Q2wmapPlugin)
#endif // QT_VERSION < 0x050000
