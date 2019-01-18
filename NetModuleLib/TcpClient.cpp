#include "TcpClient.h"

CTcpClient::CTcpClient()
{
	m_ThreadCount = 0;
	m_DisconCallbackfunction = NULL;
}

CTcpClient::~CTcpClient()
{
	while(m_ThreadCount.Get() > 0)
	{
		Sleep(10);
	}
}

bool CTcpClient::ConnectServer(char ip[16], unsigned __int16 port)
{
	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	if(s == INVALID_SOCKET)
	{
		return false;
	}

	sockaddr_in addr;
	addr.sin_addr.S_un.S_addr = inet_addr(ip);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if(connect(s, (sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(s);
		return false;
	}

	HANDLE h1 = CreateThread(NULL, 0, CTcpClient::RecvServerThread, this, CREATE_SUSPENDED, 0);
	if(NULL == h1)
	{
		closesocket(s);
		return false;
	}
	Sleep(250);
	m_ThreadCount++;

	HANDLE h2 = CreateThread(NULL, 0, CTcpClient::MsgServerThread, this, CREATE_SUSPENDED, 0);
	if(NULL == h2)
	{
		ResumeThread(h1);
		CloseHandle(h1);
		closesocket(s);
		return false;
	}	
	m_ThreadCount++;

	m_hInstance.SetSocketAndAddr(s, addr);
	m_hInstance.SetActiveState(true);
	ResumeThread(h1);
	CloseHandle(h1);
	ResumeThread(h2);
	CloseHandle(h2);
	
	return true;
}

void CTcpClient::RegsiterMsgToServer(unsigned __int16 msgID, MsgFunc msgFunc)
{
	m_MsgPointMap.insert(MSG_PointMap::value_type(msgID, msgFunc));
}

CNetPackage *CTcpClient::GetSendIdlePackage(void)
{
	return m_hInstance.GetSendIdlePackage();
}

void CTcpClient::CloseConnect(void)
{
	m_hInstance.SetActiveState(false);
}

DWORD WINAPI CTcpClient::RecvServerThread(LPVOID lParam)
{
	CTcpClient *pClient = (CTcpClient *)lParam; 
	while(pClient->m_hInstance.IsActive())
	{
		int len = 0;
		len = recv(pClient->m_hInstance.m_RecvPack.m_s, (char *)(pClient->m_hInstance.m_RecvPack.m_buf), MAX_NETPACK_SIZE, 0);
		if(len > 0)
		{
			pClient->m_hInstance.CombinationNetData(len);
		}
		else
		{
			if(NULL != pClient->m_DisconCallbackfunction)
				pClient->m_DisconCallbackfunction(&(pClient->m_hInstance));
			pClient->m_hInstance.SetActiveState(false);
		}
		Sleep(1);
	}
	pClient->m_ThreadCount--;
	return 0;
}

DWORD WINAPI CTcpClient::MsgServerThread(LPVOID lParam)
{
	CTcpClient *pClient = (CTcpClient *)lParam;
	while(pClient->m_hInstance.IsActive())
	{
		CNetPackage *pNetPack = pClient->m_hInstance.GetMsgNetPackage();
		if(NULL != pNetPack)
		{
			pNetPack->ReadInt32();
			unsigned __int16 proc = pNetPack->ReadInt16();
			MSG_PointMap::iterator it_map = pClient->m_MsgPointMap.find(proc);
			if(it_map != pClient->m_MsgPointMap.end())
			{
				MsgFunc func = NULL;
				func = it_map->second;
				if(func)
					func(pNetPack);
			}
			pNetPack->m_bUseState = false;
		}
		Sleep(1);
	}
	pClient->m_ThreadCount--;
	return 0;
}

void CTcpClient::HeratFunc(CNetPackage *pNetPack)
{//心跳处理函数
	CNetPackage *pSendPack = pNetPack->m_pHinstance->GetSendIdlePackage();
	if(NULL != pSendPack)
	{
		pSendPack->WriteProc(MSG_HEARTBEAT);
		pSendPack->SendMSG_Client();
	}
}

void CTcpClient::OpenHerat(void)
{
	RegsiterMsgToServer(MSG_HEARTBEAT, CTcpClient::HeratFunc);
}

void CTcpClient::CloseHerat(void)
{
	MSG_PointMap::iterator it_map = m_MsgPointMap.find(MSG_HEARTBEAT);
	if(it_map != m_MsgPointMap.end())
	{
		m_MsgPointMap.erase(it_map);
	}
}

void CTcpClient::SetConnectStateCallbackfunction(TcpClientCallbackfunction DisConnectFuc)
{
	m_DisconCallbackfunction = DisConnectFuc;
}

bool CTcpClient::IsActive(void)
{
	return m_hInstance.IsActive();
}