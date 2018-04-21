#ifndef MDPROTOCOL_H_
#define MDPROTOCOL_H_
#include "Protocol.h"
#include "netsync.h"
//#include "dwstype.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "PublicDefine.h"
#include "ChannelManager.h"
#include "Vmux6kProtocolDefine.h"

#pragma pack(1)


//���ܽ���������
typedef struct tagComDecoderParam
{
	unsigned short int iSlot;
	BYTE bChannel;				//ͨ���ţ�1��4�����ݰ忨���Ͷ���
	BYTE bStreamNo;				//�����ţ�1��4
	unsigned short int uiResolutionH;	//�ֱ��ʣ�H��
	unsigned short int uiResolutionV;	//�ֱ��ʣ�V��
	unsigned int uiPeerIP;
	unsigned int uiPeerPort;
	unsigned char bAudioFlag;	//��Ƶ���� 0=�ر� 1=��
	BYTE bAudioImport;			//��Ƶ����Դ  0=line ,  1=mic
	unsigned char bProtocol;	//1��TCP��2��UDP��3���鲥
	unsigned char bDecoderFomat;	//�����ʽ: MPEG1=1, MPEG2=2, MPEG4=3 ,MJPEG=4 ,H264=5, JPEG2K=6 

	//============================================
	// ��չ����
	unsigned int uiEncodecType;// 4 �ֽ�---����������ͣ�����visCommHead.H��eCODEC_TYPE����,unsigned int����	
	unsigned int uiStreamType;// 4 �ֽ�---������ʽ������visCommHead.H��eCODEC_VIDEO_FORMAT����,unsigned int����	
	char szUserName[32];		//DVR�û���
	char szPasswd[32];			//DVR����
	unsigned int uiDvrChannel;// 4 �ֽ�---DVRͨ���ţ�˵����unsigned int����	
	unsigned short usDecodecScreenId;// 2   �ֽ�---���벥����Ļ�ţ�˵����unsigned short���ͣ����ܽ�����֧�ֶ���Ļ���	
	unsigned short usDecodecChannel;// 2 �ֽ�---���벥�Ŵ��ںţ�˵����unsigned short���ͣ����ܽ�����֧�ֶര�����	
	//============================================
}COMDECODERPARAM;

typedef struct _tagDecDisplayParam
{
    /*
        4������Ļ˳������(9��16��������):
        -----------
        | 1  | 2  |
        -----------
        | 3  | 4  |
        -----------
    */

    /*
      ��ʾģʽ
      0-δ֪��1-VGA, 2-BNC,3-HDMI
     */
    int iDisplaymode;
    // ��N����ʾͨ������1��ʼ
    int iDisplayno;
    /*
      �ֱ���
      0-δ֪��δ����
      1-1920*1080
      2-800*600
      3-1024*768
      4-1280*1024
      5-1366*768
      6-1440*900
      7-1280*800
     */
    int iResolution;
    //������ͨ���ţ���1��ʼ��0��ʾδ�󶨣�128��ʾ2x2���ͨ����129��ʾ3x3, 130��ʾ4x4
    int iChannel;
}tDecDisplayParam;

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
	unsigned short slot_number;

	// ͨ����
	BYTE channel;
	BYTE stream_id;
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
	BYTE b_showtime;

	// ��ʾOSD
	BYTE b_showfont;
	unsigned short reserved; // ����ΪOSD��ɫ
}DecodecOsdParam;
enum OSD_TYPE
{
	OSD_FONT=1, // ����
	OSD_TIME=2, // ʱ��
	OSD_CONTENT1=3,  // ����1
	OSD_CONTENT2=4,  // ����2
	OSD_CONTENT3=5,  // ����3
	OSD_CONTENT4=6,  // ����4
	OSD_MONITORID=7,// ������ID
	OSD_TYPE_MAX,   // �ٽ�ֵ
};
enum OSD_COLOR
{
    OSD_COLOR_WHITE = 1,// ��ɫ
    OSD_COLOR_BLACK = 2,// ��ɫ
    OSD_COLOR_RED = 3,// ��ɫ
    OSD_COLOR_GREEN = 4, // ��ɫ
    OSD_COLOR_BLUE = 5, // ��ɫ
    OSD_COLOR_YELLOW = 6, // ��ɫ
    OSD_COLOR_PURPLE = 7, // ��ɫ
    OSD_COLOR_CYAN = 8, // ��ɫ
    OSD_COLOR_MAGENTA = 9, // ���
};


typedef struct _MDMsg
{
	VMUX6KHEAD head;
	union
	{
        CMDRESPONSE response;
        COMDECODERPARAM decparam;
        COMDECODERPARAM_EX decexparam;
        DecodecOsdParam osd;
        tDecDisplayParam display;
        int iAck;
	}body;
}MDMsg;

#pragma pack()

class CMDProtocol : public CProtocol
{

public: 
	CMDProtocol( int nPort, CChannelManager *pChnManager );
	virtual ~CMDProtocol();
    int StopEncoder(int chnno);
private:
	CSyncNetServer *m_pServer;

	static void THWorker(CSyncServerNetEnd *net, void *p);

	
	int ProcessStartDecoder(  MDMsg &msg, MDMsg &ack);


	
	int ProcessShowOSD(  MDMsg &msg, MDMsg &ack);
    int ProcessDisplay(MDMsg &msg, MDMsg &ack);
	
	int ProcessCtrlRequest(CSyncServerNetEnd *net, MDMsg &msg, MDMsg &ack);
	
	int GetMsg( CSyncNetEnd *net, MDMsg &msg, int timeout);
	
	int SendMsg(CSyncNetEnd *net, MDMsg &msg);
	
	long BuildHead(VMUX6KHEAD& Head);

	
	long CheckHead(  VMUX6KHEAD Head);
	
	long BuildCmdResponse( MDMsg &ack, int msgType, int nRet );
	
	int m_iExitFlag;
	
	long BuildDecoderParamResponse( MDMsg msg, MDMsg &ack );

	COMDECODERPARAM m_decparam[MAXDECCHANN];

	
	COMDECODERPARAM_EX m_decexparam[MAXDECCHANN];
	
	
	pthread_mutex_t m_lock;

	void MSGNTOH( MDMsg &msg );
};


#endif


