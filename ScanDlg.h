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

//摄像头容器位置
#define IDC_CARAME_INIT 10000


#pragma once


// CScanDlg 对话框

class CScanDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScanDlg)

public:
	CScanDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CScanDlg();

// 对话框数据
	enum { IDD = IDD_SCAN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	HRESULT SetupVideoWindow();
	//void ResizeVideoWindow();
	HRESULT InitAndStartPreview();
	bool BindFilter(int deviceId, IBaseFilter **pFilter);
public:
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	void ErrMsg(TCHAR *pText);
	void IMonRelease(IMoniker *&pm);
	void GrabOneFrame(BOOL bGrab);
	HRESULT Init(int iDeviceID, HWND hWnd);
	int EnumDevices(HWND hList);
	CStatic m_preview_1;
	CStatic m_preview_2;
private:
	struct _capstuff
	{
		TCHAR szCaptureFile[_MAX_PATH];
		WORD wCapFileSize;  // size in Meg
		ISampleCaptureGraphBuilder *pBuilder;
		IVideoWindow *pVW;
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
		IMoniker *rgpmVideoMenu[10];
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
		SampleGrabberCallback g_sampleGrabberCB;//回调
		int index;//摄像头索引
	} gcap;

	IMediaControl* m_pMC;
	IVideoWindow* m_pVW;
	HWND m_hWnd;
	IGraphBuilder *m_pGB;
	ICaptureGraphBuilder2* m_pCapture;
	IBaseFilter* m_pBF;
	void GetAllCapDevices();
	BOOL selectDevice(_capstuff *cur_gcap,int index);
	BOOL MakeBuilder(_capstuff *cur_gcap);
	BOOL MakeGraph(_capstuff *cur_gcap);
	HRESULT ToPreview(int DIV_ID, _capstuff *cur_gcap);
	struct _capstuff gcapList[2];//摄像头列表,存储2个摄像头
	int carameCount;//读取到的摄像头个数

public:
	afx_msg void OnBnClickedButton1();
};
