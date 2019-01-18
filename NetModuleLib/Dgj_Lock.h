#ifndef _DGJ_LOCK_H_
#define _DGJ_LOCK_H_

/*
����,�����д��
author:dugaojun
date:2010-02-01
*/

#include "windows.h"
#define MUTEX CRITICAL_SECTION
#define MUTEXINIT(m) InitializeCriticalSection(m)
#define MUTEXLOCK(m) EnterCriticalSection(m)
#define MUTEXUNLOCK(m) LeaveCriticalSection(m)
#define MUTEXDESTROY(m) DeleteCriticalSection(m)

class CDgj_Lock  
{
public:
	CDgj_Lock(void);
	virtual ~CDgj_Lock(void);
public:
	void Lock(void);
	void UnLock(void);
private:
	MUTEX m_Lock;
};

//��д�����cʵ��
typedef struct _DGJ_MULTI_READ_SINGLE_WRITE_LOCK
{
	int m_nReadCount;	//��������־
	bool m_bWriteFlag;	//д��־
	MUTEX m_Lock;
}SDGJMultiReadSingleWriteLock;
const unsigned long SDGJMultiReadSingleWriteLockSize = sizeof(SDGJMultiReadSingleWriteLock);

//������д�����
void MRSWLock_Create(SDGJMultiReadSingleWriteLock *pLock);
//���ٵ�д�����
void MRSWLock_Destroy(SDGJMultiReadSingleWriteLock *pLock);
//ȡ�õ�д�����д״̬
bool MRSWLock_GetWrite(SDGJMultiReadSingleWriteLock *pLock);
//ȡ�ö�������
int  MRSWLock_GetReadCount(SDGJMultiReadSingleWriteLock *pLock);
//����д����
void MRSWLock_EableWrite(SDGJMultiReadSingleWriteLock *pLock);
//�˳�д����
void MRSWLock_ExitWrite(SDGJMultiReadSingleWriteLock *pLock);
//���������
int MRSWLock_AddRead(SDGJMultiReadSingleWriteLock *pLock);
//�˳�������
int MRSWLock_ExitRead(SDGJMultiReadSingleWriteLock *pLock);
//��תд����
void MRSWLock_Read2Write(SDGJMultiReadSingleWriteLock *pLock);
//�߾���˯�ߺ���
void DGJ_MinSleep(void);

//��д�����c++ʵ��
class CMultiReadSingleWriteLock
{
public:
	CMultiReadSingleWriteLock();
	virtual ~CMultiReadSingleWriteLock();
public:
	void EnableWrite(void);
	void DisableWrite(void);
	void Read2Write(void);
	void ExitRead(void);
	void AddRead(void);
	bool GetWrite(void);
	int GetReadCount(void);
private:
	SDGJMultiReadSingleWriteLock m_Lock;
};

#endif
