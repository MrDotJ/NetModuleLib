// Dgj_Lock.cpp: implementation of the CDgj_Lock class.
//
//////////////////////////////////////////////////////////////////////
#include "Dgj_Lock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDgj_Lock::CDgj_Lock()
{
	MUTEXINIT(&m_Lock);
}

CDgj_Lock::~CDgj_Lock()
{
	MUTEXDESTROY(&m_Lock);
}

void CDgj_Lock::Lock(void)
{
	MUTEXLOCK(&m_Lock);
}

void CDgj_Lock::UnLock(void)
{
	MUTEXUNLOCK(&m_Lock);
}

//创建单写多读锁
void MRSWLock_Create(SDGJMultiReadSingleWriteLock *pLock)
{
	MUTEXINIT(&(pLock->m_Lock));
	pLock->m_nReadCount = 0;
	pLock->m_bWriteFlag = false;
}

//销毁单写多读锁
void MRSWLock_Destroy(SDGJMultiReadSingleWriteLock *pLock)
{
	MUTEXLOCK(&(pLock->m_Lock));
	MUTEXUNLOCK(&(pLock->m_Lock));
	MUTEXDESTROY(&(pLock->m_Lock));
}

//取得单写多读锁写状态
bool MRSWLock_GetWrite(SDGJMultiReadSingleWriteLock *pLock)
{
	bool bRet = false;
	MUTEXLOCK(&(pLock->m_Lock));
	bRet = pLock->m_bWriteFlag;
	MUTEXUNLOCK(&(pLock->m_Lock));
	return bRet;
}

//取得读计数器
int  MRSWLock_GetReadCount(SDGJMultiReadSingleWriteLock *pLock)
{
	int nRet = 0;
	MUTEXLOCK(&(pLock->m_Lock));
	nRet = pLock->m_nReadCount;
	MUTEXUNLOCK(&(pLock->m_Lock));
	return nRet;
}

//进入写操作
void MRSWLock_EableWrite(SDGJMultiReadSingleWriteLock *pLock)
{
	while (true)
	{
		MUTEXLOCK(&(pLock->m_Lock));
		if(!pLock->m_bWriteFlag)
		{
			pLock->m_bWriteFlag = true;
			MUTEXUNLOCK(&(pLock->m_Lock));
			goto MRSWLock_Enable_Wait_Read_Clean;
		}
		MUTEXUNLOCK(&(pLock->m_Lock));
		DGJ_MinSleep();
	}

MRSWLock_Enable_Wait_Read_Clean:
	while (MRSWLock_GetReadCount(pLock))
	{//等到其它读操作全部完成
		DGJ_MinSleep();
	}
}
//退出写操作
void MRSWLock_ExitWrite(SDGJMultiReadSingleWriteLock *pLock)
{
	MUTEXLOCK(&(pLock->m_Lock));
	pLock->m_bWriteFlag = false;
	MUTEXUNLOCK(&(pLock->m_Lock));
}

//进入读操作
int MRSWLock_AddRead(SDGJMultiReadSingleWriteLock *pLock)
{
	while (true)
	{
		MUTEXLOCK(&(pLock->m_Lock));
		if(!pLock->m_bWriteFlag)
		{
			pLock->m_nReadCount++;
			MUTEXUNLOCK(&(pLock->m_Lock));
			return MRSWLock_GetReadCount(pLock);
		}
		MUTEXUNLOCK(&(pLock->m_Lock));
		DGJ_MinSleep();
	}
}

//退出读操作
int MRSWLock_ExitRead(SDGJMultiReadSingleWriteLock *pLock)
{
	int nRet = 0;
	MUTEXLOCK(&(pLock->m_Lock));
	if(0<pLock->m_nReadCount)
		pLock->m_nReadCount--;
	nRet = pLock->m_nReadCount;
	MUTEXUNLOCK(&(pLock->m_Lock));
	return nRet;
}

//读转写操作
void MRSWLock_Read2Write(SDGJMultiReadSingleWriteLock *pLock)
{
	while(true)
	{
		MUTEXLOCK(&(pLock->m_Lock));
		if (!pLock->m_bWriteFlag)
		{
			pLock->m_bWriteFlag = true;
			if(0<pLock->m_nReadCount)
			{
				pLock->m_nReadCount--;
			}
			MUTEXUNLOCK(&pLock->m_Lock);
			goto MRSWLock_Read2Write_Wait_Read_Clean;
		}
		MUTEXUNLOCK(&(pLock->m_Lock));
		DGJ_MinSleep();
	}
MRSWLock_Read2Write_Wait_Read_Clean:
	while(MRSWLock_GetReadCount(pLock))
	{
		DGJ_MinSleep();
	}
}

void DGJ_MinSleep(void)
{
	Sleep(1);
}

CMultiReadSingleWriteLock::CMultiReadSingleWriteLock()
{
	MRSWLock_Create(&m_Lock);
}

CMultiReadSingleWriteLock::~CMultiReadSingleWriteLock()
{
	MRSWLock_Destroy(&m_Lock);
}

void CMultiReadSingleWriteLock::EnableWrite(void)
{
	MRSWLock_EableWrite(&m_Lock);
}

void CMultiReadSingleWriteLock::DisableWrite(void)
{
	MRSWLock_ExitWrite(&m_Lock);
}

void CMultiReadSingleWriteLock::Read2Write(void)
{
	MRSWLock_Read2Write(&m_Lock);
}

void CMultiReadSingleWriteLock::ExitRead(void)
{
	MRSWLock_ExitRead(&m_Lock);
}

void CMultiReadSingleWriteLock::AddRead(void)
{
	MRSWLock_AddRead(&m_Lock);
}

bool CMultiReadSingleWriteLock::GetWrite(void)
{
	return MRSWLock_GetWrite(&m_Lock);
}

int CMultiReadSingleWriteLock::GetReadCount(void)
{
	return MRSWLock_GetReadCount(&m_Lock);
}