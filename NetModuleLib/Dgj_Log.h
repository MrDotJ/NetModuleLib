#ifndef _DGJ_LOG_H_
#define _DGJ_LOG_H_

/*
调试日志类
author:dugaojun
date:2010-02-04
*/

enum LOG_FLAG
{
	LOG_FLAG_FILE = 1,
	LOG_FLAG_CON = 2,
	LOG_FLAG_FILE_CON = 3
};

#define DEBUG_BUFFER_MAXLEN 1024
#define DGJ_MAXFILE_NAME	256

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include "Dgj_Lock.h"

class CDgj_Log
{
public:
	/*
	构函数造
	szFileName:[in]指定文件名(不带扩展名)
	*/
	CDgj_Log(char *szFileName=NULL);
public:
	virtual ~CDgj_Log(void);
public:
	/*
	安全变参输出函数
	szBuf:[in]用户指定的输出缓冲区
	nMaxLength:[in]输出缓冲区大小
	szFormat:[in]格式化输出字符串(可以多个)
	return:格式化的字节总数
	*/

	static int SaftPrintf(char *szBuf, int nMaxLength, char *szFormat, ...);
	/*
	向指定的缓冲区输出1个时间字符串
	szBuf:[in]指定的缓冲区
	nMaxLength:[in]指定缓冲区大小
	return:输出的字节总数
	*/
	static int GetTimeStamp(char *szBuf, int nMaxLength);

	/*
	输出调试信息到文件和屏幕
	szFormat:[in]格式化字符串参数(可有多个)
	*/
	void Dbg2file(char *szformat, ...);
	void CloseLog(void);
	void SetLogFlag(LOG_FLAG flag);
private:
	CDgj_Lock m_FileLock;
	bool m_bWrite;
	LOG_FLAG m_WriteFlag;
private:
	char m_FileName[DGJ_MAXFILE_NAME];
};
#endif
