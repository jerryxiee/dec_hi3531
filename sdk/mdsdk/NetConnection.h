// NetConnection.h: interface for the CNetConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETCONNECTION_H__32EE0F75_33C7_47A8_95D2_CBFC4A80D4DD__INCLUDED_)
#define AFX_NETCONNECTION_H__32EE0F75_33C7_47A8_95D2_CBFC4A80D4DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ClientPoint.h"
#include "CommonDef.h"
#include <time.h>
#include "Vmux6kHeader.h"
//���ܽ���������
typedef struct tagComDecoderParam
{
	short int iSlot;
	BYTE bChannel;				//ͨ���ţ�1��4�����ݰ忨���Ͷ���
	BYTE bStreamNo;				//�����ţ�1��4
	short int uiResolutionH;	//�ֱ��ʣ�H��
	short int uiResolutionV;	//�ֱ��ʣ�V��
	unsigned int uiPeerIP;
	unsigned int uiPeerPort;
	unsigned char bAudioFlag;	//��Ƶ���� 0=�ر� 1=��
	BYTE bAudioImport;			//��Ƶ����Դ  0=line ,  1=mic
	unsigned char bProtocol;	//1��TCP��2��UDP��3���鲥
	unsigned char bDecoderFomat;	//�����ʽ: MPEG1=1, MPEG2=2, MPEG4=3 ,MJPEG=4 ,H264=5, JPEG2K=6 

	//============================================
	// ��չ����
	unsigned int uiEncodecType;	//4 �ֽ�---����������ͣ�����visCommHead.H��eCODEC_TYPE����,unsigned int����	
	unsigned int uiStreamType;	//4 �ֽ�---������ʽ������visCommHead.H��eCODEC_VIDEO_FORMAT����,unsigned int����	
	char szUserName[32];		//DVR�û���
	char szPasswd[32];			//DVR����
	unsigned int uiDvrChannel;	//4 �ֽ�---DVRͨ���ţ�˵����unsigned int����	
	unsigned short usDecodecScreenId;	//2 �ֽ�---���벥����Ļ�ţ�˵����unsigned short���ͣ����ܽ�����֧�ֶ���Ļ���	
	unsigned short usDecodecChannel;	//2 �ֽ�---���벥�Ŵ��ںţ�˵����unsigned short���ͣ����ܽ�����֧�ֶര�����	
	//============================================
}COMDECODERPARAM;

typedef struct tagComDecoderParam_EX
{
	COMDECODERPARAM DecoderParam;
	char szRTSPUrl[255];
}COMDECODERPARAM_EX;

enum CMD_DECODER_TYPE
{
	CMD_GET_DECODER_OSD_PARAM = 0x0206,  //��ȡ������OSD����(ʱ��λ��,����λ��,��������)
	CMD_SET_DECODER_OSD_PARAM = 0x0207,  //���ý�����OSD���� 
};

//#define GCMD_GET_CODER_OSD_PARAM                       0X0206 //��ȡ������OSD����(ʱ��λ��,����λ��,��������)
//#define GCMD_SET_DECODER_OSD_PARAM                     0X020B //���ý�����OSD���� 
typedef struct body_0206_0207_md
{
	unsigned int slot_number;

	// ͨ����
	unsigned char channel;
	unsigned char stream_id;
	unsigned int osd;

	// ��ʼλ��
	unsigned int x_time;
	unsigned int y_time;

	// �����С(����ϵͳ�����С)
	unsigned int x_font;
	unsigned int y_font;

	// OSD����
	char buffer_content[64];

	// ��ʾϵͳʱ��
	unsigned char b_showtime;

	// ��ʾOSD
	unsigned char b_showfont;
	unsigned int reserved;
}DecodecOsdParam;

enum OSD_TYPE
{
	OSD_FONT=1,
	OSD_TIME=2,
};

class CNetConnection  
{
public:
	CNetConnection();
	virtual ~CNetConnection();
	CClientPoint *m_pClient;

	int NoteHeartBeatToServer();

	int ConnectToServer( char *szIpAddr, int uiPlayPort );
	int CloseClient();
	int Send( unsigned char *szData, int nLen );
	int Worker();
	char *GetLocalAddr();
	static DWORD WINAPI WorkProc( void *pParent );
	char szInfo[10240];

	THREADHANDLE m_hdlThread;
	bool m_bIsConnected;
	int m_iExitFlag;
	time_t tmold;

	int StartDecoder( int nChannelNo, char *szUrl );

	long BuildHead(VMUX6KHEAD& Head);

	long CheckHead(const VMUX6KHEAD& Head);



};

#endif // !defined(AFX_NETCONNECTION_H__32EE0F75_33C7_47A8_95D2_CBFC4A80D4DD__INCLUDED_)


