#pragma once
#include <streams.h>
#include "Amcap/SampleCGB.h"

#pragma include_alias( "dxtrans.h", "qedit.h" )
#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__
#include "qedit.h"
#include "SampleGrabberCallback.h"


struct Carame
{
	TCHAR szCaptureFile[_MAX_PATH];
	WORD wCapFileSize;  // size in Meg
	ISampleCaptureGraphBuilder *pBuilder;
	IMediaEventEx *pME;
	IAMDroppedFrames *pDF;
	IAMVideoCompression *pVC;
	IAMVfwCaptureDialogs *pDlg;
	IAMStreamConfig *pASC;      // for audio cap
	IAMStreamConfig *pVSC;      // for video cap
	IBaseFilter *pRender;
	IBaseFilter *pVCap, *pACap;
	IGraphBuilder *pFg;
	IFileSinkFilter *pSink;
	IConfigAviMux *pConfigAviMux;
	int  iMasterStream;
	BOOL fCaptureGraphBuilt;
	BOOL fPreviewGraphBuilt;
	BOOL fCapturing;
	BOOL fPreviewing;
	BOOL fMPEG2;
	BOOL fCapAudio;
	BOOL fCapCC;
	BOOL fCCAvail;
	BOOL fCapAudioIsRelevant;
	bool fDeviceMenuPopulated;
	IMoniker *rgpmAudioMenu[10];
	IMoniker *pmVideo;
	IMoniker *pmAudio;
	double FrameRate;
	BOOL fWantPreview;
	long lCapStartTime;
	long lCapStopTime;
	WCHAR wachFriendlyName[120];
	BOOL fUseTimeLimit;
	BOOL fUseFrameRate;
	DWORD dwTimeLimit;
	int iFormatDialogPos;
	int iSourceDialogPos;
	int iDisplayDialogPos;
	int iVCapDialogPos;
	int iVCrossbarDialogPos;
	int iTVTunerDialogPos;
	int iACapDialogPos;
	int iACrossbarDialogPos;
	int iTVAudioDialogPos;
	int iVCapCapturePinDialogPos;
	int iVCapPreviewPinDialogPos;
	int iACapCapturePinDialogPos;
	long lDroppedBase;
	long lNotBase;
	BOOL fPreviewFaked;
	int iVideoInputMenuPos;
	LONG NumberOfVideoInputs;
	HMENU hMenuPopup;
	int iNumVCapDevices;
	ISampleGrabber *m_pSampGrabber;//为了截图引入qedit
	IMediaEventEx *m_pMediaEvent;//媒体控制
	IBaseFilter *pSampleGrabberFilter;//截图变量
	SampleGrabberCallback* g_sampleGrabberCB;//回调
	IVideoWindow *pVW;//视频窗口接口
	IMediaControl *m_pMC;//媒体控制接口
	int index;//摄像头索引

	Carame()
	{
		memset(this, 0, sizeof(Carame));

	}
	~Carame(){

		if (m_pMC)m_pMC->Stop();


		if (pVW){

			pVW->put_Visible(OAFALSE);

			pVW->put_Owner(NULL);
		}
		if (m_pMC){
			m_pMC->Release();
			m_pMC = NULL;
		}

		if (pVW){
			pVW->Release();
			pVW = NULL;
		}


		if (m_pSampGrabber){
			m_pSampGrabber->Release();
			m_pSampGrabber = NULL;
		}
		if (pSampleGrabberFilter){
			pSampleGrabberFilter->Release();
			pSampleGrabberFilter = NULL;
		}
		if (m_pMediaEvent){
			m_pMediaEvent->Release();
			m_pMediaEvent = NULL;
		}


		if (pBuilder){
			delete pBuilder;
			pBuilder = NULL;
		}

		if (pFg){
			pFg->Release();
			pFg = NULL;
		}


		if (g_sampleGrabberCB){
			g_sampleGrabberCB->Release();
			delete g_sampleGrabberCB;
			g_sampleGrabberCB = NULL;
		}





	}
};
//https://www.cnblogs.com/zhmx/articles/2672992.html
class CarameVideo
{
public:
	CarameVideo();
	~CarameVideo();
	BOOL selectDevice(int index);
	BOOL MakeBuilder();
	BOOL MakeGraph();
	BOOL MakeCallback();
	HRESULT ToPreview();
	void GetAllCapDevices();
	void IMonRelease(IMoniker *&pm);
	void takeAPicture(int index);//拍照
	void GetComExceptionMessage(HRESULT hr);
	void  Run(int index);

public:
	TCHAR szCaptureFile[_MAX_PATH];
	WORD wCapFileSize;  // size in Meg
	ISampleCaptureGraphBuilder *pBuilder;
	IAMDroppedFrames *pDF;
	IAMVideoCompression *pVC;
	IAMVfwCaptureDialogs *pDlg;
	IAMStreamConfig *pASC;      // for audio cap
	IAMStreamConfig *pVSC;      // for video cap
	IBaseFilter *pRender;
	IBaseFilter *pVCap, *pACap;
	IGraphBuilder *pFg;
	IFileSinkFilter *pSink;
	IConfigAviMux *pConfigAviMux;
	int  iMasterStream;
	BOOL fCaptureGraphBuilt;
	BOOL fPreviewGraphBuilt;
	BOOL fCapturing;
	BOOL fPreviewing;
	BOOL fMPEG2;
	BOOL fCapAudio;
	BOOL fCapCC;
	BOOL fCCAvail;
	BOOL fCapAudioIsRelevant;
	bool fDeviceMenuPopulated;
	IMoniker *rgpmAudioMenu[10];
	IMoniker *pmVideo;
	IMoniker *pmAudio;
	double FrameRate;
	BOOL fWantPreview;
	long lCapStartTime;
	long lCapStopTime;
	WCHAR wachFriendlyName[120];
	BOOL fUseTimeLimit;
	BOOL fUseFrameRate;
	DWORD dwTimeLimit;
	int iFormatDialogPos;
	int iSourceDialogPos;
	int iDisplayDialogPos;
	int iVCapDialogPos;
	int iVCrossbarDialogPos;
	int iTVTunerDialogPos;
	int iACapDialogPos;
	int iACrossbarDialogPos;
	int iTVAudioDialogPos;
	int iVCapCapturePinDialogPos;
	int iVCapPreviewPinDialogPos;
	int iACapCapturePinDialogPos;
	long lDroppedBase;
	long lNotBase;
	BOOL fPreviewFaked;
	int iVideoInputMenuPos;
	LONG NumberOfVideoInputs;
	HMENU hMenuPopup;
	int iNumVCapDevices;
	ISampleGrabber *m_pSampGrabber;//为了截图引入qedit
	IMediaEventEx *m_pMediaEvent;//媒体控制
	IBaseFilter *pSampleGrabberFilter;//截图变量
	SampleGrabberCallback *g_sampleGrabberCB;//回调
	IVideoWindow *pVW;//视频窗口接口
	IMediaControl *m_pMC;//媒体控制接口
	int index;//摄像头索引
	int carameCount;
	IMoniker *rgpmVideoMenu[10];
};

