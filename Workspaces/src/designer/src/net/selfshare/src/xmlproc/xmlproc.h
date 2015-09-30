// *********** 总装备部 第廿试验训练基地 试验技术部 四室 ********* //
// 文件名称：xmlproc.h
// 文件摘要：综合应用分系统xml文件处理类头文件，用来声明xml文件处理类。
// 
// 原始版本：1.0
// 作    者：肖胜杰
// 完成日期：

#ifndef XMLPROC_H
#define XMLPROC_H

#include <string>
#include <stack>
#include <vector>
#include <stdio.h>

using namespace std;

class Property
{
	public:
		string m_pName;
		string n_pVal;

		//****************************************************************
		//构造函数
		//
		//
		//****************************************************************
		Property():m_pName(""), n_pVal(""){}

};

typedef vector<Property> XmlProperty;             //用来存储各元素的属性
							
class XmlElement
{
	public:
		string       m_name;
		unsigned int m_grade;
		unsigned int m_serialNo;
		string       m_val;
		XmlProperty  m_property;
		
		//****************************************************************
		//构造函数
		//
		//
		//****************************************************************
		XmlElement():m_name(""), m_grade(0), m_serialNo(0), m_val(""){}
};

typedef stack<string>  ElNameStack;               //用来判断xml文件的格式是否正确以及输出xml文件
typedef vector<XmlElement> ElSave;				  //用来存储xml文件中的各元素以及相关信息

class XmlProc
{
	public:
		XmlProc();
		~XmlProc();
		
		bool XmlParse();
//		XmlOutput();
		bool LoadSource(const char *path);
		bool FilterHead();
		bool FilterNote();
		bool MoElementEnd();
		void SkipSpace();
		char XmlGetChar();
		unsigned short  JudgeElement();
		bool PickProperty();
		bool GetElementInfo();
		bool GetContentInfo();
//		PutElement();
		void Unload();

		ElSave      m_elSave;
		XmlProperty m_dec;
		XmlProperty m_pi;
	private:
		string        m_xmlFileContent;
		unsigned int  m_pos;
		Property      m_temp;
		unsigned int  m_elGrade;
		unsigned int  m_elSerialNo;
		ElNameStack   m_elStack;
		
};

#endif
