#pragma once
#include <afx.h>
#include  "afxwin.h"  
#include <streams.h>
#pragma include_alias( "dxtrans.h", "qedit.h" )
#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__
#include "qedit.h"
#include "SampleGrabberCallback.h"

#define WM_GRAPHNOTIFY WM_APP+1
//�������Դ�ͷŲ�����
#ifndef srelease
#define srelease(x)\
if (NULL != x){\
	x->Release();\
	x = NULL;\
}
#endif

//https://github.com/hiccupzhu/misc_starting/blob/bfed68b25b5751ec4e2c6a146df315e7e136db82/%E9%9F%B3%E8%A7%86%E9%A2%91%E7%BC%96%E8%A7%A3%E7%A0%81%E8%AF%A6%E8%A7%A3%E6%BA%90%E7%A0%81/chapter16/vss/vss/CaptureClass.cpp
//https://github.com/hiccupzhu/misc_starting/blob/bfed68b25b5751ec4e2c6a146df315e7e136db82/VCCODE2/CaptureVideo/CaptureVideo/CaptureClass.cpp
//https://github.com/sinsoul/SinSoulWebRemoteControl/blob/9ca8a530dbc339504a29db0bbe87ab1b905583e1/Puppet/SSWRC_Windows_Client/SSWRC_Windows_Client/DirectShowVideoCapture.cpp
//https://blog.csdn.net/xgx198831/article/details/7284618
//https://www.cnblogs.com/mlj318/p/3782534.html
//https://github.com/Hiseen/Old-Escope/blob/ea2675c2c190a2a434b87debbdd16c7c11677654/videomanager.cpp
//https://www.twblogs.net/a/5b901d882b71776722173828/zh-cn
//http://cn.voidcc.com/question/p-rqjaieps-bhr.html
class CCaptureClass
{
public:
	CCaptureClass();          // �๹����
	virtual ~CCaptureClass(); // ��������


	int EnumDevices(HWND hList);
	void SaveGraph(TCHAR *wFileName);            // �����˲�������
	void ConfigCameraPin(HWND hwndParent);       // ��������ͷ����Ƶ��ʽ  
	void ConfigCameraFilter(HWND hwndParent);     // ��������ͷ��ͼ�����
	BOOL Pause(); // ��ͣ
	BOOL Play(); // ����
	BOOL Stop(); // ֹͣ
	HRESULT CaptureImages(CString inFileName);    // ���񱣴���Ƶ
	BOOL CaptureBitmap(const   char  * outFile); // ����ͼƬ
	HRESULT PreviewImages(int iDeviceID, HWND hWnd); // �ɼ�Ԥ����Ƶ
	bool IsPlaying()const { return m_bIsPlaying; } //�Ƿ����ڲ���
	bool HandleEvent(); //��Ϣ����
	BOOL MakeCallback();//�ص�
	BOOL TakeAPicture();//����
private:
	HWND       m_hWnd;          // ��Ƶ��ʾ���ڵľ��
	IGraphBuilder    *m_pGB;          // �˲������������
	ICaptureGraphBuilder2  *m_pCapture;  // ��ǿ�Ͳ����˲������������
	IBaseFilter     *m_pBF;   // �����˲���
	IBaseFilter      *pNull;   // ��Ⱦ�˲���  
	IBasicVideo *pBasicVideo;// ��Ƶ�����ӿ�  
	IBaseFilter    *pGrabberF;// �����˲���
	ISampleGrabber *pGrabber;// �����˲����ӿ�
	IMediaControl    *m_pMC;   // ý����ƽӿ�  
	IMediaEventEx *pEvent; // ý���¼��ӿ�
	IVideoWindow     *m_pVW;   // ��Ƶ��ʾ���ڽӿ�
	IBaseFilter     *pMux;   // д�ļ��˲���
	bool  m_bIsPlaying; //�Ƿ��ڲ���
	bool  m_bIsContinuePlay; //�Ƿ�������ţ�����ϣ�
	SampleGrabberCallback *sGrabberCallback;
		
protected:
	bool BindFilter(int deviceId, IBaseFilter **pFilter);
	// ��ָ�����豸�˲�������������
	void ResizeVideoWindow();               // ������Ƶ��ʾ����
	HRESULT SetupVideoWindow();             // ������Ƶ��ʾ���ڵ�����
	HRESULT InitCaptureGraphBuilder();     // �����˲����������������ѯ����ֿ��ƽӿ�
	IPin * FindPin(IBaseFilter *inFilter, char *inFilterName);

};