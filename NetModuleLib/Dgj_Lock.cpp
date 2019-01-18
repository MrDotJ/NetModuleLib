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

//������д�����
void MRSWLock_Create(SDGJMultiReadSingleWriteLock *pLock)
{
	MUTEXINIT(&(pLock->m_Lock));
	pLock->m_nReadCount = 0;
	pLock->m_bWriteFlag = false;
}

//���ٵ�д�����
void MRSWLock_Destroy(SDGJMultiReadSingleWriteLock *pLock)
{
	MUTEXLOCK(&(pLock->m_Lock));
	MUTEXUNLOCK(&(pLock->m_Lock));
	MUTEXDESTROY(&(pLock->m_Lock));
}

//ȡ�õ�д�����д״̬
bool MRSWLock_GetWrite(SDGJMultiReadSingleWriteLock *pLock)
{
	bool bRet = false;
	MUTEXLOCK(&(pLock->m_Lock));
	bRet = pLock->m_bWriteFlag;
	MUTEXUNLOCK(&(pLock->m_Lock));
	return bRet;
}

//ȡ�ö�������
int  MRSWLock_GetReadCount(SDGJMultiReadSingleWriteLock *pLock)
{
	int nRet = 0;
	MUTEXLOCK(&(pLock->m_Lock));
	nRet = pLock->m_nReadCount;
	MUTEXUNLOCK(&(pLock->m_Lock));
	return nRet;
}

//����д����
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
	{//�ȵ�����������ȫ�����
		DGJ_MinSleep();
	}
}
//�˳�д����
void MRSWLock_ExitWrite(SDGJMultiReadSingleWriteLock *pLock)
{
	MUTEXLOCK(&(pLock->m_Lock));
	pLock->m_bWriteFlag = false;
	MUTEXUNLOCK(&(pLock->m_Lock));
}

//���������
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

//�˳�������
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

//��תд����
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