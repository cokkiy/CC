#include "singleparam.h"
#include "qsingleparam.h"
#include "singleparamplugin.h"

#include <QtPlugin>

SingleParamPlugin::SingleParamPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void SingleParamPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool SingleParamPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *SingleParamPlugin::createWidget(QWidget *parent)
{
    return new QSingleParam(parent);
}

QString SingleParamPlugin::name() const
{
    return QLatin1String("QSingleParam");
}

QString SingleParamPlugin::group() const
{
    return tr("dynamic");
}

QIcon SingleParamPlugin::icon() const
{
    return QIcon();
}

QString SingleParamPlugin::toolTip() const
{
    return QLatin1String("");
}

QString SingleParamPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool SingleParamPlugin::isContainer() const
{
    return false;
}

QString SingleParamPlugin::domXml() const
{
    return QLatin1String("<widget class=\"QSingleParam\" name=\"QSingleParam\">\n</widget>\n");
}

QString SingleParamPlugin::includeFile() const
{
    return QLatin1String("qsingleparam.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(singleparamplugin, SingleParamPlugin)
#endif // QT_VERSION < 0x050000
