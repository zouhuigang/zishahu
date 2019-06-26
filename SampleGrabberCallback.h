#ifndef __SAMPLEGRABBERCALLBACK_H__
#define __SAMPLEGRABBERCALLBACK_H__
#pragma include_alias( "dxtrans.h", "qedit.h" )
#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__
#include "qedit.h"

/*
定义一个类实现Sample Grabber 的回调接口IsampleGrabberCB；
调用RenderStream 依次把Still pin、Sample Grabber 和系统默认Renderer Filter 连接起来；
配置Sample Grabber 以捕获数据
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
	BOOL SaveBitmap(BYTE * pBuffer, long lBufferSize); //保存bitmap图片
public:
	BOOL m_bGetPicture;  // is get a picture
	long m_lWidth;       //存储图片的宽度
	long m_lHeight;		 //存储图片的长度
	int  m_iBitCount;    //the number of bits per pixel (bpp)
	TCHAR m_chTempPath[MAX_PATH];
	TCHAR m_chSwapStr[MAX_PATH];
	TCHAR m_chDirName[MAX_PATH];
	TCHAR m_cameraName[50];//摄像机名称
};

#endif //__SAMPLEGRABBERCALLBACK_H__