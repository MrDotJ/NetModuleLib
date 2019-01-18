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
	CNetPackage *GetSendIdlePackage(void);		//取得1个可以用于发送的空闲网络包,错误返回NULL
	//设置连接断开状态通知函数
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
	static DWORD WINAPI RecvServerThread(LPVOID lParam);	//数据接收服务处理线程
	static DWORD WINAPI MsgServerThread(LPVOID lParam);		//数据接收服务处理线程
	static void HeratFunc(CNetPackage *pNetPack);			//心跳处理函数

};

#endif