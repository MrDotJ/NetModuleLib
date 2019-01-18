#ifndef _TCPSEVER_H_
#define _TCPSEVER_H_
#include "Dgj_InitSocket.h"
#include "TcpConnectInstance.h"

typedef void (*TcpServerCallbackfunction)(CTcpConnectHinstance *pHinstance);

class CTcpServer
{
public:
	CTcpServer(int ConnectNum = 1024);
	~CTcpServer();
	bool CreateServer(char Ip[16], unsigned __int16 port);
	void RegsiterMsgToServer(unsigned __int16 msgID, MsgFunc msgFunc);
	//��������״̬����:DisConnectFuc�����ӶϿ�ʱ�ص�֪ͨ,ConnectOkFuc���������ɹ�ʱ�ص�֪ͨ
	void SetConnectStateCallbackfunction(TcpServerCallbackfunction DisConnectFuc, TcpServerCallbackfunction ConnectOkFuc);
	//�������Ӻ�ȡ��ָ������ʵ��
	CTcpConnectHinstance *GethInstanceOfNO(unsigned __int32 ConnectNO);
	void RunServer(void);
	bool IsRun(void);
	void StopServer(void);
	void OpenHerat(void);
	void CloseHerat(void);
	long GetConnectSum(void);
private:
	CTcpConnectHinstance *GetIdleConnectHinstance(void);	//ȡ��1����������
private:
	CDgj_InitSocket m_SocketInit;
	CTcpConnectHinstance *m_pHinstanceList;
	int m_MaxConnect;
	SOCKET m_s;
	MSG_PointMap m_MsgPointMap;
	TcpServerCallbackfunction m_DisconCallbackfunction;
	TcpServerCallbackfunction m_ConOkCallbackfunction;
private:
	HANDLE m_hThread1, m_hThread2, m_hThread3, m_hThread4;
	HANDLE m_hCompletion;
	bool m_bRun;
	bool m_bHerat;
	DGJ_INT m_ThreadCount;
	static DWORD WINAPI MSGManageThread(LPVOID lParam);			//��Ϣ�ַ������߳�
	static DWORD WINAPI AcceptManageThread(LPVOID lParam);		//���Ӵ����߳�
	static DWORD WINAPI ServerManageThread(LPVOID lParam);		//�������߳�
	static DWORD WINAPI HeartbeatManageThread(LPVOID lParam);	//���������߳�
	static void HeartbeatFunc(CNetPackage *pNetPack);
};
#endif