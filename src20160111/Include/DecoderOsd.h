#ifndef _DECODER_OSD_H__
#define _DECODER_OSD_H__

#include "PublicDefine.h"
#include "sample_comm.h"
#include <pthread.h>
#include "TextOnYUV.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <iconv.h>
#include "dwstype.h"
#include <list>
using namespace std;

typedef struct _tagMyOsdInfo
{
    bool bChanged; // �����Ƿ����޸ģ������Ƿ�ˢ��OSD
    int iChnno; // OSD����ͨ���ţ�����(DecodecOsdParam*)pData->channel
    void* pData;
}struMyOsdInfo;

#define WORD_SPACE 0  // osd��ʾ��֮��ļ������
#define DATE_STRING_LEN 64

class CDecoderOsd
{
public:
	CDecoderOsd(struDisplayResolution* pHdmiDR/*HDMI��ʾ�ֱ�����Ϣ*/);
	~CDecoderOsd();	
    // ������ʾ���ģʽ, channelΪ����ͨ����
    void SetOsdShowMode(int mode, int channel);
    // ���chennelͨ����osd��Ϣ��channel=0ʱ�������ͨ��osd��Ϣ
    void ClearOsdShow(int channel);
    void OnOsdShowChanged(int iShow);
	void SetOsdDate(void* pData);
	void OnDisplayResolutionChanged(const struDisplayResolution* pDR);
    void SetOsdInfo(void* pData);
    long SetMonitorId(int chnno, int mid);

private: 
    static void* ThOsdWorker(void* param);
    static void* ThMonitorWorker(void* param);
    long SetDisplayResolution(const struDisplayResolution* pDR);
    char* conv_time(int itime);
    long DisplayOsd(bool bShow);
    long DisplayMonID(bool bShow);
    long RestartOsdDisplay();
    bool IsOsdInfoChanged(void* p1, void* p2);
    long SetOsdColor(int colortype, short& OsdColor, short& OsdEdgeColor);
    long SetOsdChangedFlaged(bool);
    int SetOsdPosXY(unsigned int pos, int xy, int chnno);
    // ���OSD��ʾ
    void ClearOsdDisplay();
private:
	int m_iExitFlag;
    // Dispaly
    struDisplayResolution m_sDR;
    unsigned int m_uiDisplayX;
    unsigned int m_uiDisplayY;
    //osd
    bool m_bOsdWorkerExit;
    pthread_t m_thOsdWorker;
    pthread_t m_thMoniterWorker;
    bool m_bOsdCleared;
    bool m_bMidOsdCleared;
    char **m_szLastDate;
    pthread_mutex_t m_lockOsd;
    pthread_mutex_t m_lockRestartOsdDisplay;
    bool m_bOsdShowFromWeb;
    bool m_bOsdShowMonId;

    CTextOnYuv m_textOnYuv;
    int m_fbfd;
    struct fb_var_screeninfo m_vinfo;
    struct fb_fix_screeninfo m_finfo;
    unsigned long m_screensize;
    char *m_fbp;
    bool m_bFirst;
    bool m_bStartOsdDisplay;
    // ����ͨ��
    int m_iChannelNo;
    //��ʾģʽ
    int m_iMuxMode;
    // multimap: bool--ָʾosd��Ϣ�Ƿ�ı�(ʱ��osd�ı䲻����)��void--ָ��osd��Ϣ�ڴ�
    list<struMyOsdInfo*> m_listOsdDataNow;
    // ͨ�������ļ���������
    int *m_iMonitorIdList;
    //���������Ƿ����ı�
    bool m_bMonitorIdChanged;
};

#endif //_DECODER_OSD_H__












