#include "qsinglestate.h"
#include "qsinglestateplugin.h"

#include <QtPlugin>

QSingleStatePlugin::QSingleStatePlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void QSingleStatePlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool QSingleStatePlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *QSingleStatePlugin::createWidget(QWidget *parent)
{
    return new QSingleState(parent);
}

QString QSingleStatePlugin::name() const
{
    return QLatin1String("QSingleState");
}

QString QSingleStatePlugin::group() const
{
    return tr("dynamic");
}

QIcon QSingleStatePlugin::icon() const
{
    return QIcon(QStringLiteral("://images/singleState.png"));
}

QString QSingleStatePlugin::toolTip() const
{
    return QLatin1String("");
}

QString QSingleStatePlugin::whatsThis() const
{
    return QLatin1String("");
}

bool QSingleStatePlugin::isContainer() const
{
    return false;
}

QString QSingleStatePlugin::domXml() const
{
    QString xmls = "<ui version =\"1.0\" displayname = \"";
    xmls += tr("QSingleState");
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

QString QSingleStatePlugin::includeFile() const
{
    return QLatin1String("qsinglestate.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(qsinglestateplugin, QSingleStatePlugin)
#endif // QT_VERSION < 0x050000
