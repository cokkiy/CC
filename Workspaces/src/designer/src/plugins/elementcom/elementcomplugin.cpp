#include "elementcom.h"
#include "elementcomplugin.h"

#include <QtPlugin>

ElementcomPlugin::ElementcomPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void ElementcomPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool ElementcomPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *ElementcomPlugin::createWidget(QWidget *parent)
{
    return new Elementcom(parent);
}

QString ElementcomPlugin::name() const
{
    return QLatin1String("Elementcom");
}

QString ElementcomPlugin::group() const
{
    return QLatin1String("Base");
}

QIcon ElementcomPlugin::icon() const
{
    return QIcon();
}

QString ElementcomPlugin::toolTip() const
{
    return QLatin1String("");
}

QString ElementcomPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool ElementcomPlugin::isContainer() const
{
    return false;
}

QString ElementcomPlugin::domXml() const
{
    return QLatin1String("<widget class=\"Elementcom\" name=\"elementcom\">\n</widget>\n");
}

QString ElementcomPlugin::includeFile() const
{
    return QLatin1String("elementcom.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(elementcomplugin, ElementcomPlugin)
#endif // QT_VERSION < 0x050000
