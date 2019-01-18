#ifndef _DGJ_LOCK_H_
#define _DGJ_LOCK_H_

/*
简单锁,多读单写锁
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

//单写多读锁c实现
typedef struct _DGJ_MULTI_READ_SINGLE_WRITE_LOCK
{
	int m_nReadCount;	//读计数标志
	bool m_bWriteFlag;	//写标志
	MUTEX m_Lock;
}SDGJMultiReadSingleWriteLock;
const unsigned long SDGJMultiReadSingleWriteLockSize = sizeof(SDGJMultiReadSingleWriteLock);

//创建单写多读锁
void MRSWLock_Create(SDGJMultiReadSingleWriteLock *pLock);
//销毁单写多读锁
void MRSWLock_Destroy(SDGJMultiReadSingleWriteLock *pLock);
//取得单写多读锁写状态
bool MRSWLock_GetWrite(SDGJMultiReadSingleWriteLock *pLock);
//取得读计数器
int  MRSWLock_GetReadCount(SDGJMultiReadSingleWriteLock *pLock);
//进入写操作
void MRSWLock_EableWrite(SDGJMultiReadSingleWriteLock *pLock);
//退出写操作
void MRSWLock_ExitWrite(SDGJMultiReadSingleWriteLock *pLock);
//进入读操作
int MRSWLock_AddRead(SDGJMultiReadSingleWriteLock *pLock);
//退出读操作
int MRSWLock_ExitRead(SDGJMultiReadSingleWriteLock *pLock);
//读转写操作
void MRSWLock_Read2Write(SDGJMultiReadSingleWriteLock *pLock);
//高精度睡眠函数
void DGJ_MinSleep(void);

//单写多读锁c++实现
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
