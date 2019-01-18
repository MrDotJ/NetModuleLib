#ifndef _TCPCONNECTHINSTANCE_H_
#define _TCPCONNECTHINSTANCE_H_
//tcp����ʵ����
#include "NetPackage.h"

//���ͻ��������
#define MAX_SEND_NETPACK_QUEUE	4
//������Ϣ������
#define MAX_MSG_NETPACK_QUEUE	8

//����Э��
#define MSG_HEARTBEAT 65535

#include "map"
using namespace std;

//��Ϣ������ԭ��
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
	//���
	void CombinationNetData(DWORD bytesLen);
	CNetPackage *GetMsgNetPackage(void);
public:
	//�����ӷ��䵽�ı��
	DWORD m_ConnectNO;
	//��������
	DGJ_INT	m_HeartCount;
	//���ջ����
	CNetPackage m_RecvPack;
private:
	CDgj_Lock m_SendQueueLock;
	bool IsErrorPackage(void);
	//����1�������������������
	void AddMsgPackage(void);
	//���ͻ������
	CNetPackage m_SendQueue[MAX_SEND_NETPACK_QUEUE];
	//��������������
	CNetPackage m_MsgQueue[MAX_MSG_NETPACK_QUEUE];
	unsigned __int16 m_MsgQueueHead;
	unsigned __int16 m_MsgQueueTrail;
	
	DGJ_BOOL m_bUseState;
	unsigned char m_tmpBuf[MAX_NETPACK_SIZE];
	DWORD m_PrevPackNeedBytes;	//�ϸ�������Ҫ���ֽ���
	DWORD m_RecvBufIndex;
	DWORD m_tmpBufIndex;
};
#endif