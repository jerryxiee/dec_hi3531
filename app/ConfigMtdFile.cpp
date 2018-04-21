#include "ConfigMtdFile.h"
CConfigMtdFile::CConfigMtdFile()
{

}

CConfigMtdFile::~CConfigMtdFile()
{

}
long CConfigMtdFile::JoinConfigFilesToMtd5(const char* szDestFilename, int type)
{
    char** szFilenameList = NULL;
    unsigned int uiFileCount = 0;
    long lRet = 0;
    lRet = GetConfigFileListToMtd5(szFilenameList, uiFileCount, type);
    if(lRet != 0)
        return lRet;
    lRet = JoinConfigFiles(szDestFilename, szFilenameList, uiFileCount);

    // �ͷ��ڴ�
    for(unsigned int i=0; i<uiFileCount; i++){
        if( szFilenameList[i] != NULL ){
            delete [] szFilenameList[i];
            szFilenameList[i] = NULL;
        }
    }
    delete [] szFilenameList;
    szFilenameList = NULL;
        
    return lRet;
}

long CConfigMtdFile::JoinConfigFiles(const char* szDestFilename, int type)
{
    char** szFilenameList = NULL;
    unsigned int uiFileCount = 0;
    long lRet = 0;
    lRet = GetConfigFileList(szFilenameList, uiFileCount, type);
    if(lRet != 0)
        return lRet;
    lRet = JoinConfigFiles(szDestFilename, szFilenameList, uiFileCount);

    // �ͷ��ڴ�
    for(unsigned int i=0; i<uiFileCount; i++){
        if( szFilenameList[i] != NULL ){
            delete [] szFilenameList[i];
            szFilenameList[i] = NULL;
        }
    }
    delete [] szFilenameList;
    szFilenameList = NULL;
        
    return lRet;
}

/*
 * ������ļ�{szFilenameList}���ӳ�һ���ļ�{szDestFilename}����д��config mtd
 */
long CConfigMtdFile::JoinConfigFiles(const char* szDestFilename, 
                                          char** szFilenameList, 
                                          unsigned int uiItemCount)
{
    long lRet = -1;
    if( szDestFilename == NULL || strlen(szDestFilename) <= 0 ||
        szFilenameList == NULL || uiItemCount <=0 )
    {
        return lRet;
    }
    for(unsigned int i=0; i<uiItemCount; i++){
        if( szFilenameList[i]==NULL || strlen(szFilenameList[i])<=0 )
        {
            return lRet;
        }
    }
    
    FILE* fp_dest;
    FILE* fp_list[uiItemCount];
    bool bFailed = false;
    MtdFileHeaderT objMFH;
    ConfigFileHeaderT objCfhList[uiItemCount];
    int iRead = 0;
    int iWrite = 0;
    char *szBuffer = NULL;
    unsigned int i=0;
    char ptrStr[128] = {0};

    // �����ļ�ͷ��Ϣ
    memset(&objMFH, 0, sizeof(MtdFileHeaderT));
    objMFH.count = uiItemCount;
    strncpy(objMFH.sync, MTD_FILE_HEADER_SYNC, 8);
    for(i=0; i<uiItemCount; i++){
        memset(&objCfhList[i], 0, sizeof(ConfigFileHeaderT));
        strncpy(objCfhList[i].sync, CFG_FILE_HEADER_SYNC, 8);
        objCfhList[i].size = GetFileSize(szFilenameList[i]);
        if( objCfhList[i].size <= 0 ){
            return -1;
        }
        memset(ptrStr, 0, 128);
        strcpy(ptrStr, szFilenameList[i]);
        strcpy(objCfhList[i].name, basename(ptrStr));
    }
    
    /*
     * ���ļ����ж�д
     */
    fp_dest = fopen(szDestFilename, "w+b");
    if( fp_dest == NULL ){
        return lRet;
    }
    
    for(i=0; i<uiItemCount; i++){
        fp_list[i] = fopen(szFilenameList[i], "r");
        if( fp_list[i] == NULL ){
            bFailed = true;
            break;
        }
    }
    // ĳ����ȫ���ļ���ʧ�ܣ��˳�����
    if( bFailed == true ){
        for(i=0; i<uiItemCount; i++){
            if( fp_list[i] != NULL){
                fclose(fp_list[i]);
            }
        }
        return lRet;
    }

    /*
     * ������ļ�����д�뵽һ���ļ������ͷ��Ϣ
     */
    // mtd �ļ�ͷ
    fwrite(&objMFH, 1, sizeof(MtdFileHeaderT), fp_dest);
    for(i=0; i<uiItemCount; i++){
        // config�ļ�ͷ
        fwrite((void*)&(objCfhList[i]), 1, sizeof(ConfigFileHeaderT), fp_dest);
        // config�ļ�����
        szBuffer = new char[objCfhList[i].size + 1];
        iRead = fread(szBuffer, 1, objCfhList[i].size, fp_list[i]);
        if( iRead != objCfhList[i].size ){
            bFailed = true;
            break;
        }
        iWrite = fwrite(szBuffer, 1, iRead, fp_dest);
        if( iWrite != iRead ){
            bFailed = true;
            break;
        }
        fflush(fp_dest);
        if(szBuffer != NULL){
            delete [] szBuffer;
            szBuffer = NULL;
        }
    }
    if(szBuffer != NULL){
        delete [] szBuffer;
        szBuffer = NULL;
    }
    
    /*
     * �ر������ļ�
     */
    fclose(fp_dest);
    for(i=0; i<uiItemCount; i++){
            fclose(fp_list[i]);
    }
    // ����ʧ�ܣ�ɾ�����½���mtd�ļ�
    if( bFailed == true ){
        remove(szDestFilename);
        return lRet;
    }

    return 0;
}

/*
 * ������config mtd�������ļ�{szMtdFilename}���ɶ���ļ�{szFilenameList}(����·��)
 * �������ļ������{szDestDir}Ŀ¼��
 */
 long CConfigMtdFile::ExtracConfigFiles(const char* szMtdFilename, 
                                          const char*szDestDir, 
                                          char** &szFilenameList, 
                                          unsigned int &uiItemCount)
{
    long lRet = -1;
    if( szMtdFilename == NULL || strlen(szMtdFilename) <= 0 ||
        szDestDir == NULL || strlen(szDestDir) <= 0 )
    {
        return lRet;
    }
    mkdir(szDestDir, S_IRWXU | S_IRWXG | S_IRWXO);
 
    FILE* fp_mtd;
    bool bFailed = false;
    MtdFileHeaderT objMFH;
    ConfigFileHeaderT objCfh;
    int iRead = 0;
    int iWrite = 0;
    char *szBuffer = NULL;
    int iBuffLen = 0;
    char szFilePath[256] = {0};
    unsigned int i = 0;

    /*
     * ���ļ����ж�д
     */
    fp_mtd = fopen(szMtdFilename, "r");
    if( fp_mtd == NULL ){
        return lRet;
    }
    fread((void*)&objMFH, 1, sizeof(MtdFileHeaderT), fp_mtd);
    uiItemCount = (unsigned int)objMFH.count;
    if(uiItemCount <= 0)
    {
        return -1;
    }
    FILE* fp_list[uiItemCount];
    for(i=0; i<uiItemCount; i++)
    {
        fp_list[i] = NULL;
    }
    // У��ͬ��ͷ��Ϣ
    if( strncmp(objMFH.sync, MTD_FILE_HEADER_SYNC, sizeof(objMFH.sync)) != 0 ||
         uiItemCount <= 0 )
    {
        fclose(fp_mtd);
        return lRet;
    }

    szFilenameList = new char*[uiItemCount];
    for(i=0; i<uiItemCount; i++)
    {
        szFilenameList[i] = NULL;
    }
    for(i=0; i<uiItemCount; i++)
    {
        // ��ȡconfig�ļ�ͷ
        memset((void*)&objCfh, 0 , sizeof(ConfigFileHeaderT));
        fread((void*)&objCfh, 1, sizeof(ConfigFileHeaderT), fp_mtd);
        // У��ͬ��ͷ��Ϣ
        if( strncmp(objCfh.sync, CFG_FILE_HEADER_SYNC, sizeof(objCfh.sync)) != 0)
        {
            bFailed = true;
            break;
        }
        //�齨�ļ���
        if( szDestDir[strlen(szDestDir)-1] == '/' )
        {
            sprintf(szFilePath, "%s%s", szDestDir, objCfh.name);
        }
        else
        {
            sprintf(szFilePath, "%s/%s", szDestDir, objCfh.name);
        }
        szFilenameList[i] = new char[strlen(szFilePath)+1];
        szFilenameList[i][strlen(szFilePath)] = '\0';
        strncpy(szFilenameList[i], szFilePath, strlen(szFilePath));
        
        fp_list[i] = fopen(szFilenameList[i], "w+b");
        if( fp_list[i] == NULL ){
            bFailed = true;
            break;
        }
        
        if( szBuffer != NULL ){
            delete [] szBuffer;
            szBuffer = NULL;
        }
        iBuffLen = objCfh.size;
        if( iBuffLen < 0){
            bFailed = true;
            break;
        }
        szBuffer = new char[iBuffLen];
        iRead = fread(szBuffer, 1, iBuffLen, fp_mtd);
        if( iRead < iBuffLen ){
            bFailed = true;
            break;
        }
        iWrite = fwrite(szBuffer, 1, iBuffLen, fp_list[i]);
        fclose(fp_list[i]);
        fp_list[i] = NULL;
        
    }
    fclose(fp_mtd);
    
    // ĳ����ȫ���ļ���ʧ�ܣ��˳�����
    if( bFailed == true ){
        for(i=0; i<uiItemCount; i++){
            if( fp_list[i] != NULL){
                fclose(fp_list[i]);
            }
            if( szFilenameList[i] != NULL ){
                remove(szFilenameList[i]);
                delete [] szFilenameList[i];
                szFilenameList[i] = NULL;
            }
        }
        delete [] szFilenameList;
        szFilenameList = NULL;
        return lRet;
    }
    
    return 0;
}

long CConfigMtdFile::GetConfigFileList(char** &szFilenameList, unsigned int &uiItemCount, int iType)
{
    ////////////////////////////////////////////////////////////////////////////////////
    char szConfigfileAll[][80]={
        CFG_FILE_APPLYMAC,
        CFG_FILE_NETWORK,
        CFG_FILE_HOSTNAME,
        //CFG_FILE_DECODER,
        CFG_FILE_USER,
        CFG_FILE_WEB,
        CFG_FILE_ENCODER,
        CFG_FILE_SYS
    };
    char szConfigfileToResetAll[][80]={
        CFG_FILE_APPLYMAC,
        CFG_FILE_TORESET_NETWORK,
        CFG_FILE_TORESET_HOSTNAME,
        //CFG_FILE_TORESET_DECODER,
        CFG_FILE_TORESET_USER,
        CFG_FILE_TORESET_WEB,
        CFG_FILE_TORESET_ENCODER,
        CFG_FILE_TORESET_SYS
    };
    char szConfigfileToResetPart[][80]={
        CFG_FILE_APPLYMAC,
        CFG_FILE_NETWORK,
        CFG_FILE_HOSTNAME,
        //CFG_FILE_TORESET_DECODER,
        CFG_FILE_TORESET_USER,
        CFG_FILE_TORESET_WEB,
        CFG_FILE_TORESET_ENCODER
        CFG_FILE_TORESET_SYS,
    };
    char (*szStr)[80] = NULL;
    switch(iType)
    {
    case CFG_FILE_ALL:
        szStr = szConfigfileAll;
        break;
    case CFG_FILE_TORESET_ALL:
        szStr = szConfigfileToResetAll;
        break;
    case CFG_FILE_TORESET_PART:
        szStr = szConfigfileToResetPart;
        break;
    default:
        return -1;
    };
    
    uiItemCount = 7;
    szFilenameList = new char*[uiItemCount];
    int iStrLen = 0;
    for( unsigned int i=0; i<uiItemCount; i++){
        iStrLen = strlen(szStr[i]);
        szFilenameList[i] = new char[iStrLen + 1];
        strncpy(szFilenameList[i], szStr[i], iStrLen);
        szFilenameList[i][iStrLen] = '\0';
    }
    return 0;
}

long CConfigMtdFile::GetConfigFileListToMtd5(char** &szFilenameList, unsigned int &uiItemCount, int iType)
{

	//return GetConfigFileList(szFilenameList, uiItemCount, iType);
    ////////////////////////////////////////////////////////////////////////////////////
    char szConfigfileAll[][80]={
        CFG_FILE_DECODER
    };
    char szConfigfileToResetAll[][80]={
        CFG_FILE_TORESET_DECODER
    };
    char szConfigfileToResetPart[][80]={
        CFG_FILE_TORESET_DECODER
    };
    char (*szStr)[80] = NULL;
    switch(iType)
    {
    case CFG_FILE_ALL:
        szStr = szConfigfileAll;
        break;
    case CFG_FILE_TORESET_ALL:
        szStr = szConfigfileToResetAll;
        break;
    case CFG_FILE_TORESET_PART:
        szStr = szConfigfileToResetPart;
        break;
    default:
        return -1;
    };
    
    uiItemCount = 1;
    szFilenameList = new char*[uiItemCount];
    int iStrLen = 0;
    for( unsigned int i=0; i<uiItemCount; i++){
        iStrLen = strlen(szStr[i]);
        szFilenameList[i] = new char[iStrLen + 1];
        strncpy(szFilenameList[i], szStr[i], iStrLen);
        szFilenameList[i][iStrLen] = '\0';
    }
    return 0;
}

long CConfigMtdFile::GetFileSize( const char *szFileName )
{
    FILE *hFile = fopen(szFileName, "r" );
    if ( hFile == NULL)
    {
     _DEBUG_( "open file [%s] error. ", szFileName );
     return -1;
    }

    fseek( hFile, 0L, SEEK_END ); 
    long length = ftell( hFile );
    fclose( hFile );
    return length;
}





