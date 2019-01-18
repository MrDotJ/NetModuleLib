#ifndef _DGJ_INIT_SOCKET_H_
#define _DGJ_INIT_SOCKET_H_
#include "winsock2.h"
/*
套接字库初始化类
author:dugaojun
date:2010-02-01
*/
class CDgj_InitSocket
{
public:
	/*
	CDgj_InitSocket构造函数
	hVer:[in]高版本号
	LVer:[in]低版本号
	*/
	CDgj_InitSocket(unsigned __int8 hVer=2, unsigned __int8 lVer=2);
	/*
	取得套接字库初始化结果
	return:初套接字库初始化成功返回0,失败返回-1
	*/
	int GetInitResult();
public:
	virtual ~CDgj_InitSocket(void);
private:
	int m_InitResult;
};

#endif
