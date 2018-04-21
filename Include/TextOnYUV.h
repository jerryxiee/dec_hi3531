
#ifndef TEXTONYUV_H_

#define TEXTONYUV_H_

#include <map>
using namespace std;
#include <pthread.h>

class CTextOnYuv
{
public:
	CTextOnYuv();
	virtual ~CTextOnYuv();


	
	int DrawText( char *szText, 
					int iFont, 
					int nStartX,
					int nStartY,
					int nImageWidth, 
					int nImageHeight, 
					unsigned char *pYUV );
    //��ȡszText�е��ַ������ֵ��ַ��������ݴ�����pCodeList��pHalfListָʾÿ�������Ƿ��Ǻ���(0-����)��nCountָʾpCodeList/pHalfList�ĸ���
    // nCount����/�������������ʱֻ��ǰ����������������������ʱָʾ����
    int GetCodeList(char* szText, int iFont, unsigned char** pCodeList, int* pHalfList, int &nCount);
    // ��ȡ�ֿ��еĵ���,szTextΪ1�����ֻ���һ���ַ�, pdz�洢���������СΪiFont*iFont,���Ϊ�ַ���ΪiFont*iFont/2
    int GetDz(char* szText, int iFont, unsigned char* &pdz, int &nHalf);
private:

	void GetWordCount( char *szText, 
								  int *&pnCodes, 
								  int &nCount, 
								  int &nHalfFont );

	
	int  GetDZK( int nCode, 
						int iFont, 
						unsigned char *&pZk, 
						int &nHalf );


	int GetDzkInMem( int nCode, int nFont, unsigned char *&pZk );


	
	void AddDzkToMem( int nCode, int nFont, unsigned char *pZk );

	map<int/*code*/, unsigned char*/*szCode*/> m_mapDzk;

	pthread_mutex_t m_lock;
};

#endif 

