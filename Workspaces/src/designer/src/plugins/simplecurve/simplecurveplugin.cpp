#include "simplecurve.h"
#include "simplecurveplugin.h"

#include <QtPlugin>

SimpleCurvePlugin::SimpleCurvePlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void SimpleCurvePlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool SimpleCurvePlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *SimpleCurvePlugin::createWidget(QWidget *parent)
{
    return new SimpleCurve(parent);
}

QString SimpleCurvePlugin::name() const
{
    return QLatin1String("SimpleCurve");
}

QString SimpleCurvePlugin::group() const
{
    return QLatin1String("Base");
}

QIcon SimpleCurvePlugin::icon() const
{
    return QIcon();
}

QString SimpleCurvePlugin::toolTip() const
{
    return QLatin1String("");
}

QString SimpleCurvePlugin::whatsThis() const
{
    return QLatin1String("");
}

bool SimpleCurvePlugin::isContainer() const
{
    return false;
}

QString SimpleCurvePlugin::domXml() const
{
    return QLatin1String("<widget class=\"SimpleCurve\" name=\"simpleCurve\">\n</widget>\n");
}

QString SimpleCurvePlugin::includeFile() const
{
    return QLatin1String("simplecurve.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(simplecurveplugin, SimpleCurvePlugin)
#endif // QT_VERSION < 0x050000
