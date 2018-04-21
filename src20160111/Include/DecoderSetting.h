#ifndef DECODER_SETTING_H
#define  DECODER_SETTING_H


#include "DWConfig.h"
#include "FlashRW.h"
#include "define.h"
#include "Facility.h"
#include "IniFile.h"
#include "ConfigMtdFile.h"
#include "dwstype.h"
#include "sample_comm.h"
#include "PublicDefine.h"
#include "mdrtc.h"

class CDecoderSetting
{
public:
    CDecoderSetting();
    ~CDecoderSetting();
    long Init();
    // ��ȡ����ͨ��URL��iChn-����ͨ���ţ�szUrl-�����ȡ��URL(�������ͷ��ڴ�)
    // ʧ�ܷ���-1���ɹ�����0��������ȡ��URL������szUrl��
    long GetDynamicDecUrl(int iChn, char*& szUrl);
    // 
    // ��ȡNTP��Ϣ
    long GetNtpInfo(struNtpInfo* ntp);
    // ��ȡͨ������
    long GetChannelCount(int iMode, int &iCount);
    // �����ڻص��еĺ���
    // �û���¼����֤
    static long UserLogin(char* szUserName, char* szPassword, void* param);
    //��ȡ�û���Ϣ���������
    static long GetUserInfo(struUserInfo* &pUserInfo, int &iUserCount, void* param);
    //��ȡ������OSD��Ϣ
    static long GetDecOsdInfo(struDecOsdInfo* pDecOsdInfo, void *param);
    static long SetDecOsdInfo(struDecOsdInfo* pDecOsdInfo, void *param);
    //��ȡ������OSD��Ϣ---WEB
    static long GetDecOsdInfoWeb(struDecOsdInfo* pDecOsdInfo, void *param);
    static long SetDecOsdInfoWeb(struDecOsdInfo* pDecOsdInfo, void *param);
    // �޸��û���Ϣ
    static long EditUserInfo(const struUserInfo* pUserInfo, void* param);
    static long GetNetworkInfo(struNetworkInfo* pNI, void* param);
    static long SetNetworkInfo(const struNetworkInfo* pNI, void* param);
    // ��ȡ������SIP��������Ϣ
    static long GetSipServer(struSipServer* pSS, void* param);
    static long SetSipServer(const char* szBuf, void* param);
    static long UpgradeDevice(const char* szData, unsigned long ulLen, void* param);
    static long GetDeviceMac(char *szMAC, void* param);
    static long SetDeviceMac(const char *szMAC, void* param);
    // ��ȡ�豸�ͺ�
    static long GetDeviceModel(int& iModel, void* param);
	static long SetDeviceModel(int iModel, void* param);
    // ��ȡ�豸���к�
    static long GetDeviceSN(char* szSN, void* param);
    // �����豸���к�
    static long SetDeviceSN(const char* szSN, void* param);
    // ��ȡ�豸��������汾��
    static long GetDeviceVer(char* szVer, void* param);
    static long GetDevBuildtime(char* szTime, void* param);
    // ��ȡ�����ý���ͨ����Ϣ
    static long GetDecChannelInfo(struDecChannelInfo* pDCI, void* param);
    static long SetDecChannelInfo(const struDecChannelInfo* pDCI, void* param);
    // ��ȡ�����ý���ֱ���
    static long GetResolution(struDisplayResolution* pDR, void* param);
    static long SetResolution(const struDisplayResolution* pDR, void* param);
    // ��ȡ�����ö�̬����URL
    static long GetDynamicDecUrl(struDynamicDecUrl* pDDU, void* param);
    static long SetDynamicDecUrl(const struDynamicDecUrl* pDDU, void* param);
    static long ResetParam(int iType, void* param);
    static long GetHostname(char* szHostname, void* param);
    static long SetHostname(const char* szHostname, void* param);
    static long SetSysTime(const char* szTime, void* param);
    static long GetSysTime(char* szTime, void* param);
    // ��ȡ������NTP������IP��ַ
    static long GetNtp(char* szBuf, void* param);
    static long SetNtp(const char* szBuf, void* param);
    // OSD

    // ��ȡͨ������
    static long GetChannelCount(int iMode, int &iCount, void* param);

	void SetCB_DDU( SetDynamicDecUrlCB_T callback, void *pCBObj );
    void SetCB_OsdShow( DecOsdShowCB_T callback, void *pCBObj );
	
	void SetCB_Resolution( SetResolutionCB_T callback, void *pCBObj );

	
	long ReadDynamicDecUrl(int iChn, char*& szUrl);
  	long ReadDynamicDecUrl(int iChn, struDynamicDecUrl* pDDU);

    // ����
    long SetNetIp(char* ip, char* netmask);
    long SetGateway(char* gw);
    long GetGateway(char* gw);
private:
    long CopyFile(const char* szSrcFile, const char* szDstFile);
    long MoveFile(const char* szSrcFile, const char* szDstFile);
    long WriteCfgFileToCfgMtd(char* szFile=NULL);
	long WriteCfgFileToCfgMtd5(char* szFile=NULL);
    long check_auth(const char* szUserName, const char* szPassword);
    long load_userinfo();
    static void* ThNtpWorker(void* param);
	static void* ThWriteMtdWorker(void* param);
    long NtpSetTime(struNtpInfo *ntp);
    long SetResolution(const struDisplayResolution* pDR);
private: 
    CDWConfig* m_pDWConfig;
    CWebDecoder* m_pWebDecoder;
    CFlashRW*  m_pFlashRW;
    CConfigMtdFile* m_pConfigMtdFile;
    long m_lConfigMtdLen;
	SetDynamicDecUrlCB_T m_cbSetDDU;
	SetResolutionCB_T m_cbSetResolution;

    DecOsdShowCB_T m_cbSetOsdShow;

	char m_szStr1[1024];
	void *m_pDDUCaller;
    struUserInfo* m_pUserInfo;
    int m_iUserCount;
    struNtpInfo m_objNtpInfo;
    pthread_t m_thNtpWorker;
    bool m_bNtpWorkerExit;
    pthread_mutex_t m_ntpMutexLock;

	pthread_mutex_t m_mtdlock;
	bool m_bCfgFileChanged;// �����ļ��Ƿ����
	bool m_bWriteMtdNow;// �Ƿ�����д�����÷���
	bool m_bWriteMtdExit;
    pthread_t m_thWriteMtdWorker;
};


#endif

