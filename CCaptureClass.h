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
//定义的资源释放操作宏
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
	CCaptureClass();          // 类构造器
	virtual ~CCaptureClass(); // 类析构器


	int EnumDevices(HWND hList);
	void SaveGraph(TCHAR *wFileName);            // 保存滤波器链表
	void ConfigCameraPin(HWND hwndParent);       // 配置摄像头的视频格式  
	void ConfigCameraFilter(HWND hwndParent);     // 配置摄像头的图像参数
	BOOL Pause(); // 暂停
	BOOL Play(); // 播放
	BOOL Stop(); // 停止
	HRESULT CaptureImages(CString inFileName);    // 捕获保存视频
	BOOL CaptureBitmap(const   char  * outFile); // 捕获图片
	HRESULT PreviewImages(int iDeviceID, HWND hWnd); // 采集预览视频
	bool IsPlaying()const { return m_bIsPlaying; } //是否正在播放
	bool HandleEvent(); //消息处理
	BOOL MakeCallback();//回调
	BOOL TakeAPicture();//拍照
private:
	HWND       m_hWnd;          // 视频显示窗口的句柄
	IGraphBuilder    *m_pGB;          // 滤波器链表管理器
	ICaptureGraphBuilder2  *m_pCapture;  // 增强型捕获滤波器链表管理器
	IBaseFilter     *m_pBF;   // 捕获滤波器
	IBaseFilter      *pNull;   // 渲染滤波器  
	IBasicVideo *pBasicVideo;// 视频基本接口  
	IBaseFilter    *pGrabberF;// 采样滤波器
	ISampleGrabber *pGrabber;// 采样滤波器接口
	IMediaControl    *m_pMC;   // 媒体控制接口  
	IMediaEventEx *pEvent; // 媒体事件接口
	IVideoWindow     *m_pVW;   // 视频显示窗口接口
	IBaseFilter     *pMux;   // 写文件滤波器
	bool  m_bIsPlaying; //是否在播放
	bool  m_bIsContinuePlay; //是否持续播放（不间断）
	SampleGrabberCallback *sGrabberCallback;
		
protected:
	bool BindFilter(int deviceId, IBaseFilter **pFilter);
	// 把指定的设备滤波器捆绑到链表中
	void ResizeVideoWindow();               // 更改视频显示窗口
	HRESULT SetupVideoWindow();             // 设置视频显示窗口的特性
	HRESULT InitCaptureGraphBuilder();     // 创建滤波器链表管理器，查询其各种控制接口
	IPin * FindPin(IBaseFilter *inFilter, char *inFilterName);

};