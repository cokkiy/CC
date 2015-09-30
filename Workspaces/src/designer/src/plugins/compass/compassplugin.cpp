#include "compass.h"
#include "compassplugin.h"

#include <QtPlugin>

compassPlugin::compassPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void compassPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool compassPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *compassPlugin::createWidget(QWidget *parent)
{
    return new compass(parent);
}

QString compassPlugin::name() const
{
    return QLatin1String("compass");
}

QString compassPlugin::group() const
{
    return QLatin1String("");
}

QIcon compassPlugin::icon() const
{
    return QIcon();
}

QString compassPlugin::toolTip() const
{
    return QLatin1String("");
}

QString compassPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool compassPlugin::isContainer() const
{
    return false;
}

QString compassPlugin::domXml() const
{
    return QLatin1String("<widget class=\"compass\" name=\"compass\">\n</widget>\n");
}

QString compassPlugin::includeFile() const
{
    return QLatin1String("compass.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(compassplugin, compassPlugin)
#endif // QT_VERSION < 0x050000
