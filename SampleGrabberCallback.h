#ifndef __SAMPLEGRABBERCALLBACK_H__
#define __SAMPLEGRABBERCALLBACK_H__
#pragma include_alias( "dxtrans.h", "qedit.h" )
#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__
#include "qedit.h"

/*
����һ����ʵ��Sample Grabber �Ļص��ӿ�IsampleGrabberCB��
����RenderStream ���ΰ�Still pin��Sample Grabber ��ϵͳĬ��Renderer Filter ����������
����Sample Grabber �Բ�������
*/

class SampleGrabberCallback : public ISampleGrabberCB
{
public:
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

	HRESULT STDMETHODCALLTYPE SampleCB(double Time, IMediaSample *pSample);
	HRESULT STDMETHODCALLTYPE BufferCB(double Time, BYTE *pBuffer, long BufferLen);

	SampleGrabberCallback();
	BOOL SaveBitmap(BYTE * pBuffer, long lBufferSize); //����bitmapͼƬ
public:
	BOOL m_bGetPicture;  // is get a picture
	long m_lWidth;       //�洢ͼƬ�Ŀ��
	long m_lHeight;		 //�洢ͼƬ�ĳ���
	int  m_iBitCount;    //the number of bits per pixel (bpp)
	TCHAR m_chTempPath[MAX_PATH];
	TCHAR m_chSwapStr[MAX_PATH];
	TCHAR m_chDirName[MAX_PATH];
	TCHAR m_cameraName[50];//���������
};

#endif //__SAMPLEGRABBERCALLBACK_H__