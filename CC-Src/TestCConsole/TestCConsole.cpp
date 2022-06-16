// TestCConsole.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <fstream>
using namespace std;

#pragma pack(1)
struct ZxParam
{
	unsigned short tableNO;
	unsigned short paramNo;
	unsigned char paraName[255];
	unsigned char paraCode[255];
	unsigned char transType;
	unsigned short dataLen;
	unsigned char changeType;
	double lowLimit;
	double upLimint;
	double quotiety;
	unsigned char unit[255];
	unsigned char displayType[255];
	unsigned char theroyValue[255];
	unsigned char valueRange[255];
	unsigned int groupIndex;
	unsigned char note[255];

	ZxParam()
	{
		memset(paraName, 0, 255);
		memset(paraCode, 0, 255);
		memset(unit, 0, 255);
		memset(displayType, 0, 255);
		memset(theroyValue, 0, 255);
		memset(valueRange, 0, 255);
		memset(note, 0, 255);
	}
};

#pragma pack()

#pragma warning(disable : 4996)
int main()
{
	ZxParam* param = new ZxParam;
	FILE* f = fopen("D:\\1.dat", "rb");
	int len = sizeof(ZxParam);	
	size_t count = fread(param, 1, 1821, f);
	string s = string((char*)param->paraName, 255);
    return 0;
}

