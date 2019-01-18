#include "Dgj_InitSocket.h"

CDgj_InitSocket::CDgj_InitSocket(unsigned __int8 hVe, unsigned __int8 lVer)
{
	m_InitResult = 0;
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(hVe, lVer);
	err = WSAStartup( wVersionRequested, &wsaData);
	unsigned __int8 tmpHver = HIBYTE(wsaData.wVersion), tmplVer = LOBYTE(wsaData.wVersion);
	if ( err != 0 || tmpHver != hVe || tmplVer != lVer) 
	{
		m_InitResult = -1;
	}
}

CDgj_InitSocket::~CDgj_InitSocket(void)
{
	WSACleanup();
}

int CDgj_InitSocket::GetInitResult()
{
	return m_InitResult;
}