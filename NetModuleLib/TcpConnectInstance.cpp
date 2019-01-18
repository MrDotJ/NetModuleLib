#include "TcpConnectInstance.h"

CTcpConnectHinstance::CTcpConnectHinstance()
{
	m_RecvPack.m_pHinstance = this;
	int i=0;
	for(i=0; i<MAX_SEND_NETPACK_QUEUE; i++)
	{
		m_SendQueue[i].m_IoFlag = NET_TYPE_WRITE;
		m_SendQueue[i].m_pHinstance = this;
	}
	m_bUseState = false;
	m_RecvBufIndex = 0;
	m_tmpBufIndex = 0;
	m_PrevPackNeedBytes = 0;
	m_MsgQueueHead = 0;
	m_MsgQueueTrail = 0;
	m_HeartCount = 0;
	ZeroMemory(m_tmpBuf, MAX_NETPACK_SIZE);
}

CTcpConnectHinstance::~CTcpConnectHinstance()
{
}

void CTcpConnectHinstance::SetActiveState(bool bState)
{
	m_bUseState = bState;
	if(!bState)
	{
		closesocket(m_RecvPack.m_s);
		m_RecvPack.m_s = INVALID_SOCKET;
		int i=0;
		for(i=0; i<MAX_SEND_NETPACK_QUEUE; i++)
		{
			m_SendQueue[i].m_s = INVALID_SOCKET;
		}
	}
}

bool CTcpConnectHinstance::IsActive(void)
{
	return m_bUseState.Get();
}

CNetPackage *CTcpConnectHinstance::GetMsgNetPackage(void)
{
	int Trail = m_MsgQueueTrail;
	if(m_MsgQueue[Trail].m_bUseState.Get())
	{
		m_MsgQueueTrail = (++m_MsgQueueTrail)%MAX_MSG_NETPACK_QUEUE;
		return &m_MsgQueue[Trail];
	}
	return NULL;
}

CNetPackage *CTcpConnectHinstance::GetSendIdlePackage(void)
{
	int i=0, j=MAX_SEND_NETPACK_QUEUE-1;
	if(!IsActive())
		return NULL;
	m_SendQueueLock.Lock();
	while (i <= j)
	{
		if(!m_SendQueue[i].m_bUseState.Get())
		{
			m_SendQueue[i].m_ReadPos = 0;
			m_SendQueue[i].m_WritePos = 6;
			m_SendQueue[i].m_bUseState = true;
			m_SendQueueLock.UnLock();
			return &m_SendQueue[i];
		}

		if(!m_SendQueue[j].m_bUseState.Get())
		{
			m_SendQueue[j].m_ReadPos = 0;
			m_SendQueue[j].m_WritePos = 6;
			m_SendQueue[j].m_bUseState = true;
			m_SendQueueLock.UnLock();
			return &m_SendQueue[j];
		}
		i++;
		j--;
	}
	m_SendQueueLock.UnLock();
	return NULL;
}

void CTcpConnectHinstance::SetSocketAndAddr(SOCKET &s, sockaddr_in &addr)
{
	m_RecvPack.m_s = s;
	memcpy(&m_RecvPack.m_addr, &addr, sizeof(sockaddr_in));

	int i=0;
	for(i=0; i<MAX_SEND_NETPACK_QUEUE; i++)
	{
		m_SendQueue[i].m_s = s;
		memcpy(&m_SendQueue[i].m_addr, &addr, sizeof(sockaddr_in));
	}
}

void CTcpConnectHinstance::GetSocketAndAddr(SOCKET &s, sockaddr_in &addr)
{
	s = m_RecvPack.m_s;
	memcpy(&addr, &m_RecvPack.m_addr, sizeof(sockaddr_in));
}

void CTcpConnectHinstance::ReQuestRecvIO(void)
{
	DWORD lpNumberOfBytesRecvd=0, lpFlags=0;
	ZeroMemory(&m_RecvPack.m_ov, sizeof(m_RecvPack.m_ov));
	ZeroMemory(m_RecvPack.m_buf, MAX_NETPACK_SIZE);
	WSARecv(m_RecvPack.m_s, &m_RecvPack.m_wsaBuf, 1, &lpNumberOfBytesRecvd, &lpFlags, &m_RecvPack.m_ov, NULL);
}

bool CTcpConnectHinstance::IsErrorPackage(void)
{
	bool bError = false;
	unsigned __int32 packageHeadVal = (((unsigned __int32)(m_tmpBuf[0]))<<24);
	packageHeadVal += (((unsigned __int32)(m_tmpBuf[1]))<<16);
	packageHeadVal += (((unsigned __int32)(m_tmpBuf[2]))<<8);
	packageHeadVal += (unsigned __int32)(m_tmpBuf[3]);
	if(packageHeadVal < MIN_NETPACK_LEN || packageHeadVal > MAX_NETPACK_SIZE)
		return true;
	unsigned __int32 packageTrailVal = (((unsigned __int32)(m_tmpBuf[packageHeadVal-4]))<<24);
	packageTrailVal += (((unsigned __int32)(m_tmpBuf[packageHeadVal-3]))<<16);
	packageTrailVal += (((unsigned __int32)(m_tmpBuf[packageHeadVal-2]))<<8);
	packageTrailVal += (((unsigned __int32)(m_tmpBuf[packageHeadVal-1])));
	if(packageTrailVal != packageHeadVal)
		return true;

	return bError;
}

void CTcpConnectHinstance::AddMsgPackage(void)
{
	int head = m_MsgQueueHead;
	if(!m_MsgQueue[head].m_bUseState.Get())
	{
		m_MsgQueueHead = (++m_MsgQueueHead)%MAX_MSG_NETPACK_QUEUE;
		m_MsgQueue[head].m_ReadPos = 0;
		memcpy(m_MsgQueue[head].m_buf, m_tmpBuf, MAX_NETPACK_SIZE);
		m_MsgQueue[head].m_pHinstance = this;
		m_MsgQueue[head].m_bUseState = true;
	}
	else
	{//消息队列满丢掉此消息包
		char str[64] = {0};
		sprintf(str, "连接号%d丢包", m_ConnectNO);
		g_NetAppLog.Dbg2file(str);
	}
	memset(m_tmpBuf, 0, MAX_NETPACK_SIZE);
}

//组包
void CTcpConnectHinstance::CombinationNetData(DWORD bytesLen)
{
	try
	{
		unsigned char *pRecvPackBuf = m_RecvPack.m_buf;

		while (bytesLen > 0)
		{
			if(m_PrevPackNeedBytes > 0)
			{//上个包还需要数据
				if(bytesLen >= m_PrevPackNeedBytes)
				{//此包数据已够
					memcpy(&m_tmpBuf[m_tmpBufIndex], pRecvPackBuf+m_RecvBufIndex, m_PrevPackNeedBytes);

					bytesLen -= m_PrevPackNeedBytes;
					m_tmpBufIndex = 0;
					m_RecvBufIndex += m_PrevPackNeedBytes;
					m_PrevPackNeedBytes = 0;

					if(bytesLen == 0)
						m_RecvBufIndex = 0;
					if(!IsErrorPackage())
					{
						AddMsgPackage();	
					}
				}
				else
				{//上个包数据还不够
					memcpy(&m_tmpBuf[m_tmpBufIndex], pRecvPackBuf+m_RecvBufIndex, bytesLen);
					m_PrevPackNeedBytes -= bytesLen;
					m_tmpBufIndex += bytesLen;
					m_RecvBufIndex = 0;
					bytesLen = 0;
				}
			}
			else
			{//新的1个包开始
				if(m_tmpBufIndex == 0)
				{
					if(bytesLen >= 4)
					{
						//4字节包长在pRecvPackBuf中
						unsigned __int32 packageHeadVal = (((unsigned __int32)(pRecvPackBuf[m_RecvBufIndex]))<<24);
						packageHeadVal += (((unsigned __int32)(pRecvPackBuf[m_RecvBufIndex+1]))<<16);
						packageHeadVal += (((unsigned __int32)(pRecvPackBuf[m_RecvBufIndex+2]))<<8);
						packageHeadVal += (((unsigned __int32)(pRecvPackBuf[m_RecvBufIndex+3])));

						if(packageHeadVal < MIN_NETPACK_LEN || packageHeadVal > MAX_NETPACK_SIZE)
						{//错包
							bytesLen = 0;
							m_PrevPackNeedBytes = 0;
							m_tmpBufIndex = 0;
							m_RecvBufIndex = 0;
						}
						else
						{
							m_PrevPackNeedBytes = packageHeadVal;
							if(bytesLen >= m_PrevPackNeedBytes)
							{//此包数据已够
								memcpy(&m_tmpBuf[m_tmpBufIndex], pRecvPackBuf+m_RecvBufIndex, m_PrevPackNeedBytes);

								bytesLen -= m_PrevPackNeedBytes;
								m_tmpBufIndex = 0;
								m_RecvBufIndex += m_PrevPackNeedBytes;
								m_PrevPackNeedBytes = 0;

								if(bytesLen == 0)
									m_RecvBufIndex = 0;
								if(!IsErrorPackage())
								{
									AddMsgPackage();	
								}
							}
							else
							{//上个包数据还不够
								memcpy(&m_tmpBuf[m_tmpBufIndex], pRecvPackBuf+m_RecvBufIndex, bytesLen);
								m_PrevPackNeedBytes -= bytesLen;
								m_tmpBufIndex += bytesLen;
								m_RecvBufIndex = 0;
								bytesLen = 0;
							}
						}
					}
					else
					{//在pRecvPackBuf中可能有1-3字节包头
						memcpy(&m_tmpBuf[m_tmpBufIndex], pRecvPackBuf+m_RecvBufIndex, bytesLen);
						m_PrevPackNeedBytes = 0;
						m_tmpBufIndex += bytesLen;
						m_RecvBufIndex = 0;
						bytesLen = 0;
					}
				}
				else if(m_tmpBufIndex == 1)
				{//包头的其余3字节在pRecvPackBuf中
					if(bytesLen >= 3)
					{
						unsigned __int32 packageHeadVal = (((unsigned __int32)(m_tmpBuf[0]))<<24);
						packageHeadVal += (((unsigned __int32)(pRecvPackBuf[m_RecvBufIndex]))<<16);
						packageHeadVal += (((unsigned __int32)(pRecvPackBuf[m_RecvBufIndex+1]))<<8);
						packageHeadVal += (((unsigned __int32)(pRecvPackBuf[m_RecvBufIndex+2])));

						if(packageHeadVal < MIN_NETPACK_LEN || packageHeadVal > MAX_NETPACK_SIZE)
						{//错包
							bytesLen = 0;
							m_PrevPackNeedBytes = 0;
							m_tmpBufIndex = 0;
							m_RecvBufIndex = 0;
						}
						else
						{
							m_PrevPackNeedBytes = packageHeadVal-1;
							if(bytesLen >= m_PrevPackNeedBytes)
							{//此包数据已够
								memcpy(&m_tmpBuf[m_tmpBufIndex], pRecvPackBuf+m_RecvBufIndex, m_PrevPackNeedBytes);

								bytesLen -= m_PrevPackNeedBytes;
								m_tmpBufIndex = 0;
								m_RecvBufIndex += m_PrevPackNeedBytes;
								m_PrevPackNeedBytes = 0;

								if(bytesLen == 0)
									m_RecvBufIndex = 0;
								if(!IsErrorPackage())
								{
									AddMsgPackage();	
								}
							}
							else
							{//上个包数据还不够
								memcpy(&m_tmpBuf[m_tmpBufIndex], pRecvPackBuf+m_RecvBufIndex, bytesLen);
								m_PrevPackNeedBytes -= bytesLen;
								m_tmpBufIndex += bytesLen;
								m_RecvBufIndex = 0;
								bytesLen = 0;
							}
						}

					}
					else
					{//在pRecvPackBuf中可能有1-2字节包头
						memcpy(&m_tmpBuf[m_tmpBufIndex], pRecvPackBuf+m_RecvBufIndex, bytesLen);
						m_PrevPackNeedBytes = 0;
						m_tmpBufIndex += bytesLen;
						m_RecvBufIndex = 0;
						bytesLen = 0;
					}
				}
				else if(m_tmpBufIndex == 2)
				{
					if(bytesLen >= 2)
					{//2b包头在pRecvPackBuf中
						unsigned __int32 packageHeadVal = (((unsigned __int32)(m_tmpBuf[0]))<<24);
						packageHeadVal += (((unsigned __int32)(m_tmpBuf[1]))<<16);
						packageHeadVal += (((unsigned __int32)(pRecvPackBuf[m_RecvBufIndex]))<<8);
						packageHeadVal += (((unsigned __int32)(pRecvPackBuf[m_RecvBufIndex+1])));

						if(packageHeadVal < MIN_NETPACK_LEN || packageHeadVal > MAX_NETPACK_SIZE)
						{//错包
							bytesLen = 0;
							m_PrevPackNeedBytes = 0;
							m_tmpBufIndex = 0;
							m_RecvBufIndex = 0;
						}
						else
						{
							m_PrevPackNeedBytes = packageHeadVal-2;
							if(bytesLen >= m_PrevPackNeedBytes)
							{//此包数据已够
								memcpy(&m_tmpBuf[m_tmpBufIndex], pRecvPackBuf+m_RecvBufIndex, m_PrevPackNeedBytes);

								bytesLen -= m_PrevPackNeedBytes;
								m_tmpBufIndex = 0;
								m_RecvBufIndex += m_PrevPackNeedBytes;
								m_PrevPackNeedBytes = 0;

								if(bytesLen == 0)
									m_RecvBufIndex = 0;
								if(!IsErrorPackage())
								{
									AddMsgPackage();	
								}
							}
							else
							{//上个包数据还不够
								memcpy(&m_tmpBuf[m_tmpBufIndex], pRecvPackBuf+m_RecvBufIndex, bytesLen);
								m_PrevPackNeedBytes -= bytesLen;
								m_tmpBufIndex += bytesLen;
								m_RecvBufIndex = 0;
								bytesLen = 0;
							}
						}
					}
					else
					{
						//在pRecvPackBuf中可能有1字节包头
						memcpy(&m_tmpBuf[m_tmpBufIndex], pRecvPackBuf+m_RecvBufIndex, bytesLen);
						m_PrevPackNeedBytes = 0;
						m_tmpBufIndex += bytesLen;
						m_RecvBufIndex = 0;
						bytesLen = 0;
					}
				}
				else if(m_tmpBufIndex == 3)
				{
					if(bytesLen >= 1)
					{//1b包头在pRecvPackBuf中
						unsigned __int32 packageHeadVal = (((unsigned __int32)(m_tmpBuf[0]))<<24);
						packageHeadVal += (((unsigned __int32)(m_tmpBuf[1]))<<16);
						packageHeadVal += (((unsigned __int32)(m_tmpBuf[2]))<<8);
						packageHeadVal += (((unsigned __int32)(pRecvPackBuf[m_RecvBufIndex])));

						if(packageHeadVal < MIN_NETPACK_LEN || packageHeadVal > MAX_NETPACK_SIZE)
						{//错包
							bytesLen = 0;
							m_PrevPackNeedBytes = 0;
							m_tmpBufIndex = 0;
							m_RecvBufIndex = 0;
						}
						else
						{
							m_PrevPackNeedBytes = packageHeadVal-3;
							if(bytesLen >= m_PrevPackNeedBytes)
							{//此包数据已够
								memcpy(&m_tmpBuf[m_tmpBufIndex], pRecvPackBuf+m_RecvBufIndex, m_PrevPackNeedBytes);

								bytesLen -= m_PrevPackNeedBytes;
								m_tmpBufIndex = 0;
								m_RecvBufIndex += m_PrevPackNeedBytes;
								m_PrevPackNeedBytes = 0;

								if(bytesLen == 0)
									m_RecvBufIndex = 0;
								if(!IsErrorPackage())
								{
									AddMsgPackage();	
								}
							}
							else
							{//上个包数据还不够
								memcpy(&m_tmpBuf[m_tmpBufIndex], pRecvPackBuf+m_RecvBufIndex, bytesLen);
								m_PrevPackNeedBytes -= bytesLen;
								m_tmpBufIndex += bytesLen;
								m_RecvBufIndex = 0;
								bytesLen = 0;
							}
						}
					}
				}
				else
				{
					unsigned __int32 packageHeadVal = (((unsigned __int32)(m_tmpBuf[0]))<<24);
					packageHeadVal += (((unsigned __int32)(m_tmpBuf[1]))<<16);
					packageHeadVal += (((unsigned __int32)(m_tmpBuf[2]))<<8);
					packageHeadVal += (((unsigned __int32)(m_tmpBuf[3])));

					if(packageHeadVal < MIN_NETPACK_LEN || packageHeadVal > MAX_NETPACK_SIZE)
					{//错包
						bytesLen = 0;
						m_PrevPackNeedBytes = 0;
						m_tmpBufIndex = 0;
						m_RecvBufIndex = 0;
					}
					else
					{
						m_PrevPackNeedBytes = packageHeadVal-4;
						if(bytesLen >= m_PrevPackNeedBytes)
						{//此包数据已够
							memcpy(&m_tmpBuf[m_tmpBufIndex], pRecvPackBuf+m_RecvBufIndex, m_PrevPackNeedBytes);

							bytesLen -= m_PrevPackNeedBytes;
							m_tmpBufIndex = 0;
							m_RecvBufIndex += m_PrevPackNeedBytes;
							m_PrevPackNeedBytes = 0;

							if(bytesLen == 0)
								m_RecvBufIndex = 0;
							if(!IsErrorPackage())
							{
								AddMsgPackage();	
							}
						}
						else
						{//上个包数据还不够
							memcpy(&m_tmpBuf[m_tmpBufIndex], pRecvPackBuf+m_RecvBufIndex, bytesLen);
							m_PrevPackNeedBytes -= bytesLen;
							m_tmpBufIndex += bytesLen;
							m_RecvBufIndex = 0;
							bytesLen = 0;
						}
					}
				}
			}
		}
	}
	catch (...)
	{
	}
}