#include "tableproperty.h"

QTableProperty::QTableProperty()
{
    m_iRowNum = 5;
    m_iColNum = 4;
    m_TableVector.clear();
}
//设置表格的行数
void QTableProperty::setRowNum(const qint32 num)
{
	m_iRowNum = num;
}
//设置表格的列数
void QTableProperty::setColNum(const qint32 num)
{
	m_iColNum = num;
}
//获取表格的高度和宽度
QSize QTableProperty::size()
{
    int height = 0;
    int width = 0;
    for(int i = 0; i < m_iRowNum; i++)
    {
        QTableCell TableCell = m_TableVector.at(i*m_iColNum);
        height += TableCell.getHeight();
    }
    for(int j = 0;j< m_iColNum; j++)
    {
        QTableCell TableCell = m_TableVector.at(j);
        width += TableCell.getWidth();
    }
    QSize size(width,height);
    return size;
}
//设置指定列的宽度
void QTableProperty::setColumnWidth(int column,int width)
{
    column = column - 1;
    if(column < 0)
        column = 0;
    if(column > m_iColNum - 1)
        column = m_iColNum - 1;
    if(width <= 0)
        width = 100;
    for(int i = 0;i<m_iRowNum;i++)
    {
        QTableCell TableCell = m_TableVector.at(i*m_iColNum + column);
        TableCell.setWidth(width);
        m_TableVector.replace(i*m_iColNum + column,TableCell);
    }
}
//设置指定行的高度
void QTableProperty::setRowHeight(int row,int height)
{
    row = row - 1;
    if(row < 0)
        row = 0;
    if(row > m_iRowNum - 1)
        row = m_iRowNum -1 ;
    if(height <= 0)
        height = 20;
    for(int i = 0;i<m_iColNum;i++)
    {
        QTableCell TableCell = m_TableVector.at(row*m_iColNum);
        TableCell.setHeight(height);
        m_TableVector.replace(row*m_iColNum,TableCell);
    }
}
//设置表格的高度和宽度
void QTableProperty::setSize(const int width,const int height)
{
    QSize size = this->size();
    float wRatio = (float)width/size.width();
    float hRatio = (float)height/size.height();
    int totalHeight = 0;
    for(int i = 0; i < m_iRowNum; i++)
    {
        QTableCell TableCell = m_TableVector.at(i*m_iColNum);
        int totalWidth = 0;
        for(int j = 0;j< m_iColNum; j++)
        {
            TableCell = m_TableVector.at(i*m_iColNum + j);
            if(j == m_iColNum- 1)
                TableCell.setWidth(width - totalWidth);
            else
                TableCell.setWidth(TableCell.getWidth()*wRatio);
            if(i == m_iRowNum - 1)
                TableCell.setHeight(height - totalHeight);
            else
                TableCell.setHeight(TableCell.getHeight()*hRatio);
            m_TableVector.replace(i*m_iColNum + j,TableCell);
            TableCell = m_TableVector.at(i*m_iColNum + j);
            totalWidth += TableCell.getWidth();
        }
        TableCell = m_TableVector.at(i*m_iColNum);
        totalHeight += TableCell.getHeight();
    }
}
//追加行
void QTableProperty::addRow()
{
    for(int i = 0;i<m_iColNum;i++)
    {
        QTableCell TableCell;
        QTableCell upTableCell = m_TableVector.at((m_iRowNum-1)*m_iColNum+i);
        TableCell.setTextType(upTableCell.getTextType());
        TableCell.setHeight(upTableCell.getHeight());
        TableCell.setWidth(upTableCell.getWidth());
        m_TableVector.push_back(TableCell);
    }
    m_iRowNum++;
}
//插入行,鼠标位置所在行的上一行
void QTableProperty::insertRow(const int currentRow)
{
    for(int i = 0;i<m_iColNum;i++)
    {
        QTableCell TableCell;
        QTableCell currentTableCell = m_TableVector.at(currentRow*m_iColNum+i*2);
        TableCell.setTextType(currentTableCell.getTextType());
        TableCell.setHeight(currentTableCell.getHeight());
        TableCell.setWidth(currentTableCell.getWidth());
        m_TableVector.insert(m_TableVector.begin() + currentRow*m_iColNum + i,TableCell);
    }
    m_iRowNum++;
}
//删除鼠标位置所在行
void QTableProperty::delRow(const int currentRow)
{
    for(int i = 0;i<m_iColNum;i++)
        m_TableVector.erase(m_TableVector.begin() + currentRow*m_iColNum);
    m_iRowNum--;
    if(m_iRowNum == 0)
        m_iColNum = 0;
}
//追加列
void QTableProperty::addColumn()
{
    for(int i = 0;i<m_iRowNum;i++)
    {
        QTableCell TableCell;
        QTableCell rightTableCell = m_TableVector.at((i+1)*m_iColNum - 1 + i);
        TableCell.setParamStr(rightTableCell.getParamStr());
        TableCell.setFormulaStr(rightTableCell.getFormulaStr());
        TableCell.setHeight(rightTableCell.getHeight());
        TableCell.setWidth(rightTableCell.getWidth());
        m_TableVector.insert(m_TableVector.begin() + (i+1)*m_iColNum + i,TableCell);
    }
    m_iColNum++;
}
//插入列,鼠标位置所在列的左侧一列
void QTableProperty::insertColumn(const int currentCol)
{
    for(int i = 0;i<m_iRowNum;i++)
    {
        QTableCell TableCell;
        QTableCell currentTableCell = m_TableVector.at(i*m_iColNum + currentCol + i);
        TableCell.setParamStr(currentTableCell.getParamStr());
        TableCell.setFormulaStr(currentTableCell.getFormulaStr());
        TableCell.setHeight(currentTableCell.getHeight());
        TableCell.setWidth(currentTableCell.getWidth());
        m_TableVector.insert(m_TableVector.begin() + i*m_iColNum + currentCol + i,TableCell);
    }
    m_iColNum++;
}
//删除鼠标位置所在列
void QTableProperty::delColumn(const int currentCol)
{
    for(int i = 0;i<m_iRowNum;i++)
        m_TableVector.erase(m_TableVector.begin() + i*m_iColNum + currentCol - i);
    m_iColNum--;
    if(m_iColNum == 0)
        m_iRowNum = 0;
}
//获取表格的属性数组
QVector<QTableCell>*  QTableProperty::getTableVector()
{
    return &m_TableVector;
}
//将表格属性转换为Json字符串，该Json字符串包括表格的行列数和所有单元格的属性
QString QTableProperty::toJsonStr(float wr,float hr)
{
    QString jsonStr = QObject::tr("");
    QJsonArray stateArray;//Json数组
    for (int i = 0; i < m_iRowNum; i++)
    {
        QTableCell TableCell = m_TableVector.at(i*m_iColNum);
        int height = TableCell.getHeight()*hr;
        for (int j = 0; j < m_iColNum; j++)
            {
                QJsonObject stateObject;
                TableCell = m_TableVector.at(j);
                int width = TableCell.getWidth()*wr;
                TableCell = m_TableVector.at(i*m_iColNum + j);
                stateObject[QLatin1String("Param")] = TableCell.getParamStr();
                stateObject[QLatin1String("Formula")] = TableCell.getFormulaStr();
                stateObject[QLatin1String("ShowType")] = TableCell.getShowType();
                stateObject[QLatin1String("Text")] = TableCell.getText();
                stateObject[QLatin1String("TextType")] = TableCell.getTextType();
                stateObject[QLatin1String("TextColor")] = (int)TableCell.getTextColor().rgb();
                stateObject[QLatin1String("Font")] = TableCell.getTextFont().toString();
                stateObject[QLatin1String("TextVAlignment")] = (int)TableCell.getTextVAlignment();
                stateObject[QLatin1String("TextHAlignment")] = (int)TableCell.getTextHAlignment();
                stateObject[QLatin1String("BGColor")] = (int)TableCell.getBackgroundColor().rgba();
                stateObject[QLatin1String("height")] = height;
                stateObject[QLatin1String("width")] = width;
                stateObject[QLatin1String("TopEdgeVisible")] = (int)TableCell.getTopEdgeVisible();
                stateObject[QLatin1String("LeftEdgeVisible")] = (int)TableCell.getLeftEdgeVisible();
                stateObject[QLatin1String("LeftMargin")] = TableCell.getLeftMargin();
                stateObject[QLatin1String("RightMargin")] = TableCell.getRightMargin();
                stateObject[QLatin1String("TopMargin")] = TableCell.getTopMargin();
                stateObject[QLatin1String("BottomMargin")] = TableCell.getBottomMargin();
                stateArray.append(stateObject);//将每一行Json字符串添加到Json数组
        }
    }
    QJsonDocument doc2(stateArray);
    QByteArray byte_array = doc2.toJson(QJsonDocument::Compact);
	//在单元格属性json字符串前插入行列数字符串
    jsonStr = QString(("%1,%2")).arg(m_iRowNum).arg(m_iColNum);
    jsonStr += QString(byte_array);//将构建的Json数组赋值给接口接收字符串
    return jsonStr;
}
//解析传入的字符串，该字符串包括表格的行列数和所有单元格的属性
bool QTableProperty::analyzeTextStr(const QString textStr)
{
    if(!textStr.isEmpty())
    {
        //解析表格的行列数
		QString str = textStr;
        int pos = str.indexOf(',');
        m_iRowNum = str.left(pos).toInt();
        str = str.right(str.length()-pos-1);
        pos = str.indexOf('[');
        m_iColNum = str.left(pos).toInt();
        str = str.right(str.length()-pos);
        if(m_iRowNum > 100000 || m_iRowNum < 1 ||
           m_iColNum > 100000 || m_iColNum < 1)
            return false;
        return initTableVector(str);
    }
    return false;
}
//将传入的字符串转换为表格属性
void QTableProperty::setTableVector(const QString textStr)
{
    m_TableVector.clear();
    bool bRight = false;
    if(!textStr.isEmpty())
    {
       bRight = analyzeTextStr(textStr);
       if(m_TableVector.count() != m_iRowNum* m_iColNum)
           bRight = false;
    }
    if(!bRight)
    {
        for(int i = 0;i<m_iRowNum;i++)
            for(int j = 0;j<m_iColNum;j++)
            {
                QTableCell TableCell;
                m_TableVector.push_back(TableCell);
            }
    }
}
//将Json字符串转换为表格单元属性
bool QTableProperty::initTableVector(const QString jsonStr)
{
    QJsonParseError jerr;
	//设置编码格式
    QJsonDocument parse_document = QJsonDocument::fromJson(jsonStr.toUtf8(), &jerr);
    if(jerr.error == QJsonParseError::NoError)
    {
        if(parse_document.isArray())
        {
            QJsonArray array=parse_document.array();   //将QJsonDocument字符串转换为QJsonArray字符串
            int size=array.size();     //行数
            for(int i=0;i<size;i++)
            {
                QJsonValue value = array.at(i);   //每一行的QJsonObject类型Json字符串
                if(value.isObject())
                {
                    QJsonObject name = value.toObject();
                    QTableCell TableCell;
                    TableCell.setParamStr(name.value(QLatin1String("Param")).toString());
                    TableCell.setFormulaStr((name.value(QLatin1String("Formula")).toString()));
                    TableCell.setText((name.value(QLatin1String("Text")).toString()));
                    TableCell.setShowType(name.value(QLatin1String("ShowType")).toString());
                    TableCell.setTextType(name.value(QLatin1String("TextType")).toInt());
                    QColor color;
                    color.setRgb(name.value(QLatin1String("TextColor")).toInt());
                    TableCell.setTextColor(color);
                    QFont font;
                    font.fromString(name.value(QLatin1String("Font")).toString());
                    TableCell.setTextFont(font);
                    TableCell.setTextHAlignment((Qt::Alignment)name.value(QLatin1String("TextHAlignment")).toInt());
                    TableCell.setTextVAlignment((Qt::Alignment)name.value(QLatin1String("TextVAlignment")).toInt());
                    color.setRgba(name.value(QLatin1String("BGColor")).toInt());
                    TableCell.setBackgroundColor(color);
                    TableCell.setHeight(name.value(QLatin1String("height")).toInt());
                    TableCell.setWidth(name.value(QLatin1String("width")).toInt());
                    TableCell.setTopEdgeVisible(name.value(QLatin1String("TopEdgeVisible")).toInt());
                    TableCell.setLeftEdgeVisible(name.value(QLatin1String("LeftEdgeVisible")).toInt());
                    TableCell.setLeftMargin(name.value(QLatin1String("LeftMargin")).toInt());
                    TableCell.setRightMargin(name.value(QLatin1String("RightMargin")).toInt());
                    TableCell.setTopMargin(name.value(QLatin1String("TopMargin")).toInt());
                    TableCell.setBottomMargin(name.value(QLatin1String("BottomMargin")).toInt());
                    m_TableVector.push_back(TableCell);
                }
            }
            return true;
        }
       return false;
    }
    return false;
}
