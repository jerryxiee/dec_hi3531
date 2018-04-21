
#ifndef ST_PROTOCOL_DEFINE_H_
#define ST_PROTOCOL_DEFINE_H_

#pragma pack (push,1)

#define DECODER_TCP_PORT 9000
#define DECODER_UDP_PORT 9000
#define ENCODER_TCP_PORT 9000
#define ENCODER_UDP_PORT 9000
#define KEEPALIVE_INTERVAL 3
#define CMD_VERSION 0x20

typedef struct _StHead
{
    char version; // �汾�� ������00100000 (0x20)
    int length; // ��Ϣ������
    int workid; // ��Ϣ��ˮ��
    char flag; // ��־λ��0x00-ý������ ����-��������
    int cmd; //��Ϣָ����(ǰ�����ֽ�Ϊ0x00)
    unsigned char src[4];//��Ϣ���ͷ����룬IP��ַ
    unsigned char dst[4];//��Ϣ���շ����룬IP��ַ
    char reserved[18];//Ԥ����0
}struStHead;

/************************************
 *
 * �ӿ�4 ������<-->������
 *
 ************************************/
///////////////////////
// ָ����
#define REQ_VC_LOGIN_DECODER 0x0401 //��Ƶ��������¼����������
#define RET_VC_LOGIN_DECODER 0x0402 //��Ƶ��������¼���������
#define REQ_VC_LOGOUT_DECODER 0x0403 //��Ƶ�������ǳ�����������
#define RET_VC_LOGOUT_DECODER 0x0404 //��Ƶ�������ǳ����������
#define REQ_VC_SET_DECODER_PROFILE 0x0405 //��������������
#define RET_VC_SET_DECODER_PROFILE 0x0406 //�������������ý��
#define REQ_VC_GET_DECODER_PROFILE 0x0407 //������������ѯ
#define RET_VC_GET_DECODER_PROFILE 0x0408 //������������ѯ���
#define REQ_VC_GET_DECODER_STATE 0x0409 //����������״̬��ѯ��δ����/���յ���/�����鲥
#define RET_VC_GET_DECODER_STATE 0x040a //����������״̬��ѯ���
#define REQ_VC_SET_UNICAST_ENCODER 0x040b //���ý�������������ĵ�����������
#define RET_VC_SET_UNICAST_ENCODER 0x040c //��������������ĵ�������������
#define REQ_VC_GET_UNICAST_ENCODER 0x040d //��ѯ���������ӵĵ���������
#define RET_VC_GET_UNICAST_ENCODER 0x040e //��ѯ���������ӵĵ������������
#define REQ_VC_LOCK_DECODER_VIDEO 0x040f //���ý�����ͼ������
#define RET_VC_LOCK_DECODER_VIDEO 0x0410 //���ý�����ͼ���������
#define REQ_VC_UNLOCK_DECODER_VIDEO 0x0411 //���ý�����ͼ�����
#define RET_VC_UNLOCK_DECODER_VIDEO 0x0412 //���ý�����ͼ��������
#define REQ_VC_SET_MULTICAST_GROUP 0x0413 //���ý�����������鲥��
#define RET_VC_SET_MULTICAST_GROUP 0x0414 //���ý�����������鲥��ķ��ؽ��
#define REQ_VC_SET_RELAY 0x0415 //���ý�����ת����IP��ַ
#define RET_VC_SET_RELAY 0x0416 //���ý�����ת����IP��ַ�ķ��ؽ��

/////////////////////////////////
//��Ϣ�ṹ��
/////////////////////////////////

//���������������¼���󼰷���
typedef struct _ReqVcLoginDec
{
    unsigned char szVcIp[4];//��Ƶ������IP
    unsigned char szDecIp[4];//������IP
    unsigned char byteLevel;// 0x00, ����Ա���ȼ�
    char szKey[64]; //��¼��Կ��ȫ0x00
}ReqVcLoginDec;
typedef struct _RetVcLoginDec
{
    unsigned char byteRet;//��½��� 0x00ʧ�ܣ�0x01�ɹ�
    unsigned char byteErr;//˵��:0x01:��Ч�豸 0x02:��Ч��Կ 0x03:�ѵ�¼
}RetVcLoginDec;

// ��������������˳���¼����
typedef struct _ReqVcLogoutDec
{
    unsigned char szVcIp[4];//��Ƶ������IP
    unsigned char szDecIp[4];//������IP
}ReqVcLogoutDec;
typedef struct _RetVcLogoutDec
{
    unsigned char byteRet;//��½��� 0x00ʧ�ܣ�0x01�ɹ�
    unsigned char byteErr;//˵��:0x00
}RetVcLogoutDec;

//���������ý�������������
typedef struct _ReqVcSetDecProfile
{
    unsigned char szVcIp[4];//��Ƶ������IP
    unsigned char szDecId[4];//�������豸��ţ�ȫ0x00
    unsigned char szDecIp[4];//�������µ�IP
    unsigned char szDecNetmask[4];//��������������
    unsigned char szDecGateway[4];//����������
    unsigned char szDecNetManagerIp[4];//�����������豸��ַ
    unsigned char szDecTcpPort[2];//�����������˿�TCP
    unsigned char szDecUdpPort[2];//�����������˿�UDP
    unsigned char szDecId2[4];// �������豸��ţ�ȫ0x00
}ReqVcSetDecProfile;
typedef struct _RetVcSetDecProfile
{
    unsigned char byteRet;//��½��� 0x00ʧ�ܣ�0x01�ɹ�
    unsigned char byteErr;//˵��:0x01��֧�ֵ����ԣ�0x02Ȩ�޳�ͻ
}RetVcSetDecProfile;

// ��������ȡ��������������
typedef struct _ReqVcGetDecProfile
{
    unsigned char szVcIp[4];//��Ƶ������IP
    unsigned char szDecIp[4];//������IP
}ReqVcGetDecProfile;
typedef struct _RetVcGetDecProfile
{
    unsigned char szDecIp[4];//�������µ�IP
    unsigned char szDecNetmask[4];//��������������
    unsigned char szDecGateway[4];//����������
    unsigned char szDecNetManagerIp[4];//�����������豸��ַ
    unsigned char szDecMulticastIp[4];//�������鲥��ַ
    unsigned char szDecTcpPort[2];//�����������˿�TCP
    unsigned char szDecUdpPort[2];//�����������˿�UDP
    unsigned char szDecId[4];// �������豸��ţ�ȫ0x00
}RetVcGetDecProfile;

// ��������ѯ����������״̬
typedef struct _ReqVcGetDecState
{
    unsigned char szVcIp[4];//��Ƶ������IP
    unsigned char szDecIp[4];//������IP
}ReqVcGetDecState;
typedef struct _RetVcGetDecState
{
    unsigned char byteState;// 0x00δ���ӱ����� 0x01���յ����� 0x02�����鲥��
}RetVcGetDecState;

//���������ý�������������ĵ�������
typedef struct _ReqVcSetUnicastEnc
{
    unsigned char szVcIp[4];//��Ƶ������IP
    unsigned char szDecIp[4];//������IP
    unsigned char szEncId[4];//��������ţ�ȫ0x00
    unsigned char szEncIp[4];//������IP
    unsigned short usTcpPort;//TCP�˿�
    unsigned short usUdpPort;//udp�˿�
    char szKey[64];//��������½��Կ��ȫ0x00
    unsigned char byteAction;//���Ӻ�����0x00��½�󲻶��� 0x01��½������������
}ReqVcSetUnicastEnc;
typedef struct _RetVcSetUnicastEnc
{
    unsigned char byteRet;//��� 0x00ʧ�ܣ�0x01�ɹ�
    unsigned char byteErr;//˵��:0x00
}RetVcSetUnicastEnc;

//��������ѯ���������ӵĵ���������
typedef struct _ReqVcGetUnicastEnc
{
    unsigned char szVcIp[4];//��Ƶ������IP
    unsigned char szDecIp[4];//������IP
}ReqVcGetUnicastEnc;
typedef struct _RetVcGetUnicastEnc
{
    unsigned char szEncId[4];//������ID��ȫ0x00
    unsigned char szEncIp[4];//������IP
    unsigned char szTcpPort[2];//TCP�˿�
    unsigned char szUdpPort[2];//UDP�˿�
    char szConnectedTime[4];//0x00
}RetVcGetUnicastEnc;

// ���������ý�����ͼ������
typedef struct _ReqVcLockDecVideo
{
    unsigned char szVcIp[4];//��Ƶ������IP
    unsigned char szDecIp[4];//������IP
}ReqVcLockDecVideo;
typedef struct _RetVcLockDecVideo
{
    unsigned char byteRet;//��� 0x00ʧ�ܣ�0x01�ɹ�
    unsigned char byteErr;//˵��:0x01Ȩ�޲���
}RetVcLockDecVideo;

// ���������ý�����ͼ��������
typedef struct _ReqVcUnlockDecVideo
{
    unsigned char szVcIp[4];//��Ƶ������IP
    unsigned char szDecIp[4];//������IP
}ReqVcUnlockDecVideo;
typedef struct _RetVcUnlockDecVideo
{
    unsigned char byteRet;//��� 0x00ʧ�ܣ�0x01�ɹ�
    unsigned char byteErr;//˵��:0x01Ȩ�޲���
}RetVcUnlockDecVideo;

//��������������ƶ����鲥��ַ
typedef struct _ReqVcSetMulticastGroup
{
    unsigned char szMulticastIp[4];//�鲥��ַ�����������ܵ��鲥��ַ
}ReqVcSetMulticastGroup;
typedef struct _RetVcSetMulticastGroup
{
    unsigned char byteRet;//��� 0x00ʧ�ܣ�0x01�ɹ�
    unsigned char byteErr;//˵��:0x00
}RetVcSetMulticastGroup;

// ���ý�����ת���� IP��ַ
typedef struct _ReqVcSetRelay
{
    unsigned char byteType;//ת������ 0x00������0x01�鲥
    unsigned char szIp[4];//ת��IP��ַ
    unsigned char byteStart;//��ʼ/ֹͣ:0x00ֹͣת����0x01��ʼת��
}ReqVcSetRelay;
typedef struct _RetVcSetRelay
{
    unsigned char byteRet;//��� 0x00ʧ�ܣ�0x01�ɹ�
    unsigned char byteErr;//˵��:0x00
}RetVcSetRelay;

/************************************
 *
 * �ӿ�6 ������<-->������
 *
 ************************************/
/////////////////////////
//ָ����
#define REQ_DECODER_LOGIN_ENCODER 0x0601 //��¼������
#define RET_DECODER_LOGIN_ENCODER 0x0602 //��¼����������
#define REQ_DECODER_LOGOUT_ENCODER 0x0603 //�ǳ�������
#define RET_DECODER_LOGOUT_ENCODER 0x0604 //��¼����������
#define REQ_DECODER_CONNECT_ENCODER 0x0605 //����������->���������
#define RET_DECODER_CONNECT_ENCODER 0x0606 //����������->�������������
#define REQ_DECODER_DISCONNECT_ENCODER 0x0607 //���������->���������
#define RET_DECODER_DISCONNECT_ENCODER 0x0608 //���������->�������������
#define MSG_ENCODER_KEEPALIVE_DECODER 0x0609 //�����������ź�

/////////////////////////////////
//��Ϣ�ṹ��
/////////////////////////////////
// ���������͵�½����������
typedef struct _ReqDecLoginEnc
{
    unsigned char szDecIp[4];//������IP
    unsigned char szEncIp[4];//������IP
    unsigned char byteLevel;//����Ա���ȼ���0x00
    char szKey[64];//��½��Կ��ȫ0x00
}ReqDecLoginEnc;
typedef struct _RetDecLoginEnc
{
    unsigned char byteRet;//��� 0x00ʧ�ܣ�0x01�ɹ�
    unsigned char byteErr;//˵��:0x01��Ч�豸��0x02��Ч��Կ��0x03�ѵ�¼
}RetDecLoginEnc;

// ���������͵ǳ�����������
typedef struct _ReqDecLogoutEnc
{
    unsigned char szDecIp[4];//������IP
    unsigned char szEncIp[4];//������IP
}ReqDecLogoutEnc;
typedef struct _RetDecLogoutEnc
{
    unsigned char byteRet;//��� 0x00ʧ�ܣ�0x01�ɹ�
    unsigned char byteErr;//˵��:0x00
}RetDecLogoutEnc;

//����������������ͽ���������->������������ӵ�����
typedef struct _ReqDecConnectEnc
{
    unsigned char szDecIp[4];//������IP
    unsigned char szEncId[4];//���������,0x00
    unsigned char szEncIp[4];//������IP
    unsigned char szUdpPort[2];//UDP�˿�    
}ReqDecConnectEnc;
typedef struct _RetDecConnectEnc
{
    unsigned char byteRet;//��� 0x00ʧ�ܣ�0x01�ɹ�
    unsigned char byteErr;//˵��:0x00
}RetDecConnectEnc;

//����������������Ͳ��������->������������ӵ�����
typedef struct _ReqDecDisconnectEnc
{
    unsigned char szDecIp[4];//������IP
    unsigned char szEncIp[4];//������IP
    unsigned char szVcIp[4];//��Ƶ������IP
}ReqDecDisconnectEnc;
typedef struct _RetDecDisconnectEnc
{
    unsigned char byteRet;//��� 0x00ʧ�ܣ�0x01�ɹ�
    unsigned char byteErr;//˵��:0x00
}RetDecDisconnectEnc;

// ����������������������ź�
typedef struct _MsgEncKeepaliveDec
{
    unsigned char szDecIp[4];//������IP
    unsigned char szEncIp[4];//������IP
}MsgEncKeepaliveDec;


#pragma pack (pop)

#endif //ST_PROTOCOL_DEFINE_H_


