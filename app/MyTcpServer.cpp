
#pragma warning(disable: 4996)

#include <stdio.h>
#include <vector>
#include <assert.h>
#include <iostream>
#include <time.h>
#include <string.h>
#include "MyTcpServer.h"
using namespace std;
using namespace server_space;

#pragma comment(lib,"ws2_32")

//////////////////////////////////////////////////////////////////////////

CRITICAL_SECTION m_lock;							// socketӳ����
CRITICAL_SECTION m_data_lock;						// ������

map<SOCKET,socket_map_list*> m_sock_map;			// socketӳ������
map<SOCKET,SOCK_DATA_POOL*>  m_sock_data_pool_map;	// ��������

HANDLE g_listen_thread = NULL;						// �����߳̾��
bool g_exit = false;								// �Ƿ����˳������ӱ�־

process_handler_fun*  process_handler = NULL;		// �ص�����ָ��
void *g_pProcessObject = NULL;						// Э�鴦�����

//////////////////////////////////////////////////////////////////////////


//���ݴ�������
void server_space::DataProcessCenter(SOCKET sock, char* in_buf, int in_len, char*& out_buf, int& out_len)
{
	//string content = "";
	//EnterCriticalSection(&m_data_lock);
	//map<SOCKET,SOCK_DATA_POOL*>::iterator it = m_sock_data_pool_map.begin();
	//it = m_sock_data_pool_map.find(sock);
	//if(it != m_sock_data_pool_map.end())
	//{
	//	it->second->data.append(in_buf,in_len);
	//}
	//content  = it->second->data;
	//LeaveCriticalSection(&m_data_lock);

	//if(1)
	//{
	//	int packSize = 0x00;
	//	//���ûص�������������,��������Ӧ��Ҫ���͵�������Ϣ
	//	//process_handler((char*)content.data(), content.size(), out_buf,out_len,packSize);
	//	process_handler((char*)in_buf, in_len, out_buf,out_len,packSize);
	//	if(packSize)
	//	{
	//		EnterCriticalSection(&m_data_lock);
	//		it = m_sock_data_pool_map.begin();
	//		it = m_sock_data_pool_map.find(sock);
	//		if(it != m_sock_data_pool_map.end())
	//		{
	//			it->second->data = it->second->data.substr(packSize, it->second->data.size());
	//		}
	//		LeaveCriticalSection(&m_data_lock);
	//	}
	//}
	
	//else
	//{
	//	if(content.find("quit") != string::npos)
	//	{
	//		out_len = 0x01;
	//	}
	//	else
	//	{
	//		while(1)
	//		{
	//			string::size_type pos = content.find(".\n");
	//			if(pos != string::npos)
	//			{
	//				content = content.substr(pos + strlen(".\n"), content.length());
	//			}
	//			else
	//				break;
	//		}
	//		it->second->data = content;
	//	}
	//}
	
	//LeaveCriticalSection(&m_data_lock);

	if ( NULL==process_handler )
	{
		return;
	}

	int packSize = 0x00;
	// ���ûص�������������,��������Ӧ��Ҫ���͵�������Ϣ
	process_handler( sock, (char*)in_buf, in_len, out_buf,out_len,packSize, g_pProcessObject );
}

//���socket ӳ������
void server_space::AddSockMapInfo(SOCKET sock, socket_map_list* pObj)
{
	EnterCriticalSection(&m_lock);
	m_sock_map[sock] = pObj;
	LeaveCriticalSection(&m_lock);
}


//���socket���ݴ洢����
void server_space::AddSockDataPool(SOCKET sock, SOCK_DATA_POOL*pData)
{
	EnterCriticalSection(&m_data_lock);
	m_sock_data_pool_map[sock] = pData;
	LeaveCriticalSection(&m_data_lock);
}

//�ͷ�socketӳ�����Դ
void server_space::ReleaseSockMapSource(LPSOCKET_INFORMATION pData)
{
	if(pData)
	{
		if(pData->DataBuf.buf)
		{
			delete [] pData->DataBuf.buf;
			pData->DataBuf.buf = NULL;
			pData->DataBuf.len = 0x00;
		}
	}
}


//����sock�ͷ���Դ
void server_space::DeleteSockMap(SOCKET sock)
{
	EnterCriticalSection(&m_lock);
	map<SOCKET,socket_map_list*>::iterator it = m_sock_map.begin();
	it = m_sock_map.find(sock);
	if(it != m_sock_map.end())
	{
		socket_map_list* pObj = it->second;
		closesocket(sock);
		LPSOCKET_INFORMATION pData = pObj->recv_link;
		ReleaseSockMapSource(pData);
		delete pData;
		pData = NULL;

		pData = pObj->send_link;
		ReleaseSockMapSource(pData);
		delete pData;
		pData = NULL;

		if(pObj)
		{
			delete pObj;
		}
		m_sock_map.erase(it);
	}
	LeaveCriticalSection(&m_lock);

	EnterCriticalSection(&m_data_lock);
	map<SOCKET,SOCK_DATA_POOL*>::iterator it_data = m_sock_data_pool_map.begin();
	it_data = m_sock_data_pool_map.find(sock);
	if(it_data != m_sock_data_pool_map.end())
	{
		it_data->second->data.clear();
		delete it_data->second;
		m_sock_data_pool_map.erase(it_data);
	}
	LeaveCriticalSection(&m_data_lock);

}


//�����˳�ʱ��������Դ
void server_space::DeleteResource()
{
	//����socket�������Դ
	EnterCriticalSection(&m_lock);

	while(1)
	{
		map<SOCKET,socket_map_list*>::iterator it = m_sock_map.begin();
		if(it != m_sock_map.end())
		{
			socket_map_list* pObj = it->second;
			closesocket(it->first);
			LPSOCKET_INFORMATION pData = pObj->recv_link;
			ReleaseSockMapSource(pData);
			delete pData;
			pData = NULL;

			pData = pObj->send_link;
			ReleaseSockMapSource(pData);
			delete pData;
			pData = NULL;

			if(pObj)
			{
				delete pObj;
			}
			m_sock_map.erase(it);
		}
		else
			break;
	}

	LeaveCriticalSection(&m_lock);

	//����������Դ
	EnterCriticalSection(&m_data_lock);

	while(1)
	{
		map<SOCKET,SOCK_DATA_POOL*>::iterator it_data = m_sock_data_pool_map.begin();
		if(it_data != m_sock_data_pool_map.end())
		{
			it_data->second->data.clear();
			delete it_data->second;
			m_sock_data_pool_map.erase(it_data);
		}
		else
			break;
	}
	LeaveCriticalSection(&m_data_lock);

}


//��ȡ��ǰ����������
unsigned int server_space::GetCurrentSessionCount()
{
	unsigned int count = 0x00;
	EnterCriticalSection(&m_lock);
	count = m_sock_map.size();
	LeaveCriticalSection(&m_lock);
	
	return count;
}

//�����׽�����Դ
void server_space::CleanSocketInfo(LPSOCKET_INFORMATION obj)
{
	if(!obj)
		return ;

	LPSOCKET_INFORMATION pScokInfo = (LPSOCKET_INFORMATION)obj;
	cout<<"disconnect _____________________:  "<<pScokInfo->Socket<<endl;
	DeleteSockMap(pScokInfo->Socket);
}

//��ȡ������socket��Դ
LPSOCKET_INFORMATION server_space::GetSendSockInfo(SOCKET sock, int bufferSize)
{
	LPSOCKET_INFORMATION info = NULL;
	EnterCriticalSection(&m_lock);
	map<SOCKET,socket_map_list*>::iterator it = m_sock_map.begin();
	it = m_sock_map.find(sock);
	if(it != m_sock_map.end())
	{
		socket_map_list* pObj = it->second;
		info = pObj->send_link;
	}
	if(info)
	{	
		assert(bufferSize > 0x00);
		memset(info->DataBuf.buf, 0x00, bufferSize);
		info->DataBuf.len = bufferSize;
		memset(&(info->Overlapped), 0x00, sizeof(OVERLAPPED));
		info->BytesSEND = 0x00;
	}
	LeaveCriticalSection(&m_lock);

	return info;
}

//��ȡ�µ�socket��Դ
LPSOCKET_INFORMATION server_space::GetNewSockInfo( SOCKET sock, int bufferSize)
{
	assert(bufferSize > 0x00);
	LPSOCKET_INFORMATION new_obj = new SOCKET_INFORMATION;
	new_obj->Socket = sock;
	new_obj->DataBuf.buf = new char[bufferSize];
	new_obj->DataBuf.len = bufferSize;
	memset(&new_obj->Overlapped,0,sizeof(WSAOVERLAPPED));
	memset(new_obj->DataBuf.buf,0,bufferSize);
	return new_obj;
}

///�����߳�
DWORD WINAPI server_space::AcceptThread(LPVOID lpParameter)
{
	TcpServerWin* pServer = (TcpServerWin*)lpParameter;
	if(!pServer)
		return FALSE;

	

	pServer->m_ListenSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP );
	if(pServer->m_ListenSocket == INVALID_SOCKET)
	{
		int err = WSAGetLastError();
		cout<<" WSASocket failed : error code : "<<err<<endl;
		return -1;
	}

	SOCKADDR_IN ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	ServerAddr.sin_port = htons(pServer->m_port);

	if(bind(pServer->m_ListenSocket,(LPSOCKADDR)&ServerAddr,sizeof(ServerAddr)) != 0x00)
	{
		int err = WSAGetLastError();
		cout<<" bind failed : error code : "<<err<<endl;
		return -1;
	}

	listen(pServer->m_ListenSocket,300);

	cout<<"listening....."<<endl;

	SOCKADDR_IN ClientAddr;
	int addr_length = sizeof(ClientAddr);
	while ( false==g_exit )
	{

		LPSOCKET_INFORMATION  SI = new SOCKET_INFORMATION;

		//���ϵĽ��տͻ��˵�����
		if ((SI->Socket = accept(pServer->m_ListenSocket,(SOCKADDR*)&ClientAddr, &addr_length)) != INVALID_SOCKET)
		{
			if( true==g_exit)
			{
				closesocket(SI->Socket);
				delete SI;
				break;
			}

			LPSOCKET_INFORMATION  send_info = new SOCKET_INFORMATION;
			send_info->DataBuf.buf = NULL;
			send_info->DataBuf.len = 0x00;
			socket_map_list *pobj = new socket_map_list;

			int nZero = 0;
			int nRet = setsockopt(SI->Socket, SOL_SOCKET, SO_SNDBUF, (char *)&nZero, sizeof(nZero));
			if(nRet == SOCKET_ERROR)
			{
				delete SI;
				delete send_info;
				delete pobj;
				continue;
			}

			printf("accept ip:%s port:%d\n",inet_ntoa(ClientAddr.sin_addr),ClientAddr.sin_port);
			memset(&SI->Overlapped,0,sizeof(WSAOVERLAPPED));
			SI->DataBuf.buf = new char[DATA_BUFSIZE];
			SI->DataBuf.len = DATA_BUFSIZE;
			memset(SI->DataBuf.buf,0,DATA_BUFSIZE);
			send_info->Socket = SI->Socket;

			send_info->DataBuf.buf = new char[DATA_BUFSIZE];
			send_info->DataBuf.len = DATA_BUFSIZE;
			memset(send_info->DataBuf.buf,0,DATA_BUFSIZE);

			pobj->recv_link = SI;
			pobj->send_link = send_info;

			SOCK_DATA_POOL* pDataPool = new SOCK_DATA_POOL;

			server_space::AddSockDataPool(SI->Socket,pDataPool);
			server_space::AddSockMapInfo(SI->Socket,pobj);

			//�ɷ����պ���
			DWORD Flags = 0x00;
			if(WSARecv(SI->Socket, &SI->DataBuf, 1, &SI->BytesRECV, &Flags, &SI->Overlapped, RecvWorkerRoutine) == SOCKET_ERROR)
			{
				int err = WSAGetLastError();
				if(err != WSA_IO_PENDING)
				{
					server_space::CleanSocketInfo(SI);
					continue;
				}  
			}
		}
		else
		{
			delete SI;
		}
	}

	return FALSE;
}

//�������ݵĻص�����
void CALLBACK server_space::RecvWorkerRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{
	LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION)Overlapped;
	if (Error != 0 || BytesTransferred == 0)
	{
		CleanSocketInfo(SI);
		return;
	}

	//�������ݴ���
	char* outBuf=NULL;
	int outLen = 0x00;
	DataProcessCenter(SI->Socket, SI->DataBuf.buf, BytesTransferred,outBuf, outLen);
	if(outLen <= 0)
	{
		CleanSocketInfo(SI);
		return;
	}

	//��ȡ����buffer,������Ҫ���͵����ݵ�buffer
	LPSOCKET_INFORMATION new_obj = GetSendSockInfo(SI->Socket,outLen);
	memcpy(new_obj->DataBuf.buf, outBuf, outLen);
	delete [] outBuf;

	memset(SI->DataBuf.buf,0,DATA_BUFSIZE);
	DWORD flag = 0x00;
	memset(&new_obj->Overlapped, 0x00, sizeof(new_obj->Overlapped));
	new_obj->BytesSEND = outLen;
	//��������
	if(WSASend(new_obj->Socket, &new_obj->DataBuf, 1, &new_obj->BytesSEND, flag, &new_obj->Overlapped, SendWorkerRoutine) == SOCKET_ERROR)
	{
		int iError = WSAGetLastError();
		if( iError!= WSA_IO_PENDING )
		{
			CleanSocketInfo(new_obj);
			return;
		}
	}

	//�����ɷ����պ���
	DWORD Flags = 0x00;
	if(WSARecv(SI->Socket, &SI->DataBuf, 1, &SI->BytesSEND, &Flags, &SI->Overlapped, server_space::RecvWorkerRoutine) == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		if(WSAGetLastError() != WSA_IO_PENDING)
		{
			CleanSocketInfo(SI);
			return;
		}
	}
}


void CALLBACK server_space::SendWorkerRoutine(DWORD Error, DWORD BytesTransferred, LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{

}

//���ô������ݵĻص�����
void server_space::SetProcessHandler(process_handler_fun fun, void *pObject)
{
	process_handler = fun;
	g_pProcessObject = pObject;
}



TcpServerWin::TcpServerWin()
{
	//Ĭ�϶˿ں�
	m_port = 6000;

	g_listen_thread = NULL;

	InitializeCriticalSection(&m_lock);
	InitializeCriticalSection(&m_data_lock);
}


TcpServerWin::~TcpServerWin()
{
	// �رռ���Socket
	if ( INVALID_SOCKET != m_ListenSocket)
	{
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;
	}

	// �رռ����߳�
	if ( NULL!=g_listen_thread )
	{
		// ���ü����߳��˳���־
		g_exit = true;

		// �ȴ������߳��˳�
		WaitForSingleObject(g_listen_thread,INFINITE);
		CloseHandle(g_listen_thread);
		g_listen_thread = NULL;
	}

	DeleteResource();

	::WSACleanup();

	DeleteCriticalSection(&m_lock);
	DeleteCriticalSection(&m_data_lock);
}

//��������
void TcpServerWin::Start(int port)
{
	//���������̺߳���
	m_port = port;
	g_listen_thread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AcceptThread, this, NULL, NULL);
}



