#include "paramstyleconfig.h"
#include  "selfshare/src/config/xdocument.h"
#include <QDir>
#include <QDebug>
QParamStyleConfig::QParamStyleConfig()
{
    m_ParamIndexList.clear();
    m_ParamStyleList.clear();
}
bool QParamStyleConfig::LoadParamFromXML(const QString& dir)
{
    bool bResult = true;
    m_ParamIndexList.clear();
    //目录
    QDir qDir(dir);
    QFileInfoList fileList = qDir.entryInfoList();
    for (auto file : fileList)
    {
        //**********装载style_global_***.xml等文件***//
        if (file.fileName().startsWith(QObject::tr("style_global_"), Qt::CaseInsensitive)
            && file.fileName().endsWith(QObject::tr(".xml"), Qt::CaseInsensitive))
        {
            //通过文件名获取域名，style_global_ZX.xml的域名为ZX
			QString domainStr = file.fileName();
            domainStr.remove(0,QObject::tr("style_global_").length());
            domainStr = domainStr.left(domainStr.length() - QObject::tr(".xml").length());
            domainStr = QObject::tr("#") + domainStr + QObject::tr(".");
            XDocument doc;
            if (!doc.Load(file.absoluteFilePath()))
            {
                qWarning() << QObject::tr("Load file: %1 failure.").arg(file.absoluteFilePath());
                bResult = false;
            }
			//读取XML文件索引列表，写入索引链表
            ElementList indexList = doc.getChild().getChildrenByName(QObject::tr("索引列表"));
            for (XElement parameter : indexList)
            {
                ElementList Indexs = parameter.childElements;
                for (XElement parameter : Indexs)
                {
                    QParamIndex pIndex;
                    pIndex.setIndexName(domainStr + parameter.getAttributeValue(QObject::tr("名称")));
                    pIndex.setText(parameter.getChildValue(QObject::tr("文本")));
                    pIndex.setColorStr(parameter.getChildValue(QObject::tr("颜色")));
                    pIndex.setFontNameStr(parameter.getChildValue(QObject::tr("字体名称")));
                    pIndex.setFontSizeStr(parameter.getChildValue(QObject::tr("字体大小")));
                    pIndex.setFontBoldStr(parameter.getChildValue(QObject::tr("粗体")));
                    pIndex.setFontItalicStr(parameter.getChildValue(QObject::tr("斜体")));
                    m_ParamIndexList.push_back(pIndex);
                }
            }
			//读取XML文件样式列表，写入样式链表
            ElementList styleList = doc.getChild().getChildrenByName(QObject::tr("样式列表"));
            for (XElement parameter : styleList)
            {
                ElementList styles = parameter.childElements;
                for (XElement parameter : styles)
                {
                    QParamStyle paramStyle;
                    paramStyle.setStyleName(domainStr + parameter.getAttributeValue(QObject::tr("名称")));
                    ElementList childstyles = parameter.childElements;
					//子样式
                    for (XElement parameter : childstyles)
                    {
                        QString index = parameter.getChildValue(QObject::tr("索引名称"));
                        index = domainStr + index;
                        double max = parameter.getChildDoubleValue(QObject::tr("值上限"));
                        double min = parameter.getChildDoubleValue(QObject::tr("值下限"));
                        paramStyle.addParamStyle(index,max,min);
                    }
                    m_ParamStyleList.push_back(paramStyle);
                }
            }
        }
    }
    return bResult;
}
//获取样式索引链表
list<QParamStyle>* QParamStyleConfig::getParamStyleList()
{
    return &m_ParamStyleList;
}
//样式链表
list<QParamIndex> *QParamStyleConfig::getParamIndexList()
{
    return &m_ParamIndexList;
}
