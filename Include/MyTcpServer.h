#ifndef  __TCP__SERVER__WIN__
#define  __TCP__SERVER__WIN__

#include <vector>
#include "winsockdef.h"
#include <netinet/in.h>
#include <arpa/inet.h>

#include <list>
#include <map>
using namespace std;

// ���ݴ�����ָ��
typedef void process_handler_fun(SOCKET s, char* in_buffer, int buf_len, char*& out_buffer, int& out_len, int& packetSize, void* pObject);

namespace server_space
{

#define DATA_BUFSIZE 1024			// ���ջ�������С
#define MAXSESSION 10000			// ���������
#define HEART_BEAT_INTERVAL 20		//�������
#define closesocket(x) close(x)
	typedef void *LPVOID;
	typedef struct _WSABUF {
	    ULONG len;     /* the length of the buffer */
	    char  *buf; /* the pointer to the buffer */
	} WSABUF,  * LPWSABUF;


	typedef struct _OVERLAPPED
		{
		unsigned long Internal;
		unsigned long InternalHigh;
		union 
			{
			struct 
				{
				DWORD Offset;
				DWORD OffsetHigh;
				}	;
			void * Pointer;
			}	;
		HANDLE hEvent;
		}	OVERLAPPED;
	
	typedef struct _OVERLAPPED *LPOVERLAPPED;
	typedef struct _OVERLAPPED *    LPWSAOVERLAPPED;


	
	typedef struct _WSAOVERLAPPED {
		DWORD	 Internal;
		DWORD	 InternalHigh;
		DWORD	 Offset;
		DWORD	 OffsetHigh;
		HANDLE hEvent;
	} WSAOVERLAPPED,  * LPWSAOVERLAPPED;

	typedef struct _SOCKET_INFORMATION
	{
		OVERLAPPED Overlapped;
		SOCKET Socket;
		WSABUF DataBuf;
		DWORD BytesSEND;
		DWORD BytesRECV;
	} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;

	typedef struct socket_map
	{
		LPSOCKET_INFORMATION recv_link;
		LPSOCKET_INFORMATION send_link;
	}socket_map_list;


	// Ϊÿ���ͻ��˴洢�������ݣ�ֱ���д�������
	typedef struct sock_data_pool
	{
		string data;

	}SOCK_DATA_POOL;


	// �����׽������ӳ����Դ
	void AddSockMapInfo(SOCKET ,socket_map_list* pObj);

	// �����׽���ƥ��������ݳ���
	void AddSockDataPool(SOCKET, SOCK_DATA_POOL*);

	// �����׽������ٶ�Ӧ����Դ
	void DeleteSockMap(SOCKET sock);

	// �����׽�����Դ
	void CleanSocketInfo(LPSOCKET_INFORMATION obj);

	// �ͷ��׽��ֵ�ӳ����Դ
	void ReleaseSockMapSource(LPSOCKET_INFORMATION obj);

	// ��ȡ�׽��ֵķ�����Դ
	LPSOCKET_INFORMATION GetSendSockInfo(SOCKET sock, int bufferSize);

	// ��ȡһ���µ��׽�����Դ
	LPSOCKET_INFORMATION GetNewSockInfo(SOCKET sock ,int bufferSize);

	// ��ȡ��ǰ�Ŀͻ�������
	unsigned int GetCurrentSessionCount();

	// ���ûص�������������
	void SetProcessHandler(process_handler_fun* fun, void *pObject);

	// ���ݴ������ĺ���
	void DataProcessCenter(SOCKET sock,			// �׽���
							char* in_buf,		// ��ȥ������ָ��
							int in_len,			// ���ݳ���
							char*& out_buf,		// ���ص�����ָ��
							int& out_len);		// ���ص����ݳ���

	// �����˳�ʱ���ͷ���Դ
	void DeleteResource();

	class TcpServerWin
	{
	public:
		// ���캯��
		TcpServerWin();

		// ��������
		~TcpServerWin();

		// ��������
		void Start(int port);

	public:
		// �����̺߳���
		static  DWORD WINAPI AcceptThread(LPVOID lpParameter);

		// �����������
		static  DWORD WINAPI CheckHeartBeat(LPVOID lpParameter);

		// �������ݻص�����
		void   RecvWorkerRoutine(DWORD Error, DWORD BytesTransferred,LPWSAOVERLAPPED Overlapped, DWORD InFlags);

		// �������ݻص�����
		void   SendWorkerRoutine(DWORD Error, DWORD BytesTransferred,LPWSAOVERLAPPED Overlapped, DWORD InFlags);

	private:
		// ��ʼ������
		SOCKET  m_ListenSocket;
		unsigned int m_port;
	};
}

#endif

