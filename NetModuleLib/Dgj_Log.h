#ifndef _DGJ_LOG_H_
#define _DGJ_LOG_H_

/*
������־��
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
	��������
	szFileName:[in]ָ���ļ���(������չ��)
	*/
	CDgj_Log(char *szFileName=NULL);
public:
	virtual ~CDgj_Log(void);
public:
	/*
	��ȫ����������
	szBuf:[in]�û�ָ�������������
	nMaxLength:[in]�����������С
	szFormat:[in]��ʽ������ַ���(���Զ��)
	return:��ʽ�����ֽ�����
	*/

	static int SaftPrintf(char *szBuf, int nMaxLength, char *szFormat, ...);
	/*
	��ָ���Ļ��������1��ʱ���ַ���
	szBuf:[in]ָ���Ļ�����
	nMaxLength:[in]ָ����������С
	return:������ֽ�����
	*/
	static int GetTimeStamp(char *szBuf, int nMaxLength);

	/*
	���������Ϣ���ļ�����Ļ
	szFormat:[in]��ʽ���ַ�������(���ж��)
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
