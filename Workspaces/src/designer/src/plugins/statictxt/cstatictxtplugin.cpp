#include "cstatictxt.h"
#include "cstatictxtplugin.h"

#include <QtPlugin>

CstatictxtPlugin::CstatictxtPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void CstatictxtPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool CstatictxtPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *CstatictxtPlugin::createWidget(QWidget *parent)
{
    return new Cstatictxt(parent);
}
/*
QString CstatictxtPlugin::name() const
{
    return QLatin1String("Cstatictxt");
}

QString CstatictxtPlugin::group() const
{
    return tr("static");
}
*/
QString CstatictxtPlugin::name() const
{
    return QLatin1String("Cstatictxt");
}

QString CstatictxtPlugin::group() const
{
    return tr("static");
}




QIcon CstatictxtPlugin::icon() const
{
    return QIcon(QStringLiteral("://images/statictxt.png"));
}

QString CstatictxtPlugin::domXml() const
{
    QString xmls = "<ui version =\"1.0\" displayname = \"";
    xmls += tr("Cstatictxt");
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
    xmls += "       <width>150</width>\n";
    xmls += "       <height>100</height>\n";
    xmls += "       </rect>\n";
    xmls += "   </property>\n";
    xmls += "</widget>\n";
    xmls += "</ui>\n";
    return xmls;
}

QString CstatictxtPlugin::toolTip() const
{
        return tr("静态文本图元");
}

QString CstatictxtPlugin::whatsThis() const
{
//    return QLatin1String("");
        return tr("静态文本图元");
}

bool CstatictxtPlugin::isContainer() const
{
    return false;
}

QString CstatictxtPlugin::includeFile() const
{
    return QLatin1String("cstatictxt.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(cstatictxtplugin, CstatictxtPlugin)
#endif // QT_VERSION < 0x050000
