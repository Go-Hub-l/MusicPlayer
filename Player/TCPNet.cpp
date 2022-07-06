#include "TCPNet.h"

bool  TCPNet::m_bFlagQuit = true;

IKernel* TCPNet::m_pKernel = NULL;

TCPNet::TCPNet(IKernel* pKernel)
{
	m_sockClient = NULL;
	m_pKernel = pKernel;
}


TCPNet::~TCPNet(void)
{
}
bool TCPNet::InitNetWork()
{
	//1.ѡ������ -- 
	WORD wVersionRequested;
    WSADATA wsaData;
    int err;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		// printf("WSAStartup failed with error: %d\n", err);
		return false;
	}


	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		// printf("Could not find a usable version of Winsock.dll\n");
		UnInitNetWork();
		return false;
	}
   
        

	//2.��Ӷ�곤 -- socket 
	m_sockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sockClient == INVALID_SOCKET)
	{
		UnInitNetWork();
		return false;
	}

	//3.ѡַ --- 
	sockaddr_in  addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);		// ��������IP
	addrServer.sin_port = htons(_DEFPORT);						// �������Ķ˿ں�
	if (SOCKET_ERROR == connect(m_sockClient, (const sockaddr*)&addrServer, sizeof(addrServer)))
	{
		UnInitNetWork();
		return false;
	}

	//5.�����߳�
	m_hThreadRecv = (HANDLE)_beginthreadex(NULL, 0, &ThreadRecv, this, 0, 0);
	
	return true;
}

unsigned _stdcall TCPNet::ThreadRecv(void * lpvoid)
{
	TCPNet* pThis = (TCPNet*)lpvoid;
	
	int		nRelReadNum;
	int		nPackSize;
	char*	pszBuf = NULL;
	while (TCPNet::m_bFlagQuit)
	{
		nRelReadNum = recv(pThis->m_sockClient, (char*)&nPackSize, sizeof(int), 0);
		if (nRelReadNum <= 0)
			continue;
		// �������������ݰ�
		pszBuf = new char[nPackSize];
		int noffset = 0;
		while (nPackSize)		// to revise-------------------
		{
			nRelReadNum = recv(pThis->m_sockClient, pszBuf + noffset, nPackSize, 0);
			noffset += nRelReadNum;
			nPackSize -= nRelReadNum;
		}
		//����
		m_pKernel->dealData(pszBuf);


		// �ͷſռ�
		delete[] pszBuf;
		pszBuf = NULL;

	}
	return 0;
}

void TCPNet::UnInitNetWork()
{
	m_bFlagQuit = false;
	if (m_hThreadRecv)
	{
		if (WAIT_TIMEOUT == WaitForSingleObject(m_hThreadRecv, 100))
			TerminateThread(m_hThreadRecv, -1);

		CloseHandle(m_hThreadRecv);
		m_hThreadRecv = NULL;
	}
	// ж�ؿ�
	WSACleanup();
	if (m_sockClient)
	{
		closesocket(m_sockClient);
		m_sockClient = NULL;
	}
}

bool TCPNet::SendData(char* szbuf,int nlen)
{
	if (!szbuf || nlen <= 0)
		return false;
	// ֮�������������ݣ��ڶ��ν��յ�ʱ�����յ������ݣ�
	if (send(m_sockClient, szbuf, nlen, 0) < 0)
		return false;
	
	return true;
}
