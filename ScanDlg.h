#include <streams.h>

#include <windows.h>
#include <dbt.h>
#include <mmreg.h>
#include <msacm.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <commdlg.h>
#include <atlbase.h>
#include <strsafe.h>
#include "stdafx.h"
#include "Amcap/crossbar.h"
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
	CCrossbar *pCrossbar;
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

	Carame()
	{
		memset(this, 0, sizeof(Carame));
		pVW = NULL;
		m_pMC = NULL;
		m_pSampGrabber = NULL;

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

#pragma once



class CScanDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScanDlg)

public:
	CScanDlg(CWnd* pParent = NULL);
	virtual ~CScanDlg();
	enum { IDD = IDD_SCAN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
	HRESULT SetupVideoWindow();
	HRESULT InitAndStartPreview();
	bool BindFilter(int deviceId, IBaseFilter **pFilter);
public:
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	void IMonRelease(IMoniker *&pm);
	void GrabOneFrame(BOOL bGrab);
	CStatic m_preview_1;
	CStatic m_preview_2;
private:
	void GetAllCapDevices();
	BOOL selectDevice(Carame* cur_gcap, int index);
	BOOL MakeBuilder(Carame* cur_gcap);
	BOOL MakeGraph(Carame* cur_gcap);
	BOOL MakeCallback(Carame* cur_gcap);
	HRESULT ToPreview(int DIV_ID, Carame* cur_gcap);
	int carameCount;
	IMoniker *rgpmVideoMenu[10];
	Carame* gcapList;
	void takeAPicture(Carame* cur_gcap, int index);//拍照
	HRESULT ShowVideo(Carame* cur_gcap, int DIV_ID);
	UINT __cdecl WaitProc(CScanDlg * pThis);

public:
	afx_msg void OnBnClickedButton1();
	virtual void PostNcDestroy();
};
