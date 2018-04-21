
#pragma once 

#include "Protocol.h"
#include "netsync.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "PublicDefine.h"
#include "ChannelManager.h"


#pragma pack(1)

//Э��ͷ
typedef struct tagZyHead
{
	BYTE bHead1;
	BYTE bHead2;
	BYTE bHead3;
	BYTE bHead4;
	BYTE bHead5;
	BYTE bHead6;
	BYTE bVersion;
	BYTE bFlag;
	BYTE bMsgType_h;
	BYTE bMsgType_l;
	BYTE bLength_h;
	BYTE bLength_l;
	BYTE bReserved1;
	BYTE bReserved2;
	BYTE bReserved3;
	BYTE bChecksum;
}ZYHEAD;


//������ACK����ֵ
typedef struct tagZyCmdResponse
{
	int iRet;
}ZYCMDRESPONSE;

//����������Ϣ
typedef struct tagZyStreamControl
{
	BYTE bStreamNo;	//�����ţ�1��4
	unsigned int iProtocol;	/* 1=TCP, 2=UDP,3=MUTITYCAST*/
	unsigned int iPeerIP;
	unsigned int iPeerPort;
}ZYSTREAMCONTROL;

//�������ͶԶ���Ϣ
typedef struct tagZyClientControl
{
	unsigned int iProtocol;	/* 1=TCP, 2=UDP,3=MUTITYCAST*/
	unsigned int iPeerIP;
	unsigned int iPeerPort;
}ZYCLIENTCONTROL;

//�����ṹ����������
typedef struct tagZyStreamHead
{
	BYTE bStreamNo;	//�����ţ�1��4
	BYTE bType;		//0: ������(ES,TS,PS)  1: ��ý�崫��Э��ͷ
	BYTE bReserve[3];
}ZYSTREAMHEAD;

//�������������
typedef struct tagZyEncoderParam
{
	unsigned short sReserved;
	unsigned short bStreamNo;				//�����ţ�1��4
	unsigned int iInterfaceType;			  //��Ƶ���� 0=HDMI 1=VGA 2=YPbPr
	unsigned int iInputFormat;   //�����ʽ	˵����int���ͣ�MPEG1=1, MPEG2=2, MPEG4=3 ,MJPEG=4 ,H264=5, JPEG2K=6
	unsigned int iSwitch;        //��������  1=On, 0=off
	unsigned int iResolution;    //�ֱ���
	unsigned int iFrameRate;	//֡��, unsigned int����(PAL:0-25,NTSC:0-30)
	unsigned int iCodeRate;		//����, unsigned int����,��λΪK
	unsigned int uiGOP;			//GOP��unsigned int����
	unsigned int uiGOP_M;		//GOP_M, unsigned int����,��ʾGOP����Ϸ�ʽIP=0,IBP=1,IBBP=2

	unsigned int uiCodeRateType;//��������	˵����0=CBR, 1=VBR
	unsigned int uiQP;
	unsigned int uiReserved;
}ZYENCODERPARAM;



//�������������
typedef struct tagZyVideoParam
{
	unsigned int iReserved;
	unsigned char bReserved; 	
	unsigned char bBrightness;
	unsigned char bContrast;
	unsigned char bHue;
	unsigned char bSaturation;
	unsigned char bSharpness;
}ZYVIDEOPARAM;


//������OSD����
typedef struct tagZyOsdParam
{
	unsigned int bReserved;				//�����ţ�1��4
	unsigned int uiFontColor;	//OSD������ɫ(0=��ɫ,1=��ɫ)
	unsigned int uiShowTime;    //�Ƿ���ʾʱ�� ��0-hiden ��1-show��
	unsigned int uiTimeX;
	unsigned int uiTimeY;
	unsigned int uiShowWord;   //�Ƿ���ʾ���� ��0-hiden ��1-show��
	unsigned int uiWordX;
	unsigned int uiWordY;
	char szWord[64];
	unsigned int bReserved2;
}ZYOSDPARAM;

//������ʱ������
typedef struct tagZyTimeParam
{
	unsigned int iReserved;
	BYTE bYear;
	BYTE bMonth;
	BYTE bDay;
	BYTE bWeek;
	BYTE bHour;
	BYTE bMinute;
	BYTE bSecond;
	BYTE bReserve;
}ZYTIMEPARAM;


typedef struct tagZyPTZ
{

	unsigned int iReserved;
	BYTE bAction;
	BYTE bPresetNo;
	BYTE bSpeed;
	BYTE bReserved;
}ZYPTZ;

//����������
typedef struct tagZyDecoderParam
{
	unsigned int iCodecType;				//�������ͣ�1��4
	unsigned int iStreamNo;	            //�����ţ�H��
	unsigned int iProtocol;
	unsigned int iPeerIp;
	unsigned int iPeerPort;
}ZYDECODERPARAM;


//����������
typedef struct tagZyDecoderStreamInfo
{
	unsigned int iCodecType;				//�������ͣ�1��4
	unsigned int iStreamNo;	            //�����ţ�H��
	unsigned int iHeadFlag;              
	unsigned int iProtocol;
	unsigned int iPeerIp;
	unsigned int iPeerPort;
	unsigned int iStreamNo2;	            //�����ţ�H��
	unsigned int iHeadFlag2;              
	unsigned int iProtocol2;
	unsigned int iPeerIp2;
	unsigned int iPeerPort2;

}ZYDECODERSTREAMINFO;


typedef struct tagZyCameraSettingBody
{
	unsigned short usParam;
	unsigned short usReserved;
}ZYCAMERASETTINGBODY;


typedef struct tagZyCameraSettingHead
{
	unsigned int iReserved;
	unsigned short usModuleType; //������ͺ�(1�� Sony EH4300��о 2�� Sayno HMD500��о 3:Hitachi SC220��о)
	unsigned short usCtrlCmd;
}ZYCAMERASETTINGHEAD;

typedef struct tagZyCameraSettingParam
{
	ZYCAMERASETTINGHEAD header;
	ZYCAMERASETTINGBODY body;
}ZYCAMERASETTINGPARAM;

//����ģʽ
typedef struct tagZyWorkModeParam
{
	unsigned int uiWorkMode;			//����ģʽ  0=��������1=��������2=�������
}ZYWORKMODEPARAM;


// ��Ƶ��ʧ/�ڵ�������Ϣ
typedef struct tagZyVideoAlarm
{
	BYTE bAlarmStatus0;		//����״̬����n ��bitλ���ڵ�n·����ͨ����Ƶ��ʧ���������� 0=��������1=����
	BYTE bAlarmStatus1;		//����״̬����n ��bitλ���ڵ�n·����ͨ����Ƶ�ڵ����������� 0=��������1=����
	BYTE bReserve[2];		//Reserved,��0
}ZYVIDEOALARM;	


//�豸��������
typedef struct tagZyRebootParam
{
	unsigned int iReserved;
}ZYREBOOTPARAM;



typedef struct tagZyGetStreamCtl
{
	unsigned int iStreamNo;
	unsigned short bStreamNo;	//�����ţ�1��4
	unsigned short bVideoFormat; //������׼1 hd-h.264 2 Sd-h.264
	unsigned  iHeadFlag;  //�������ԣ�1 = ��׼��,2 = ������
	unsigned  int iProtocol;	/* 1=TCP, 2=UDP,3=MUTITYCAST*/
	unsigned int iPeerIP;
	unsigned int iPeerPort;
}ZYGETSTREAMCTL;


//�������ͶԶ���Ϣ
typedef struct tagZyClientCtl
{
	unsigned short bStreamNo;	//�����ţ�1��4
	unsigned short bVideoFormat; //������׼1 hd-h.264 2 Sd-h.264
	unsigned int iHeadFlag;  //�������ԣ�1 = ��׼��,2 = ������
	unsigned int iProtocol;	/* 1=TCP, 2=UDP,3=MUTITYCAST*/
	unsigned int iPeerIP;
	unsigned int iPeerPort;
}ZYCLIENTCTL;



//�޸��� ��ȡ������Ϣ,��ʼ��������,������������,��������,��� ��׼��/������ ������
//���޸�ֻ���Vbox6671��Vaux6167��Vbox400��Vbox1400�ͺ���Ч
//����������Ϣ
typedef struct tagZyStreamCtl
{
	unsigned short bStreamNo;	//�����ţ�1��4
	unsigned short bVideoFormat; //������׼1 hd-h.264 2 Sd-h.264
	unsigned int iHeadFlag;  //�������ԣ�1 = ��׼��,2 = ������
	unsigned int iProtocol;	/* 1=TCP, 2=UDP,3=MUTITYCAST*/
	unsigned int iPeerIP;
	unsigned int iPeerPort;
}ZYSTREAMCTL;

//////

#pragma  pack()

enum ZyMsgGroup
{
	ZY_SYSTEM_PROTOCOL = 0, //ϵͳ��Э�飨catalog = 0��
	ZY_CONFIG_PROTOCOL,	//�豸����Э�飨catalog = 1��
	ZY_ENCODER_PROTOCOL,	//�������Э�飨catalog = 2��
	ZY_ALARM_PROTOCOL,		//�澯Э�飨catalog = 3��
	ZY_STREAM_PROTOCOL,	//ý�������Ϳ���Э�飨catalog = 4��
	ZY_CAMCONTROL_PROTOCOL	//��̨��������Э��
};

enum ZyNetProtocol
{
	ZY_NET_TCP = 1,
	ZY_NET_UDP,
	ZY_NET_MULTICAST,
	ZY_NET_RTSP
};


enum ZyPTZType
{
	/*	
	1=Up,2=StopUp,3=Down,4=StopDown,5=Left,6=StopLeft,
	7=Right,8=StopRight,9=ZoomTele,10=StopZoomTele,11=ZoomWide,
	12=StopZoomWide,13=FocusNear,14=StopFocusNear,15=FocusFar,16=StopFocusFar,
	17=IrisOpen,18=StopIrisOpen,19=IrisClose,20=StopIrisClose,21=RainBrush,
	22=StopRainBrush,23=SetPtzPre,24=GotoPtzPre,25=ClearPtzPre,26=Vidicon,
	27=StopVidicon,28=AssistantSwitch,29=StopAssistantSwitch,30=Water 31=StopWater */

	ZYPTZ_UP 								= 1,
	ZYPTZ_STOPUP							= 2, 
	ZYPTZ_DOWN								= 3,	 
	ZYPTZ_STOPDOWN							= 4,
	ZYPTZ_LEFT								= 5, 
	ZYPTZ_STOPLEFT							= 6,
	ZYPTZ_RIGHT								= 7,
	ZYPTZ_STOPRIGHT							= 8,
	ZYPTZ_ZOOMTELE							= 9,
	ZYPTZ_STOPZOOMTELE						= 10, 
	ZYPTZ_ZOOMWIDE							= 11,
	ZYPTZ_STOPZOOMWIDE						= 12,
	ZYPTZ_FOCUSNEAR							= 13,
	ZYPTZ_STOPFOCUSNEAR						= 14,
	ZYPTZ_FOCUSFAR							= 15,
	ZYPTZ_STOPFOCUSFAR						= 16,
	ZYPTZ_IRISOPEN							= 17,
	ZYPTZ_STOPIRISOPEN						= 18,
	ZYPTZ_IRISCLOSE							= 19,
	ZYPTZ_STOPIRISCLOSE						= 20,
	ZYPTZ_RAINBRUSH							= 21,
	ZYPTZ_STOPRAINBRUSH						= 22,
	ZYPTZ_SETPTZPRE							= 23,
	ZYPTZ_GOTOPTZPRE						= 24,

	ZYPTZ_CLEARPTZPRE						= 25,
	ZYPTZ_VIDICON							= 26,
	ZYPTZ_STOPVIDICON						= 27,
	ZYPTZ_ASSISTANTSWITCH					= 28,
	ZYPTZ_STOPASSISTANTSWITCH				= 29,
	ZYPTZ_WATER								= 30,
	ZYPTZ_STOPWATER							= 31,	
	
	


	/*
	ZYPTZ_ZOOMTELE							= 7,
		ZYPTZ_STOPZOOMTELE						= 9, 
		ZYPTZ_ZOOMWIDE							= 5,
		ZYPTZ_STOPZOOMWIDE						= 9,
		ZYPTZ_FOCUSNEAR 						= 13,
		ZYPTZ_STOPFOCUSNEAR 					= 17,
		ZYPTZ_FOCUSFAR							= 15,
		ZYPTZ_STOPFOCUSFAR						= 17,
		ZYPTZ_IRISOPEN							= 19,
		ZYPTZ_STOPIRISOPEN						= 18,
		ZYPTZ_IRISCLOSE 						= 21,
		ZYPTZ_STOPIRISCLOSE 					= 23,
		ZYPTZ_RAINBRUSH 						= 21,
		ZYPTZ_STOPRAINBRUSH 					= 23,
		ZYPTZ_SETPTZPRE 						= 29,
		ZYPTZ_GOTOPTZPRE						= 31,

	*/

};

typedef struct _ZYMsg
{
	ZYHEAD head;
	union
	{
		ZYCMDRESPONSE ret;
		ZYSTREAMCTL streamctl;
		ZYPTZ ptz;
		ZYTIMEPARAM timeset;
		ZYENCODERPARAM encparam;
		
	}body;
}ZYMsg;





class CZyProtocol : public CProtocol
{

public: 
	CZyProtocol( int nPort, CChannelManager *pChnManager );
	virtual ~CZyProtocol();
    int StopEncoder(int chnno);
private:
	CSyncNetServer *m_pServer;

	static void THWorker(CSyncServerNetEnd *net, void *p);

	int GetMsg( CSyncNetEnd *net, ZYMsg &msg, int timeout);
	
	int SendMsg(CSyncNetEnd *net, ZYMsg &msg);
	
	long BuildHead(ZYHEAD& Head);

	
	long CheckHead(  ZYHEAD Head);
	
	
	long BuildCmdResponse( ZYMsg &ack, int msgType_l, int msgType_h, int nRet );


	void FillHeadCheckSum( ZYHEAD &Head );
	
	void MSGNTOH( ZYMsg &msg );

	void MSGHTON( ZYMsg &msg );
	
	int m_iExitFlag;
	pthread_mutex_t m_lock;

	
	int ProcessCtrlRequest(CSyncServerNetEnd *net, ZYMsg &msg, ZYMsg &ack);
	
	int ProcessGetSendInfo( ZYMsg msg, ZYMsg &ack );
	
	int ProcessStartSendInfo( ZYMsg msg, ZYMsg &ack );

	int ProcessStopSendInfo( ZYMsg msg, ZYMsg &ack );
	
	int ProcessPtz( ZYMsg msg, ZYMsg &ack );

	
	int ProcessSetTime( ZYMsg msg, ZYMsg &ack );

	
	int ProcessSetCodecParam( ZYMsg msg, ZYMsg &ack );

	
	bool IsMulticastIP (unsigned long dwIP);


};


