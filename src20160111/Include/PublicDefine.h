#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <stdint.h>
#include <time.h>

#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>



#define LOGCONTENTMAXSIZE 2048

//extern char* GetTimeNow(char* buff);


#define BASE_TL_PRIVATE			192/* 192-255 are private */

#define TL_DEV_REG_RW 			_IOWR('V', BASE_TL_PRIVATE + 9, unsigned int)
#define TL_PW_VALID				_IOW('V', BASE_TL_PRIVATE + 10, unsigned char)
#define TL_AUDIO_SWITCH			_IOW('V', BASE_TL_PRIVATE + 12, unsigned int)
#define TL_RS485_CTL			_IOW('V', BASE_TL_PRIVATE + 13, unsigned int)
#define TL_GET_FORMAT			_IOWR('V', BASE_TL_PRIVATE + 14, unsigned int)
#define TL_KEYPAD_CTL			_IOW('V', BASE_TL_PRIVATE + 15, unsigned int)
#define TL_ALARM_OUT			_IOW('V', BASE_TL_PRIVATE + 16, unsigned int)
#define TL_BUZZER_CTL			_IOW('V', BASE_TL_PRIVATE + 17, unsigned int)
#define TL_ALARM_IN				_IOWR('V', BASE_TL_PRIVATE + 18, unsigned int)
#define TL_SET_HARDWARE_TYPE	_IOWR('V', BASE_TL_PRIVATE + 19, int)
#define TL_SATA_STATUS		    _IOWR('V', BASE_TL_PRIVATE + 20, int)
#define TL_SCREEN_CTL			_IOW('V', BASE_TL_PRIVATE + 21, unsigned int)
#define TL_POWER_CTL			_IOW('V', BASE_TL_PRIVATE + 22, unsigned int)
#define TL_DVR_RST			    _IOWR('V', BASE_TL_PRIVATE + 23, int)

enum ePTZAction
{
	PTZ_STOP = 0x00,				//00H��ֹͣ
	PTZ_RIGHT,						//01H������ת
	PTZ_LEFT,						//02H������ת
	PTZ_UP,							//03H������ת
	PTZ_DONW,						//04H������ת
	PTZ_RIGHTUP,					//05H��������ת
	PTZ_RIGHTDOWN,					//06H��������ת
	PTZ_LEFTUP,						//07H��������ת
	PTZ_LEFTDOWN,					//08H��������ת
	PTZ_ZOOMIN,						//09H���Ŵ�
	PTZ_ZOOMOUT,					//0AH����С
	PTZ_NEAR,						//0BH��������
	PTZ_FAR,						//0CH��Զ����
	PTZ_LBIG,						//0DH����Ȧ��
	PTZ_LSMALL,						//0EH����ȦС
	PTZ_SETPOSITION,				//0FH������Ԥ��λ
	PTZ_GETPOSITION,				//10H����ȡԤ��λ
	PTZ_OPEN,						//11H�����ؿ�
	PTZ_CLOSE,						//12H�����ع�
	PTZ_WASH,						//13H��WASH
	PTZ_WIPE,						//14H��WIPE
	PTZ_CLEARPRESET,				//15H��Clear Preset
	PTZ_SHUTTER,					//16H������
	PTZ_PLUS,						//17H������
	PTZ_TV_PTZ_LIGHT,				//18H��TV_PTZ_LIGHT;
	PTZ_TV_PTZ_MENU,				//19H��TV_PTZ_MENU ;
	PTZ_CCTV_PTZ_RESET,				//1AH��CCTV_PTZ_RESET;
	PTZ_CCTV_PTZ_CHROMAADJUST,		//1BH��CCTV_PTZ_CHROMAADJUST;
	PTZ_CCTV_PTZ_SATURATIONADJUST,	//1CH��CCTV_PTZ_SATURATIONADJUST;
	PTZ_CCTV_PTZ_CONTRASTADJUST,	//1DH��CCTV_PTZ_CONTRASTADJUST;
	PTZ_SETAUX,						//1EH��SETAUX ������չ����
	PTZ_CLEARAUX,					//1FH��CLEARAUX ֹͣ��չ����
	PTZ_VALIDATE,					//20H��������֤
	PTZ_VALIDATESTOP,				//21H������������֤
	PTZ_SET_ZERO_POSITION,			//22H��������ʼ�Ƕ�ԭ��
	PTZ_SET_PAN_POSITION,			//23H������ˮƽ�Ƕȷ�λ
	PTZ_SET_TILT_POSITION,			//24H�����ô�ֱ�Ƕȷ�λ
	PTZ_SET_ZOOM_POSITION,			//25H�����þ�ͷ���ŽǶȷ�λ
	PTZ_QUERY_PAN_POSITION,			//26H����ѯˮƽ�Ƕȷ�λ
	PTZ_QUERY_TILT_POSITION,		//27H����ѯ��ֱ�Ƕȷ�λ
	PTZ_QUERY_ZOOM_POSITION,		//28H����ѯ��ͷ���ŽǶȷ�λ
	PTZ_GAMMA,		                //29H������GAMMAֵ
    PTZ_BRIGHTNESS,		            //30H����������ֵ
    PTZ_SHARPNESS,					//31H-   �������
    PTZ_SETGARRISON, 				//32H-   ����
    PTZ_CLEARALARM,                  //33H-   �������
};

enum eDeviceType
{
	DeviceModel_Encoder = 0x0,
	DeviceModel_HDEncoder = 0x1,
	DeviceModel_Decoder = 0x2,
};

extern int g_nMaxEncChn;
extern int g_nMaxViChn;
extern int g_nDeviceType;

extern char g_szSipServerIp[256];
extern char g_szSipServerId[256];
extern char g_szLocalIp[50];

#define MAXDECCHANN 4
#define MAXDISPLAYNUM 3
#define MAXENCCHAN 100
#define MAXALARMINCHN 4
#define MAXALARMOUTCHN 1

#define MAXNET_COUNT  3
#define MAXOSD_COUNT 7
#define MAXVI_COUNT 16




#define	PELCOPAN_RIGHT	0x02
#define	PELCOPAN_LEFT	0x04
#define	PELCOTILT_UP	0x08
#define	PELCOTILT_DOWN	0x10

#define	PELCO_ZOOMOUT	0x40
#define	PELCO_ZOOMIN	0x20


#define DEV_WD "/dev/watchdog"
#define DEV_RS485 "/dev/ttyAMA2"
#define DEV_TL9800 "/dev/tl_R9800" 

enum StreamCastType
{
	StreamCastType_Multicast = 0x01,
	StreamCastType_Unicast = 0x02,
	StreamCastType_TcpServer = 0x03,
	StreamCastType_TcpClient = 0x04, 
	StreamCastType_Callback = 0x05,
	StreamCastType_RTSP_OVER_RTP = 0x06,
	StreamCastType_MulticastTcp = 0x07,
};


#pragma pack (push,1)

typedef struct _EncNetParam
{
	int iEnable; //0: disable 1: enable
	int iIndex;
	int iNetType; //0: udp 1: tcp 2: rtsp 3: RTP
	char szPlayAddress[256];
	int iPlayPort;
	int iMux; //0: es 1: ps 2: ts 
}EncNetParam;

typedef struct _EncNetSettings
{
	int iChannelNo;
	EncNetParam net[MAXNET_COUNT];
}EncNetSettings;

typedef struct _OsdParam
{	
	int iEnable;
	int iX;
	int iY;
	int iType; //0: date 1: time  2: date and time 3: content 4: name
	char szOsd[256];
}EncOsdParam;


typedef struct _EncOsdSettings
{
	int iChannelNo; 
	char szId[256];
	int iFont; //0: 16���� 1: 32
	EncOsdParam osd[MAXOSD_COUNT]; 
}EncOsdSettings;

typedef struct __EncoderParam
{
	int iProfile; // 0: baseline 1: main profile 2: high profile default: 2
	int iBitRate;    // KB   < 10 * 1024   default: 1500 
	int iFPS;  // 1~30    default: 25
 	int iResolution; //0: QCIF, 1: CIF,  2:D1, 3:720P, 4:1080P  default: 2 
	int iCbr;  //0: cbr, 1: vbr, 2: fqp default:0 	
	int iGop; //default: 10
	int iIQP;//default:20
	int iPQP; //default: 23
	int iMinQP; //default: 24
	int iMaxQP;  //default: 32
	int iAudioEnable; //0: enable 1: disable
	int iAudioFormat; //1:G711A 2:G711u   
	int iAudioRate; //8000 12000 11025 16000 22050 2400 32000 44100  48000
	int iBits; //0: 8bits 1: 16bits 2: 32bits
}EncoderParam;

enum MD_OSD_SHOW_MODE // ���Խ��л����
{
    MD_OSD_SHOW_NONE = 0,  // ����ʾ
    MD_OSD_SHOW_INFO = 1,  // ������Ϣ
    MD_OSD_SHOW_MONID = 2, // ������ID
};
enum MD_DISPLAY_MODE
{
    MD_DISPLAY_MODE_MESS = 0, // ��ͬ��ʾͨ��������ͬ����ͨ��
    MD_DISPLAY_MODE_1MUX = 1,  // ������ʾͨ������ͬһ������ͨ����������ʾ
    MD_DISPLAY_MODE_4MUX = 2, // 4������ʾ
    MD_DISPLAY_MODE_9MUX=3,  // 9������ʾ
    MD_DISPLAY_MODE_16MUX = 4, // 16������ʾ
};

enum DEC_OSD_POSXY
{
    DEC_OSD_X = 1, // X
    DEC_OSD_Y = 2, // Y
};
enum DEC_OSD_COLOR
{
    DEC_OSD_COLOR_WHITE = 0xffff,// ��ɫ
    DEC_OSD_COLOR_BLACK = 0x8000,// ��ɫ
    DEC_OSD_COLOR_RED = 0xfc00,// ��ɫ
    DEC_OSD_COLOR_GREEN = 0x83e0, // ��ɫ
    DEC_OSD_COLOR_BLUE = 0x801f, // ��ɫ
    DEC_OSD_COLOR_YELLOW = 0xffe0, // ��ɫ
    DEC_OSD_COLOR_PURPLE = 0xb00c, // ��ɫ
    DEC_OSD_COLOR_CYAN = 0x83ff, // ��ɫ
    DEC_OSD_COLOR_MAGENTA = 0xfc1f, // ���
};

enum OsdType
{
	OsdType_Date, 
	OsdType_Time, 
	OsdType_DateAndTime, 
	OsdType_Content,
	OsdType_Name,
};

enum ResolutionType
{
	ResolutionType_QCIF, 
	ResolutionType_CIF, 
	ResolutionType_D1, 
	ResolutionType_720P, 
	ResolutionType_1080P
};

enum MuxType
{
	MuxType_ES = 0x00, 
	MuxType_PS, 
	MuxType_TS
};

enum NetType
{
	NetType_UDP = 0x00, 
	NetType_TCP, 
	NetType_RTSP,
};

typedef struct _AlarmInSetting
{
	int iEnable;
	char szId[100];
	int iNormalStatus; //0: always open 1: always close
	int iTrigerAlarmOut; // triger alarm out.  -1: disable   >=0 : alarm out channel no.
	int iStableTime;   // stable time. second    1~600 
	int iOutDelay; //output delay time second.   0-300
}AlarmInSetting;

typedef struct _AlarmOutSetting
{
	int iEnable;  //
	char szId[100];
	int iNormalStatus; //0: open 1: close; 
	int iStatus; //  0: open 1: close;
}AlarmOutSetting;

typedef struct _AlarmSettings
{
	AlarmInSetting alarmin[MAXALARMINCHN];
	AlarmOutSetting alarmout[MAXALARMOUTCHN];
}AlarmSettings;

typedef struct _PtzSetting
{
	int iPtzType; //0: pelcoD
	int iAddressCode;
}PtzSetting;

typedef struct _PtzPortSetting
{
	int iBaude; // 0:2400 1:4800(ȱʡ) 2:9600 3:19200 4:38400 5:51200 
	int iDataBits; //0: 5 1:6 2:7 3:8(ȱʡ)
	int iParity;   //0:��(ȱʡ) 1:��У�� 2: żУ��
	int iStopBits; //0:1(ȱʡ) 1:1.5 2:2
}PtzPortSetting;

typedef struct _ForwardportSetting
{
	int iEnable; // ʹ�ܣ� 0-��1-��
	int iType; //0-udp��1-tcp
	int iPort;   //�˿�
}ForwardportSetting;

typedef struct _EncSetting
{
	EncOsdSettings osd;
	EncNetSettings net;
	EncoderParam enc;
	int iEnable;
	char szName[256];
	char szDeviceId[256];
}EncSetting;

typedef struct _EncSettings
{
	int iChannelCount;
	EncSetting encset[MAXENCCHAN];
	PtzSetting ptz[MAXENCCHAN/2];
	PtzPortSetting ptzport;
	AlarmSettings alarm;
}EncSettings;

#pragma pack (pop)


//��ȡʱ���ַ���
inline char* GetDateTimeNowUs(char* buff)
{
	struct timeval tv;
	struct tm stm;
	struct tm stmres;
	gettimeofday(&tv,NULL);
	stm = *(localtime_r(&tv.tv_sec,&stmres));
	sprintf(buff, "%04d-%02d-%02d %02d:%02d:%02d:%06ld",
		stm.tm_year + 1900, stm.tm_mon + 1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec, tv.tv_usec);
	return buff;
}

//��ȡʱ���ַ���
inline char* GetDateTimeNow(char* buff)
{
	struct timeval tv;
	struct tm stm;
	struct tm stmres;
	gettimeofday(&tv,NULL);
	stm = *(localtime_r(&tv.tv_sec,&stmres));
	sprintf(buff, "%04d-%02d-%02d %02d:%02d:%02d",
		stm.tm_year + 1900, stm.tm_mon + 1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec);
	return buff;
}

inline void myMsleep(unsigned int mstime) //������λ: ����
{
	   struct timeval tv;
	   tv.tv_sec = mstime/1000;
	   tv.tv_usec = mstime%1000 * 1000;
	   select(0,NULL, NULL, NULL, &tv);
}

//��ȡʱ���ַ���
inline char* GetTimeNowOnly(char* buff)
{
	struct timeval tv;
	struct tm stm;
	struct tm stmres;
	gettimeofday(&tv,NULL);
	stm = *(localtime_r(&tv.tv_sec,&stmres));
	sprintf(buff, "%02d:%02d:%02d",
		stm.tm_hour, stm.tm_min, stm.tm_sec);
	return buff;
}


//��ȡʱ���ַ���
inline char* GetDateNowOnly(char* buff)
{
	struct timeval tv;
	struct tm stm;
	struct tm stmres;
	gettimeofday(&tv,NULL);
	stm = *(localtime_r(&tv.tv_sec,&stmres));
	sprintf(buff, "%04d-%02d-%02d",
		stm.tm_year + 1900, stm.tm_mon + 1, stm.tm_mday );
	return buff;
}

inline void TruncateLogFile()
{
    static int iTime = time(NULL);
    int iTimeNow = time(NULL);
    if((iTimeNow - iTime)<10)
    {
        return;
    }
    iTime = iTimeNow;
    //printf("\n###################### TruncateLogFile called!\n\n");
    long MAX_LOG_SIZE = 20*1024*1024;//����log��󳤶�
    long CLR_LOG_SIZE = 12*1024*1024;//�ü�����
    char* log_file_path = "/tmp/declog";
    int fd = -1;
    void* start = NULL;
    long lFileSize = 0;
    char* szBuff = NULL;
    struct stat sb;
    fd = open(log_file_path, O_RDWR);
    if(fd < 0 )
    {
        printf("\n######################TruncateLogFile: fd <0\n\n");
        return;
    }
    fstat(fd, &sb);
    lFileSize = sb.st_size;
    if(lFileSize < MAX_LOG_SIZE)
    {
        printf("\n###################### Log file size = %d, no truncated!\n\n", lFileSize);
        close(fd);
        return;
    }
    printf("\n###################### Log file size = %d, need to be truncated!\n", lFileSize);
    start = mmap(NULL, lFileSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(start == MAP_FAILED)
    {
        close(fd);
        printf("\n###################### start mmap failed!\n\n");
        if(lFileSize > MAX_LOG_SIZE)
        {
            goto GOTO_ERR1;
        }
        return;
    }
    // �ü��ļ�
    szBuff = new char[lFileSize-CLR_LOG_SIZE];
    if(! szBuff)
    {
        munmap(start, lFileSize);
        close(fd);
        goto GOTO_ERR1;
    }
    memcpy(szBuff, (char*)start+CLR_LOG_SIZE, lFileSize-CLR_LOG_SIZE);
    munmap(start, lFileSize);
    close(fd);
    
GOTO_ERR1:
    // �������LOG����
    FILE *logfile = fopen( log_file_path, "w" );
    if(logfile)
    {
        if(szBuff)
        {
            fwrite(szBuff, 1, lFileSize-CLR_LOG_SIZE, logfile);
            fflush(logfile);
        }
        fclose(logfile);
    }
    delete [] szBuff;
}


//��־��ԭ�ӣ�ֻ���ն˴�ӡ����������־����ӿڴ���
#ifndef _DEBUG_
#define _DEBUG_(...) \
	do {\
		char szTime[32]={0};\
		GetDateTimeNowUs(szTime);\
		char _log_szBuf[LOGCONTENTMAXSIZE]={0};\
		snprintf(_log_szBuf, sizeof(_log_szBuf),__VA_ARGS__);\
		printf("[_DEBUG_] [%s %s %s:%d %s] %s\n",szTime,MODULENAME,__FILE__,__LINE__,__FUNCTION__,_log_szBuf);\
		if(1) \
        { \
    		TruncateLogFile(); \
    		FILE *logfile; \
    		logfile = fopen( "/tmp/declog", "a+" );\
    		if ( logfile != NULL ) \
    		{\
    			fprintf( logfile, "[_DEBUG_] [%s %s %s:%d %s] %s\n",szTime,MODULENAME,__FILE__,__LINE__,__FUNCTION__,_log_szBuf);\
    			fclose( logfile ); \
    		}\
        } \
	} while(0)
#endif





