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

//����ͷ����λ��
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
	ISampleGrabber *m_pSampGrabber;//Ϊ�˽�ͼ����qedit
	IMediaEventEx *m_pMediaEvent;//ý�����
	IBaseFilter *pSampleGrabberFilter;//��ͼ����
	SampleGrabberCallback *g_sampleGrabberCB;//�ص�
	IVideoWindow *pVW;//��Ƶ���ڽӿ�
	IMediaControl *m_pMC;//ý����ƽӿ�
	int index;//����ͷ����

	//���캯��
	Carame()
	{
		TRACE("==================================�ṹ���ʼ��\n");
		memset(this, 0, sizeof(Carame));
		pVW = NULL;
		m_pMC = NULL;
		m_pSampGrabber = NULL;
		//����������ĸ�ʽ 
		//memset(&nNum,0,sizeof(Stu));

	}
	//���������ͷ��ڴ�
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
	
		TRACE("==================================�ṹ���ͷ��ڴ�\n");
	}
};

#pragma once


// CScanDlg �Ի���

class CScanDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScanDlg)

public:
	CScanDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CScanDlg();

// �Ի�������
	enum { IDD = IDD_SCAN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
	

	IMediaControl* m_pMC;//ý����ƣ�IVideoWindow
	IVideoWindow* m_pVW;//IMediaControl
	HWND m_hWnd;
	IGraphBuilder *m_pGB;
	ICaptureGraphBuilder2* m_pCapture;
	IBaseFilter* m_pBF;
	void GetAllCapDevices();
	BOOL selectDevice(Carame* cur_gcap, int index);
	BOOL MakeBuilder(Carame* cur_gcap);
	BOOL MakeGraph(Carame* cur_gcap);
	BOOL MakeCallback(Carame* cur_gcap);//�ص�
	HRESULT ToPreview(int DIV_ID, Carame* cur_gcap);
	int carameCount;//��ȡ��������ͷ����
	void stopVideo();//�ͷ�����ͷ
	IMoniker *rgpmVideoMenu[10];//����ͷ����
	Carame* gcapList;//����ͷ�б�
	void takeAPicture(Carame* cur_gcap, int index);//����
	HRESULT ShowVideo(Carame* cur_gcap, int DIV_ID);
	UINT __cdecl WaitProc(CScanDlg * pThis);

public:
	afx_msg void OnBnClickedButton1();
};
