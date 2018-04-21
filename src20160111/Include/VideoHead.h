#ifndef VIDEOHEAD_H_
#define VIDEOHEAD_H_

#pragma pack(1)

enum VodPlayStatus
{
	VODPLAYSTATUS_PLAY = 0x01,
	VODPLAYSTATUS_PAUSE = 0x02,
	VODPLAYSTATUS_STOP = 0x03,
	VODPLAYSTATUS_BACKWARD = 0x04,
 	VODPLAYSTATUS_STEP = 0x05,
 	VODPLAYSTATUS_STEPBACK = 0x06,
	VODPLAYSTATUS_EXIT = 0x07,
};


enum enumCodecType
{
	UNKNOWN_VIDEO	=	0, 
	MPEG2_VIDEO		=	1,
	MPEG4_VIDEO		=	2,
	H264_VIDEO		=	3, 
	JPEG2000_VIDEO	=	4, 
	HK_H264_VIDEO	=   5,
	HD_H246_VIDEO	=	6,
	MJPG_VIDEO		=   7,
	MVR_H264_VIDEO  =   0x0100,
	MVR416_H264_VIDEO = 0x0200,
};


enum FrameType
{
	Unkwn_Frame=0,I_Frame=1,P_Frame=2,B_Frame=3,Param_Sets=4,
};

enum CodecType
{
	Unkwn_Codec=0,MPEG_2=1,MPEG_4=2,H_264=3, JPEG_2000 = 4, HD_H264 = 5, HD_H264_QBOX = 6, MJPEG_QBOX = 7,
};

enum StreamType
{
	Unkwn_Strm = 0, 
	ES_Strm = 1, 
	TS_Strm = 2,
	QBox_Strm = 3,
};

enum GOPStructrue
{
	I = 0, IP = 1, IPB = 2, IPBB = 3, OTHERS = 4, 
};

struct FrameInfo
{
	unsigned char* pStart;
	unsigned int nLen;
	int nFrameType;
	int nCodecType;
	int nStreamType;
	int nFrameRate;
	unsigned long long ullTimeStamp;
	unsigned int uiFrameCount;
	unsigned long long ullPosInFile;
};

struct SequenceInfo
{
	int nCodecType;
	int StreamType;
	int nGOPStructure;
	int nGOPSize;
	int nGOPBFrameSize;
	int nGOPPFrameSize;
	unsigned int uiFrameNum;
	unsigned long long ullStartTime;
	unsigned long long ullEndTime;
	int nFrameRate;
	int nWidth;
	int nHeight;
};

//Э��ͷ
struct StreamHeader
{
    char                szSync[4];      //$$$$
    unsigned long long  uiSize;         //���峤�ȣ���ȥ��ͷ
    unsigned long long	ullTimeStamp;   //Gettimeofday();ʱ���
};

struct VideoES
{
	int PID;
	int type;
	int flags;
};


typedef struct tagElementaryStream
{
	int PID;
	int type;
	int flags;
}ElementaryStream;


struct ADVHeader
{
	unsigned int u32_ADVSync;          //0xFFFFFFF0(0xFFFFFFF1)
	unsigned int u32_Index;            //increment per field
	unsigned char  u8_StreamFormat;      //J2c or JP2 or ADV Raw
	unsigned char  u8_VideoStandard;     //Luminance or Chrominance
	unsigned short u16_Reserved5;        //ignored
	unsigned int u32_PayloadLengthInBytes;    // = Payload Length/4
};

//JPEG2000ͷ 128���ֽ�
struct Jp2kHeader
{
	unsigned int u32_SynCode;          //0x3E3E3E3E
	unsigned short u16_Reserved1;        //ignored
	unsigned char  u8_FrameRate;         //FrameRate
	unsigned char  u8_VFormat;           //Pal or NTSC
	unsigned short u16_Reserved2;        //ignored
	unsigned short u16_HRes;             //Horizontal Resolution
	unsigned short u16_VRes;             //Vertical Resolution
	unsigned int u32_Reserved3;        //ignored
	unsigned int u32_Reserved4;        //ignored
	unsigned int u32_PreLength;        //Length of last component
	unsigned int u32_CurLength;        //length of the following component
	unsigned short u16_CheckOut;         //Check Out
//	UINT64 u64_TimeStamp;        //Time Stamp (us)
	unsigned int u32_sec;
	unsigned int u32_usec;
	unsigned char  u8_Overlay[88];       //char array to overlay

	ADVHeader advHeader;
};

//����264��vissͷ��ժ�Ծɰ����BocomSplt�еĶ��壬�п��ܹ��ڣ���Ҫ��Ӳ����ȷ��vissͨ��ͷ���ĵ�
struct VissHead
{
	char m_Heads[4];		//��־ͷ
	char m_ChipType;		//оƬ����
	char m_PackType;		//��װ��ʽ,A/V,Version
	char m_FrameRate;		//֡��
	char m_CompressType;	//ѹ����ʽ��֡���ͣ�P/N
	short m_DataRate;		//����
	short m_SizeH;			//�ֱ���H
	short m_SizeV;			//�ֱ���V
	short m_PackSize;		//���ݰ�����
	short m_GOP;			//GOP��Ϣ
	short m_PackCount;		//������
	short m_PackID;			//�����
	int m_FrameID;			//֡ID��L��
	short m_Reserve1;		//����1
	short m_Reserve2;		//����2
	short m_CheckSum;		//У��
	unsigned int m_TimeStampSec;	//ʱ������룩
	unsigned int m_TimeStampuSec;	//ʱ�����΢�룩
};

//����ͷ
struct IndexPacket
{
	unsigned short u16_StartCode;
	unsigned int u32_framecounter;
	unsigned long long u64_offset;
	unsigned long long u64_timestamp;		//In ms
};


struct RTPHeader
{
#ifdef RTP_BIG_ENDIAN
	uint8_t version:2;
	uint8_t padding:1;
	uint8_t extension:1;
	uint8_t csrccount:4;
	
	uint8_t marker:1;
	uint8_t payloadtype:7;
#else // little endian
	uint8_t csrccount:4;
	uint8_t extension:1;
	uint8_t padding:1;
	uint8_t version:2;
	
	uint8_t payloadtype:7;
	uint8_t marker:1;
#endif // RTP_BIG_ENDIAN
	
	uint16_t sequencenumber;
	uint32_t timestamp;
	uint32_t ssrc;
};

struct RTPExtensionHeader
{
	uint16_t extid;
	uint16_t length;
};

struct RTPSourceIdentifier
{
	uint32_t ssrc;
};

struct RTCPCommonHeader
{
#ifdef RTP_BIG_ENDIAN
	uint8_t version:2;
	uint8_t padding:1;
	uint8_t count:5;
#else // little endian
	uint8_t count:5;
	uint8_t padding:1;
	uint8_t version:2;
#endif // RTP_BIG_ENDIAN

	uint8_t packettype;
	uint16_t length;
};

struct RTCPSenderReport
{
	uint32_t ntptime_msw;
	uint32_t ntptime_lsw;
	uint32_t rtptimestamp;
	uint32_t packetcount;
	uint32_t octetcount;
};

struct RTCPReceiverReport
{
	uint32_t ssrc; // Identifies about which SSRC's data this report is...
	uint8_t fractionlost;
	uint8_t packetslost[3];
	uint32_t exthighseqnr;
	uint32_t jitter;
	uint32_t lsr;
	uint32_t dlsr;
};

struct RTCPSDESHeader
{
	uint8_t sdesid;
	uint8_t length;
};


#pragma pack()

#endif

