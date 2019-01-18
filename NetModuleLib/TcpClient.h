#ifndef _TCPCLIENT_H_
#define _TCPCLIENT_H_
#include "Dgj_InitSocket.h"
#include "TcpConnectInstance.h"

typedef void (*TcpClientCallbackfunction)(CTcpConnectHinstance *pHinstance);

class CTcpClient
{
public:
	CTcpClient();
	~CTcpClient();
	bool ConnectServer(char ip[16], unsigned __int16 port);
	void CloseConnect(void);
	void RegsiterMsgToServer(unsigned __int16 msgID, MsgFunc msgFunc);
	CNetPackage *GetSendIdlePackage(void);		//ȡ��1���������ڷ��͵Ŀ��������,���󷵻�NULL
	//�������ӶϿ�״̬֪ͨ����
	void SetConnectStateCallbackfunction(TcpClientCallbackfunction DisConnectFuc);
	bool IsActive(void);
	void OpenHerat(void);
	void CloseHerat(void);
private:
	TcpClientCallbackfunction m_DisconCallbackfunction;
	CDgj_InitSocket m_SocketInit;
	CTcpConnectHinstance m_hInstance;
	MSG_PointMap m_MsgPointMap;
	DGJ_INT m_ThreadCount;
	static DWORD WINAPI RecvServerThread(LPVOID lParam);	//���ݽ��շ������߳�
	static DWORD WINAPI MsgServerThread(LPVOID lParam);		//���ݽ��շ������߳�
	static void HeratFunc(CNetPackage *pNetPack);			//����������

};

#endif