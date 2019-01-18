#include "Dgj_Log.h"

CDgj_Log::CDgj_Log(char *szFileName)
{
	ZeroMemory(m_FileName, DGJ_MAXFILE_NAME);
	if(NULL != szFileName)
	{
		int len = strlen(szFileName);
		memcpy(m_FileName, szFileName, len);
		memcpy(m_FileName+len, ".txt", strlen(".txt"));
	}
	else
	{
		GetTimeStamp(m_FileName, DGJ_MAXFILE_NAME);
		int len = strlen(m_FileName);
		memcpy(m_FileName+len, ".txt", strlen(".txt"));
	}
	m_bWrite = true;
	m_WriteFlag = LOG_FLAG_FILE;
}

CDgj_Log::~CDgj_Log(void)
{
}

int CDgj_Log::SaftPrintf(char *szBuf, int nMaxLength, char *szFormat, ...)
{
	int nListCount=0;
	va_list pArgList;
	if(!szBuf)
		goto SAFEPRINTF_END_PROCESS;
	va_start(pArgList, szFormat);
	nListCount += _vsnprintf(szBuf+nListCount, nMaxLength-nListCount, szFormat, pArgList);
	va_end(pArgList);
	if (nListCount > (nMaxLength-1))
	{
		nListCount = nMaxLength -1;
	}
	*(szBuf+nListCount) = 0;
SAFEPRINTF_END_PROCESS:
	return nListCount;
}

int CDgj_Log::GetTimeStamp(char *szBuf, int nMaxLength)
{
	time_t t;
	struct tm *pTm = NULL;
	int nLength = 0;
	time(&t);
	pTm = localtime(&t);
	nLength = CDgj_Log::SaftPrintf(szBuf, nMaxLength, "%d-%d-%d-%d", pTm->tm_mon+1, pTm->tm_mday, pTm->tm_hour, pTm->tm_min);
	szBuf[nLength] = 0;
	return nLength;
}

void CDgj_Log::Dbg2file(char *szFormat, ...)
{
	if(!m_bWrite)
		return;
	char szBuf[DEBUG_BUFFER_MAXLEN]={0};
	char szTime[256]={0};
	int nListCount=0;
	
	va_list pArgList;
	va_start(pArgList, szFormat);
	nListCount += _vsnprintf(szBuf+nListCount, DEBUG_BUFFER_MAXLEN-nListCount, szFormat, pArgList);
	va_end(pArgList);
	
	nListCount = DEBUG_BUFFER_MAXLEN -1;
	*(szBuf+nListCount) = 0;
	GetTimeStamp(szTime, 256);
	m_FileLock.Lock();
	try
	{
		FILE *fp=NULL;
		fp = fopen(m_FileName, "a+");
		if(fp)
		{
			switch(m_WriteFlag)
			{
			case LOG_FLAG_FILE_CON:
				fprintf(fp, "[%s]:%s\n", szTime, szFormat);
				fflush(fp);
				printf("[%s]:%s\n", szTime, szFormat);
				break;
			case LOG_FLAG_FILE:
				fprintf(fp, "[%s]:%s\n", szTime, szFormat);
				fflush(fp);
				break;
			case LOG_FLAG_CON:
				printf("[%s]:%s\n", szTime, szFormat);
				break;
			}
			fclose(fp);
		}
		else
			nListCount = 0;
	}
	catch(...)
	{
		;;
	}
	m_FileLock.UnLock();
}

void CDgj_Log::CloseLog(void)
{
	m_bWrite = false;
}

void CDgj_Log::SetLogFlag(LOG_FLAG flag)
{
	m_WriteFlag = flag;
}