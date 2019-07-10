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

	//构造函数
	Carame()
	{
		TRACE("==================================结构体初始化\n");
		memset(this, 0, sizeof(Carame));
		pVW = NULL;
		m_pMC = NULL;
		m_pSampGrabber = NULL;
		//或者是下面的格式 
		//memset(&nNum,0,sizeof(Stu));

	}
	//析构函数释放内存
	~Carame(){

		if (m_pMC)m_pMC->Stop();
		if (pVW){

			pVW->put_Visible(OAFALSE);

			pVW->put_Owner(NULL);
		}
		

		if (g_sampleGrabberCB){
			g_sampleGrabberCB->Release();
			delete g_sampleGrabberCB;
			g_sampleGrabberCB = NULL;
		}

		if (pBuilder){
			delete pBuilder;
			pBuilder = NULL;
		}
		if (m_pSampGrabber){
			m_pSampGrabber->Release();
			m_pSampGrabber = NULL;
		}

		
		
		//CoUninitialize();
	
		TRACE("==================================结构体释放内存\n");
	}
};

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
	void IMonRelease(IMoniker *&pm);
	void GrabOneFrame(BOOL bGrab);
	CStatic m_preview_1;
	CStatic m_preview_2;
private:
	

	IMediaControl* m_pMC;//媒体控制，IVideoWindow
	IVideoWindow* m_pVW;//IMediaControl
	HWND m_hWnd;
	IGraphBuilder *m_pGB;
	ICaptureGraphBuilder2* m_pCapture;
	IBaseFilter* m_pBF;
	void GetAllCapDevices();
	BOOL selectDevice(Carame* cur_gcap, int index);
	BOOL MakeBuilder(Carame* cur_gcap);
	BOOL MakeGraph(Carame* cur_gcap);
	BOOL MakeCallback(Carame* cur_gcap);//回调
	HRESULT ToPreview(int DIV_ID, Carame* cur_gcap);
	int carameCount;//读取到的摄像头个数
	void stopVideo();//释放摄像头
	IMoniker *rgpmVideoMenu[10];//摄像头驱动
	Carame* gcapList;//摄像头列表
	void takeAPicture(Carame* cur_gcap, int index);//拍照
	HRESULT ShowVideo(Carame* cur_gcap, int DIV_ID);
	UINT __cdecl WaitProc(CScanDlg * pThis);

public:
	afx_msg void OnBnClickedButton1();
};
