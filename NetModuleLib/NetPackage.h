#ifndef _NETPACKAGE_H_
#define _NETPACKAGE_H_
#include "winsock2.h"
#include "Dgj_Thread.h"
#include "Dgj_Log.h"
#include "vc_warning_disable.h"
#pragma comment(lib, "ws2_32.lib")

//��С����
#define MIN_NETPACK_LEN	10

//�������С
#define MAX_NETPACK_SIZE 16*1024
enum NET_IO_FLAG
{
	NET_TYPE_READ = 1,
	NET_TYPE_WRITE = 2
};


//�������ݰ���
//�����������ʽ:��ͷ4b+Э��2b+��Ϣ����+��β4b
//��ȷ�İ�:��������10bС��MAX_NETPACK_SIZE,��ͷ=��β

class CTcpConnectHinstance;
class CNetPackage
{
public:
	CNetPackage();
	~CNetPackage();
	CNetPackage &operator=(CNetPackage &pack);
public:
	//���������
	void WriteProc(unsigned __int16 proc);
	void WriteInt8(unsigned __int8 int8Param);
	void WriteInt16(unsigned __int16 int16Param);
	void WriteInt24(unsigned __int32 int24Param);
	void WriteInt32(unsigned __int32 int32Param);
	void WriteInt64(unsigned __int64 int64Param);
	void WriteString(char *str);
	void WriteStruct(void *pStruct, unsigned __int32 structSize);
public:
	//���������
	__int8 ReadInt8(void);
	__int16 ReadInt16(void);
	__int32 ReadInt24(void);
	__int32 ReadInt32(void);
	__int64 ReadInt64(void);
	void ReadString(char *strBuf, int strBufSize);
	void ReadStruct(void *pStructBuf, unsigned __int32 structSize);
public:
	//��д��˰�������ͨ���ʹ˰���ص�SOCKET���ͳ�ȥ
	int SendMSG_Server(void);//����Ƿ����������ô˺�����������
	int SendMSG_Client(void);//����ǿͻ������ô˺�����������
public:
	unsigned __int32 m_ReadPos;
	unsigned __int32 m_WritePos;
	unsigned char m_buf[MAX_NETPACK_SIZE];
	NET_IO_FLAG m_IoFlag;
	DGJ_BOOL m_bUseState;
	WSABUF m_wsaBuf;	
	OVERLAPPED m_ov;
	SOCKET m_s;
	sockaddr_in m_addr;
public:
	CTcpConnectHinstance *m_pHinstance;
};


extern CDgj_Log g_NetAppLog;

#endif