#include "DecoderOsd.h"
#include <arpa/inet.h>
#include <stdio.h>
#include "TextOnYUV.h"
#include "define.h"
#include "mytde.h"
#include "Text2Bmp.h"
#include "MDProtocol.h"
#include "DecoderSetting.h"
#include "mdrtc.h"
#include <stdlib.h>
#include <locale.h>
#include <string.h>

CDecoderOsd::CDecoderOsd(struDisplayResolution* pHdmiDR)
{	
    //Dispaly
    m_iChannelNo = 0;
    m_iMuxMode = MD_DISPLAY_MODE_1MUX;
    m_thOsdWorker = 0;
    m_uiDisplayX = 1920;
    m_uiDisplayY = 1080;
    memset(&m_sDR, 0, sizeof(struDisplayResolution));
    SetDisplayResolution(pHdmiDR);
    // osd
    m_listOsdDataNow.clear();
    m_bStartOsdDisplay=false;
    m_bOsdWorkerExit=false;
    m_iExitFlag = 0;
    m_bOsdShowFromWeb = false;
    m_bOsdShowMonId = false;

    struDecOsdInfo sosdweb;
    CDecoderSetting::GetDecOsdInfoWeb(&sosdweb, NULL);

    if(sosdweb.show & MD_OSD_SHOW_INFO)
        m_bOsdShowFromWeb = true;
    if(sosdweb.show & MD_OSD_SHOW_MONID)
        m_bOsdShowMonId = true;


    m_bOsdCleared = false;
    m_bMidOsdCleared = false;
    m_bFirst = true;
    m_bMonitorIdChanged = false;
    m_iMonitorIdList = new int[MAXDECCHANN];
    memset(m_iMonitorIdList, 0, sizeof(int)*MAXDECCHANN);
    m_szLastDate = new char*[MAXDECCHANN];
    for(int i=0; i<MAXDECCHANN; i++)
    {
        m_szLastDate[i] = new char[DATE_STRING_LEN];
        memset(m_szLastDate[i], 0, DATE_STRING_LEN*sizeof(char));
    }

    pthread_mutex_init(&m_lockOsd, NULL);
    pthread_mutex_init(&m_lockRestartOsdDisplay, NULL);
    m_thMoniterWorker=0;

    m_fbfd = 0;
    m_screensize = 0;
    m_fbp = 0;
    if(m_bOsdShowFromWeb == true)
    {
        RestartOsdDisplay();
    }
	
    pthread_create( &m_thMoniterWorker, NULL, ThMonitorWorker, (void*)this);
}

CDecoderOsd::~CDecoderOsd()
{
    munmap(m_fbp, m_screensize);
    m_fbp = 0;
    close(m_fbfd);
    m_bStartOsdDisplay = false;

	m_iExitFlag = 1;
    m_bOsdWorkerExit = true;

    //_DEBUG_("join m_thMoniterWorker");
    if ( m_thMoniterWorker!= 0 )
	{
		pthread_join( m_thMoniterWorker, NULL );
		m_thMoniterWorker = 0;
	}
    //_DEBUG_("join m_thOsdWorker");
	if ( m_thOsdWorker != 0 )
	{
		pthread_join( m_thOsdWorker, NULL );
		m_thOsdWorker = 0;
	}

    pthread_mutex_destroy( &m_lockOsd );
    pthread_mutex_destroy( &m_lockRestartOsdDisplay);
    
    for(int i=0; i<MAXDECCHANN; i++)
    {
        PTR_DELETE_A(m_szLastDate[i]);
    }
    PTR_DELETE_A(m_szLastDate);

    PTR_DELETE_A(m_iMonitorIdList);

    DecodecOsdParam* ptr = NULL;
    struMyOsdInfo* ptrMyosd = NULL;
    list<struMyOsdInfo*>::iterator  it, itend;
    itend = m_listOsdDataNow.end();
    for (it=m_listOsdDataNow.begin(); it!=itend; ++it)
    {
        ptrMyosd = *it;
        if(ptrMyosd->pData == NULL)
        {
            continue;
        }
        ptr = (DecodecOsdParam*)ptrMyosd->pData;
        if(ptr != NULL)
        {
            delete ptr;
            ptr = NULL;
        }
        delete ptrMyosd;
        ptrMyosd = NULL;
    }
    m_listOsdDataNow.clear();
    
    //_DEBUG_("~CDecoderOsd end");
}

void* CDecoderOsd::ThMonitorWorker(void * param)
{
    CDecoderOsd* pThis = (CDecoderOsd*)param;
    sleep(3);
    while( pThis->m_iExitFlag ==0)
    {
        if(pThis->m_bStartOsdDisplay==false)
        {
            pThis->RestartOsdDisplay();
        }
        sleep(1);
    }
    return NULL;
}
void* CDecoderOsd::ThOsdWorker(void* param)
{
    CDecoderOsd* pThis = (CDecoderOsd*)param;
    while( pThis->m_bOsdWorkerExit == false )
    {
        //
        // OSD��ʾ
        if(pThis->m_bOsdShowFromWeb==false)
        {
            // ���OSD��ʾ
            //_DEBUG_("########### Clear OSD");
            pThis->DisplayOsd(false);
        }
        else if(pThis->m_bOsdShowFromWeb==true)
        {
            // ʹ��OSD��ʾ
            //_DEBUG_("########### SHOW OSD");
            pThis->DisplayOsd(true);
        }
        //
        // ������ID OSD
        if(pThis->m_bOsdShowMonId==false)
        {
            // ��ռ�����ID OSD��ʾ
            //_DEBUG_("########### Clear Monitor OSD");
            pThis->DisplayMonID(false);
        }
        else if(pThis->m_bOsdShowMonId==true)
        {
            // ʹ�ܼ�����ID OSD��ʾ
            //_DEBUG_("########### SHOW Monitor OSD");
            pThis->DisplayMonID(true);
        }
        
        myMsleep(400);
    }
    return NULL;
}

long CDecoderOsd::SetMonitorId(int chnno, int mid)
{
    if(chnno<=0 || chnno>MAXDECCHANN)
    {
        return -1;
    }
    if(m_iMonitorIdList[chnno-1] != mid)
    {
        m_iMonitorIdList[chnno-1] = mid;
        m_bMonitorIdChanged = true;
    }

#if 0 // ��ӡ������Ϣ
    printf("\n###### Monitor ID: \n");
    for(int i=0; i<MAXDECCHANN; i++)
    {
        printf("   CHN_NO: %d, MON_ID: %d\n", i+1, m_iMonitorIdList[i]);
    }
    printf("######\n\n");
#endif
    return 0;
}

void CDecoderOsd::SetOsdInfo(void* pData)
{
    if(pData == NULL)
        return;
    DecodecOsdParam* pTmpData = (DecodecOsdParam*)pData;
#if 0 // ��ӡ������Ϣ
    _DEBUG_("\nOSD: channel=%d, monitor=%d, type=%d, x,y=%d,%d, font=%d, show=%d, color=%d", 
             pTmpData->channel, pTmpData->slot_number, pTmpData->osd, pTmpData->x_time, pTmpData->y_time, 
             pTmpData->x_font, pTmpData->b_showfont, pTmpData->reserved);
    printf("####OSD text: ");
    for(int i=0; i<64; i++)
    {
        if((unsigned char)pTmpData->buffer_content[i])
        {
            if((unsigned char)pTmpData->buffer_content[i]>127 || (unsigned char)pTmpData->buffer_content[i]<32)
            {
                printf("#%02x", pTmpData->buffer_content[i]);
            }
            else
            {
                printf("%c", pTmpData->buffer_content[i]);
            }
        }
    }
    printf("\n\n");
#endif
    // �жϺϷ���
    if(pTmpData->osd < OSD_FONT || pTmpData->osd > OSD_TYPE_MAX)
    {
        _DEBUG_("invalid osd type %d !", pTmpData->osd);
        return;
    }
    bool bOsdInfoChanged = false;
    struMyOsdInfo* ptr = NULL;
    DecodecOsdParam* ptrosd = NULL;
    list<struMyOsdInfo*>::iterator  it, itend;
    
    // ���Ϊʱ��OSD��Ϣ������Ҫ����ʱ��
    if(pTmpData->osd == OSD_TIME)
    {
        SetOsdDate(pData);
    }
    // ����MONITOR ID
    if(m_iMonitorIdList[pTmpData->channel-1] != pTmpData->slot_number)
    {
        m_iMonitorIdList[pTmpData->channel-1] = pTmpData->slot_number;
        m_bMonitorIdChanged = true;
    }
    m_iMonitorIdList[pTmpData->channel-1] = pTmpData->slot_number;

    // �����Ƿ����д�����OSD��Ϣ������osd����    
    pthread_mutex_lock(&m_lockOsd);
    itend = m_listOsdDataNow.end();
    for(it = m_listOsdDataNow.begin(); it != itend; it++)
    {
        ptr = *it;
        ptrosd = (DecodecOsdParam*)ptr->pData;
        if(ptrosd->channel == pTmpData->channel && ptrosd->osd == pTmpData->osd)
        {
            break;
        }
    }
    // δ�ҵ��������
    if(it == itend)
    {
        struMyOsdInfo* pmyosd = new struMyOsdInfo;
        memset(pmyosd, 0, sizeof(struMyOsdInfo));
        DecodecOsdParam* posd = new DecodecOsdParam;
        memset(posd, 0, sizeof(DecodecOsdParam));
        memcpy(posd, pData, sizeof(DecodecOsdParam));
        pmyosd->pData = (void*)posd;
        pmyosd->bChanged = true;
        pmyosd->iChnno = pTmpData->channel;
        m_listOsdDataNow.push_back(pmyosd);
        _DEBUG_("ADD osd=%d", pTmpData->osd);
    }
    else // �ҵ�������������Ƿ��޸�����
    {
         //�ҵ����ж��Ƿ����޸�
         if(IsOsdInfoChanged((void *)ptrosd, (void *)pTmpData) == true)
         {
             bOsdInfoChanged = true;
         }
         if(bOsdInfoChanged == true)
         {
             memcpy(ptrosd, pData, sizeof(DecodecOsdParam));
             ptr->bChanged = true;
             _DEBUG_("CHANGE osd=%d", pTmpData->osd);
         }
         else
         {
            if(ptr->bChanged == true)
            {
                //ptr->bChanged = false;
                //_DEBUG_("CHANGE osd=%d false", pTmpData->osd);
            }
         }
    }
    pthread_mutex_unlock(&m_lockOsd);

    // �ж��Ƿ���Ҫˢ����Ļ, ����OSD��Ϣ���ģ�����Ҫˢ������OSD��ʾ
    if(bOsdInfoChanged)
    {
        DisplayOsd(false);
        DisplayMonID(false);
    }
}

bool CDecoderOsd::IsOsdInfoChanged(void* p1, void* p2)
{
    if(p1 == NULL || p2 == NULL)
        return false;
    DecodecOsdParam* posd1 = (DecodecOsdParam*)p1;
    DecodecOsdParam* posd2 = (DecodecOsdParam*)p2;    

    if( posd1->b_showfont == posd2->b_showfont && 
        posd1->x_time == posd2->x_time && 
        posd1->y_time == posd2->y_time && 
        posd1->reserved == posd2->reserved && // ��ɫ
        posd1->x_font == posd2->x_font
       )
    {
        //ʱ��osd�ַ������ݲ����ж�
        if(posd1->osd != OSD_TIME)
        {
            if(strcmp(posd1->buffer_content, posd2->buffer_content)==0)
            {
                return false;
            }
            return true;
        }        
        return false;
    }
    return true;
}

void CDecoderOsd::SetOsdShowMode(int mode, int channel)
{
    m_iMuxMode= mode;
    m_iChannelNo = channel;
    m_bMonitorIdChanged = true;
    ClearOsdDisplay();
}

void CDecoderOsd::ClearOsdShow(int channel)
{
    //��channel==0ʱ���������ͨ��OSD��Ϣ
    // ���channel����ͨ����OSD��Ϣ
    DecodecOsdParam* ptr = NULL;
    struMyOsdInfo* ptrMyosd = NULL;
    list<struMyOsdInfo*>::iterator  it, it2, itend;
    pthread_mutex_lock(&m_lockOsd);
    itend = m_listOsdDataNow.end();
    for (it=m_listOsdDataNow.begin(); it!=itend; )
    {
        it2 = it++;
        ptrMyosd = *it2;
        if(ptrMyosd->iChnno == channel || channel == 0)
        {
            ptr = (DecodecOsdParam*)ptrMyosd->pData;
            if(ptr != NULL)
            {
                delete ptr;
                ptr = NULL;
            }
            delete ptrMyosd;
            //ptrMyosd = NULL;
            m_listOsdDataNow.erase(it2);
        }
    }

    pthread_mutex_unlock(&m_lockOsd);
    // ����ʾOSD
    DisplayOsd(false);
    DisplayMonID(false);
}

void CDecoderOsd::OnOsdShowChanged(int iShow)
{
    if(iShow & MD_OSD_SHOW_INFO)
        m_bOsdShowFromWeb = true;
    else
        m_bOsdShowFromWeb = false;

    
    if(iShow & MD_OSD_SHOW_MONID)
        m_bOsdShowMonId = true;
    else
        m_bOsdShowMonId = false;

    m_bFirst = true;
}

void CDecoderOsd::SetOsdDate(void* pData)
{
    if(pData == NULL) return;
    DecodecOsdParam* ptr = (DecodecOsdParam*)pData;
    char *p = ptr->buffer_content;
    int iServerTime = (int)((p[0]&0xffffffff)<<24) + (int)((p[1]&0xffffffff)<<16) + (int)((p[2]&0xffffffff)<<8) + (int)p[3];
    int iLocalRtcTime  = (int)CRtc::read_rtc(0);
    int iLocalSysTime = 0;
    //_DEBUG_("Server Time: %d", iServerTime);
    //_DEBUG_("Local RTC Time: %d", iLocalRtcTime);
    //������ֵ������±���ʱ��
    int iValue = 3;
    if( abs(iServerTime-iLocalRtcTime) > iValue )
    {
        //���ñ���ʱ��
        CRtc::write_rtc((time_t)(iServerTime), 0);
        // ����ϵͳʱ��
        char szCmd[200] = {0};
        sprintf(szCmd, "/bin/date -s \"%s\"", conv_time(iServerTime));
        system(szCmd);
        goto GOTO_RET1;
    }
    iLocalSysTime = (int)time(NULL);

    // ����Ӳ��ʱ���ϵͳʱ�䣬��Ӳ��ʱ��Ϊ׼
    if( abs(iLocalSysTime-iLocalRtcTime) > iValue )
    {
        // ����ϵͳʱ��
        char szCmd[200] = {0};
        sprintf(szCmd, "/bin/date -s \"%s\"", conv_time(iLocalRtcTime));
        system(szCmd);
        goto GOTO_RET1;
    }
GOTO_RET1:
    return;
}

long CDecoderOsd::SetDisplayResolution(const struDisplayResolution* pDR)
{
    /*
    0-δ֪��δ����
    1-1920x1080
    2-800x600
    3-1024x768
    4-1280x1024
    5-1366x768
    6-1440x900
    7-1280x800
    */
    unsigned int uiDisplayX=0;
    unsigned int uiDisplayY=0;
    switch(pDR->resolution)
    {
        case 1:
            uiDisplayX = 1920;
            uiDisplayY = 1080;
            break;
        case 2:
            uiDisplayX = 800;
            uiDisplayY = 600;
            break;
        case 3:
            uiDisplayX = 1024;
            uiDisplayY = 768;
            break;
        case 4:
            uiDisplayX = 1280;
            uiDisplayY = 1024;
            break;
        case 5:
            uiDisplayX = 1366;
            uiDisplayY = 768;
            break;
        case 6:
            uiDisplayX = 1440;
            uiDisplayY = 900;
            break;
        case 7:
            uiDisplayX = 1280;
            uiDisplayY = 800;
            break;
        default:
            _DEBUG_("unkonwn resolution %d", pDR->resolution);
            return -1;
            break;
    }
    if(m_uiDisplayX==uiDisplayX && m_uiDisplayY==uiDisplayY)
    {
        return -1;
    }
    else
    {
        m_uiDisplayX = uiDisplayX;
        m_uiDisplayY = uiDisplayY;
        memcpy(&m_sDR, pDR, sizeof(struDisplayResolution));
    }
    return 0;
}

void CDecoderOsd::OnDisplayResolutionChanged(const struDisplayResolution* pDR)
{
    if(pDR == NULL)
    {
        return;
    }
    // hdmi�ֱ����޸ģ�����������OSD��ʾ
    if(pDR->displaymode == 3)
    {
        SetDisplayResolution(pDR);
    }
    RestartOsdDisplay();
}

char* CDecoderOsd::conv_time(int itime)

{
    static char szBuff[80] = {0};
    memset(szBuff, 0, 80);
    struct tm mytm;
    localtime_r((time_t*)&itime, &mytm);
    sprintf(szBuff, "%d-%02d-%02d %02d:%02d:%02d", mytm.tm_year+1900, \
           mytm.tm_mon+1, mytm.tm_mday, mytm.tm_hour, \
           mytm.tm_min, mytm.tm_sec);
    return szBuff;
}

void CDecoderOsd::ClearOsdDisplay()
{
    m_bOsdCleared = true;
    m_bMidOsdCleared = true;
    memset(m_fbp, 0, m_screensize);
    m_bFirst = true;
    for(int i=0; i<MAXDECCHANN; i++)
    {
        memset(m_szLastDate[i], 0 , DATE_STRING_LEN*sizeof(char));
    }
}

long CDecoderOsd::DisplayOsd(bool bShow)
{
    // ���OSD��ʾ
    if(bShow == false || m_iMuxMode == MD_DISPLAY_MODE_MESS)
    {
        if(m_bOsdCleared == false)
        {
            ClearOsdDisplay();
        }
        //_DEBUG_("not show osd, return");
        return 0;
    }

    // ʱ���ַ���
    char szDate[64] = {0};
    // �������б�
    unsigned char* pCodeList[64];
    // �����Ƿ�Ϊ�ַ�����
    int pHalfList[64]={0};
    // ָʾ�������������ĳ���
    int nCount = 64;
    // ��������ֵ
    int iRet = 0;
    //ѭ�������
    int m,i,j;
    // ʱ���ַ�����ȣ����ڼ���ַ�
    unsigned int uiDateStrWidth = 0;
    // �����ַ�����ȣ����ڼ���ַ�
    unsigned int uiTextStrWidth = 0;
    // OSD�߶�
    unsigned int uiOsdHeight = 0; 
    // ��ɫ
    short sOsdColor = OSD_COLOR_WHITE;
    short sOsdEdgeColor = OSD_COLOR_BLACK;
    //
    // OSD����
    // ��������С
    int iFont = 64;
    // �����ȣ�������ַ���ΪiFont��һ�룬����ΪiFont
    int iFontWidth = 0;
    // OSD��ʾ��Ҫ�����ĳ���
    int iClearLen = 0;
    //����
    unsigned int uiX = 0;
    unsigned int uiY = 0;
    // ��������
    unsigned int iIndex = 0;
    unsigned int iStride = m_vinfo.xres * 2;
    int iLocalTime = 0;
    list<struMyOsdInfo*>::iterator  it, itend;
    int iPosXY = 0;
    DecodecOsdParam* ptr = NULL;
    struMyOsdInfo* ptrMyosd = NULL;

    if( m_bFirst == true && m_bOsdCleared==false)
    {
        ClearOsdDisplay();
    }
    
    pthread_mutex_lock(&m_lockOsd);
    
    itend = m_listOsdDataNow.end();
    for (it=m_listOsdDataNow.begin(); it!=itend; ++it)
    {
        ptrMyosd = *it;
        ptr = (DecodecOsdParam*)ptrMyosd->pData;
        if(m_iMuxMode == MD_DISPLAY_MODE_1MUX)
        {
            if(ptrMyosd->iChnno != m_iChannelNo || ptr->b_showfont == 0)
            {
                continue;
            }
        }
        else
        {
            if(ptr->b_showfont == 0)
            {
                continue;
            }
        }
        
        if(!m_bFirst)
        {
            if(ptrMyosd->bChanged == false && ptr->osd != OSD_TIME)
            {
                // ����δ�ı����OSDΪ����ʾ�����޸�OSD��ʾ
                continue;
            }
        }
        else
        {
            if(ptrMyosd->bChanged == false)
            {
                ptrMyosd->bChanged = true;
            }
        }
        if(m_iMuxMode == MD_DISPLAY_MODE_1MUX) // 1����ģʽ
        {
            // �����С
            iFont = ptr->x_font;
            if(iFont>=64)
                iFont = 64;
            else if(iFont>=32)
                iFont = 32;
            else
                iFont = 16;
        }
        else if(m_iMuxMode == MD_DISPLAY_MODE_4MUX 
                 || 
                 m_iMuxMode == MD_DISPLAY_MODE_9MUX
                 ) // 4/9����ģʽ
        {
            // �����С
            iFont = ptr->x_font;
            if(iFont>=32)
                iFont = 32;
            else
                iFont = 16;
        }
        else if(m_iMuxMode == MD_DISPLAY_MODE_16MUX) //16����ģʽ
        {
            iFont = 16;
        }
        else
        {
            iFont = 16;
        }
        
        // ����
        iPosXY = SetOsdPosXY(ptr->x_time, DEC_OSD_X, ptrMyosd->iChnno);
        if(iPosXY < 0)
        {
            continue;
        }
        uiX = (unsigned int)iPosXY;

        iPosXY = SetOsdPosXY(ptr->y_time, DEC_OSD_Y, ptrMyosd->iChnno);
        if(iPosXY < 0)
        {
            continue;
        }
        uiY = (unsigned int)iPosXY;
        
        // �������塢�����ɫ
        SetOsdColor(ptr->reserved, sOsdColor, sOsdEdgeColor);

        //
        // �޸�osd��ʾ
        //
        // ���жϸ߶��Ƿ񳬹����ֱ���
        uiOsdHeight = uiY + iFont;
        if( uiOsdHeight > m_uiDisplayY)
        {
            _DEBUG_("ERROR: OSD[%d] Height %d > %d Screen Height!", ptr->osd, uiOsdHeight, m_uiDisplayY);
            iRet = -1;
            //goto GOTO_ERR2;
            continue;
        }
        // ���⡢������ʾ
        if(ptr->osd!=OSD_TIME && ptrMyosd->bChanged==true)
        {
            iRet = m_textOnYuv.GetCodeList( ptr->buffer_content, iFont, &pCodeList[0], &pHalfList[0], nCount);
            if(iRet != 0)
            {
                iRet = -1;
                continue;
            }

            // ��ʼ��ӡ���ֲ��ֵ������¼����ַ������
            uiTextStrWidth = uiX;
            //memset(m_fbp + uiY*iStride, 0, iStride*iFont);
            for(m=0; m<nCount; m++)
            {
                if(pHalfList[m]==0)
                {
                    iFontWidth = iFont;
                }
                else
                {
                    iFontWidth = iFont / 2;
                }
                if((uiTextStrWidth+iFontWidth+WORD_SPACE) > m_uiDisplayX)
                {
                    iRet = -1;
                    _DEBUG_("Error: OSD[Text] Width %d > %d Screen Width[word index=%d]", (uiTextStrWidth+iFontWidth+WORD_SPACE), m_uiDisplayX, m+1);
                    //goto GOTO_ERR1;
                    break;;
                }
                for(j=0; j<iFont; j++)
                {
                    for(i=0; i<iFontWidth; i++)
                    {
                        if(pCodeList[m][j*iFontWidth+i]==255)
                        {
                            //����
                            *((unsigned short *)(m_fbp + (j+uiY)*iStride + uiTextStrWidth*2 + i*2)) = sOsdColor;
                        }
                        else if(pCodeList[m][j*iFontWidth+i]==128)
                        {
                            //���
                            *((unsigned short *)(m_fbp + (j+uiY)*iStride + uiTextStrWidth*2 + i*2)) = sOsdEdgeColor;
                        }
                    }
                }
                if(m != nCount-1)
                {
                    uiTextStrWidth = uiTextStrWidth + iFontWidth + WORD_SPACE;
                }
            }
            // ������ʾ���޸�OSD״̬Ϊδ�޸�
            ptrMyosd->bChanged = false;
        }
        // ʱ����ʾ
        else if(ptr->osd == OSD_TIME)
        {
            // ��ȡ��ǰʱ����ַ���
            iLocalTime = (int)time(NULL);//(int)CRtc::read_rtc(0);
            sprintf(szDate, "%s", conv_time(iLocalTime));
            if(strcmp(m_szLastDate[ptr->channel-1], szDate) == 0)
            {
                continue;
            }
            // ��ȡʱ���ַ��������б�
            iRet = m_textOnYuv.GetCodeList( szDate, iFont, &pCodeList[0], &pHalfList[0], nCount);
            if(iRet != 0)
            {
                iRet = -1;
                //goto GOTO_ERR2;
                continue;
            }
            uiDateStrWidth = uiX;
            // ��ȡ���λ�ȡ��ʱ���ַ������ϴε�ʱ���ַ����Ĳ�ͬ����λ����������ȷ����ͬ�ַ����Ŀ��
            for(i=0; i<nCount; i++)
            {
                if(pHalfList[i]==0)
                {
                    iFontWidth = iFont;
                }
                else
                {
                    iFontWidth = iFont / 2;
                }
                // ȷ��δ�ı���ַ������
                if(i != 0)
                {
                    uiDateStrWidth = uiDateStrWidth + iFontWidth + WORD_SPACE;
                }
            
                if(m_szLastDate[ptr->channel-1][i] != szDate[i])
                {
                    iIndex = i;
                    break;
                }
            }
            for(m=iIndex; m<nCount; m++)
            {
                if(pHalfList[m]==0)
                {
                    iFontWidth = iFont;
                }
                else
                {
                    iFontWidth = iFont / 2;
                }
                if( (uiDateStrWidth+iFontWidth+WORD_SPACE) > m_uiDisplayX)
                {
                    iRet = -1;
                    _DEBUG_("Error: OSD[Date] Width=%d > %d Screen Width[word index=%d]", (uiDateStrWidth+iFontWidth+WORD_SPACE), m_uiDisplayX, m+1);
                    //goto GOTO_ERR1;
                    break;
                }
            
                for(j=0; j<iFont; j++)
                {
                    // �����ͬ���ݵ���ʾ
                    iClearLen = ((nCount-iIndex) * (iFontWidth+WORD_SPACE) - WORD_SPACE) * 2;
                    memset(m_fbp + (j+uiY)*iStride + uiDateStrWidth*2, 0, iClearLen);
                    for(i=0; i<iFontWidth; i++)
                    {
                        if(pCodeList[m][j*iFontWidth+i]==255)
                        {
                            //��ɫ����
                            *((unsigned short *)(m_fbp + (j+uiY)*iStride + uiDateStrWidth*2 + i*2)) = sOsdColor;
                        }
                        else if(pCodeList[m][j*iFontWidth+i]==128)
                        {
                            //��ɫ���
                            *((unsigned short *)(m_fbp + (j+uiY)*iStride + uiDateStrWidth*2 + i*2)) = sOsdEdgeColor;
                        }
                    }
                }
                if(m != nCount-1)
                {
                    uiDateStrWidth = uiDateStrWidth + iFontWidth + WORD_SPACE;
                }
            }
            sprintf(m_szLastDate[ptr->channel-1], "%s", szDate);
        }
        else
        {
            continue;
        }
    }
    // ��ʾ������ID����ʾλ��Ϊ���½�

GOTO_ERR1:
    m_bFirst = false;
    m_bOsdCleared = false;

GOTO_ERR2:
    pthread_mutex_unlock(&m_lockOsd);

    return iRet;
}
long CDecoderOsd::DisplayMonID(bool bShow)
{
    //_DEBUG_("#####   mode=%d", m_iMuxMode);
    // ��ʾ������ID����ʾλ��Ϊ���½�
    ///////////////////////////////////////////////////////
    if(bShow == false || m_iMuxMode == MD_DISPLAY_MODE_MESS)
    {
        if(m_bMidOsdCleared == false)
        {
            ClearOsdDisplay();
        }
        return 0;
    }
    if(m_bMidOsdCleared == false && m_bMonitorIdChanged == false)
    {
        //δ�����ı䣬�������´�ӡ�������ţ�ֱ�ӷ���
        return 0;
    }
    // monitor id buff
    char szMonIdBuff[32]={0};
    // �������б�
    unsigned char* pCodeList[64];
    // �����Ƿ�Ϊ�ַ�����
    int pHalfList[64]={0};
    // ָʾ�������������ĳ���
    int nCount = 64;
    // ��������ֵ
    int iRet = 0;
    //ѭ�������
    int m,i,j=0;
    // �����ַ�����ȣ����ڼ���ַ�
    unsigned int uiTextStrWidth = 0;
    // OSD�߶�
    unsigned int uiOsdHeight = 0;
    // ��ɫ
    short sOsdColor = OSD_COLOR_WHITE;
    short sOsdEdgeColor = OSD_COLOR_BLACK;
    //
    // OSD����
    // ��������С
    int iFont = 64;
    // �����ȣ�������ַ���ΪiFont��һ�룬����ΪiFont
    int iFontWidth = 0;
    //����
    unsigned int uiX = 0;
    unsigned int uiY = 0;
    unsigned int tmpX,tmpY;
    unsigned int iStride = m_vinfo.xres * 2;
    int iPosXY = 0;
    int iForLoopCnt = 0;
    int chnno = 0;
    int iMonIdStrWidth = 0;

    switch(m_iMuxMode)
    {
        case MD_DISPLAY_MODE_1MUX:
            chnno = m_iChannelNo-1;
            iFont = 64;
            iForLoopCnt = m_iChannelNo;
            iMonIdStrWidth = iFont/2*10; // 4��ǰ׺"MON ", �����6λ���ļ�������
            tmpX = m_uiDisplayX-iMonIdStrWidth;
            tmpY = m_uiDisplayY-iFont*2;
            break;
        case MD_DISPLAY_MODE_4MUX:
            iFont = 32;
            iForLoopCnt = 4;
            iMonIdStrWidth = iFont*10;
            tmpX = m_uiDisplayX-iMonIdStrWidth;
            tmpY = m_uiDisplayY-iFont*3;
            break;
        case MD_DISPLAY_MODE_9MUX:
            iFont = 32;
            iForLoopCnt = 9;
            iMonIdStrWidth = iFont*14;
            tmpX = m_uiDisplayX-iMonIdStrWidth;
            tmpY = m_uiDisplayY-iFont*4;
            break;
        case MD_DISPLAY_MODE_16MUX:
            iFont = 16;
            iForLoopCnt = 16;
            iMonIdStrWidth = iFont*18;
            tmpX = m_uiDisplayX-iMonIdStrWidth;
            tmpY = m_uiDisplayY-iFont*5;
            break;
        default:
            iForLoopCnt = 0;
            return -1;
    };

    for ( ; chnno<iForLoopCnt; chnno++)
    {
        // ����
        iPosXY = SetOsdPosXY(tmpX, DEC_OSD_X, chnno+1);
        if(iPosXY < 0)
        {
            continue;
        }
        uiX = (unsigned int)iPosXY;
        
        iPosXY = SetOsdPosXY(tmpY, DEC_OSD_Y, chnno+1);
        if(iPosXY < 0)
        {
            continue;
        }
        uiY = (unsigned int)iPosXY;
        // �������塢�����ɫ
        SetOsdColor(OSD_COLOR_WHITE, sOsdColor, sOsdEdgeColor);
        
        //
        // �޸�osd��ʾ
        //
        // ���жϸ߶��Ƿ񳬹����ֱ���
        uiOsdHeight = uiY + iFont;
        if( uiOsdHeight > m_uiDisplayY)
        {
            _DEBUG_("ERROR: OSD[MonitorID-%d] Height %d > %d Screen Height!", chnno+1, uiOsdHeight, m_uiDisplayY);
            iRet = -1;
            continue;
        }
        if(m_iMonitorIdList[chnno] <= 0)
        {
            continue;
        }
        sprintf(szMonIdBuff, "MON %d", m_iMonitorIdList[chnno]);
        iRet = m_textOnYuv.GetCodeList( szMonIdBuff, iFont, &pCodeList[0], &pHalfList[0], nCount);
        if(iRet != 0)
        {
            iRet = -1;
            continue;
        }
        
        // ��ʼ��ӡ���ֲ��ֵ������¼����ַ������
        uiTextStrWidth = uiX;
        //memset(m_fbp + uiY*iStride, 0, iStride*iFont);
        for(m=0; m<nCount; m++)
        {
            if(pHalfList[m]==0)
            {
                iFontWidth = iFont;
            }
            else
            {
                iFontWidth = iFont / 2;
            }
            if((uiTextStrWidth+iFontWidth+WORD_SPACE) > m_uiDisplayX)
            {
                iRet = -1;
                _DEBUG_("Error: OSD[MonitorID-%d] Width %d > %d Screen Width[word index=%d]", chnno+1, \
                            (uiTextStrWidth+iFontWidth+WORD_SPACE), m_uiDisplayX, m+1);
                break;
            }
            for(j=0; j<iFont; j++)
            {
                //memset(m_fbp + (j+uiY)*iStride + uiX*2, 0, iClearLen*2);
                for(i=0; i<iFontWidth; i++)
                {
                    if(pCodeList[m][j*iFontWidth+i]==255)
                    {
                        //����
                        *((unsigned short *)(m_fbp + (j+uiY)*iStride + uiTextStrWidth*2 + i*2)) = sOsdColor;
                    }
                    else if(pCodeList[m][j*iFontWidth+i]==128)
                    {
                        //���
                        *((unsigned short *)(m_fbp + (j+uiY)*iStride + uiTextStrWidth*2 + i*2)) = sOsdEdgeColor;
                    }
                }
            }
            if(m != nCount-1)
            {
                uiTextStrWidth = uiTextStrWidth + iFontWidth + WORD_SPACE;
            }
        }
        m_bMidOsdCleared = false;
        m_bMonitorIdChanged = false;
    }

    return iRet;
}

long CDecoderOsd::RestartOsdDisplay()
{
    pthread_mutex_lock(&m_lockRestartOsdDisplay);
    m_bOsdWorkerExit = true;
    if ( m_thOsdWorker != 0 )
    {
        pthread_join( m_thOsdWorker, NULL );
        m_thOsdWorker = 0;
    }
    long lRet = -1;
    struct fb_bitfield stR32 = {10, 5, 0};
    struct fb_bitfield stG32 = {5, 5, 0};
    struct fb_bitfield stB32 = {0, 5, 0};
    struct fb_bitfield stA32 = {15, 1, 0};
    //
    // ��֡�����豸
    if(m_fbfd)
    {
        // ����
        ClearOsdDisplay();
        
        munmap(m_fbp, m_screensize);
        m_fbp = 0;
        close(m_fbfd);
        m_fbfd=0;
        m_bStartOsdDisplay = false;
    }
    m_fbfd = open("/dev/fb1", O_RDWR);  
    if( m_fbfd<= 0 )
    {
        _DEBUG_("open /dev/fb1 error!\n");
        goto GOTO_ERR1;
    }
    if( ioctl(m_fbfd, FBIOGET_VSCREENINFO, &m_vinfo) )
    {  
        // ��ȡ��Ļ�ɱ����
        _DEBUG_("error: ioctl FBIOGET_VSCREENINFO\n");
        goto GOTO_ERR1;
    }

    m_vinfo.xres_virtual = m_uiDisplayX;
    m_vinfo.yres_virtual = m_uiDisplayY;
    m_vinfo.xres = m_uiDisplayX;
    m_vinfo.yres = m_uiDisplayY;
    m_vinfo.activate  		= FB_ACTIVATE_NOW;
    m_vinfo.bits_per_pixel	= 16;
    m_vinfo.xoffset = 0;
    m_vinfo.yoffset = 0;
    m_vinfo.red   = stR32;
    m_vinfo.green = stG32;
    m_vinfo.blue  = stB32;
    m_vinfo.transp = stA32;

    if (ioctl(m_fbfd, FBIOPUT_VSCREENINFO, &m_vinfo) < 0)
    {
        _DEBUG_("process frame buffer device error:FBIOPUT_VSCREENINFO\n");
        goto GOTO_ERR1;
    }
    if (ioctl(m_fbfd, FBIOGET_FSCREENINFO, &m_finfo) < 0)
    {
        _DEBUG_("error: ioctl FBIOGET_FSCREENINFO\n");
        goto GOTO_ERR1;
    }

    // ��ӡ��Ļ�ɱ����
    //printf("SCREEN: %d x %d, %dbpp\n", m_vinfo.xres, m_vinfo.yres, m_vinfo.bits_per_pixel);
    // �������ֽڴ�С
    //m_screensize = m_vinfo.xres * m_vinfo.yres * m_vinfo.bits_per_pixel / 2;
    m_screensize = m_finfo.smem_len;
    // ӳ��
    m_fbp = (char *)mmap(0, m_screensize, PROT_READ|PROT_WRITE, MAP_SHARED, m_fbfd, 0);
    if( (int)m_fbp == -1 )
    {
        _DEBUG_("error: mmap\n");
        goto GOTO_ERR1;
    }
    lRet = 0;
GOTO_ERR1:
    if(lRet != 0)
    {
        if(m_fbfd)
        {
            close(m_fbfd);
            m_fbfd = 0;
        }
        m_bStartOsdDisplay = false;
        pthread_mutex_unlock(&m_lockRestartOsdDisplay);
        return lRet;
    }
    // ����
    ClearOsdDisplay();
    m_bOsdWorkerExit = false;
    if(pthread_create(&m_thOsdWorker, NULL, ThOsdWorker, (void*)this) == 0)
    {
        m_bStartOsdDisplay = true;
    }
    else
    {
        m_thOsdWorker = 0;
    }
    pthread_mutex_unlock(&m_lockRestartOsdDisplay);

    return 0;
}

long CDecoderOsd::SetOsdColor(int colortype, short& OsdColor, short& OsdEdgeColor)
{
    OsdColor = DEC_OSD_COLOR_WHITE;
    OsdEdgeColor = DEC_OSD_COLOR_BLACK;
    switch (colortype)
    {
        case OSD_COLOR_WHITE:
            OsdColor = DEC_OSD_COLOR_WHITE;
            OsdEdgeColor = DEC_OSD_COLOR_BLACK;
            break;
        case OSD_COLOR_BLACK:
            OsdColor = DEC_OSD_COLOR_BLACK;
            OsdEdgeColor = DEC_OSD_COLOR_WHITE;
            break;
        case OSD_COLOR_RED:
            OsdColor = DEC_OSD_COLOR_RED;
            OsdEdgeColor = DEC_OSD_COLOR_WHITE;
            break;
        case OSD_COLOR_GREEN:
            OsdColor = DEC_OSD_COLOR_GREEN;
            OsdEdgeColor = DEC_OSD_COLOR_WHITE;
            break;
        case OSD_COLOR_BLUE:
            OsdColor = DEC_OSD_COLOR_BLUE;
            OsdEdgeColor = DEC_OSD_COLOR_WHITE;
            break;
        case OSD_COLOR_YELLOW:
            OsdColor = DEC_OSD_COLOR_YELLOW;
            OsdEdgeColor = DEC_OSD_COLOR_BLACK;
            break;
        case OSD_COLOR_PURPLE:
            OsdColor = DEC_OSD_COLOR_PURPLE;
            OsdEdgeColor = DEC_OSD_COLOR_WHITE;
            break;
        case OSD_COLOR_CYAN:
            OsdColor = DEC_OSD_COLOR_CYAN;
            OsdEdgeColor = DEC_OSD_COLOR_BLACK;
            break;
        case OSD_COLOR_MAGENTA:
            OsdColor = DEC_OSD_COLOR_MAGENTA;
            OsdEdgeColor = DEC_OSD_COLOR_WHITE;
            break;
        default:
            break;
    };
    /*
    if((unsigned short)OsdColor == DEC_OSD_COLOR_WHITE)
    {
        OsdEdgeColor = DEC_OSD_COLOR_BLACK;
    }
    else
    {
        OsdEdgeColor = DEC_OSD_COLOR_WHITE;
    }
    */
    return 0;
}

int CDecoderOsd::SetOsdPosXY(unsigned int pos, int xy, int chnno)
{
    int iValue = -1;
    unsigned int uiSize = 0;
    int mux = m_iMuxMode;
    
    if(xy == DEC_OSD_X)
    {
        uiSize = m_uiDisplayX;
    }
    else if(xy == DEC_OSD_Y)
    {
        uiSize = m_uiDisplayY;
    }
    else
    {
        goto GOTO_ERR1;
    }
    
    if(mux == MD_DISPLAY_MODE_1MUX)
    {
        iValue = pos;
    }
    else if(mux == MD_DISPLAY_MODE_4MUX)
    {
        unsigned int pos1 = pos / 2;
        unsigned int pos2 = uiSize/2 + pos/2;
        switch(chnno)
        {
          case 1:
              iValue = pos1;
              break;
          case 2:
              iValue = (xy==DEC_OSD_X) ? pos2 : pos1;
              break;
          case 3:
              iValue = (xy==DEC_OSD_X) ? pos1 : pos2;
              break;
          case 4:
              iValue = pos2;
              break;
          default :
              goto GOTO_ERR1;
        };
    }
    else if(mux == MD_DISPLAY_MODE_9MUX)
    {
        unsigned int pos1 = pos / 3;
        unsigned int pos2 = uiSize/3 + pos/3;
        unsigned int pos3 = (uiSize/3) * 2 + pos/3;
        switch(chnno)
        {
          case 1:
              iValue = pos1;
              break;
          case 2:
              iValue = (xy==DEC_OSD_X) ? pos2 : pos1;
              break;
          case 3:
              iValue = (xy==DEC_OSD_X) ? pos3 : pos1;
              break;
          case 4:
              iValue = (xy==DEC_OSD_X) ? pos1 : pos2;
              break;
          case 5:
              iValue = pos2;
              break;
          case 6:
              iValue = (xy==DEC_OSD_X) ? pos3 : pos2;
              break;
          case 7:
              iValue = (xy==DEC_OSD_X) ? pos1 : pos3;
              break;
          case 8:
              iValue = (xy==DEC_OSD_X) ? pos2 : pos3;
              break;
          case 9:
              iValue = pos3;
              break;
          default :
              goto GOTO_ERR1;
        };
    }
    else if(mux == MD_DISPLAY_MODE_16MUX)
    {
        unsigned int pos1 = pos / 4;
        unsigned int pos2 = uiSize/4 + pos/4;
        unsigned int pos3 = (uiSize/4) * 2 + pos/4;
        unsigned int pos4 = (uiSize/4) * 3 + pos/4;
        switch(chnno)
        {
          case 1:
              iValue = pos1;
              break;
          case 2:
              iValue = (xy==DEC_OSD_X) ? pos2 : pos1;
              break;
          case 3:
              iValue = (xy==DEC_OSD_X) ? pos3 : pos1;
              break;
          case 4:
              iValue = (xy==DEC_OSD_X) ? pos4 : pos1;
              break;
          case 5:
              iValue = (xy==DEC_OSD_X) ? pos1 : pos2;
              break;
          case 6:
              iValue = pos2;
              break;
          case 7:
              iValue = (xy==DEC_OSD_X) ? pos3 : pos2;
              break;
          case 8:
              iValue = (xy==DEC_OSD_X) ? pos4 : pos2;
              break;
          case 9:
              iValue = (xy==DEC_OSD_X) ? pos1 : pos3;
              break;
          case 10:
              iValue = (xy==DEC_OSD_X) ? pos2 : pos3;
              break;
          case 11:
              iValue = pos3;
              break;
          case 12:
              iValue = (xy==DEC_OSD_X) ? pos4 : pos3;
              break;
          case 13:
              iValue = (xy==DEC_OSD_X) ? pos1 : pos4;
              break;
          case 14:
              iValue = (xy==DEC_OSD_X) ? pos2 : pos4;
              break;
          case 15:
              iValue = (xy==DEC_OSD_X) ? pos3 : pos4;
              break;
          case 16:
              iValue = pos4;
              break;
          default :
              goto GOTO_ERR1;
        };
    }
    
GOTO_ERR1:
    return iValue;
}




