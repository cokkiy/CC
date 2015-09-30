// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：xdocument.cpp
// 文件摘要：XDocument类实现，实现了加载xml文档并分析生产元素结构
//          提供了查找子元素的各种方法
//
// 原始版本：1.0
// 作   者：张立民
// 完成日期：2015-08-26
//**************************************************************//
#include "xdocument.h"
#include "xattribute.h"
#include <QDebug>
#include <stack>
#include <QTextCodec>
#include <QDebug>

XDocument::XDocument()
{

}

//从指定Io设备加载xml文档内容并分析产生文档元素结构
//如果文档格式正确，返回true，否则返回false，具体问题可看日志文件
bool XDocument::Load(QIODevice * device)
{
    Q_ASSERT(device->isOpen()&&device->isReadable()&&device->isTextModeEnabled());

    QTextStream stream(device);

    QString all=stream.readAll();

    //分析文件并生产xml元素节点
    return parse(all);
}


//从指定文件路径加载xml文档内容并分析产生文档元素结构
//如果文档不存在或格式不正确，返回false，具体问题可看日志文件，否则返回true
bool XDocument::Load(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        qCritical()<<"不能打开文件:"<<fileName<<"，请确认文件坐在。"<<endl;

        return false;
    }

    bool result=Load(&file);

    //关闭打开的文件
    file.close();

    return result;
}

//一个token结束，根据类型处理该token
bool XDocument::finishToken(int tokenType, stack<XAttribute>& unhandledAttributes,
                            stack<XElement>& unhandledElements, const QString& curToken)
{
    switch(tokenType)
    {
    case 0: //代表子节点名称
    {
        XElement element;
        element.Name=curToken;
        //加入到当前尚未处理完毕的子元素栈中
        unhandledElements.push(element);
    }
        break;
    case 1: //代表属性
    {
        XAttribute attribute;
        attribute.Name=curToken;
        //加入到当前尚未处理完毕的属性栈中
        unhandledAttributes.push(attribute);
    }
        break;
    case 2: //代表属性值
    {
        XAttribute attr=unhandledAttributes.top();
        attr.Value=curToken;
        unhandledAttributes.pop();

        XElement* element=&unhandledElements.top();
        element->addAttribute(attr);
    }
        break;
    case 3: //3:代表元素值；
    {
        XElement* element=&unhandledElements.top();
        element->Value=curToken;
    }
        break;
    case 4: //4：代表元素结束值（元素名称）
    {
        XElement element=unhandledElements.top();
        if(element.Name.compare(curToken,Qt::CaseInsensitive)!=0)
        {
            qCritical()<<"xml文档格式不正确，有未闭合的标签："<<element.Name<<endl;

            return false;
        }

        finishElement(unhandledElements);
    }
        break;
    }

    return true;
}


//关闭一个元素
void XDocument::finishElement(stack<XElement>& unhandledElements)
{
    XElement element = unhandledElements.top();
    unhandledElements.pop();
    if(unhandledElements.empty())
    {
        //最后一个元素，加入到XDocument
        this->m_child=element;
    }
    else
    {
        XElement* parent = &unhandledElements.top();
        parent->addChildElement(element);
    }
}

//分析xml文档内容，生成xml元素节点
bool XDocument::parse(const QString& content)
{
    //当前尚未处理完毕的子元素
    stack<XElement> unhandledElements;

    //当前尚未处理完毕的属性
    stack<XAttribute> unhandledAttributes;

    QString curToken="";

    //当前标识类型，0：代表元素名称；1;代表属性名称；2：代表属性值；
    //            3:代表元素值；4：代表元素结束值（元素名称）
    int tokenType=0;

    //当前字符的前一个字符
    QChar prevChar='\0';

    //是否是注释
    bool isComment=false;

    //是否xml首行
    bool isXmlSchemaLine=false;

    //处理位置是否正在<,>符号之中
    bool inLittleBiggerPair=false;

    //开始处理元素结束标志
    //bool beginEndTag=false;

    //处理位置是否正在属性值当中
    bool inAttrValue=false;

    //处理位置是否正在元素值当中
    bool inElementValue=false;

    //是否处理完当前标识
    bool finishedToken=true;

    //准备处理属性值了，但还没遇到，即刚遇到"="
    bool prepareAttrValue=false;

    //属性开始字符，以"（双引号）开始还是'单引号开始
    QChar attrValueBeginChar='"';

    //后续也许是元素的值
    bool mayElementValueFollowed=false;

    //后续也许是属性名称
    bool mayAttrNameFollowed=false;

    //完成标签结果
    bool finishTokenResult=true;

    //迭代器
    for(int index=0;index<content.length();index++)
    {
        QChar c=content[index];

        if(c=='\r'||c=='\n')
        {
            continue;
        }

        //如果是在注释中或者是在xml说明行中，则继续直到遇到结束符">"
        if((isComment||isXmlSchemaLine)&&c!='>')
        {
            continue;
        }


        if(c=='<')
        {
            //前面可能有元素的值
            if(inElementValue && !finishedToken)
            {
                finishTokenResult=finishToken(tokenType,unhandledAttributes,unhandledElements,curToken);
                curToken="";
                finishedToken=true;
                inElementValue=false;
            }

            //开始一个元素或者开始了一个元素结束符
            //如果后面跟的是'/'，则开始一个元素结束符，否则，开始了一个元素

            //已经移到"<"后面开始处理了
            inLittleBiggerPair=true;

            //置当前标识为""(空)
            curToken="";

            //首先，我们假设开始了一个元素，设置要处理的后续标识(token)为子节点名称
            //如果，后面是/，则再确定开始了元素结束符
            tokenType=0;

            //开始了元素，不可能是元素的值了
            mayElementValueFollowed=false;
        }
        else if(c==' ' || c=='\t') //空格或者制表符
        {
            //空格可以存在与除了元素名称和属性名称以外的任何地方,属性值、元素值内可以有空格
            if(inAttrValue || inElementValue)
            {
                //如果正在属性或者元素值当中，则保留空格
                curToken+=c;
            }
            else
            {
                //否则，只处理<，>之间的第一个空格，其他空格忽略
                if(inLittleBiggerPair && (prevChar!=' ' || prevChar!='\t'))
                {
                    //空格表示着一个token结束了
                    //排除紧跟着<后面的空格
                    if(curToken!="" && !finishedToken)
                    {
                        finishTokenResult=finishToken(tokenType, unhandledAttributes, unhandledElements, curToken);
                        finishedToken=true; //处理完当前标识了
                        curToken=""; //重置当前标识

                        //后面可能是属性名称
                        mayAttrNameFollowed=true;
                    }
                }
            }
        }
        else if(c=='/')
        {
            // 处理'/’字符，'/'字符位于元素结束位置
            // 可能的位置有两个,(1)<e attr="attr" value="e value"/>
            // 或者(2)<e attr="attr">value</e>
            //第二种情况下，'/'前面总是跟着<号，标志着开始了元素结束符
            //如果前面不是'<‘,则是第一种情况，标志着着当前元素结束了
            if(prevChar=='<')
            {
                //第二种情况
                //beginEndTag=true; //开始处理元素结束符，后面的字符是元素结束符
                tokenType=4; //后续标识是元素结束标志
            }
            else
            {
                //第一种情况，如果有没有处理完的标识，则处理完它
                if(!finishedToken)
                {
                    finishTokenResult=finishToken(tokenType,unhandledAttributes,unhandledElements,curToken);
                    finishedToken=true; //处理完当前标识了
                    curToken=""; //重置当前标识
                }

            }
        }
        else if(c=='=')
        {
            //“=”可能位于属性值或者元素值当中
            if(inAttrValue || inElementValue)
            {
                curToken+=c;
            }
            else
            {
                //也许属性名称到此结束
                if(curToken!="" && !finishedToken)
                {
                    finishTokenResult=finishToken(tokenType,unhandledAttributes,unhandledElements,curToken);
                    finishedToken=true; //处理完当前标识了
                    curToken=""; //重置当前标识
                }

                //“=”后面跟的是属性值，也可能是空格，空格后面才是属性值
                //当遇到""“或”’“后才开始真正的属性值
                prepareAttrValue=true;
            }
        }
        else if(c=='\''||c=='"')
        {
            //单引号和双引号可以位于许多地方

            if(prepareAttrValue)
            {
                //如果正在准备处理属性值，则遇到的第一个'或"符号表示属性值开始
                prepareAttrValue=false;
                attrValueBeginChar=c;
                tokenType=2; //属性值
                inAttrValue=true; //进入属性值

                prevChar=c;

                continue;
            }

            //在属性值中
            if(inAttrValue)
            {
                if(prevChar=='\\' || attrValueBeginChar!=c)
                {
                    //如果前面是转义字符或者与开始字符不同的引号，则是属性内容
                    curToken+=c;
                }
                else
                {
                    //前面不是转义字符并且与开始字符相同，代表着属性值结束
                    inAttrValue=false;
                    finishTokenResult=finishToken(tokenType,unhandledAttributes,unhandledElements,curToken);
                    finishedToken=true;
                    curToken="";
                }

            }
            else if(inElementValue)
            {
                //在元素值当中
                curToken+=c;
            }
        }
        else if(c=='\\')
        {
            //符号\,可能是转义字符，也可能是本身
            if(inAttrValue||inElementValue)
            {
                if(prevChar=='\\')
                {
                    curToken+=c;
                }
            }
            else
            {
                //否则，格式不正确
                qFatal("xml 文档格式不正确，有不可识别的字符（\\）");
                return false;

            }
        }

        else if(c=='!')
        {
            //开始注释
            isComment=true;
        }
        else if(c=='?')
        {
            //xml文件首行
            isXmlSchemaLine=true;
        }
        else if(c=='>')
        {
            //可能的位置,1) /> 元素结束
            //          2) >,用于闭合对应的<，后续可能开始子元素，也可能是元素的值，也可能是闭合元素结束符
            //             比如，<e><c>111</c></e>,<e attr="a"/>
            //             也有可能是注释<!--xxxxx --> 或者 <? edited with XMLSpy v2009 ?>

            //如果是注释行或者说明行，则到这儿就结束了
            if(isComment||isXmlSchemaLine)
            {
                //到这儿已经跳出注释和xml说明行了
                isComment=false;
                isXmlSchemaLine=false;
                prevChar='\0';
                continue;
            }

            // 不可能是属性名称了
            mayAttrNameFollowed=false;


            if(prevChar!='/')
            {
                //首先关闭当前标识，到这儿标识一定结束了
                if(!finishedToken)
                {
                    finishTokenResult=finishToken(tokenType,unhandledAttributes,unhandledElements,curToken);
                    curToken="";
                    finishedToken=true;
                }

                //后面可能是元素的值，也可能是子元素
                mayElementValueFollowed=true;

            }
            else
            {
                //该元素结束了，则
                finishElement(unhandledElements);
            }

            //不在<,>之间
            inLittleBiggerPair=false;
        }

        else //其他字符
        {
            //可能是元素值，也不是空格，则一定是元素值
            if(mayElementValueFollowed && c!=' ')
            {
                inElementValue=true;
                tokenType=3;
                mayElementValueFollowed=false;
            }

            if(mayAttrNameFollowed && c!=' ')
            {
                tokenType=1;
                mayAttrNameFollowed=false;
            }

            if(c==' ')
            {
                if(inAttrValue||inElementValue)
                {
                    curToken+=c;
                    finishedToken=false;
                }
            }
            else
            {
                curToken+=c;
                finishedToken=false;
            }
        }

        //检查标签完成结果,如果有问题，则直接返回结果，不继续处理
        if(!finishTokenResult)
            return false;

        //当前字符保存到上一个字符，准备开始处理下一字符
        prevChar=c;
    }

    return true;
}

//获取xml文档的第一级子元素
XElement XDocument::getChild() const
{
    return m_child;
}

