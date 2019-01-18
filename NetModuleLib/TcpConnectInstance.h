#ifndef _TCPCONNECTHINSTANCE_H_
#define _TCPCONNECTHINSTANCE_H_
//tcp连接实例类
#include "NetPackage.h"

//发送缓冲包队列
#define MAX_SEND_NETPACK_QUEUE	4
//完整消息包队列
#define MAX_MSG_NETPACK_QUEUE	8

//心跳协议
#define MSG_HEARTBEAT 65535

#include "map"
using namespace std;

//消息处理函数原型
typedef void (*MsgFunc)(CNetPackage *pNetPack);
typedef map<DWORD, MsgFunc, less<unsigned __int16> > MSG_PointMap;

class CTcpConnectHinstance
{
public:
	CTcpConnectHinstance();
	~CTcpConnectHinstance();
public:
	void SetActiveState(bool bState);
	bool IsActive(void);
	CNetPackage *GetSendIdlePackage(void);
	void SetSocketAndAddr(SOCKET &s, sockaddr_in &addr);
	void GetSocketAndAddr(SOCKET &s, sockaddr_in &addr);
	void ReQuestRecvIO(void);
	//组包
	void CombinationNetData(DWORD bytesLen);
	CNetPackage *GetMsgNetPackage(void);
public:
	//此连接分配到的编号
	DWORD m_ConnectNO;
	//心跳计数
	DGJ_INT	m_HeartCount;
	//接收缓冲包
	CNetPackage m_RecvPack;
private:
	CDgj_Lock m_SendQueueLock;
	bool IsErrorPackage(void);
	//加入1个完整网络包到队列中
	void AddMsgPackage(void);
	//发送缓冲队列
	CNetPackage m_SendQueue[MAX_SEND_NETPACK_QUEUE];
	//网络完整包队列
	CNetPackage m_MsgQueue[MAX_MSG_NETPACK_QUEUE];
	unsigned __int16 m_MsgQueueHead;
	unsigned __int16 m_MsgQueueTrail;
	
	DGJ_BOOL m_bUseState;
	unsigned char m_tmpBuf[MAX_NETPACK_SIZE];
	DWORD m_PrevPackNeedBytes;	//上个包还需要的字节数
	DWORD m_RecvBufIndex;
	DWORD m_tmpBufIndex;
};
#endif