#include "TcpServe.h"

CTcpServer::CTcpServer(int ConnectNum /* = 1000 */)
{
	m_MaxConnect = ConnectNum;
	m_pHinstanceList = new CTcpConnectHinstance[m_MaxConnect];
	m_s = INVALID_SOCKET;
	m_hCompletion = INVALID_HANDLE_VALUE;
	m_bRun = false;
	m_ThreadCount = 0;
	m_bHerat = false;
	m_DisconCallbackfunction = NULL;
	m_ConOkCallbackfunction = NULL;
}

CTcpServer::~CTcpServer()
{
	delete []m_pHinstanceList;
}

void CTcpServer::OpenHerat(void)
{
	m_bHerat = true;
	RegsiterMsgToServer(MSG_HEARTBEAT, CTcpServer::HeartbeatFunc);
}

void CTcpServer::CloseHerat(void)
{
	m_bHerat = false;
	MSG_PointMap::iterator it_map = m_MsgPointMap.find(MSG_HEARTBEAT);
	if(it_map != m_MsgPointMap.end())
	{
		m_MsgPointMap.erase(it_map);
	}
}

CTcpConnectHinstance *CTcpServer::GethInstanceOfNO(unsigned __int32 ConnectNO)
{
	return &m_pHinstanceList[ConnectNO];
}

bool CTcpServer::CreateServer(char Ip[16], unsigned __int16 port)
{
	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	if(s == INVALID_SOCKET)
		return false;

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr(Ip);
	addr.sin_port = ntohs(port);
	
	if(SOCKET_ERROR==bind(s, (sockaddr *)&addr, sizeof(addr)))
	{
		closesocket(s);
		return false;
	}

	m_hCompletion = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 2);
	if(NULL == m_hCompletion)
	{
		closesocket(s);
		return false;
	}
	
	HANDLE h1, h2, h3, h4;
	h1 = CreateThread(NULL, 0, CTcpServer::MSGManageThread, this, CREATE_SUSPENDED, 0);
	if(NULL == h1)
	{
		closesocket(s);
		CloseHandle(m_hCompletion);
		return false;
	}
	m_ThreadCount++;
	Sleep(250);
	
	h2 = CreateThread(NULL, 0, CTcpServer::AcceptManageThread, this, CREATE_SUSPENDED, 0);
	if(NULL == h2)
	{
		closesocket(s);
		CloseHandle(m_hCompletion);
		ResumeThread(h1);
		CloseHandle(h1);
		return false;
	}
	Sleep(250);

	h3 = CreateThread(NULL, 0, CTcpServer::ServerManageThread, this, CREATE_SUSPENDED, 0);
	if(NULL == h3)
	{
		closesocket(s);
		CloseHandle(m_hCompletion);
		ResumeThread(h1);
		CloseHandle(h1);
		ResumeThread(h2);
		CloseHandle(h2);
		return false;
	}
	m_ThreadCount++;
	Sleep(250);

	h4 = CreateThread(NULL, 0, CTcpServer::HeartbeatManageThread, this, CREATE_SUSPENDED, 0);
	if(NULL == h4)
	{
		closesocket(s);
		CloseHandle(m_hCompletion);
		ResumeThread(h1);
		CloseHandle(h1);
		ResumeThread(h2);
		CloseHandle(h2);
		ResumeThread(h3);
		CloseHandle(h3);
		return false;
	}
	m_ThreadCount++;
	if(listen(s, 5) == SOCKET_ERROR)
	{
		closesocket(s);
		CloseHandle(m_hCompletion);
		ResumeThread(h1);
		CloseHandle(h1);
		ResumeThread(h2);
		CloseHandle(h2);
		ResumeThread(h3);
		CloseHandle(h3);
		ResumeThread(h4);
		CloseHandle(h4);
		return false;
	}

	m_s = s;
	m_bRun = true;
	m_hThread1 = h1;
	m_hThread2 = h2;
	m_hThread3 = h3;
	m_hThread4 = h4;

	return true;
}

void CTcpServer::RunServer(void)
{
	if(m_bRun)
	{
		ResumeThread(m_hThread1);
		CloseHandle(m_hThread1);
		ResumeThread(m_hThread2);
		CloseHandle(m_hThread2);
		ResumeThread(m_hThread3);
		CloseHandle(m_hThread3);
		ResumeThread(m_hThread4);
		CloseHandle(m_hThread4);
	}
}

bool CTcpServer::IsRun(void)
{
	return m_bRun;
}

void CTcpServer::RegsiterMsgToServer(unsigned __int16 msgID, MsgFunc msgFunc)
{
	m_MsgPointMap.insert(MSG_PointMap::value_type(msgID, msgFunc));
}

DWORD WINAPI CTcpServer::AcceptManageThread(LPVOID lParam)
{//连接请求处理线程
	CTcpServer *pServer = (CTcpServer *)lParam;
	
	while (pServer->m_bRun)
	{
		sockaddr_in clientaddr;
		int len = sizeof(clientaddr);
		memset(&clientaddr, 0, len);
		
		SOCKET sNew = accept(pServer->m_s, (sockaddr *)&clientaddr, &len);
		if(INVALID_SOCKET != sNew)
		{
			CTcpConnectHinstance *pHinstance  = pServer->GetIdleConnectHinstance();
			if(NULL == pHinstance)
			{//连接已满,丢掉此连接
				closesocket(sNew);
				
				g_NetAppLog.Dbg2file("没有空闲连接了丢掉此连接!");
			}
			else
			{
				pHinstance->SetSocketAndAddr(sNew, clientaddr);
				pHinstance->SetActiveState(true);
				//将此连接加入到完成端口中
				CreateIoCompletionPort((HANDLE)sNew, pServer->m_hCompletion, NULL, 0);
				//提交1个IO接收请求
				pHinstance->ReQuestRecvIO();
				if(NULL != pServer->m_ConOkCallbackfunction)
					pServer->m_ConOkCallbackfunction(pHinstance);
			}
		}
		else
		{
			break;
		}
		Sleep(1);
	}

	return 0;
}

DWORD WINAPI CTcpServer::MSGManageThread(LPVOID lParam)
{//消息处理线程
	CTcpServer *pServer = (CTcpServer *)lParam;
	while (pServer->m_bRun)
	{
		int i=0;
		for (i=0; i<pServer->m_MaxConnect; i++)
		{
			if(pServer->m_pHinstanceList[i].IsActive())
			{
				CNetPackage *pNetPack = pServer->m_pHinstanceList[i].GetMsgNetPackage();
				if(NULL != pNetPack)
				{
					pNetPack->ReadInt32();
					unsigned __int16 proc = pNetPack->ReadInt16();
					MSG_PointMap::iterator it_map = pServer->m_MsgPointMap.find(proc);
					if(it_map != pServer->m_MsgPointMap.end())
					{
						MsgFunc func = NULL;
						func = it_map->second;
						if(func)
							func(pNetPack);
					}
					pNetPack->m_bUseState = false;
				}
			}
		}
		Sleep(1);
	}

	pServer->m_ThreadCount--;
	return 0;
}

DWORD WINAPI CTcpServer::ServerManageThread(LPVOID lParam)
{//服务处理线程
	CTcpServer *pServer = (CTcpServer *)lParam;
	while (pServer->m_bRun)
	{
		DWORD lpNumberOfBytesTransferred=0, lpCompletionKey=0, dwError;
		OVERLAPPED *pOverlapped=NULL;
		int bResult = GetQueuedCompletionStatus(pServer->m_hCompletion, 
			&lpNumberOfBytesTransferred, &lpCompletionKey, &pOverlapped, 1000);
		dwError = GetLastError();

		if(bResult != 0)
		{//IO操作成功完成
			CNetPackage *pNetPack = (CNetPackage *)CONTAINING_RECORD(pOverlapped, CNetPackage, m_ov);
			if(0 == lpNumberOfBytesTransferred)
			{//服务方套接字关闭了
				if(NULL != pServer->m_DisconCallbackfunction)
					pServer->m_DisconCallbackfunction(pNetPack->m_pHinstance);
				pNetPack->m_pHinstance->SetActiveState(false);
			}
			else
			{
				switch(pNetPack->m_IoFlag)
				{
				case NET_TYPE_READ://接收操作完成了
					pNetPack->m_pHinstance->CombinationNetData(lpNumberOfBytesTransferred);
					pNetPack->m_pHinstance->ReQuestRecvIO();
					break;
				case NET_TYPE_WRITE://发送操作完成了
					pNetPack->m_bUseState = false;
					break;
				}
			}
		}
		else
		{
			if(NULL != pOverlapped)
			{
				CNetPackage *pNetPack = (CNetPackage *)CONTAINING_RECORD(pOverlapped, CNetPackage, m_ov);
				if(NULL != pServer->m_DisconCallbackfunction)
					pServer->m_DisconCallbackfunction(pNetPack->m_pHinstance);
				pNetPack->m_pHinstance->SetActiveState(false);
			}
			else
			{
				if(WAIT_TIMEOUT == dwError)
				{
					continue;
				}
				else
				{
					g_NetAppLog.Dbg2file("完成端口错误!");
				}
			}
		}
		Sleep(1);
	}
	pServer->m_ThreadCount--;
	return 0;
}

DWORD WINAPI CTcpServer::HeartbeatManageThread(LPVOID lParam)
{//心跳处理线程
	CTcpServer *pServer = (CTcpServer *)lParam;
	while (pServer->m_bRun)
	{
		if(pServer->m_bHerat)
		{
			int i=0;
			for (i=0; i<pServer->m_MaxConnect; i++)
			{
				if(pServer->m_pHinstanceList[i].IsActive())
				{
					if(pServer->m_pHinstanceList[i].m_HeartCount > 10)
					{//10秒心跳无响应
						pServer->m_pHinstanceList[i].SetActiveState(false);
						pServer->m_pHinstanceList[i].m_HeartCount = 0;
					}
					else
					{
						CNetPackage *pNetPackage = pServer->m_pHinstanceList[i].GetSendIdlePackage();
						if(NULL != pNetPackage)
						{
							pServer->m_pHinstanceList[i].m_HeartCount++;
							pNetPackage->WriteProc(MSG_HEARTBEAT);
							pNetPackage->SendMSG_Server();
						}
					}
				}
				else
				{
					pServer->m_pHinstanceList[i].m_HeartCount = 0;
				}
			}
		}
		Sleep(1000);
	}
	pServer->m_ThreadCount--;
	return 0;
}

void CTcpServer::HeartbeatFunc(CNetPackage *pNetPack)
{
	pNetPack->m_pHinstance->m_HeartCount = 0;
}

CTcpConnectHinstance *CTcpServer::GetIdleConnectHinstance(void)
{
	int i=0, j=m_MaxConnect-1;
	while (i <= j)
	{
		if(!m_pHinstanceList[i].IsActive())
		{
			m_pHinstanceList[i].m_ConnectNO = i;
			return &m_pHinstanceList[i];
		}

		if(!m_pHinstanceList[j].IsActive())
		{
			m_pHinstanceList[j].m_ConnectNO = j;
			return &m_pHinstanceList[j];
		}
		i++;
		j--;
	}

	return NULL;
}


void CTcpServer::StopServer(void)
{
	m_bRun = false;
	while (m_ThreadCount.Get() > 0)
	{
		Sleep(10);
	}
	int i=0;
	for(i=0; i<m_MaxConnect; i++)
	{
		if(m_pHinstanceList[i].IsActive())
			m_pHinstanceList[i].SetActiveState(false);
	}
	CloseHandle(m_hCompletion);
}

void CTcpServer::SetConnectStateCallbackfunction(TcpServerCallbackfunction DisConnectFuc, TcpServerCallbackfunction ConnectOkFuc)
{
	m_DisconCallbackfunction = DisConnectFuc;
	m_ConOkCallbackfunction = ConnectOkFuc;
}

long CTcpServer::GetConnectSum(void)
{
	int i=0, NUM = 0;
	for(i=0; i<m_MaxConnect; i++)
	{
		if(m_pHinstanceList[i].IsActive())
		{
			NUM++;
		}
	}
	return NUM;
}