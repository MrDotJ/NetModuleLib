#include "NetPackage.h"
#include "TcpConnectInstance.h"

CDgj_Log g_NetAppLog("NetAppLog");
CNetPackage::CNetPackage()
{
	m_ReadPos = 0;
	m_WritePos = 6;
	memset(m_buf, 0, MAX_NETPACK_SIZE);
	m_IoFlag = NET_TYPE_READ;
	m_bUseState = false;
	m_wsaBuf.buf = (char *)m_buf;
	m_wsaBuf.len = MAX_NETPACK_SIZE;
	memset(&m_ov, 0, sizeof(m_ov));
	m_pHinstance = NULL;
}

CNetPackage::~CNetPackage()
{
}

CNetPackage &CNetPackage::operator =(CNetPackage &pack)
{
	m_ReadPos = pack.m_ReadPos;
	m_WritePos = pack.m_WritePos;
	memcpy(m_buf, pack.m_buf, MAX_NETPACK_SIZE);
	m_IoFlag = pack.m_IoFlag;
	m_bUseState = pack.m_bUseState;
	m_wsaBuf.buf = (char *)m_buf;
	m_wsaBuf.len = MAX_NETPACK_SIZE;
	memset(&m_ov, 0, sizeof(m_ov));
	m_s = pack.m_s;
	memcpy(&m_addr, &pack.m_addr, sizeof(m_addr));
	m_pHinstance = pack.m_pHinstance;
	return *this;
}

void CNetPackage::WriteProc(unsigned __int16 proc)
{
	m_buf[4] = (unsigned char)(proc>>8);
	m_buf[5] = (unsigned char)(proc);
}

void CNetPackage::WriteInt8(unsigned __int8 int8Param)
{
	m_buf[m_WritePos] = int8Param;
	m_WritePos++;
}

void CNetPackage::WriteInt16(unsigned __int16 int16Param)
{
	m_buf[m_WritePos++] = (unsigned char)(int16Param>>8);
	m_buf[m_WritePos++] = (unsigned char)(int16Param);
}

void CNetPackage::WriteInt24(unsigned __int32 int24Param)
{
	m_buf[m_WritePos++] = (unsigned char)(int24Param>>16);
	m_buf[m_WritePos++] = (unsigned char)(int24Param>>8);
	m_buf[m_WritePos++] = (unsigned char)(int24Param);
}

void CNetPackage::WriteInt32(unsigned __int32 int32Param)
{
	m_buf[m_WritePos++] = (unsigned char)(int32Param>>24);
	m_buf[m_WritePos++] = (unsigned char)(int32Param>>16);
	m_buf[m_WritePos++] = (unsigned char)(int32Param>>8);
	m_buf[m_WritePos++] = (unsigned char)(int32Param);
}

void CNetPackage::WriteInt64(unsigned __int64 int64Paramc)
{
	m_buf[m_WritePos++] = (unsigned char)(int64Paramc>>56);
	m_buf[m_WritePos++] = (unsigned char)(int64Paramc>>48);
	m_buf[m_WritePos++] = (unsigned char)(int64Paramc>>40);
	m_buf[m_WritePos++] = (unsigned char)(int64Paramc>>32);
	m_buf[m_WritePos++] = (unsigned char)(int64Paramc>>24);
	m_buf[m_WritePos++] = (unsigned char)(int64Paramc>>16);
	m_buf[m_WritePos++] = (unsigned char)(int64Paramc>>8);
	m_buf[m_WritePos++] = (unsigned char)(int64Paramc);
}

void CNetPackage::WriteString(char *str)
{
	if(str == NULL)
	{
		WriteInt16(0);
		return;
	}
	unsigned __int16 len = strlen(str);
	WriteInt16(len);
	if(0 == len)
		return;
	memcpy(&m_buf[m_WritePos], str, len);
	m_WritePos += len;
}

void CNetPackage::WriteStruct(void *pStruct, unsigned __int32 structSize)
{
	if(pStruct == NULL || structSize == 0)
	{
		WriteInt16(0);
		return;
	}

	memcpy(&m_buf[m_WritePos], pStruct, structSize);
	m_WritePos += structSize;
}

__int8 CNetPackage::ReadInt8(void)
{
	__int8 Value = m_buf[m_ReadPos++];
	return Value;
}

__int16 CNetPackage::ReadInt16(void)
{
	unsigned __int16 Value = (((unsigned __int16)(m_buf[m_ReadPos++]))<<8);
	Value += (unsigned __int16)(m_buf[m_ReadPos++]);
	return Value;
}

__int32 CNetPackage::ReadInt24(void)
{
	unsigned __int32 Value = (((unsigned __int32)(m_buf[m_ReadPos++]))<<16);
	Value += (((unsigned __int32)(m_buf[m_ReadPos++]))<<8);
	Value += (unsigned __int32)(m_buf[m_ReadPos++]);
	return Value;
}

__int32 CNetPackage::ReadInt32(void)
{
	unsigned __int32 Value = (((unsigned __int32)(m_buf[m_ReadPos++]))<<24);
	Value += (((unsigned __int32)(m_buf[m_ReadPos++]))<<16);
	Value += (((unsigned __int32)(m_buf[m_ReadPos++]))<<8);
	Value += (unsigned __int32)(m_buf[m_ReadPos++]);
	return Value;
}

__int64 CNetPackage::ReadInt64(void)
{
	unsigned __int64 Value = (((unsigned __int64)(m_buf[m_ReadPos++]))<<56);
	Value += (((unsigned __int64)(m_buf[m_ReadPos++]))<<48);
	Value += (((unsigned __int64)(m_buf[m_ReadPos++]))<<40);
	Value += (((unsigned __int64)(m_buf[m_ReadPos++]))<<32);
	Value += (((unsigned __int64)(m_buf[m_ReadPos++]))<<24);
	Value += (((unsigned __int64)(m_buf[m_ReadPos++]))<<16);
	Value += (((unsigned __int64)(m_buf[m_ReadPos++]))<<8);
	Value += (unsigned __int64)(m_buf[m_ReadPos++]);
	return Value;
}

void CNetPackage::ReadString(char *strBuf, int strBufSize)
{
	unsigned __int16 len = ReadInt16();
	if(strBuf == NULL || strBufSize == 0)
	{
		m_ReadPos += len;
		return;
	}

	memcpy(strBuf, &m_buf[m_ReadPos], len);
	m_ReadPos += len;
}

void CNetPackage::ReadStruct(void *pStructBuf, unsigned __int32 structSize)
{
	if(pStructBuf == NULL)
	{
		m_ReadPos += structSize;
		return;
	}
	memcpy(pStructBuf, &m_buf[m_ReadPos], structSize);
	m_ReadPos += structSize;
}

int CNetPackage::SendMSG_Server(void)
{
	unsigned __int32 totalLen = m_WritePos+4;
	//写包尾
	WriteInt32(totalLen);
	unsigned __int32 oldHead = m_WritePos;
	m_WritePos = 0;
	//写包头
	WriteInt32(totalLen);
	m_WritePos = oldHead;
	ZeroMemory(&m_ov, sizeof(m_ov));
	DWORD lpNumberOfBytesSent=0, dwFlags=0;
	m_IoFlag = NET_TYPE_WRITE;
	
	m_wsaBuf.len = totalLen;
	return WSASend(m_s, &m_wsaBuf, 1, &lpNumberOfBytesSent, dwFlags, &m_ov, NULL);
}

int CNetPackage::SendMSG_Client(void)
{
	unsigned __int32 totalLen = m_WritePos+4;
	//写包尾
	WriteInt32(totalLen);
	unsigned __int32 oldHead = m_WritePos;
	m_WritePos = 0;
	//写包头
	WriteInt32(totalLen);

	unsigned __int32 sendLen = 0, needBytes = 0, sendTatol = 0;
	sendLen = send(m_s, (char *)m_buf, totalLen, 0);
	needBytes = totalLen - sendLen;
	
	while (needBytes > 0)
	{
		sendTatol += sendLen;
		sendLen = send(m_s, (char *)(m_buf+sendTatol), needBytes, 0);
		needBytes = needBytes - sendLen;
	}
	m_bUseState = false;
	return totalLen;
}