#include "taskinstance.h"
#include "../lib/shared/const_var.h"
#include <QFile>
#include <QDomDocument>
#include <QDebug>
const QString c_TaskInfo = "TaskInfo";
const QString c_Task = "Task";
const QString c_TaskId = "TaskId";
const QString c_TaskName = "TaskName";
const QString c_TaskCode = "TaskCode";
const QString c_TaskPhase = "TaskPhase";
const QString c_TaskPhaseName = "TaskPhaseName";
const QString c_CurTaskFlag = "CurrTaskFlag";
const QString c_IsCurTask = "true";

TaskInstance::TaskInstance()
{

}

QString TaskInstance::currentTaskPath()
{
    return m_currentTaskPath;
}

QString TaskInstance::currentTaskCode()
{
    return m_currentTaskCode;
}

int TaskInstance::load(QString dir)
{
    m_currentTaskPath = dir+QString("/../zx_config/");
    m_currentTaskCode = QString("");
    QFile file(dir+g_taskConfigRelativeFileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QDomDocument domDocument;
        QString errorStr;
        int errorLine;
        int errorColumn;
        if (!domDocument.setContent(&file, true, &errorStr, &errorLine,
                                    &errorColumn)) {
            qDebug() << tr("Task Configure File") << tr("Parse error at line %1, column %2:\n%3")
                        .arg(errorLine)
                        .arg(errorColumn)
                        .arg(errorStr);
        }
        else
        {
            QDomElement root = domDocument.documentElement();
            if (root.tagName() != c_TaskInfo) {
                qDebug() << tr("The file is not an Task Configure File");
            }
            QDomElement child = root.firstChildElement(c_Task);
            while (!child.isNull()) {
                QDomElement t_TaskPhase = child.firstChildElement(c_TaskPhase);
                while(!t_TaskPhase.isNull())
                {
                    QString t_CurTaskFlag = t_TaskPhase.firstChildElement(c_CurTaskFlag).text();
                    if (t_CurTaskFlag.compare(c_IsCurTask)==0)
                    {
                        m_currentTaskPath += child.firstChildElement(c_TaskCode).text();
                        m_currentTaskCode = child.firstChildElement(c_TaskCode).text();
                        file.close();
                        return 1;
                    }
                    t_TaskPhase = t_TaskPhase.nextSiblingElement(c_Task);
                }
                child = child.nextSiblingElement(c_Task);
            }
        }
        file.close();
    }
    else
    {
        qDebug() << tr("The Task Configure File ")
                 << g_taskConfigRelativeFileName
                 << tr(" canot open!!");
    }
    return -1;
}
