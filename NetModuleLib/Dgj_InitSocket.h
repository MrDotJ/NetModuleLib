#ifndef _DGJ_INIT_SOCKET_H_
#define _DGJ_INIT_SOCKET_H_
#include "winsock2.h"
/*
�׽��ֿ��ʼ����
author:dugaojun
date:2010-02-01
*/
class CDgj_InitSocket
{
public:
	/*
	CDgj_InitSocket���캯��
	hVer:[in]�߰汾��
	LVer:[in]�Ͱ汾��
	*/
	CDgj_InitSocket(unsigned __int8 hVer=2, unsigned __int8 lVer=2);
	/*
	ȡ���׽��ֿ��ʼ�����
	return:���׽��ֿ��ʼ���ɹ�����0,ʧ�ܷ���-1
	*/
	int GetInitResult();
public:
	virtual ~CDgj_InitSocket(void);
private:
	int m_InitResult;
};

#endif
