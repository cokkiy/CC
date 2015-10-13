// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：xmlproc.cpp
// 文件摘要：综合应用分系统xml文件处理类实现文件，用来实现xml文件处理类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：


#include "xmlproc.h"
//*************************************
//初始化成员变量
//
//
//*************************************
XmlProc::XmlProc()		//构造函数
{
	m_elGrade = 0;
	m_elSerialNo = 1;
	m_xmlFileContent = "";
	m_pos = 0;
}

//*************************************
//清空xml元素队列和元素名称栈
//
//
//*************************************
XmlProc::~XmlProc()
{
	m_elGrade = 0;
	m_elSerialNo = 1;
	m_xmlFileContent = "";
	m_pos = 0;
	m_elSave.clear();
	while(false == m_elStack.empty()) 
	{
		m_elStack.pop();
	}
}

//********************************************
//将xml文件的内容加载到内存中，方便进行解析
//
//
//********************************************
bool XmlProc::LoadSource(const char *path)
{
	char c = ' ';
	FILE *file;
	if(NULL !=path)
	{
		file = fopen(path, "r");
		if(NULL == file) return false;
		else						//加载过程
		{
			c = fgetc(file);
			while(0 == feof(file)) 
			{
				m_xmlFileContent.append(1,c);
				c = fgetc(file);
			}
			fclose(file);
			return true;
		}
	}	
	else	
	{
		return false;
	}
}

//*************************************
//关闭xml文件，并清空各成员变量
//
//
//*************************************
void XmlProc::Unload()
{
	m_elGrade = 0;
	m_elSerialNo = 1;
	m_xmlFileContent = "";
	m_pos = 0;
	m_elSave.clear();
	while(false == m_elStack.empty()) 
	{
		m_elStack.pop();
	}
}

//*************************************
//判断xml文件中各元素
//
//
//*************************************
unsigned short  XmlProc::JudgeElement()
{
	char c = ' ';
	unsigned short tempVal = 0;
	if(m_pos < m_xmlFileContent.length()) 
	{
		c = XmlGetChar();
		switch(c)
		{
			case '<': if(m_pos < m_xmlFileContent.length()) c = XmlGetChar();
								else break;
					  switch(c)
					  {
						case '?': tempVal = 1;break;							//xml处理指令
						case '!': tempVal = 2;break;							//注释
								  							
						case 47 : tempVal = 5;break;              //   '/'  元素结尾
 						default:  m_pos--;							          //元素开头		
								  		tempVal = 4;
								  		break;
					  }
					  break;
			default:  m_pos--;															//元素内容	
					  		tempVal = 3;  
					  		break;
		}
	}
	return tempVal;
}

//*************************************
//xml文件内容解析
//
//
//*************************************
bool XmlProc::XmlParse()										
{	
	unsigned short flag=0;
	char c = ' ';
	if(m_xmlFileContent.length() > 0)
	{
		while('<' != c)																							//过滤xml文件中开始的无效字符，xml文件中只有第一个'<'前面可以有无效字符
		{
			if(m_pos >= m_xmlFileContent.length()) return false; 
			c = XmlGetChar();
		}
		m_pos--;
		while(m_pos < m_xmlFileContent.length())										//主要解析过程
		{
			flag = JudgeElement();
			switch(flag)
			{
			case 1:	if(false == FilterHead()) return false;						//解析处理指令
							break;	
			case 2: if(false == FilterNote()) return false;           //过滤注释
							break;
			case 3: if(false == GetContentInfo()) return false;				//提取夹在子元素之间的上一级的内容
							break;
			case 4: if(false == GetElementInfo()) return false;				//处理元素（嵌套的）包括元素属性、内容和叶子元素的元素尾		
							break;
			case 5: if(false == MoElementEnd()) return false;					//处理非叶子元素的元素尾	
							break;
			default:return false;
			}
		SkipSpace();																								//过滤空白字符
		}
		if(false == m_elStack.empty()) return false;								//元素头尾不匹配
	}
	else	return false; 	
	return true;
}

//*************************************
//提取夹在子元素之间的上一级的内容
//
//
//*************************************
bool XmlProc::GetContentInfo()
{
	XmlElement xmlEl;
	char c = ' ';
	unsigned short flag = 0;
	string str, strWithSpace;

	c = XmlGetChar();																			//读取一个字符
	while('<' != c)																							//提取元素内容
	{
		if(m_pos >= m_xmlFileContent.length()) return false;			//如果到文件为则退出
		str.append(1,c);
		c = XmlGetChar();
		while((c == 0x20)||(c == 0x9)||(c == 0xD)||(c == 0xA))  	//判断空白是内容中的还是内容后的，内容中的保留，内容后的删除
		{
			if(m_pos >= m_xmlFileContent.length()) return false;
			strWithSpace.append(1,c);
			c = XmlGetChar();
		}
		if(('<' != c)&&(strWithSpace.length()>0))
		{
			str.append(strWithSpace);																//保留内容中间部分的空白
			strWithSpace = "";
		}
		while('<' == c)																						//如果内容中混有注释先过滤注释
		{
			m_pos--;
			flag = JudgeElement();																	//判断元素的类型
			switch(flag)
			{
				case 2: if(false == FilterNote()) return false;      	//过滤内容中和内容后的注释
								else 
								{
									SkipSpace();																//过滤空白
									if(m_pos >= m_xmlFileContent.length()) return false;
									c = XmlGetChar();
								}
									break;
				case 4:	break;
				default :return false;
			}
			if(2 != flag) break;
		}
	}
	xmlEl.m_val = str;
	xmlEl.m_grade = m_elGrade-1;
	xmlEl.m_serialNo = m_elSerialNo;
	m_elSave.push_back(xmlEl);
	m_pos--;

	return true;
}

//*************************************
//处理非叶子元素的元素尾
//
//
//*************************************
bool XmlProc::MoElementEnd()
{
	char c = ' ';
	string str,name;
	
	if(m_pos >= m_xmlFileContent.length()) return false;
	c = XmlGetChar();

	while((c != 0x20)&&(c != 0x9)&&(c != 0xD)&&(c != 0xA)&&(c != '>'))		//获取元素尾中的元素名
	{
		if((c == '<')||(m_pos >= m_xmlFileContent.length())) return false;
		str.append(1,c);
		c = XmlGetChar();
	}
	name = m_elStack.top();																		//获取栈顶元素名
	if(0 == name.compare(str)) m_elStack.pop();								//两个元素名相同则将栈顶弹出		
	else																											//否则报错
	{
		return false;
	}
	m_elGrade--;																								//元素级别减一，级别最高的为叶子元素，最低的0为根元素
	return true;
}

//*************************************
//过滤注释
//
//
//*************************************
bool XmlProc::FilterNote()
{
	char c = ' ';
	string str;
	if(m_pos < m_xmlFileContent.length())
	{
		c = XmlGetChar(); 
		if('-' != c) return false;
		c = XmlGetChar(); 
		if('-' != c) return false;
		c = XmlGetChar(); 
		while(1)
		{
			while('-' != c)
			{
				if(m_pos >= m_xmlFileContent.length()) return false;		//注释格式不全返回false
				c=XmlGetChar();
			}
			c = XmlGetChar(); 
			if('-' != c) continue;
			c=XmlGetChar();
			if('>' != c) return false;
			break;
		}
//		while('>' != c)
//		{
//			if((c == '<')||(m_pos >= m_xmlFileContent.length())) return false;		//注释格式不全返回false
//			c=XmlGetChar();
//		}
	}
	else return false;		//注释格式不全返回false
	return true;
}

//*************************************
//过滤处理指令
//
//
//*************************************
bool XmlProc::FilterHead()
{
	char c = ' ';
	string str;
	
	SkipSpace();
	if(m_pos < m_xmlFileContent.length())
	{	
		c = XmlGetChar(); 
		while('?' != c)	   
		{
			if(m_pos >= m_xmlFileContent.length()) return false;       //指令格式不全返回false
			c = XmlGetChar();
		}
		c=XmlGetChar();
		if('>' != c) return false;
	}
	else return false;		//注释格式不全返回false
	return true;
}

//*************************************
//提取处理指令或元素等的属性
//
//
//*************************************
bool XmlProc::PickProperty()
{
	char c = ' ';
	string str;	
	c = XmlGetChar();																												//读取一个字符
	while((c != 0x20)&&(c != 0x9)&&(c != 0xD)&&(c != 0xA)&&('=' != c))			//提取属性名称
	{
		if((c == '<')||(m_pos >= m_xmlFileContent.length())) return false;		//判断格式
		str.append(1,c);
		c = XmlGetChar();
	}
	m_pos--;																																//读位置减一
	m_temp.m_pName = str;																										//保存属性名
	str="";
	SkipSpace();																														//滤过空字符
	if(m_pos >= m_xmlFileContent.length()) return false; 
	c = XmlGetChar(); 								//读取一个字符
	if(c != '=')																														//判断格式
	{
		return false;
	}
	else																																		//处理等号后的元素属性值
	{
		SkipSpace();																													//滤过空字符	
		if(m_pos >= m_xmlFileContent.length()) return false;
		c = XmlGetChar(); 							//读取一个字符
		if ((c == '"')||('\'' == c)) 																					//有属性值或无属性值
		{
			if(m_pos >= m_xmlFileContent.length()) return false;
			c = XmlGetChar(); 
		}
		else																																	//格式错误
		{
			return false;
		}
		while(('"' != c)&&('\'' != c))     																		//提取属性值
		{
			if((c == '>')||(m_pos >= m_xmlFileContent.length())) return false;
			str.append(1,c);
			c = XmlGetChar();
		}
		m_temp.n_pVal = str;
	}
	return true;
}
//******************************************
//过滤空白
//
//
//******************************************
void XmlProc::SkipSpace()
{
	char c = ' ';
	if(m_pos < m_xmlFileContent.length())															//读位置不能超过文件长度
	{
		c = XmlGetChar();																								//读取一个字符
		while(((c == 0x20)||(c == 0x9)||(c == 0xD)||(c == 0xA))&&(m_pos < m_xmlFileContent.length()))	//如果为空字符并且读位置不超过文件长度
		{
			c = XmlGetChar();																							//读一个字符
		}
		if((m_pos <= m_xmlFileContent.length())&&(c != 0x20)&&(c != 0x9)&&(c != 0xD)&&(c != 0xA)) m_pos--;									//当读取到非空字符，将读位置向前移一位
	}
}

//****************************************************************
//处理元素（嵌套的）包括元素属性、内容和叶子元素的元素尾
//
//
//****************************************************************
bool XmlProc::GetElementInfo()
{
	char c = ' ';
	unsigned short flag = 0;
	XmlElement xmlEl;
	string str,strWithSpace,name;
	
	SkipSpace();
	if(m_pos >= m_xmlFileContent.length()) return false;
	c = XmlGetChar();
	while((c != 0x20)&&(c != 0x9)&&(c != 0xD)&&(c != 0xA)&&(c != '>')&&(c != 47))  //获取元素名
	{
		if((c == '<')||(m_pos >= m_xmlFileContent.length())) return false;				 //格式错，返回false
		str.append(1,c);																														 //将字符存入临时字符串变量
		c = XmlGetChar();																														 //读取一个字节
	}
	m_pos--;
	xmlEl.m_name = str;																															//获取元素名
	xmlEl.m_grade = m_elGrade;																											//获取元素级
	xmlEl.m_serialNo = m_elSerialNo;																								//获取元素序号
	m_elSerialNo++;
	m_elGrade++;																												//元素级别加一
	str = "";
	SkipSpace();																												//过滤空白
	if(m_pos >= m_xmlFileContent.length()) return false;
	c = XmlGetChar(); 
	if(47 == c)																													//如果是不带属性的空元素
	{
		if(m_pos >= m_xmlFileContent.length()) return false; 
		c = XmlGetChar(); 																								//读取一个字符
		if('>' == c)
		{
			m_elSave.push_back(xmlEl);																			//元素压栈
			m_elGrade--;																										//元素级别减一
			return true;
		}
		else
		{
			return false;
		}
	}
	while('>' != c)																											//提取元素属性
	{
		m_pos--;
		if(true == PickProperty())
		{
			xmlEl.m_property.push_back(m_temp);															//存储获取的元素属性
		}
		else
		{
			return false;
		}
		SkipSpace();																												//过滤空白
		if(m_pos >= m_xmlFileContent.length()) return false; 
		c = XmlGetChar(); 
		if(47 == c)																													//如果是带属性的空元素
		{
			if(m_pos >= m_xmlFileContent.length()) return false;
			c = XmlGetChar(); 
			if('>' == c)
			{
				m_elSave.push_back(xmlEl);																			//元素压栈
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	SkipSpace();																													//过滤空白
	flag = JudgeElement();																								//判断待处理元素		
	switch(flag)
	{
		case 2: if(false == FilterNote()) return false;       							//过滤内容前面的注释
						SkipSpace();																								//过滤空白
						if(m_pos >= m_xmlFileContent.length()) return false; 
						c = XmlGetChar(); 
						while('<' == c)						
						{
							m_pos--;
							flag = JudgeElement();
							switch(flag)
							{
								case 2: if(false == FilterNote()) return false; 				//过滤内容前面的注释
												else 
												{
													SkipSpace();																	//过滤空白
													if(m_pos >= m_xmlFileContent.length()) return false; 
													c = XmlGetChar(); 
												}
												break;
								case 4:	break;																//如果是元素头，跳出	
								case 5: m_pos--;															//如果是元素尾，将读位置减1，跳出												
												break;
								default:return false;
							}
							if(2 != flag) break;
						}
						m_pos--;																										//读位置减一
						if(2 != flag) break;																				//如果元素无内容，跳出
		case 3: c = XmlGetChar();																						//处理元素内容
						while('<' != c)																							//提取元素内容
						{
							if(m_pos >= m_xmlFileContent.length()) return false;			//如果到文件为则退出
							str.append(1,c);
							c = XmlGetChar();
							while((c == 0x20)||(c == 0x9)||(c == 0xD)||(c == 0xA))  	//判断空白是内容中的还是内容后的，内容中的保留，内容后的删除
							{
								if(m_pos >= m_xmlFileContent.length()) return false;
								strWithSpace.append(1,c);
								c = XmlGetChar();
							}
							if(('<' != c)&&(strWithSpace.length()>0))
							{
								str.append(strWithSpace);																//保留内容中间部分的空白
								strWithSpace = "";
							}
							while('<' == c)																						//如果内容中混有注释先过滤注释
							{
								m_pos--;
								flag = JudgeElement();																	//判断元素的类型
								switch(flag)
								{
									case 2: if(false == FilterNote()) return false;      	//过滤内容中和内容后的注释
													else 
													{
														SkipSpace();																//过滤空白
														if(m_pos >= m_xmlFileContent.length()) return false; 
														c = XmlGetChar(); 
													}
													break;
									case 4:	break;																				//是嵌套的元素
									case 5: m_pos--;
													break;
									default :return false;
								}
								if(2 != flag) break;
							}
						}
						m_pos--;
						xmlEl.m_val = str;
						str = "";
						break;
		case 4: m_elStack.push(xmlEl.m_name);																//如果有嵌套的下一级元素，先把当前元素名压栈
						m_elSave.push_back(xmlEl);																	//保存元素到存储区
						
						if(false == GetElementInfo()) return false;									//处理下一级元素
						else return true;																						//处理完成反回
						break;
		case 5: m_elSave.push_back(xmlEl);																	//如果本级为叶子元素，则处理元素尾
						if(m_pos >= m_xmlFileContent.length()) return false;
						c = XmlGetChar(); 
						while((c != 0x20)&&(c != 0x9)&&(c != 0xD)&&(c != 0xA)&&(c != '>'))	//提取元素尾中的元素名
						{
							if((c == '<')||(m_pos >= m_xmlFileContent.length())) return false;
							str.append(1,c);
							c = XmlGetChar();
						}
						if(0 != xmlEl.m_name.compare(str)) return false;							//比较与当前处理的元素的元素名是否一致
						m_elGrade--;																									//元素级别减一
						return true;
		default:return false;
	}
	SkipSpace();																														//过滤空白
	flag = JudgeElement();																									//判断待处理元素		
	switch(flag)
	{
		case 4: m_elStack.push(xmlEl.m_name);																	//如果有嵌套的下一级元素，先把当前元素名压栈
						m_elSave.push_back(xmlEl);																		//保存元素到存储区
						if(false == GetElementInfo()) return false;										//处理下一级元素
						else return true;																							//处理完成反回
						break;
		default: m_elSave.push_back(xmlEl);																		//如果本级为叶子元素，则处理元素尾
						if(m_pos >= m_xmlFileContent.length()) return false; 
						c = XmlGetChar(); 
						while((c != 0x20)&&(c != 0x9)&&(c != 0xD)&&(c != 0xA)&&(c != '>'))			//提取元素尾中的元素名
						{
							if((c == '<')||(m_pos >= m_xmlFileContent.length())) return false;
							str.append(1,c);
							c = XmlGetChar();
						}
						if(0 != xmlEl.m_name.compare(str)) return false;							//比较与当前处理的元素的元素名是否一致
						m_elGrade--;																									//元素级别减一
						break;
	}
	return true;
}

//*************************************
//提取字符
//
//
//*************************************
char XmlProc::XmlGetChar()
{
	char c = ' ';
	c = m_xmlFileContent.at(m_pos);   	//读取一个字符
	m_pos++;														//读取位置加一
	return c;														//将读取到的非零字节返回
}





























