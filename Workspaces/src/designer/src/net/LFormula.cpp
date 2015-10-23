// LFormula.cpp: implementation of the LFormula class.
//
//////////////////////////////////////////////////////////////////////

#include "LFormula.h"
#include <QDebug>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#include <math.h>
const int FUN_NAME_NUM = 9;
const char* fun_name[FUN_NAME_NUM]={"JT0","JD1","JD2","JD3","YCC","ABS","DJS","SET","TIM"};
// 4 表示 错误
// 1 表示 后面的比前面的优先
// 0 表示 前面的比后面的优先
// 2 表示 ( ) { }  [ ]
//运算符关系表
const int OPERATOR_NUM = 17;
const char OperatorRelation[OPERATOR_NUM][OPERATOR_NUM]={
	//           ;   }   ]   )   ,  fun  (   [    {   +   -   *   /   &	  |	  >>  <<		
	/* ; */		{4,  4,  4,  4,  4,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0},
	/* } */		{4,  4,  4,  0,  0,  4,  4,  4,   4,  0,  0,  0,  0,  0,  0,  0,  0},
	/* ] */		{0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0,  0},
	/* ) */		{0,  4,  4,  1,  1,  4,  4,  4,   4,  0,  0,  0,  0,  0,  0,  0,  0},
	/* , */		{0,  0,  0,  0,  1,  1,  1,  1,   1,  0,  0,  0,  0,  0,  0,  0,  0},
	/*fun*/		{0,  4,  4,  4,  1,  1,  1,  1,   1,  1,  1,  1,  1,  1,  1,  1,  1},
	/* ( */		{4,  4,  4,  2,  1,  1,  1,  1,   1,  1,  1,  1,  1,  1,  1,  1,  1},
	/* [ */		{4,  4,  2,  4,  1,  4,  4,  4,   4,  0,  0,  0,  0,  1,  1,  1,  1},
	/* { */		{4,  2,  4,  4,  1,  4,  4,  4,   4,  0,  0,  0,  0,  1,  1,  1,  1},
	/* + */		{0,  4,  4,  0,  0,  1,  1,  1,   1,  0,  0,  1,  1,  1,  1,  1,  1},
	/* - */		{0,  4,  4,  0,  0,  1,  1,  1,   1,  0,  0,  1,  1,  1,  1,  1,  1},
	/* * */		{0,  4,  4,  0,  1,  1,  1,  1,   1,  0,  0,  0,  0,  0,  0,  0,  0},
	/* / */		{0,  4,  4,  0,  1,  1,  1,  1,   1,  0,  0,  0,  0,  0,  0,  0,  0},
	/* & */		{0,  0,  0,  0,  1,  1,  1,  1,   1,  1,  1,  0,  0,  0,  0,  0,  0},
	/* | */		{0,  0,  0,  0,  1,  1,  1,  1,   1,  1,  1,  0,  0,  0,  0,  0,  0},
	/* >>*/		{0,  0,  0,  0,  1,  1,  1,  1,   1,  1,  1,  0,  0,  0,  0,  0,  0},
	/*<< */		{0,  0,  0,  0,  1,  1,  1,  1,   1,  1,  1,  0,  0,  0,  0,  0,  0}
	};

bool OperatorData::GetValue(double& ret_d)
{
	if (type==DATA)
	{
		ret_d = data;
        return true;
	}
	else if (type==PARAM)
	{
        if(param)
        {
            return param->getValue(ret_d);
        }
	}
    return false;
}

bool OperatorData::GetValue(QVariant &reslut)
{
    if (type==DATA)
    {
        reslut = QVariant(data);
        return true;
    }
    else if (type==PARAM && param)
    {
        return param->getValue(reslut);
    }
    return false;
}

bool OperatorData::GetCode(std::string &code)
{
    if (type == PARAM && param)
    {
        return param->getCode(code);
    }
    return false;
}

void OperatorData::GetCodeValue(double& ret_d)
{
	if (type==DATA)
	{
		ret_d = data;
	}
	else if (type==PARAM)
	{
//		if (!::GetCodeValue((stru_Param*)param,ret_d))
//		{
//            throw(formula_error, ERRORPARAM);
//		}
	}
	else
	{
        //throw(formula_error, INVALIDDATATYPE);
	}
}
LFormula::LFormula(Config*cf)
    :m_bUse(true),m_config(cf),m_formula(""),
      m_savTime(0),m_savDate(0),m_primaryParam(NULL)
{

}

LFormula::~LFormula()
{

}

AbstractParam* LFormula::addParam(unsigned short tn,unsigned short pn,ParseType type)
{
    unsigned int tp = INDEX(tn,pn);
    switch(type)
    {
    case HistoryData:
    {
        if(m_zxparamMap.find(tp)==m_zxparamMap.end())
        {
            m_zxparamMap[tp] = m_config->m_zxParamBuf.GetBuffer(tn,pn);
            m_primaryParam = m_config->m_zxParamBuf.GetParamBuffer(tn,pn);
        }
        return &(m_zxparamMap[tp]);
    }
        break;
    case CurData:
    {
        m_primaryParam = m_config->m_zxParamBuf.GetParamBuffer(tn,pn);
        return m_primaryParam;
    }
        break;
    }
    return NULL;
}

AbstractParam* LFormula::getParam(std::string formula)
{
    if(m_primaryParam!=NULL&&m_formula == formula)
        return m_primaryParam;
    m_formula = formula;
    unsigned short tn;
    unsigned short pn;
    if(getParam(formula,tn,pn))
    {
        m_primaryParam = m_config->m_zxParamBuf.GetParamBuffer(tn,pn);
        return m_primaryParam;
    }
    return NULL;
}

void LFormula::updateParam(unsigned short tn,unsigned short pn)
{
    m_zxparamMap[INDEX(tn,pn)] = m_config->m_zxParamBuf.GetBuffer(tn,pn);
}

void LFormula::updateParam()
{
    FormulaZXParamMap::iterator it = m_zxparamMap.begin();
    while(it!=m_zxparamMap.end())
    {
        unsigned int pn = it->first;
        AbstractParam t_param = m_config->m_zxParamBuf.GetBuffer(TABNO(pn),PARAMNO(pn));
        double t_val = 0.0;
        if(t_param.getValueFromCode(t_val))
        {
            m_zxparamMap[pn].setValue(t_val);
        }
        else
        {
            m_zxparamMap[pn] = t_param;
        }
        it++;
    }
}

bool LFormula::insertOneHistroyParam()
{
    FormulaZXParamMap::iterator it = m_zxparamMap.begin();
    bool bUpdate = false;
    while(it!=m_zxparamMap.end())
    {
        unsigned int pn = it->first;
        HistoryParams* t_buf = &m_tHistoryMap[pn];
        if (!t_buf->empty())
        {
            vector<HistoryParam>* paramVector = t_buf->back();
            HistoryParam param = paramVector->back();
            paramVector->pop_back();
            m_zxparamMap[pn].setValue(param.getValue());
            m_zxparamMap[pn].SetParamTime(param.getTime());
            bUpdate = true;
        }
//         if(t_buf->size()>0)
//         {
//             HistoryParam t_hp = t_buf->front();
//             //t_buf->pop_front();
//             t_buf->erase(t_buf->begin());
//             m_zxparamMap[pn].setValue(t_hp.getValue());
//             m_zxparamMap[pn].SetParamTime(t_hp.getTime());
//             bUpdate = true;
//         }
        else
        {
            break;
        }
        it++;
    }
    if(bUpdate)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool LFormula::updateAllHistroyParam()
{
    m_tHistoryMap.clear();
    FormulaZXParamMap::iterator it = m_zxparamMap.begin();
    int time = 0;
    int date = 0;
    bool bUpdate = false;
    while(it!=m_zxparamMap.end())
    {
        unsigned int pn = it->first;
        time = m_savTime;
        date = m_savDate;
        HistoryParams buf = m_config->m_zxHistoryParamBuf.GetBuffer(TABNO(pn),PARAMNO(pn),date,time);
        m_tHistoryMap[pn] = buf;
        bUpdate = true;
        it++;
    }
    if(bUpdate)
    {
        if(time>m_savTime)
        {
            m_savTime = time;
            m_savDate = date;
        }
        return true;
    }
    else
    {
        return false;
    }
}

int LFormula::getParamCount()
{
    return 0;//(int)m_zxparamMap.count();
}

int LFormula::compute(std::string formula,double& result)
{
    if(parse(formula))
    {
        if(compute(result))
        {
            return 1;
        }
    }
    return -1;
}

int LFormula::compute(std::string formula,QVariant& result)
{
    if(parse(formula))
    {
        if(compute(result))
        {
            return 1;
        }
    }
    return -1;
}

int LFormula::compute(std::string formula,QVector<double>& result)
{
    if(parse(formula,HistoryData))
    {
        bool b_compute = false;
        //get All data
        if(updateAllHistroyParam())
        {
            //insert A data
            while(insertOneHistroyParam())
            {
                //compute once
                double t_result = 0.0;
                if(compute(t_result))
                {
                    result.push_back(t_result);
                    b_compute = true;
                }
            }
            if(b_compute)
                return 1;
        }
    }
    return -1;
}

int LFormula::compute(std::string formulaX,std::string formulaY,QVector<double>& resultX,QVector<double>& resultY)
{
    return -1;
}
int LFormula::code(std::string formula,std::string& code)
{
    AbstractParam* t_param = getParam(formula);
    if(t_param!=NULL&&t_param->getCode(code))
    {
        return 1;
    }
    return -1;
}

bool LFormula::compute(double& ret_d)
{
//	try
//	{
    if (m_bUse)
	{
		double t_ret_d = 0;
		std::stack<OperatorData> t_Data_Stack;
		int fun_param_num = 0;
		for (int pos=0;pos<m_OperData.size();pos++)
		{
			OperatorData t_OD = m_OperData[pos];
			OperatorData t_OD_a,t_OD_b,t_OD_c;
			switch(t_OD.type)
			{
			case DATA:
			case PARAM:
				t_Data_Stack.push(t_OD);
				break;
			case COMMA:
				fun_param_num++;
				break;
			case SEMICOLON:
				fun_param_num = 0;
				while (!t_Data_Stack.empty())
				{
					t_Data_Stack.pop();
				}				
			    break;
			case ADD:
				if (t_Data_Stack.size()<2)
				{
                    return false;
                    //throw(formula_error,NOENOUGHFUNPARAM);
				}
				t_OD_b = t_Data_Stack.top();
				t_Data_Stack.pop();
				t_OD_a = t_Data_Stack.top();
				t_Data_Stack.pop();
                if(!fun_ADD(t_OD_a,t_OD_b,t_ret_d))
                    return false;
				t_OD.type = DATA;
				t_OD.data = t_ret_d;
				t_Data_Stack.push(t_OD);		
			    break;
			case SUB:
				if (t_Data_Stack.size()<2)
				{
                    return false;
                    //throw(formula_error,NOENOUGHFUNPARAM);
				}
				t_OD_b = t_Data_Stack.top();
				t_Data_Stack.pop();
				t_OD_a = t_Data_Stack.top();
				t_Data_Stack.pop();
				fun_SUB(t_OD_a,t_OD_b,t_ret_d);
				t_OD.type = DATA;
				t_OD.data = t_ret_d;
				t_Data_Stack.push(t_OD);		
			    break;
			case MUL:
				if (t_Data_Stack.size()<2)
				{
                    return false;
                    //throw(formula_error,NOENOUGHFUNPARAM);
				}
				t_OD_b = t_Data_Stack.top();
				t_Data_Stack.pop();
				t_OD_a = t_Data_Stack.top();
				t_Data_Stack.pop();
				fun_MUL(t_OD_a,t_OD_b,t_ret_d);
				t_OD.type = DATA;
				t_OD.data = t_ret_d;
				t_Data_Stack.push(t_OD);		
			    break;
			case DIV:
				if (t_Data_Stack.size()<2)
				{
                    return false;
                    //throw(formula_error,NOENOUGHFUNPARAM);
				}
				t_OD_b = t_Data_Stack.top();
				t_Data_Stack.pop();
				t_OD_a = t_Data_Stack.top();
				t_Data_Stack.pop();
				fun_DIV(t_OD_a,t_OD_b,t_ret_d);
				t_OD.type = DATA;
				t_OD.data = t_ret_d;
				t_Data_Stack.push(t_OD);		
			    break;
			case AND:
				if (t_Data_Stack.size()<2)
				{
                    return false;
                    //throw(formula_error,NOENOUGHFUNPARAM);
				}
				t_OD_b = t_Data_Stack.top();
				t_Data_Stack.pop();
				t_OD_a = t_Data_Stack.top();
				t_Data_Stack.pop();
				fun_AND(t_OD_a,t_OD_b,t_ret_d);
				t_OD.type = DATA;
				t_OD.data = t_ret_d;
				t_Data_Stack.push(t_OD);		
			    break;
			case OR:
				if (t_Data_Stack.size()<2)
				{
                    return false;
                    //throw(formula_error,NOENOUGHFUNPARAM);
				}
				t_OD_b = t_Data_Stack.top();
				t_Data_Stack.pop();
				t_OD_a = t_Data_Stack.top();
				t_Data_Stack.pop();
				fun_OR(t_OD_a,t_OD_b,t_ret_d);
				t_OD.type = DATA;
				t_OD.data = t_ret_d;
				t_Data_Stack.push(t_OD);		
			    break;
			case SHIFT_RIGHT:
				if (t_Data_Stack.size()<2)
				{
                    return false;
                    //throw(formula_error,NOENOUGHFUNPARAM);
				}
				t_OD_b = t_Data_Stack.top();
				t_Data_Stack.pop();
				t_OD_a = t_Data_Stack.top();
				t_Data_Stack.pop();
				fun_SHIFT_RIGHT(t_OD_a,t_OD_b,t_ret_d);
				t_OD.type = DATA;
				t_OD.data = t_ret_d;
				t_Data_Stack.push(t_OD);		
			    break;
			case SHIFT_LEFT:
				if (t_Data_Stack.size()<2)
				{
                    return false;
                    //throw(formula_error,NOENOUGHFUNPARAM);
				}
				t_OD_b = t_Data_Stack.top();
				t_Data_Stack.pop();
				t_OD_a = t_Data_Stack.top();
				t_Data_Stack.pop();
				fun_SHIFT_LEFT(t_OD_a,t_OD_b,t_ret_d);
				t_OD.type = DATA;
				t_OD.data = t_ret_d;
				t_Data_Stack.push(t_OD);		
			    break;
			case FUN:
				switch(t_OD.funname)
				{
				case JT0:
					if (t_Data_Stack.size()<2)
					{
                        return false;
                        //throw(formula_error,NOENOUGHFUNPARAM);
					}
					t_OD_b = t_Data_Stack.top();
					t_Data_Stack.pop();
					t_OD_a = t_Data_Stack.top();
					t_Data_Stack.pop();
					fun_JT0(t_OD_a,t_OD_b,t_ret_d);
					t_OD.type = DATA;
					t_OD.data = t_ret_d;
					t_Data_Stack.push(t_OD);
					fun_param_num =0;
					break;
				case JD1:
					if (t_Data_Stack.size()<1)
					{
                        return false;
                        //throw(formula_error,NOENOUGHFUNPARAM);
					}
					t_OD_a = t_Data_Stack.top();
					t_Data_Stack.pop();
					fun_JD1(t_OD_a,t_ret_d);
					t_OD.type = DATA;
					t_OD.data = t_ret_d;
					t_Data_Stack.push(t_OD);
					fun_param_num =0;
				    break;
				case JD2:
					if (t_Data_Stack.size()<1)
					{
                        return false;
                        //throw(formula_error,NOENOUGHFUNPARAM);
					}
					t_OD_a = t_Data_Stack.top();
					t_Data_Stack.pop();
					fun_JD2(t_OD_a,t_ret_d);
					t_OD.type = DATA;
					t_OD.data = t_ret_d;
					t_Data_Stack.push(t_OD);
					fun_param_num =0;
				    break;
				case JD3:
					if (t_Data_Stack.size()<1)
					{
                        return false;
                        //throw(formula_error,NOENOUGHFUNPARAM);
					}
					t_OD_a = t_Data_Stack.top();
					t_Data_Stack.pop();
					fun_JD3(t_OD_a,t_ret_d);
					t_OD.type = DATA;
					t_OD.data = t_ret_d;
					t_Data_Stack.push(t_OD);
					fun_param_num =0;
				    break;
				case YCC:
					if (t_Data_Stack.size()<2)
					{
                        return false;
                        //throw(formula_error,NOENOUGHFUNPARAM);
					}
					t_OD_b = t_Data_Stack.top();
					t_Data_Stack.pop();
					t_OD_a = t_Data_Stack.top();
					t_Data_Stack.pop();
					fun_YCC(t_OD_a,t_OD_b,t_ret_d);
					fun_param_num =0;
					break;
				case ABS:
					if (t_Data_Stack.size()<1)
					{
                        return false;
                        //throw(formula_error,NOENOUGHFUNPARAM);
					}
					t_OD_a = t_Data_Stack.top();
					t_Data_Stack.pop();
					fun_ABS(t_OD_a,t_ret_d);
					fun_param_num =0;
				    break;
				case DJS:
					if (t_Data_Stack.size()<1)
					{
                        return false;
                        //throw(formula_error,NOENOUGHFUNPARAM);
					}
					t_OD_a = t_Data_Stack.top();
					t_Data_Stack.pop();
					fun_DJS(t_OD_a,t_ret_d);
					fun_param_num =0;
				    break;
				case SET:
					if (t_Data_Stack.size()<3)
					{
                        return false;
                        //throw(formula_error,NOENOUGHFUNPARAM);
					}
					t_OD_c = t_Data_Stack.top();
					t_Data_Stack.pop();
					t_OD_b = t_Data_Stack.top();
					t_Data_Stack.pop();
					t_OD_a = t_Data_Stack.top();
					t_Data_Stack.pop();
					fun_SET(t_OD_a,t_OD_b,t_OD_c,t_ret_d);
					fun_param_num =0;
				    break;
				case TIM:
					if (t_Data_Stack.size()<2)
					{
                        return false;
                        //throw(formula_error,NOENOUGHFUNPARAM);
					}
					t_OD_b = t_Data_Stack.top();
					t_Data_Stack.pop();
					t_OD_a = t_Data_Stack.top();
					t_Data_Stack.pop();
					fun_TIM(t_OD_a,t_OD_b,t_ret_d);
					fun_param_num =0;
				    break;
				default:
                    return false;
                    //throw(formula_error,ERRORFUNNAME);
				    break;
				}
			    break;
			default:
                //throw(formula_error,INVALIDOPERATOR);
			    break;
			}
		}	
/************************************************************
'	created:	6:12:2012   16:40
'	author:		刘裕贵
'	
'	purpose:	运算完成后，还有数据就提取出数据，并作为最终
的计算结果，要经过充分测试才能启用
************************************************************/		
		while (!t_Data_Stack.empty())
		{
			OperatorData t_OD_ret = t_Data_Stack.top();
			t_Data_Stack.pop();
			return t_OD_ret.GetValue(ret_d);
		}		
	}
    return false;
}

bool LFormula::compute(QVariant& ret_d)
{
    if (m_bUse)
    {
        double t_ret_d = 0;
        std::stack<OperatorData> t_Data_Stack;
        int fun_param_num = 0;
        for (int pos=0;pos<m_OperData.size();pos++)
        {
            OperatorData t_OD = m_OperData[pos];
            OperatorData t_OD_a,t_OD_b,t_OD_c;
            switch(t_OD.type)
            {
            case DATA:
            case PARAM:
                t_Data_Stack.push(t_OD);
                break;
            case COMMA:
                fun_param_num++;
                break;
            case SEMICOLON:
                fun_param_num = 0;
                while (!t_Data_Stack.empty())
                {
                    t_Data_Stack.pop();
                }
                break;
            case ADD:
                if (t_Data_Stack.size()<2)
                {
                    qDebug()<<NOENOUGHFUNPARAM;
                    return false;
                }
                t_OD_b = t_Data_Stack.top();
                t_Data_Stack.pop();
                t_OD_a = t_Data_Stack.top();
                t_Data_Stack.pop();
                if(!fun_ADD(t_OD_a,t_OD_b,t_ret_d))
                    return false;
                t_OD.type = DATA;
                t_OD.data = t_ret_d;
                t_Data_Stack.push(t_OD);
                break;
            case SUB:
                if (t_Data_Stack.size()<2)
                {
                    qDebug()<<NOENOUGHFUNPARAM;
                    return false;
                }
                t_OD_b = t_Data_Stack.top();
                t_Data_Stack.pop();
                t_OD_a = t_Data_Stack.top();
                t_Data_Stack.pop();
                if(!fun_SUB(t_OD_a,t_OD_b,t_ret_d))
                {
                    qDebug()<<"减法计算出错";
                    return false;
                }
                t_OD.type = DATA;
                t_OD.data = t_ret_d;
                t_Data_Stack.push(t_OD);
                break;
            case MUL:
                if (t_Data_Stack.size()<2)
                {
                    qDebug()<<NOENOUGHFUNPARAM;
                    return false;
                }
                t_OD_b = t_Data_Stack.top();
                t_Data_Stack.pop();
                t_OD_a = t_Data_Stack.top();
                t_Data_Stack.pop();
                fun_MUL(t_OD_a,t_OD_b,t_ret_d);
                t_OD.type = DATA;
                t_OD.data = t_ret_d;
                t_Data_Stack.push(t_OD);
                break;
            case DIV:
                if (t_Data_Stack.size()<2)
                {
                    qDebug()<<NOENOUGHFUNPARAM;
                    return false;
                }
                t_OD_b = t_Data_Stack.top();
                t_Data_Stack.pop();
                t_OD_a = t_Data_Stack.top();
                t_Data_Stack.pop();
                if(!fun_DIV(t_OD_a,t_OD_b,t_ret_d))
                {
                    qDebug()<<"除法计算出错";
                    return false;
                }
                t_OD.type = DATA;
                t_OD.data = t_ret_d;
                t_Data_Stack.push(t_OD);
                break;
            case AND:
                if (t_Data_Stack.size()<2)
                {
                    return false;
                    //throw(formula_error,NOENOUGHFUNPARAM);
                }
                t_OD_b = t_Data_Stack.top();
                t_Data_Stack.pop();
                t_OD_a = t_Data_Stack.top();
                t_Data_Stack.pop();
                fun_AND(t_OD_a,t_OD_b,t_ret_d);
                t_OD.type = DATA;
                t_OD.data = t_ret_d;
                t_Data_Stack.push(t_OD);
                break;
            case OR:
                if (t_Data_Stack.size()<2)
                {
                    return false;
                    //throw(formula_error,NOENOUGHFUNPARAM);
                }
                t_OD_b = t_Data_Stack.top();
                t_Data_Stack.pop();
                t_OD_a = t_Data_Stack.top();
                t_Data_Stack.pop();
                fun_OR(t_OD_a,t_OD_b,t_ret_d);
                t_OD.type = DATA;
                t_OD.data = t_ret_d;
                t_Data_Stack.push(t_OD);
                break;
            case SHIFT_RIGHT:
                if (t_Data_Stack.size()<2)
                {
                    qDebug()<<NOENOUGHFUNPARAM;
                    return false;
                }
                t_OD_b = t_Data_Stack.top();
                t_Data_Stack.pop();
                t_OD_a = t_Data_Stack.top();
                t_Data_Stack.pop();
                fun_SHIFT_RIGHT(t_OD_a,t_OD_b,t_ret_d);
                t_OD.type = DATA;
                t_OD.data = t_ret_d;
                t_Data_Stack.push(t_OD);
                break;
            case SHIFT_LEFT:
                if (t_Data_Stack.size()<2)
                {
                    qDebug()<<NOENOUGHFUNPARAM;
                    return false;
                }
                t_OD_b = t_Data_Stack.top();
                t_Data_Stack.pop();
                t_OD_a = t_Data_Stack.top();
                t_Data_Stack.pop();
                fun_SHIFT_LEFT(t_OD_a,t_OD_b,t_ret_d);
                t_OD.type = DATA;
                t_OD.data = t_ret_d;
                t_Data_Stack.push(t_OD);
                break;
            case FUN:
                switch(t_OD.funname)
                {
                case JT0:
                    if (t_Data_Stack.size()<2)
                    {
                        qDebug()<<NOENOUGHFUNPARAM;
                        return false;
                    }
                    t_OD_b = t_Data_Stack.top();
                    t_Data_Stack.pop();
                    t_OD_a = t_Data_Stack.top();
                    t_Data_Stack.pop();
                    fun_JT0(t_OD_a,t_OD_b,t_ret_d);
                    t_OD.type = DATA;
                    t_OD.data = t_ret_d;
                    t_Data_Stack.push(t_OD);
                    fun_param_num =0;
                    break;
                case JD1:
                    if (t_Data_Stack.size()<1)
                    {
                        qDebug()<<NOENOUGHFUNPARAM;
                        return false;
                    }
                    t_OD_a = t_Data_Stack.top();
                    t_Data_Stack.pop();
                    fun_JD1(t_OD_a,t_ret_d);
                    t_OD.type = DATA;
                    t_OD.data = t_ret_d;
                    t_Data_Stack.push(t_OD);
                    fun_param_num =0;
                    break;
                case JD2:
                    if (t_Data_Stack.size()<1)
                    {
                        qDebug()<<NOENOUGHFUNPARAM;
                        return false;
                    }
                    t_OD_a = t_Data_Stack.top();
                    t_Data_Stack.pop();
                    fun_JD2(t_OD_a,t_ret_d);
                    t_OD.type = DATA;
                    t_OD.data = t_ret_d;
                    t_Data_Stack.push(t_OD);
                    fun_param_num =0;
                    break;
                case JD3:
                    if (t_Data_Stack.size()<1)
                    {
                        qDebug()<<NOENOUGHFUNPARAM;
                        return false;
                    }
                    t_OD_a = t_Data_Stack.top();
                    t_Data_Stack.pop();
                    fun_JD3(t_OD_a,t_ret_d);
                    t_OD.type = DATA;
                    t_OD.data = t_ret_d;
                    t_Data_Stack.push(t_OD);
                    fun_param_num =0;
                    break;
                case YCC:
                    if (t_Data_Stack.size()<2)
                    {
                        qDebug()<<NOENOUGHFUNPARAM;
                        return false;
                    }
                    t_OD_b = t_Data_Stack.top();
                    t_Data_Stack.pop();
                    t_OD_a = t_Data_Stack.top();
                    t_Data_Stack.pop();
                    fun_YCC(t_OD_a,t_OD_b,t_ret_d);
                    fun_param_num =0;
                    break;
                case ABS:
                    if (t_Data_Stack.size()<1)
                    {
                        qDebug()<<NOENOUGHFUNPARAM;
                        return false;
                    }
                    t_OD_a = t_Data_Stack.top();
                    t_Data_Stack.pop();
                    fun_ABS(t_OD_a,t_ret_d);
                    fun_param_num =0;
                    break;
                case DJS:
                    if (t_Data_Stack.size()<1)
                    {
                        qDebug()<<NOENOUGHFUNPARAM;
                        return false;
                    }
                    t_OD_a = t_Data_Stack.top();
                    t_Data_Stack.pop();
                    fun_DJS(t_OD_a,t_ret_d);
                    fun_param_num =0;
                    break;
                case SET:
                    if (t_Data_Stack.size()<3)
                    {
                        qDebug()<<NOENOUGHFUNPARAM;
                        return false;
                    }
                    t_OD_c = t_Data_Stack.top();
                    t_Data_Stack.pop();
                    t_OD_b = t_Data_Stack.top();
                    t_Data_Stack.pop();
                    t_OD_a = t_Data_Stack.top();
                    t_Data_Stack.pop();
                    fun_SET(t_OD_a,t_OD_b,t_OD_c,t_ret_d);
                    fun_param_num =0;
                    break;
                case TIM:
                    if (t_Data_Stack.size()<2)
                    {
                        qDebug()<<NOENOUGHFUNPARAM;
                        return false;
                    }
                    t_OD_b = t_Data_Stack.top();
                    t_Data_Stack.pop();
                    t_OD_a = t_Data_Stack.top();
                    t_Data_Stack.pop();
                    fun_TIM(t_OD_a,t_OD_b,t_ret_d);
                    fun_param_num =0;
                    break;
                default:
                    qDebug()<<ERRORFUNNAME;
                    return false;
                    break;
                }
                break;
            default:
                qDebug()<<INVALIDOPERATOR;
                break;
            }
        }
/************************************************************
'	created:	6:12:2012   16:40
'	author:		刘裕贵
'
'	purpose:	运算完成后，还有数据就提取出数据，并作为最终
的计算结果，要经过充分测试才能启用
************************************************************/
        while (!t_Data_Stack.empty())
        {
            OperatorData t_OD_ret = t_Data_Stack.top();
            t_Data_Stack.pop();
            return t_OD_ret.GetValue(ret_d);
        }
    }
    return false;
}

bool LFormula::change(std::string newformula)
{
    if(m_formula!=newformula)
    {
        m_formula = newformula;
        parse(m_formula);
        return true;
    }
    else
        return false;
}

bool LFormula::parse(std::string formula,ParseType type)
{
    //没有使用公式
    if (formula.empty())
    {
        m_bUse = false;
        return false;
    }
    if(m_formula == formula)
    {
        return true;
    }
    m_formula = formula;
    //去除空格
    //formula.replace(' ','');

    m_bUse = true;
    m_OperData.clear();
    int length = formula.size();
    int readpos = 0;
    int pos=0;
    string data;
    std::stack< OperatorData > t_OP_Stack;
    for (;pos<length;pos++)
    {
        char opera = formula.at(pos);
        AbstractParam* pParam = NULL;
        WORD tn = 0;
        WORD pn = 0;
        OperatorData opdata1;
        OperatorData opdata0;
        int nextpos=0;
        switch(opera)
        {
        case ';':
            data = formula.substr(readpos,pos-readpos);
            ParseData(t_OP_Stack,data, opdata1);
            opdata1.type = SEMICOLON;
            HandleOperator(t_OP_Stack,opdata1);
            readpos = pos+1;
            break;
        case '{':
            data = formula.substr(readpos,pos-readpos);
            ParseData(t_OP_Stack,data, opdata1);
            nextpos = formula.find('}',pos);
            //不匹配 '{' '}'
            if (nextpos==-1)
            {
                return false;
            }
            data = formula.substr(pos+1,nextpos-pos-1);
            if (!data.empty())
            {
                int commaPos = data.find(',',0);
                if(commaPos>0)
                    pParam = addParam(atoi(data.substr(0,commaPos).c_str()),
                                  atoi(data.substr(commaPos+1,nextpos-pos-1).c_str())
                                      ,type);
                else
                    return false;
            }
            else
            {

                return false;
            }
            if (pParam!=NULL)
            {
                opdata1.type = PARAM;
                opdata1.param = pParam;
                m_OperData.push_back(opdata1);
            }
            else//不存在参数
            {
                return false;
            }
            pos = nextpos;
            readpos = pos+1;
            break;
        case '}'://不会遇到
            return false;
            break;
        case '[':
            data = formula.substr(readpos,pos-readpos);
            ParseData(t_OP_Stack,data, opdata1);
            nextpos = formula.find(']',pos);
            //不匹配 '[' ']'
            if (nextpos==-1)
            {
                return false;
            }
            data = formula.substr(pos+1,nextpos-pos-1);
            if (!data.empty())
            {
                int commaPos = data.find(',',0);
                if(commaPos>0)
                    pParam = addParam(atoi(data.substr(0,commaPos).c_str()),
                                  atoi(data.substr(commaPos+1,nextpos-pos-1).c_str())
                                      ,type);
                else
                    return false;
            }
            else
            {
                return false;
            }
            if (pParam!=NULL)
            {
                opdata1.type = PARAM;
                opdata1.param = pParam;
                m_OperData.push_back(opdata1);
            }
            else//不存在参数
            {
                return false;
            }
            pos = nextpos;
            readpos = pos+1;
            break;
        case ']'://不会遇到
            return false;
            break;
        case '(':
            data = formula.substr(readpos,pos-readpos);
            ParseData(t_OP_Stack,data, opdata1);
            opdata1.type = BRACKET_L;
            HandleOperator(t_OP_Stack,opdata1);
            readpos = pos+1;
            break;
        case ')':
            data = formula.substr(readpos,pos-readpos);
            ParseData(t_OP_Stack,data, opdata1);
            opdata1.type = BRACKET_R;
            HandleOperator(t_OP_Stack,opdata1);
            readpos = pos+1;
            break;
        case ',':
            data = formula.substr(readpos,pos-readpos);
            ParseData(t_OP_Stack,data, opdata1);
            opdata1.type = COMMA;
            HandleOperator(t_OP_Stack,opdata1);
            readpos = pos+1;
            break;
        case '+':
            data = formula.substr(readpos,pos-readpos);
            ParseData(t_OP_Stack,data, opdata1);
            opdata1.type = ADD;
            HandleOperator(t_OP_Stack,opdata1);
            readpos = pos+1;
            break;
        case '-':
            data = formula.substr(readpos,pos-readpos);
            ParseData(t_OP_Stack,data, opdata1);
            opdata1.type = SUB;
            HandleOperator(t_OP_Stack,opdata1);
            readpos = pos+1;
            break;
        case '*':
            data = formula.substr(readpos,pos-readpos);
            ParseData(t_OP_Stack,data, opdata1);
            opdata1.type = MUL;
            HandleOperator(t_OP_Stack,opdata1);
            readpos = pos+1;
            break;
        case '/':
            data = formula.substr(readpos,pos-readpos);
            ParseData(t_OP_Stack,data, opdata1);
            opdata1.type = DIV;
            HandleOperator(t_OP_Stack,opdata1);
            readpos = pos+1;
            break;
        case '&':
            data = formula.substr(readpos,pos-readpos);
            ParseData(t_OP_Stack,data, opdata1);
            opdata1.type = AND;
            HandleOperator(t_OP_Stack,opdata1);
            readpos = pos+1;
            break;
        case '|':
            data = formula.substr(readpos,pos-readpos);
            ParseData(t_OP_Stack,data, opdata1);
            opdata1.type = OR;
            HandleOperator(t_OP_Stack,opdata1);
            readpos = pos+1;
            break;
        case '>'://>>
            data = formula.substr(readpos,pos-readpos);
            ParseData(t_OP_Stack,data, opdata1);
            opdata1.type = SHIFT_RIGHT;
            HandleOperator(t_OP_Stack,opdata1);
            pos++;
            readpos = pos+1;
            break;
        case '<'://<<
            data = formula.substr(readpos,pos-readpos);
            ParseData(t_OP_Stack,data, opdata1);
            opdata1.type = SHIFT_LEFT;
            HandleOperator(t_OP_Stack,opdata1);
            pos++;
            readpos = pos+1;
            break;
        default:
            break;
        }
    }
    data = formula.substr(readpos,pos-readpos);
    if (!data.empty())
    {
        OperatorData opdata;
        ParseData(t_OP_Stack,data, opdata);
        HandleOperator(t_OP_Stack);
    }
    while(!t_OP_Stack.empty())
    {
        m_OperData.push_back(t_OP_Stack.top());
        t_OP_Stack.pop();
    }
	return true;
}
bool LFormula::HandleOperator(std::stack<OperatorData >& ostack,const OperatorData& opdata1)
{	
	OperatorData opdata0;
	while (!ostack.empty())
	{
		opdata0 = ostack.top();
		if (opdata0.type>=OPERATOR_NUM&&opdata1.type>=OPERATOR_NUM)
		{            
            return false;
		}
		switch(OperatorRelation[opdata0.type][opdata1.type])
		{
		case 1:
			ostack.push(opdata1);
            return true;
		case 0:
			m_OperData.push_back(opdata0);
			ostack.pop();
		    break;
		case 2:// (  )
			ostack.pop();
            return true;
		case 4://错误
            return false;
		    break;
		default:
            return false;
		    break;
		}
	}
	if (ostack.empty())
	{
		ostack.push(opdata1);
	}
    return true;
}
void LFormula::HandleOperator(std::stack<OperatorData >& ostack)
{	
	OperatorData opdata0;
	while (!ostack.empty())
	{
		opdata0 = ostack.top();
		ostack.pop();
		m_OperData.push_back(opdata0);		
	}
}
//绝对时减去T0时间 得到相对时
void LFormula::fun_JT0(OperatorData a, OperatorData b,double &ret)
{
//	//绝对时
//	stru_Param* pa = NULL;
//	//T0时间
//	stru_Param* pb = NULL;
//	//减T0函数的两个参数必须使用PARAM类型
//	if (a.type==PARAM&&b.type==PARAM)
//	{
//		pa = (stru_Param*)a.param;
//		pb = (stru_Param*)b.param;
//	}
//	else
//	{
//        throw(formula_error,JT0INVALIDPARAM);
//	}
//	if (pa!=NULL&&pb!=NULL)
//	{
//		//绝对时间参数无效
//		if (pa->ulParamDate==0xffffffff||pa->ulParamTime==0xffffffff)
//		{
//            throw(formula_error,JT0INVALIDPARAM);
//		}
//		//T0参数无效
//		//相对时为0
//		if (pb->ulParamDate==0xffffffff||pb->ulParamTime==0xffffffff)
//		{
//			ret = 0;
//			return;
//		}
//		double a_d=0;
//		double b_d=0;
//		if (::GetValue(pa,a_d)&&::GetValue(pb,b_d))
//		{
//			stru_C3IDate* adate = (stru_C3IDate*)&(pa->ulParamDate);
//			stru_C3IDate* bdate = (stru_C3IDate*)&(pb->ulParamDate);
///************************************************************
//'	created:	27:11:2012   11:14
//'	author:		刘裕贵
//'
//'	purpose:	 处理刘向峰发现的切换信道，造成日期非法报错的问题
//************************************************************/
//// 			int t_year_sub = (adate->Year_1000-bdate->Year_1000)*1000 + (adate->Year_100-bdate->Year_100)*100 +
//// 				(adate->Year_10-bdate->Year_10)*10 + (adate->Year-bdate->Year);
//// 			int t_month_sub = (adate->Month_10 - bdate->Month_10)*10 + (adate->Month - bdate->Month);
//			int t_day_sub = (adate->Day_10 - bdate->Day_10)*10 + (adate->Day - bdate->Day);
////			int t_subDays = t_year_sub*365+t_month_sub*31+t_day_sub;
//			int t_subDays = t_day_sub;
//			//绝对时减去T0时间
//			//只处理最多相差一天的数据
//			if (t_subDays>1||t_subDays<-1)
//			{
//				t_subDays = 0;
//			}
//			ret = a_d - b_d	+ t_subDays*864000000;
//		}
//		else//参数日期非法
//		{
//            throw(formula_error,JT0INVALIDPARAM);
//		}
//	}
//	else
//	{
//        throw(formula_error,JT0INVALIDPARAM);
//	}
}
//正常的俯仰角度值转换到-90°~ 270之间
void LFormula::fun_JD1(OperatorData a, double &ret)
{
	a.GetValue(ret);
	while(ret>270)
	{
		ret = ret - 360;
	}
	while(ret<-90)
	{
		ret = ret + 360;
	}
}
//方位角度值转换到-180°~ 180°之间
void LFormula::fun_JD2(OperatorData a, double &ret)
{
	a.GetValue(ret);
	while(ret>180)
	{
		ret = ret - 360;
	}
	while(ret<-180)
	{
		ret = ret + 360;
	}
}
//方位角度值转换到0°~ 360°之间
void LFormula::fun_JD3(OperatorData a, double &ret)
{
	a.GetValue(ret);
	while(ret>360)
	{
		ret = ret - 360;
	}
	while(ret<0)
	{
		ret = ret + 360;
	}
}
bool LFormula::fun_ADD(OperatorData a,OperatorData b,double &ret)
{
	double a_d = 0;
	double b_d = 0;
    if(a.GetValue(a_d)&&b.GetValue(b_d))
    {
        ret = a_d + b_d;
        return true;
    }
    else
        return false;
}
bool LFormula::fun_SUB(OperatorData a,OperatorData b,double &ret)
{
	double a_d = 0;
	double b_d = 0;
    if(a.GetValue(a_d)&&b.GetValue(b_d))
    {
        ret = a_d - b_d;
        return true;
    }
    else
        return false;
}
void LFormula::fun_MUL(OperatorData a,OperatorData b,double &ret)
{
	double a_d = 0;
	double b_d = 0;
	a.GetValue(a_d);
	b.GetValue(b_d);
	ret = a_d * b_d;
}
bool LFormula::fun_DIV(OperatorData a,OperatorData b,double &ret)
{
	double a_d = 0;
	double b_d = 0;
    if(a.GetValue(a_d)&&b.GetValue(b_d))
    {
        if(fabs(b_d-0)>MINIMUSDATA)
        {
            ret = a_d / b_d;
            return true;
        }
        else
        {
            qDebug()<<"除数为０";
            return false;
        }
    }
    return false;

}
void LFormula::fun_AND(OperatorData a,OperatorData b,double &ret)
{
    double a_d = 0;
    double b_d = 0;
    a.GetValue(a_d);
    b.GetValue(b_d);
    qint64 a_i64 = (qint64)a_d;
    qint64 b_i64 = (qint64)b_d;
    ret = (double)(a_i64 & b_i64);
}
void LFormula::fun_OR(OperatorData a,OperatorData b,double &ret)
{
    double a_d = 0;
    double b_d = 0;
    a.GetValue(a_d);
    b.GetValue(b_d);
    qint64 a_i64 = (qint64)a_d;
    qint64 b_i64 = (qint64)b_d;
    ret = (double)(a_i64 | b_i64);
}
void LFormula::fun_SHIFT_RIGHT(OperatorData a,OperatorData b,double &ret)
{
    double a_d = 0;
    double b_d = 0;
    a.GetValue(a_d);
    b.GetValue(b_d);
    qint64 a_i64 = (qint64)a_d;
    qint64 b_i64 = (qint64)b_d;
    ret = (double)(a_i64 >> b_i64);
}
void LFormula::fun_SHIFT_LEFT(OperatorData a,OperatorData b,double &ret)
{
    double a_d = 0;
    double b_d = 0;
    a.GetValue(a_d);
    b.GetValue(b_d);
    qint64 a_i64 = (qint64)a_d;
    qint64 b_i64 = (qint64)b_d;
    ret = (double)(a_i64 << b_i64);
}
//YCC({1,2},0xffffffff)
void LFormula::fun_YCC(OperatorData a,OperatorData b, double &ret)
{
//	double b_d = 0;
//	double a_d = 0;
//	a.GetCodeValue(a_d);
//	b.GetValue(b_d);
//	if(fabs(a_d-b_d)<DOUBLE_MIN_VALUE)
//	{
//		stru_Param* pa = NULL;
//		if (a.type==PARAM)
//		{
//			pa = (stru_Param*)a.param;
//			pa->ulParamDate = 0xffffffff;
//			pa->ulParamTime = 0xffffffff;
//			//跳出公式计算，并不进行后面的公式计算，也不需要改变参数的值
//            throw(formula_error,NODISPLAY);
//		}
//	}
//	else//设置参数的值
//	{
//		a.GetValue(ret);
//	}
}
//绝对值
void LFormula::fun_ABS(OperatorData a,double &ret)
{
	double a_d = 0;
	a.GetValue(a_d);
	ret = fabs(a_d);
}
//倒计时,只显示负数的绝对值,正数显示为零
void LFormula::fun_DJS(OperatorData a,double &ret)
{
	double a_d = 0;
	a.GetValue(a_d);
	if (a_d<0)
	{
		ret = -a_d;
	}
	else
	{
		ret = 0;
	}
}
//设置参数值,判断参数judge如果等于常量cont，则设置源参数sour的值，否则不更新值
void LFormula::fun_SET(OperatorData sour,OperatorData judge,OperatorData cont,double &ret)
{
//	double t_djudge = 0;
//	judge.GetValue(t_djudge);
//	double t_dcont = 0;
//	cont.GetValue(t_dcont);
//	if (fabs(t_djudge-t_dcont)<DOUBLE_MIN_VALUE)
//	{
//		sour.GetValue(ret);
//	}
//	else
//	{
//        //throw(formula_error,NOSETVALUE);
//	}
}
//将sour的参数时间设置到dest参数值上,
void LFormula::fun_TIM(OperatorData dest,OperatorData sour,double &ret)
{
//	if (sour.type==PARAM&&dest.type==PARAM)
//	{
//		stru_Param* pa = NULL;
//		stru_Param* pb = NULL;
//		pa = (stru_Param*)dest.param;
//		pb = (stru_Param*)sour.param;
//		::SetValue(pa,pb->ulParamTime);
//		::SetParamTime(pa);
//		ret = pb->ulParamTime;
//	}
//	else
//	{
//        throw(formula_error,INVALIDPARAM);
//	}
}
bool LFormula::ParseData(std::stack<OperatorData >& ostack,std::string data, OperatorData &opdataD)
{
    if (data.empty())
	{
        return false;
	}
	//数据
	if (data[0]<='9'&&data[0]>='0')
	{
		double data_dD=0;
        if (data.find('x')!=-1)
		{
            qint64 t_x=0;
            sscanf(data.c_str(),"%I64x",&t_x);
            data_dD = t_x;
		}
		else
		{
            sscanf(data.c_str(),"%lf",&data_dD);
		}
		opdataD.type = DATA;
		opdataD.data = data_dD;
		m_OperData.push_back(opdataD);
	}
	else//函数
	{
		bool find_fun = false;
        char pos=0;
		for (;pos<FUN_NAME_NUM;pos++)
		{
            if(data.find(fun_name[pos]))
            {
                find_fun = true;
                break;
            }
		}
		if (find_fun)
		{
			opdataD.type = FUN;
			opdataD.funname = (FunName)pos;
			HandleOperator(ostack,opdataD);
		}
		else
		{
            return false;
		}
	}
    return true;
}

/*!
  获取数据中包含的参数指针
  @return AbstractParam*  参数指针，NULL为没有
*/
bool LFormula::getParam(std::string formula,unsigned short &tn,unsigned short & pn)
{
    int pos = 0;
    int readpos = formula.find('[');
    if(readpos==-1)
        return false;
    int nextpos = formula.find(']',readpos);
    if(nextpos == -1)
        return false;
    std::string data = formula.substr(readpos+1,nextpos-readpos-1);
    if (!data.empty())
    {
        int commaPos = data.find(',',0);
        if(commaPos>0)
        {
            tn = atoi(data.substr(0,commaPos).c_str());
            pn = atoi(data.substr(commaPos+1,nextpos-pos-1).c_str());
            return true;
        }
    }
    return false;
}

AbstractParam* LFormula::getParam()
{
    return m_primaryParam;
}

/*
 *获得时标
*/
int LFormula::getDate()
{
    return m_savDate;
}

int LFormula::getTime()
{
    return m_savTime;
}

/*
 *设置时标
*/
void LFormula::setDate(int date)
{
    m_savDate = date;
}

void LFormula::setTime(int time)
{
    m_savTime = time;
}

bool LFormula::singleParam()
{
    if(m_OperData.size()==1)
    {
        OperatorData t_d = m_OperData.at(0);
        if(t_d.type==PARAM)
            return true;
    }
    return false;
}
