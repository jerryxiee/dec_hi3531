#ifndef __VMUX6K_PROTOCOL_H_2015
#define __VMUX6K_PROTOCOL_H_2015

typedef unsigned char BYTE;

#pragma pack(1)

//Э��ͷ
typedef struct tagVmux6kHead
{
	BYTE bHead1;
	BYTE bHead2;
	BYTE bVersion;
	BYTE bFlag;
	BYTE bMsgType;
	BYTE bLength_h;
	BYTE bLength_l;
	BYTE bChecksum;
}VMUX6KHEAD;



//������ACK����ֵ
typedef struct tagCmdResponse
{
	short int iSlot;
	short int iChannel;
	int iRet;
}CMDRESPONSE;

//����������Ϣ
typedef struct tagStreamControl
{
	short int iSlot;
	BYTE bChannel;	//ͨ���ţ�1��4�����ݰ忨���Ͷ���
	BYTE bStreamNo;	//�����ţ�1��4
	int iProtocol;	/* 1=TCP, 2=UDP,3=MUTITYCAST*/
	int iPeerIP;
	int iPeerPort;
}STREAMCONTROL;

//�������ͶԶ���Ϣ
typedef struct tagClientControl
{
	int iProtocol;	/* 1=TCP, 2=UDP,3=MUTITYCAST*/
	int iPeerIP;
	int iPeerPort;
}CLIENTCONTROL;

//�����ṹ����������
typedef struct tagStreamHead
{
	short int iSlot;
	BYTE bChannel;	//ͨ���ţ�1��4�����ݰ忨���Ͷ���
	BYTE bStreamNo;	//�����ţ�1��4
	BYTE bType;		//0: ������(ES,TS,PS)  1: ��ý�崫��Э��ͷ
	BYTE bReserve[3];
}STREAMHEAD;

//�������������
typedef struct tagEncoderParam
{
	short int iSlot;
	BYTE bChannel;				//ͨ���ţ�1��4�����ݰ忨���Ͷ���
	BYTE bStreamNo;				//�����ţ�1��4
	int iEncodeType;			//�����ʽ	˵����int���ͣ�MPEG1=1, MPEG2=2, MPEG4=3 ,MJPEG=4 ,H264=5, JPEG2K=6
	unsigned int iFrameRate;	//֡��, unsigned int����(PAL:0-25,NTSC:0-30)
	unsigned int iCodeRate;		//����, unsigned int����,��λΪK
	BYTE iEncodePolicy;			//������� ˵���� 0��֡�����ȣ�1����������
	BYTE iEncodeWay;			//���뷽ʽ	˵���� 0�������࣬1����֡��
	//BYTE bReserve[2];			//����
	BYTE bStreamFlag;			//�������أ�0=�رգ�1=�� ����������Ϊ1�������ã��̶�Ϊ�򿪣�
	BYTE bReserve;				//����
	unsigned int uiGOP;			//GOP��unsigned int����
	unsigned int uiGOP_M;		//GOP_M, unsigned int����,��ʾGOP����Ϸ�ʽ
	unsigned int uiBasicQP;		//����QP��unsigned int����
	unsigned int uiMaxQP;		//���QP��unsigned int����
	unsigned int uiCodeRateType;//��������	˵����0=CBR, 1=VBR
	unsigned int uiMaxCodeRate;	//�������˵��������VBRģʽ����Ч����λK
	unsigned int uiMinCodeRate;	//��С����	˵��������VBRģʽ����Ч����λK
	short int uiResolutionH;	//�ֱ��ʣ�H��
	short int uiResolutionV;	//�ֱ��ʣ�V��
}ENCODERPARAM;

//������OSD����
typedef struct tagOsdParam
{
	short int iSlot;
	BYTE bChannel;				//ͨ���ţ�1��4�����ݰ忨���Ͷ���
	BYTE bStreamNo;				//�����ţ�1��4
	unsigned int uiFontColor;	//OSD������ɫ(0=��ɫ,1=��ɫ)

	unsigned int uiTimeX;
	unsigned int uiTimeY;
	unsigned int uiWordX;
	unsigned int uiWordY;
	char szWord[64];
	BYTE bShowTime;//�Ƿ���ʾʱ�� ��0-hiden ��1-show��
	BYTE bShowWord;//�Ƿ���ʾ���� ��0-hiden ��1-show��
	BYTE bReserve[2];
}OSDPARAM;

//������ʱ������
typedef struct tagTimeParam
{
	short int iSlot;
	short int iChannel;
	BYTE bYear;
	BYTE bMonth;
	BYTE bDay;
	BYTE bWeek;
	BYTE bHour;
	BYTE bMinute;
	BYTE bSecond;
	BYTE bReserve;
}TIMEPARAM;


//��̨��������
typedef struct tagPtzParam
{
	short int iSlot;
	short int iChannel;
	BYTE bPtzType;
	BYTE bPresetNo;
	BYTE bSpeed;
}PTZPARAM;


//����������
typedef struct tagDecoderParam
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
	//	BYTE bReserve[2];
}DECODERPARAM;

//����ģʽ
typedef struct tagWorkModeParam
{
	short int iSlot;		//�忨�ţ�1��16
	short int iChannel;		//ͨ���ţ�1��8�����ݰ忨���Ͷ���
	int uiWorkMode;			//����ģʽ  0=��������1=��������2=�������
}WORKMODEPARAM;


// ��Ƶ��ʧ/�ڵ�������Ϣ
typedef struct tagVmux6kVideoAlarm
{
	short int iSlot;		//�忨�ţ�1��16
	short int iChannel;		//ͨ���ţ�1��4�����ݰ忨���Ͷ���
	BYTE bAlarmStatus0;		//����״̬����n ��bitλ���ڵ�n·����ͨ����Ƶ��ʧ���������� 0=��������1=����
	BYTE bAlarmStatus1;		//����״̬����n ��bitλ���ڵ�n·����ͨ����Ƶ�ڵ����������� 0=��������1=����
	BYTE bReserve[2];		//Reserved,��0
}VMUX6KVIDEOALARM;	


//�豸��������
typedef struct tagRebootParam
{
	short int iSlot;
	short int iChannel;
}REBOOTPARAM;


//�޸��� ��ȡ������Ϣ,��ʼ��������,������������,��������,��� ��׼��/������ ������
//���޸�ֻ���Vbox6671��Vaux6167��Vbox400��Vbox1400�ͺ���Ч
//����������Ϣ
typedef struct tagStreamCtl
{
	short int iSlot;
	BYTE bChannel;	//ͨ���ţ�1��4�����ݰ忨���Ͷ���
	BYTE bStreamNo;	//�����ţ�1��4
	int iHeadFlag;  //�������ԣ�1 = ��׼��,2 = ������
	int iProtocol;	/* 1=TCP, 2=UDP,3=MUTITYCAST*/
	int iPeerIP;
	int iPeerPort;
}STREAMCTL;

//�������ͶԶ���Ϣ
typedef struct tagClientCtl
{
	int iHeadFlag;  //�������ԣ�1 = ��׼��,2 = ������
	int iProtocol;	/* 1=TCP, 2=UDP,3=MUTITYCAST*/
	int iPeerIP;
	int iPeerPort;
}CLIENTCTL;

//////

#pragma  pack()

enum MsgGroup
{
	SYSTEM_PROTOCOL = 0, //ϵͳ��Э�飨catalog = 0��
	CONFIG_PROTOCOL,	//�豸����Э�飨catalog = 1��
	ENCODER_PROTOCOL,	//�������Э�飨catalog = 2��
	ALARM_PROTOCOL,		//�澯Э�飨catalog = 3��
	STREAM_PROTOCOL,	//ý�������Ϳ���Э�飨catalog = 4��
	CAMCONTROL_PROTOCOL	//��̨��������Э��
};

enum NetProtocol
{
	NET_TCP = 1,
	NET_UDP,
	NET_MULTICAST,
	NET_URL,
	NET_CLOSE,
};

#endif



