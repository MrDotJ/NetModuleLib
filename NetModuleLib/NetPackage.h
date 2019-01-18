#ifndef _NETPACKAGE_H_
#define _NETPACKAGE_H_
#include "winsock2.h"
#include "Dgj_Thread.h"
#include "Dgj_Log.h"
#include "vc_warning_disable.h"
#pragma comment(lib, "ws2_32.lib")

//最小包长
#define MIN_NETPACK_LEN	10

//包缓冲大小
#define MAX_NETPACK_SIZE 16*1024
enum NET_IO_FLAG
{
	NET_TYPE_READ = 1,
	NET_TYPE_WRITE = 2
};


//网络数据包类
//完整网络包格式:包头4b+协议2b+消息内容+包尾4b
//正确的包:包长大于10b小于MAX_NETPACK_SIZE,包头=包尾

class CTcpConnectHinstance;
class CNetPackage
{
public:
	CNetPackage();
	~CNetPackage();
	CNetPackage &operator=(CNetPackage &pack);
public:
	//打包函数组
	void WriteProc(unsigned __int16 proc);
	void WriteInt8(unsigned __int8 int8Param);
	void WriteInt16(unsigned __int16 int16Param);
	void WriteInt24(unsigned __int32 int24Param);
	void WriteInt32(unsigned __int32 int32Param);
	void WriteInt64(unsigned __int64 int64Param);
	void WriteString(char *str);
	void WriteStruct(void *pStruct, unsigned __int32 structSize);
public:
	//解包函数组
	__int8 ReadInt8(void);
	__int16 ReadInt16(void);
	__int32 ReadInt24(void);
	__int32 ReadInt32(void);
	__int64 ReadInt64(void);
	void ReadString(char *strBuf, int strBufSize);
	void ReadStruct(void *pStructBuf, unsigned __int32 structSize);
public:
	//将写入此包的数据通过和此包相关的SOCKET发送出去
	int SendMSG_Server(void);//如果是服务器方调用此函数发送数据
	int SendMSG_Client(void);//如果是客户方调用此函数发送数据
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