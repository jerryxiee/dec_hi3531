#ifndef DEVUPDATE_PROTOCOL_DEFINE_H_
#define DEVUPDATE_PROTOCOL_DEFINE_H_

#pragma pack (push,1)

#define UP_MULTI_ADDR "224.20.15.210"
#define UP_MULTI_PORT 53541
#define UP_SERVER_PORT 20152
#define UP_VERSION 0x0201
#define UP_HEAD_FLAG "UPGRADE"

typedef struct _UpHead
{
    char flag[8]; // ��־�ֶ�UPGRADE
    unsigned int version; // �汾��0x0201
    unsigned int cmd; // ͨѶָ��
    unsigned int workid; // ��Ϣ��ˮ��
    unsigned int length; // ��Ϣ�峤��(��������Ϣͷ)
    char reserved[40];// Ԥ����0x00
}struUpHead;

enum UpdateCmd
{
    // �鲥�����豸
    UP_CMD_REQ_DETECT_DEVICES=0xf000,
    UP_CMD_RET_DETECT_DEVICES=0xf001,
    
    // �����豸
    UP_MSG_REQ_UPGRADE_DEV=0xf103,
    UP_MSG_RET_UPGRADE_DEV=0xf104,
    // �����豸
    UP_MSG_REQ_REBOOT_DEV=0xf105,
    UP_MSG_RET_REBOOT_DEV=0xf106,
    // ʱ��У׼
    UP_MSG_REQ_SETTIME=0xf107,
    UP_MSG_RET_SETTIME=0xf108,
    
};

enum UP_ERROR_NO
{
    UP_E_SUCCESS = 0x00,
    UP_E_FAILED = 0x01,
    UP_E_INVALID_FILE = 0x02,

};

///////////////////////////////////////////////////////////////////
// �鲥�����豸
typedef struct _UpCmdReqDetectDev
{
    struUpHead head;

}struUpCmdReqDetectDev;

typedef struct _UpDevInfo
{
    char szDevName[16]; // �豸����
    char szSysVersion[32];// �豸�汾
    char szBuildtime[32];// �豸��������
    char szDevIp[32]; // �豸IP *
    char szDevNetmask[32]; // ��������
    char szDevGateway[32]; // ����
    char szDevMac[32]; // MAC��ַ *
    unsigned int iCtlFlag; // �Ƿ����� 1-�ǣ�0-�� *
    char szCtlIp[32]; // ���ӷ�Ip
    char szReserved[40]; // ����

}struUpDevInfo;

typedef struct _UpCmdRetDetectDev
{
    struUpHead head;
    struUpDevInfo body;
}struUpCmdRetDetectDev;

// �����豸
typedef struct _UpReqUpgradeDev
{
    struUpHead head;
    char *szData; // ��������
}struUpReqUpgradeDev;

typedef struct _UpRetUpgradeDev
{
    struUpHead head;
    int iRet;
}struUpRetUpgradeDev;

// �豸����
typedef struct _UpReqRebootDev
{
    struUpHead head;

}struUpReqRebootDev;

typedef struct _UpRetRebootDev
{
    struUpHead head;
    int iRet;
}struUpRetRebootDev;

// ʱ��У׼
typedef struct _UpReqSettime
{
    struUpHead head;
    char szTime[40]; // ʱ��"2015-02-10 18:30:59"
}struUpReqSettime;

typedef struct _UpRetSettime
{
    struUpHead head;
    int iRet;
}struUpRetSettime;


#pragma pack (pop)

#endif //DEVUPDATE_PROTOCOL_DEFINE_H_






