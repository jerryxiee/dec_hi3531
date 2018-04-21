// NetConnection.cpp: implementation of the CNetConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NetConnection.h"
#include "ErrorCode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define MAX_PROTOCOL_BUF	(3*1024*1024)		//Ĭ�ϵĽ���Э��buf�����ֵ
#define MAX_PROTOCOL_BUF_INCREASE	(20*1024*1024)	//����̬���ӵĽ���Э��buf�����ֵ

#define RECVDATA_OUTTIME_ONCE 3			//ÿ�ν������ݵĳ�ʱʱ��
#define DISCONNECTION_OUTTIME_SECOND 45		//��ʱû�н������ݶϿ����ӵ�ʱ��
#define DISCONNECTION_OUTTIME_TIMES (DISCONNECTION_OUTTIME_SECOND/RECVDATA_OUTTIME_ONCE)	//��ʱû�н������ݶϿ����ӵ��������γ�ʱ����
#define TIMEOUT_HEARTBEAT 20


CNetConnection::CNetConnection()
{
	m_pClient = new CClientPoint( );
		m_iExitFlag = 0;
	m_hdlThread = NULL;
	m_bIsConnected = false;

}

CNetConnection::~CNetConnection()
{
	if (m_pClient != NULL )
	{
		delete m_pClient;
		m_pClient = NULL;
	}
}

int CNetConnection::ConnectToServer( char *szIpAddr, int uiPlayPort )
{
	INFO_( "close client");
	CloseClient();
	INFO_( "close client ok.");
	int nRet = m_pClient->ConnectToController( szIpAddr, uiPlayPort );
	INFO_( "connect to server ok.");
	if ( nRet !=  0 ) return E_CLI_COMM_CONNECT;

#ifdef _WIN32
	m_hdlThread =  CreateThread( NULL , NULL, WorkProc, (void*)this, NULL , NULL );
#else
	pthread_create( &m_hdlThread, NULL, WorkProc, (void*)this );
#endif
	if ( m_hdlThread == NULL )
	{
		ERROR_( "create workproc thread failed." );
		return E_CLI_CREATE_THREAD;
	}
	m_bIsConnected = true;
	return SUCCESS;
}

int CNetConnection::CloseClient()
{
	m_pClient->CloseClient();
	m_iExitFlag = 1;

	if ( m_hdlThread != NULL )
	{
#ifdef _WIN32
		WaitForSingleObject( m_hdlThread, 2000 );
		CloseHandle( m_hdlThread );
#else
		pthread_join( m_hdlThread, NULL );
#endif
		m_hdlThread = NULL;
		INFO_( "thread exit.");
	}
	m_iExitFlag = 0;
	return 0;
}

int CNetConnection::Send( unsigned char *szData, int nLen )
{
	return m_pClient->SendBySize(szData, nLen );
}

#ifdef _WIN32
DWORD WINAPI CNetConnection::WorkProc( void *pParent )
#else 
void *CNetConnection::WorkProc(void *pParent )
#endif
{
	CNetConnection *pLogic = ( CNetConnection *)pParent;
	//pLogic->Worker();

#ifndef _WIN32
	pthread_detach( pthread_self() );
#endif 
	return NULL;
}

int CNetConnection::NoteHeartBeatToServer()
{
	return 0;
}

int CNetConnection::Worker()
{
	int iRet=0;
	char* szBuf=NULL;
	char* szProtocolBuf = NULL;
	int iMaxProtocolLen=MAX_PROTOCOL_BUF;
	int iRecvOneTimes= 1024*1024;//ÿ�ν��յĳ���
	int iRecved=0;//���ص��ν��յ����ݳ���

	szBuf = new char[iMaxProtocolLen];
	szProtocolBuf = new char[iMaxProtocolLen];

	unsigned int iTotalPos = 0;
	//int iLastPos = 0;
	tmold = time(&tmold);
	
	while ( m_iExitFlag == 0 )
	{
		iRecved = m_pClient->RecvBySizeT( szBuf + iTotalPos ,iRecvOneTimes, 1 ); //��ʱ����
		if (iRecved > 0 )
		{
			
        }
		else if(iRecved==0)//���ݽ��ճ�ʱ
		{
		
		}
		else//����ʧ��
		{
			iRet=iRecved;
			_snprintf( szInfo, 10230, "[%s]����Э������ʧ��,���ӶϿ�,iRecved=%d",m_pClient->GetAddr(),iRecved, 10240);
			ERROR_(szInfo);
			break;
		}
		time_t tmnow;
		tmnow = time(&tmnow);
		
		if ( tmnow - tmold >= TIMEOUT_HEARTBEAT )
		{
			NoteHeartBeatToServer();
			tmold = tmnow;
		}	
	}//end while

	INFO_( "close client");
	m_pClient->CloseClient();

	INFO_("send disconnect notify");
//	if ( g_disconnectcallback != NULL ) g_disconnectcallback((void*)this, g_pCaller);


	INFO_("delete objs");

	if (szBuf!=NULL)
	{
		delete[] szBuf;
		szBuf = NULL;
	}
	if ( szProtocolBuf != NULL )
	{
		delete[] szProtocolBuf;
		szProtocolBuf = NULL;
	}
	_snprintf( szInfo, 10230,"[%s] �Ͽ�����",m_pClient->GetAddr(), 10240);
	ERROR_( szInfo );
	return iRet;
}

char *CNetConnection::GetLocalAddr()
{
	if ( m_pClient == NULL ) return (char*)"0.0.0.0";
	return m_pClient->GetLocalAddr();
}

long CNetConnection::BuildHead(VMUX6KHEAD& Head)
{
	memset(&Head, 0, sizeof(VMUX6KHEAD));
	Head.bHead1 = 0xBF;
	Head.bHead2 = 0xEC;
	Head.bVersion = 0x01;
	return 0;
}

long CNetConnection::CheckHead(const VMUX6KHEAD& Head)
{
	if (Head.bHead1 != 0xBF)
	{
		ERROR( "CComDecodecDriver::CheckHead head1 is 0x%x", Head.bHead1);
		return -1;
	}

	if (Head.bHead2 != 0xEC)
	{
		ERROR( "CComDecodecDriver::CheckHead head2 is 0x%x", Head.bHead2);
		return -1;
	}

	if ((Head.bVersion & 0xf) != 0x1)
	{		
		ERROR( "CComDecodecDriver::CheckHead version is 0x%x", Head.bVersion);
		return -1;
	}

	if ((Head.bFlag & 0x10) != 0x10)
	{
		ERROR( "CComDecodecDriver::CheckHead flag is 0x%x", Head.bFlag);
		return -1;
	}

	if (((Head.bHead1 + Head.bHead2 + Head.bVersion + Head.bFlag \
		+ Head.bMsgType + Head.bLength_h + Head.bLength_l) & 0xff) != Head.bChecksum)
	{
		ERROR( "CComDecodecDriver::CheckHead checksum is 0x%x", Head.bChecksum);
		return -1;
	}
	return 0;
}

int CNetConnection::StartDecoder( int nChannelNo, char *szUrl )
{
	int lRet = ERR_BCD_OPERATE_CODEC_FAILED;

	unsigned int uiStreamNo = 0;
	unsigned int uiEncodecType = 0;
	unsigned int uiStreamType = 0;
	unsigned int uiDvrChannel = nChannelNo;
	unsigned int uiCastType = 0;
	
	//���͡�����buffer
	char szSendBuf[1024];
	char szRecvBuf[1024];
	memset(szSendBuf, 0, sizeof(szSendBuf));
	memset(szRecvBuf, 0, sizeof(szRecvBuf));

	//Э�����ݰ����ȶ���
	int iHeadLen = sizeof(VMUX6KHEAD);
	int iExBodyLen = sizeof(COMDECODERPARAM_EX);
	int iBodyLen = sizeof(COMDECODERPARAM);
	int iCmdAckLen = sizeof(CMDRESPONSE);

	//����
	VMUX6KHEAD head;
	BuildHead(head);
	head.bFlag = 0x0f;
	head.bMsgType = 0x05;
	head.bLength_h = (iExBodyLen & 0xff00) >> 8;
	head.bLength_l = iExBodyLen & 0x00ff;
	head.bChecksum = ((head.bHead1 + head.bHead2 + head.bVersion + head.bFlag \
		+ head.bMsgType + head.bLength_h + head.bLength_l) & 0xff);

	//����Э��ͷ
	memcpy(szSendBuf, &head, iHeadLen);

	//����Э����
	COMDECODERPARAM_EX* pSendCmd = (COMDECODERPARAM_EX*)(&(szSendBuf[iHeadLen]));

	//�޸�����ֶ�
	pSendCmd->DecoderParam.iSlot = htons(nChannelNo);
	pSendCmd->DecoderParam.bChannel = htons(nChannelNo);
	pSendCmd->DecoderParam.bStreamNo = nChannelNo;
	pSendCmd->DecoderParam.uiEncodecType = 0;
	pSendCmd->DecoderParam.uiStreamType = 0;
	pSendCmd->DecoderParam.uiDvrChannel = nChannelNo;
	pSendCmd->DecoderParam.usDecodecScreenId = htons(nChannelNo);
	pSendCmd->DecoderParam.usDecodecChannel = htons(nChannelNo);
	pSendCmd->DecoderParam.bProtocol = NET_URL;
	strcpy( pSendCmd->szRTSPUrl, szUrl );

	Send( (unsigned char*)pSendCmd, sizeof(iHeadLen+iExBodyLen));


	//��ȡ���ý��
	memset(szRecvBuf, 0, sizeof(szRecvBuf));


	lRet = m_pClient->RecvBySizeT( szRecvBuf, iHeadLen+iCmdAckLen, 3000);
	if ( lRet != (iHeadLen+iCmdAckLen) )
	{
		//g_pCCommPortPool->ClosePort(m_dataCommPort.GetUbiLogicId());	//ʧ�ܣ���رո�ͨѶ�˿�
		return ERR_BCD_OPERATE_CODEC_FAILED;
	}

	if (CheckHead(*(VMUX6KHEAD*)(szRecvBuf)) != 0)
	{
		return ERR_BCD_OPERATE_CODEC_FAILED;
	}

	int* pRet = (int*)(szRecvBuf + iHeadLen + 4);
	if (*pRet != 0)
	{
		return ERR_BCD_OPERATE_CODEC_FAILED;
	}
	return 0;
}



