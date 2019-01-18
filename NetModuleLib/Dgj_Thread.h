#ifndef _DGJ_THREAD_H_
#define _DGJ_THREAD_H_
/*
多线程安全变量模板类
author:dugaojun
date:2010-02-01
*/
#include "Dgj_Lock.h"
template<class MVAR_TYPE>
class CDgj_Mvar
{
	public:
		CDgj_Mvar(){m_pBegin = new char[sizeof(MVAR_TYPE)];}
		virtual ~CDgj_Mvar(){delete []m_pBegin;}
		int GetLenght();
		MVAR_TYPE Set(MVAR_TYPE &val);
		MVAR_TYPE Get();
		MVAR_TYPE operator=(MVAR_TYPE val);
		MVAR_TYPE operator~();
		MVAR_TYPE operator++(int);
		MVAR_TYPE operator++();
		MVAR_TYPE operator--(int);
		MVAR_TYPE operator--();
		MVAR_TYPE operator+(MVAR_TYPE val);
		MVAR_TYPE operator-(MVAR_TYPE val);
		MVAR_TYPE operator*(MVAR_TYPE val);
		MVAR_TYPE operator/(MVAR_TYPE val);
		MVAR_TYPE operator&(MVAR_TYPE val);
		MVAR_TYPE operator|(MVAR_TYPE val);
		MVAR_TYPE operator^(MVAR_TYPE val);
		MVAR_TYPE operator+=(MVAR_TYPE val);
		MVAR_TYPE operator-=(MVAR_TYPE val);
		MVAR_TYPE operator*=(MVAR_TYPE val);
		MVAR_TYPE operator/=(MVAR_TYPE val);
		MVAR_TYPE operator&=(MVAR_TYPE val);
		MVAR_TYPE operator|=(MVAR_TYPE val);
		MVAR_TYPE operator^=(MVAR_TYPE val);
		bool operator>(MVAR_TYPE val);
		bool operator<(MVAR_TYPE val);
		bool operator>=(MVAR_TYPE val);
		bool operator<=(MVAR_TYPE val);
		bool operator==(MVAR_TYPE val);
		bool operator!=(MVAR_TYPE val);
	private:
		char *m_pBegin;
		CDgj_Lock m_csLockHandle;	
};

typedef CDgj_Mvar<char>				DGJ_CHAR;
typedef CDgj_Mvar<unsigned char>	DGJ_UCHAR;
typedef CDgj_Mvar<short>			DGJ_SHORT;
typedef CDgj_Mvar<unsigned short>	DGJ_USHORT;
typedef CDgj_Mvar<int>				DGJ_INT;
typedef CDgj_Mvar<unsigned int>		DGJ_UINT;
typedef CDgj_Mvar<long>				DGJ_LONG;
typedef CDgj_Mvar<unsigned long>	DGJ_ULONG;
typedef CDgj_Mvar<float>			DGJ_FLOAT;
typedef CDgj_Mvar<double>			DGJ_DOUBLE;
typedef CDgj_Mvar<bool>				DGJ_BOOL;
typedef CDgj_Mvar<LPVOID>			DGJ_LPVOID;
typedef CDgj_Mvar<BYTE>				DGJ_BYTE;

//-------------------------------------------------------------------------------------------------------------------------------------

template<class MVAR_TYPE>
int CDgj_Mvar<MVAR_TYPE>::GetLenght()
{
	return sizeof(MVAR_TYPE);
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::Set(MVAR_TYPE &val)
{
	m_csLockHandle.Lock();
	memcpy(m_pBegin, (char *)&val, sizeof(MVAR_TYPE));
	m_csLockHandle.UnLock();
	return val;
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::Get()
{
	MVAR_TYPE val;
	m_csLockHandle.Lock();
	memcpy(&val, m_pBegin, sizeof(MVAR_TYPE));
	m_csLockHandle.UnLock();
	return val;
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::operator=(MVAR_TYPE val)
{
	MVAR_TYPE myval;
	m_csLockHandle.Lock();
	myval = val;
	memcpy(m_pBegin, &myval, sizeof(MVAR_TYPE));
	m_csLockHandle.UnLock();
	return myval;
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::operator~()
{
	MVAR_TYPE myval;
	m_csLockHandle.Lock();
	memcpy(&myval, &m_pBegin, sizeof(MVAR_TYPE));
	myval ~= myval;
	m_csLockHandle.UnLock();
	return myval;
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::operator++(int)
{
	MVAR_TYPE val;
	m_csLockHandle.Lock();
	memcpy(&val, m_pBegin, sizeof(MVAR_TYPE));
	val++;
	memcpy(m_pBegin, &val, sizeof(MVAR_TYPE));
	m_csLockHandle.UnLock();
	return val;
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::operator++()
{
	MVAR_TYPE val;
	m_csLockHandle.Lock();
	memcpy(&val, m_pBegin, sizeof(MVAR_TYPE));
	val++;
	memcpy(m_pBegin, &val, sizeof(MVAR_TYPE));
	m_csLockHandle.UnLock();
	return val;
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::operator--(int)
{
	MVAR_TYPE val;
	m_csLockHandle.Lock();
	memcpy(&val, m_pBegin, sizeof(MVAR_TYPE));
	val--;
	memcpy(m_pBegin, &val, sizeof(MVAR_TYPE));
	m_csLockHandle.UnLock();
	return val;
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::operator--()
{
	MVAR_TYPE val;
	m_csLockHandle.Lock();
	memcpy(&val, m_pBegin, sizeof(MVAR_TYPE));
	val--;
	memcpy(m_pBegin, &val, sizeof(MVAR_TYPE));
	m_csLockHandle.UnLock();
	return val;
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::operator+(MVAR_TYPE val)
{
	MVAR_TYPE myval;
	m_csLockHandle.Lock();
	memcpy(&myval, m_pBegin, sizeof(MVAR_TYPE));
	myval += val;
	m_csLockHandle.UnLock();
	return myval;
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::operator-(MVAR_TYPE val)
{
	MVAR_TYPE myval;
	m_csLockHandle.Lock();
	memcpy(&myval, m_pBegin, sizeof(MVAR_TYPE));
	myval -= val;
	m_csLockHandle.UnLock();
	return myval;
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::operator*(MVAR_TYPE val)
{
	MVAR_TYPE myval;
	m_csLockHandle.Lock();
	memcpy(&myval, m_pBegin, sizeof(MVAR_TYPE));
	myval *= val;
	m_csLockHandle.UnLock();
	return myval;
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::operator/(MVAR_TYPE val)
{
	MVAR_TYPE myval;
	m_csLockHandle.Lock();
	memcpy(&myval, m_pBegin, sizeof(MVAR_TYPE));
	myval /= val;
	m_csLockHandle.UnLock();
	return myval;
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::operator&(MVAR_TYPE val)
{
	MVAR_TYPE myval;
	m_csLockHandle.Lock();
	memcpy(&myval, m_pBegin, sizeof(MVAR_TYPE));
	myval &= val;
	m_csLockHandle.UnLock();
	return myval;
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::operator|(MVAR_TYPE val)
{
	MVAR_TYPE myval;
	m_csLockHandle.Lock();
	memcpy(&myval, m_pBegin, sizeof(MVAR_TYPE));
	myval |= val;
	m_csLockHandle.UnLock();
	return myval;
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::operator^(MVAR_TYPE val)
{
	MVAR_TYPE myval;
	m_csLockHandle.Lock();
	memcpy(&myval, m_pBegin, sizeof(MVAR_TYPE));
	myval ^= val;
	m_csLockHandle.UnLock();
	return myval;
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::operator+=(MVAR_TYPE val)
{
	MVAR_TYPE myval;
	m_csLockHandle.Lock();
	memcpy(&myval, m_pBegin, sizeof(MVAR_TYPE));
	myval += val;
	memcpy(m_pBegin, &myval, sizeof(MVAR_TYPE));
	m_csLockHandle.UnLock();
	return myval;
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::operator-=(MVAR_TYPE val)
{
	MVAR_TYPE myval;
	m_csLockHandle.Lock();
	memcpy(&myval, m_pBegin, sizeof(MVAR_TYPE));
	myval -= val;
	memcpy(m_pBegin, &myval, sizeof(MVAR_TYPE));
	m_csLockHandle.UnLock();
	return myval;
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::operator*=(MVAR_TYPE val)
{
	MVAR_TYPE myval;
	m_csLockHandle.Lock();
	memcpy(&myval, m_pBegin, sizeof(MVAR_TYPE));
	myval *= val;
	memcpy(m_pBegin, &myval, sizeof(MVAR_TYPE));
	m_csLockHandle.UnLock();
	return myval;
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::operator/=(MVAR_TYPE val)
{
	MVAR_TYPE myval;
	m_csLockHandle.Lock();
	memcpy(&myval, m_pBegin, sizeof(MVAR_TYPE));
	myval /= val;
	memcpy(m_pBegin, &myval, sizeof(MVAR_TYPE));
	m_csLockHandle.UnLock();
	return myval;
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::operator&=(MVAR_TYPE val)
{
	MVAR_TYPE myval;
	m_csLockHandle.Lock();
	memcpy(&myval, m_pBegin, sizeof(MVAR_TYPE));
	myval &= val;
	memcpy(m_pBegin, &myval, sizeof(MVAR_TYPE));
	m_csLockHandle.UnLock();
	return myval;
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::operator|=(MVAR_TYPE val)
{
	MVAR_TYPE myval;
	m_csLockHandle.Lock();
	memcpy(&myval, m_pBegin, sizeof(MVAR_TYPE));
	myval |= val;
	memcpy(m_pBegin, &myval, sizeof(MVAR_TYPE));
	m_csLockHandle.UnLock();
	return myval;
}

template<class MVAR_TYPE>
MVAR_TYPE CDgj_Mvar<MVAR_TYPE>::operator^=(MVAR_TYPE val)
{
	MVAR_TYPE myval;
	m_csLockHandle.Lock();
	memcpy(&myval, m_pBegin, sizeof(MVAR_TYPE));
	myval ^= val;
	memcpy(m_pBegin, &myval, sizeof(MVAR_TYPE));
	m_csLockHandle.UnLock();
	return myval;
}

template<class MVAR_TYPE>
bool CDgj_Mvar<MVAR_TYPE>::operator>(MVAR_TYPE val)
{
	MVAR_TYPE myval;
	bool bRet;
	m_csLockHandle.Lock();
	memcpy(&myval, m_pBegin, sizeof(MVAR_TYPE));
	bRet = (myval>val);
	m_csLockHandle.UnLock();
	return bRet;
}

template<class MVAR_TYPE>
bool CDgj_Mvar<MVAR_TYPE>::operator<(MVAR_TYPE val)
{
	MVAR_TYPE myval;
	bool bRet;
	m_csLockHandle.Lock();
	memcpy(&myval, m_pBegin, sizeof(MVAR_TYPE));
	bRet = (myval<val);
	m_csLockHandle.UnLock();
	return bRet;
}

template<class MVAR_TYPE>
bool CDgj_Mvar<MVAR_TYPE>::operator>=(MVAR_TYPE val)
{
	MVAR_TYPE myval;
	bool bRet;
	m_csLockHandle.Lock();
	memcpy(&myval, m_pBegin, sizeof(MVAR_TYPE));
	bRet = (myval>=val);
	m_csLockHandle.UnLock();
	return bRet;
}

template<class MVAR_TYPE>
bool CDgj_Mvar<MVAR_TYPE>::operator<=(MVAR_TYPE val)
{
	MVAR_TYPE myval;
	bool bRet;
	m_csLockHandle.Lock();
	memcpy(&myval, m_pBegin, sizeof(MVAR_TYPE));
	bRet = (myval<=val);
	m_csLockHandle.UnLock();
	return bRet;
}

template<class MVAR_TYPE>
bool CDgj_Mvar<MVAR_TYPE>::operator==(MVAR_TYPE val)
{
	MVAR_TYPE myval;
	bool bRet;
	m_csLockHandle.Lock();
	memcpy(&myval, m_pBegin, sizeof(MVAR_TYPE));
	bRet = (myval==val);
	m_csLockHandle.UnLock();
	return bRet;
}

template<class MVAR_TYPE>
bool CDgj_Mvar<MVAR_TYPE>::operator!=(MVAR_TYPE val)
{
	MVAR_TYPE myval;
	bool bRet;
	m_csLockHandle.Lock();
	memcpy(&myval, m_pBegin, sizeof(MVAR_TYPE));
	bRet = (myval!=val);
	m_csLockHandle.UnLock();
	return bRet;
}

#endif