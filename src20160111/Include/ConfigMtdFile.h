#ifndef _CONFIG_MTD_FILE_H
#define _CONFIG_MTD_FILE_H


#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mtd/mtd-user.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <libgen.h>
#include "define.h"
#include "Facility.h"
#include "IniFile.h"
#include "dwstype.h"

#include "PublicDefine.h"


#pragma pack(1)

/*
    д��config mtd�������ļ��ṹ����(������FileHeaderͷ����FlashRW.h)

    |---------------------------|        -->��ʼ�������ļ�ͷ
    |----- MtdFileHeaderT ------|
    |___________________________|
    |                           |        -->��һ�������ļ�ͷ
    |----- ConfigFileHeaderT ---|
    |                           |
    |----- ConfigFile#01 -------|        -->��һ�������ļ�����
    |___________________________|
    |                           |
    |----- ConfigFIleHeaderT ---|        -->�ڶ����ļ�
    |                           |
    |----- ConfigFile#02 -------|
    |___________________________|        -->��N���ļ�
    |                           |            
    |          .......          |
    |___________________________|

*/
typedef struct _mtd_file_header
{
	char sync[8]; //="MFHEADER"
    int count; // �ļ�����
    //unsigned int length;// �����ļ�����
}MtdFileHeaderT;

typedef struct _config_file_header
{
	char sync[8]; //="CFHEADER"
	int size; // �ļ�����
	char name[64]; // �ļ����ƣ�������·��
}ConfigFileHeaderT;

#define MTD_FILE_HEADER_SYNC "MFHEADER"
#define CFG_FILE_HEADER_SYNC "CFHEADER"

class CConfigMtdFile
{
public:
    CConfigMtdFile();
    ~CConfigMtdFile();

    /*
     * ������ļ�{szFilenameList}���ӳ�һ���ļ�{szDestFilename}����д��config mtd
     */
    long JoinConfigFiles(const char* szDestFilename, int type=CFG_FILE_ALL);
	long JoinConfigFilesToMtd5(const char* szDestFilename, int type=CFG_FILE_ALL);
    long JoinConfigFiles(const char* szDestFilename, char** szFilenameList, \
                             unsigned int uiItemCount);

    /*
     * ������config mtd�������ļ�{szMtdFilename}���ɶ���ļ�{szFilenameList}(����·��)
     * �������ļ������{szDestDir}Ŀ¼��
     */
    long ExtracConfigFiles(const char* szMtdFilename, const char*szDestDir, \
                                 char** &szFilenameList, unsigned int &uiItemCount);

    long GetConfigFileList(char** &szFilenameList, unsigned int &uiItemCount, int iType=CFG_FILE_ALL);
    long GetConfigFileListToMtd5(char** &szFilenameList, unsigned int &uiItemCount, int iType=CFG_FILE_ALL);
     
private:
    //��ȡ�ļ���С
    long GetFileSize( const char *szFileName );

private:
    
};

#pragma pack()


#endif //_CONFIG_MTD_FILE_H


