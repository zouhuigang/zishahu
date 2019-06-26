// ScanDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "zshu.h"
#include "ScanDlg.h"
#include "afxdialogex.h"

// CScanDlg 对话框

IMPLEMENT_DYNAMIC(CScanDlg, CDialogEx)

CScanDlg::CScanDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScanDlg::IDD, pParent)
{

	//初始化
	m_hWnd = NULL;
	m_pVW = NULL;
	m_pMC = NULL;
	m_pGB = NULL;
	m_pCapture = NULL;
}

CScanDlg::~CScanDlg()
{
}

void CScanDlg::ErrMsg(TCHAR *pText){
	::MessageBox(NULL, pText, TEXT("Error!"), MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
}

void CScanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PREVIEW_AVI, m_preview);
	DDX_Control(pDX, IDC_PREVIEW_AVI2, m_preview2);
}


BEGIN_MESSAGE_MAP(CScanDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, &CScanDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CScanDlg 消息处理程序


void CScanDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CDialog *pdlg = (CDialog *)AfxGetMainWnd();
	pdlg->DestroyWindow();
	CDialogEx::OnClose();
}

BOOL CScanDlg::MakeBuilder(_capstuff *cur_gcap)
{
	// we have one already
	if (cur_gcap->pBuilder)
		return TRUE;

	cur_gcap->pBuilder = new ISampleCaptureGraphBuilder();
	if (NULL == cur_gcap->pBuilder)
	{
		return FALSE;
	}

	return TRUE;
}

// Make a graph object we can use for capture graph building
//
BOOL CScanDlg::MakeGraph(_capstuff *cur_gcap)
{
	// we have one already
	if (cur_gcap->pFg)
		return TRUE;
	HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
		IID_IGraphBuilder, (LPVOID *)&cur_gcap->pFg);

	//新增
	/*hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER,
		IID_ICaptureGraphBuilder2, (LPVOID*) & cur_gcap->pFg);
	if (FAILED(hr))
		return hr;*/

	return (hr == NOERROR) ? TRUE : FALSE;
}

//查看视频设备
//https://blog.csdn.net/laoyi_grace/article/details/6231429
//https://github.com/asbelldk/bsam_work/blob/08ec8da9aa09714a2ac636eeb1a74aa4ab10337e/DKProject/DirectShow/DlgWebcam.cpp
//https://www.cnblogs.com/nemolog/archive/2005/11/03/268353.html
void CScanDlg::GetAllCapDevices()
{
	UINT uIndex = 0;
	//释放视频设备列表
	for (int i = 0; i<NUMELMS(gcap.rgpmVideoMenu); i++){
		IMonRelease(gcap.rgpmVideoMenu[i]);
	}

	//枚举所有的视频捕捉设备
	ICreateDevEnum *pCreateDevEnum = 0;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void**)&pCreateDevEnum);//创建所有系统设备枚举
	if (hr != NOERROR)
	{
		MessageBox(TEXT("Error Creatint Device Enumerator"));
		return;
	}
	IEnumMoniker *pEm = 0;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);//筛选出视频输入设备
	if (hr != NOERROR)
	{
		MessageBox(TEXT("Sorry, you have no video capture hardware./r/n/r/n"));
	}
	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;

	while ((hr = pEm->Next(1, &pM, &cFetched)) == S_OK)//循环直到最后一个设备为止
	{
		IPropertyBag *pBag = 0;
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);//把对象绑定到相应的属性对象
		if (SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR)
			{
				CString strDeviceName = var.bstrVal;
				//CString strMenuItem;
				//strMenuItem.Format("%d)%s",(uIndex+1),strDeviceName);
				//打印
				//MessageBox(strDeviceName);
				//将所有摄像头设备存储到rgpmVideoMenu里面
				SysFreeString(var.bstrVal);
				gcap.rgpmVideoMenu[uIndex] = pM;
				pM->AddRef();
			}
			pBag->Release();
		}
		pM->Release();
		uIndex++;
	}
	pEm->Release();
}

void CScanDlg::IMonRelease(IMoniker *&pm)
{
	if (pm)
	{
		pm->Release();
		pm = 0;
	}
}






//将摄像头数据，输出到mfc的picture控件中,初始化摄像头DIV_ID为容器id==IDC_PREVIEW_AVI
HRESULT CScanDlg::ToPreview(int DIV_ID, _capstuff *cur_gcap){
	HRESULT hr = S_OK;//运行是否成功
	HWND hwndPreview = NULL;//预览窗口
	cur_gcap->pSampleGrabberFilter = NULL;
	cur_gcap->m_pSampGrabber = NULL;


	//插入截图程序
	/*
	截图处理程序
	通过插入SampleGrabber Filter（对应的ID 为CLSID_SampleGrabber）
	并回调应用程序实现，主要在于ISampleGrabberCB 类的成员函数BufferCB。BufferCB 是 一个可获取当前图像Sample 指针的回调函数
	.CoCreateInstance( CLSID_SampleGrabber )
	*/
	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (LPVOID*)&cur_gcap->pSampleGrabberFilter);
	if (FAILED(hr)){
		MessageBox(TEXT("Fail to create SampleGrabber, maybe qedit.dll is not registered?"));
		return hr;
	}

	//hr = cur_gcap->pFg->AddFilter(pSampleGrabberFilter, L"Sample Grabber");
	hr = cur_gcap->pFg->AddFilter(cur_gcap->pSampleGrabberFilter, L"Sample Grabber");
	if (FAILED(hr))
		return hr;

	//CLSID_FilterGraph改为CLSID_SampleGrabber
	hr = cur_gcap->pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber, (LPVOID*)&cur_gcap->m_pSampGrabber);
	if (FAILED(hr)){
		MessageBox(TEXT("IID_ISampleGrabber QueryInterface Fail?"));
		return hr;
	}

	//设置视频格式
	//set media type
	AM_MEDIA_TYPE mediaType;
	ZeroMemory(&mediaType, sizeof(AM_MEDIA_TYPE));
	//Find the current bit depth
	/*HDC hdc = GetDC(NULL);
	int iBitDepth = GetDeviceCaps(hdc, BITSPIXEL);
	g_sampleGrabberCB.m_iBitCount = iBitDepth;
	ReleaseDC(NULL, hdc);*/

	mediaType.formattype = FORMAT_VideoInfo;
	mediaType.subtype = MEDIASUBTYPE_RGB32;

	hr = cur_gcap->m_pSampGrabber->SetMediaType(&mediaType);
	/*VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*)mediaType.pbFormat;
	g_sampleGrabberCB.m_lWidth = vih->bmiHeader.biWidth;
	g_sampleGrabberCB.m_lHeight = vih->bmiHeader.biHeight;*/
	//FreeMediaType(mediaType);

	hr = cur_gcap->pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,
		cur_gcap->pVCap, cur_gcap->pSampleGrabberFilter, NULL);
	if (FAILED(hr))
		return hr;


	hr = cur_gcap->m_pSampGrabber->GetConnectedMediaType(&mediaType);
	if (FAILED(hr)){
		MessageBox(TEXT("Failt to read the connected media type"));
		return hr;
	}
	
	VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*)mediaType.pbFormat;
	cur_gcap->g_sampleGrabberCB.m_lWidth = vih->bmiHeader.biWidth;
	cur_gcap->g_sampleGrabberCB.m_lHeight = vih->bmiHeader.biHeight;
	

	//只能设置SetOneShot为TRUE， 因为使用SetPositions 函数始终返回 E_NOTIMPL：Method is not supported.如果为false
	//WaitForCompletion(INFINITE, &evCode)函数会一直等待下去。
	hr = cur_gcap->m_pSampGrabber->SetOneShot(FALSE);

	hr = cur_gcap->m_pSampGrabber->SetBufferSamples(TRUE);
	//设置回调
	hr = cur_gcap->m_pSampGrabber->SetCallback(&cur_gcap->g_sampleGrabberCB, 1);


	/*

	注释掉，不能截图
	IBaseFilter *pSampleGrabberFilter;
	hr = m_pCaptureGB->RenderStream(&PIN_CATEGORY_PREVIEW,&MEDIATYPE_Video,
	m_pDevFilter,pSampleGrabberFilter,NULL);
	*/
	//连接视频捕捉图像的Filters
	/*hr = cur_gcap->pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Interleaved, cur_gcap->pVCap, NULL, NULL);
	if (FAILED(hr))
	{
		hr = cur_gcap->pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, cur_gcap->pVCap, NULL, NULL);
		if (FAILED(hr))
		{
			hr = cur_gcap->pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, cur_gcap->pVCap, NULL, NULL);
		}
	}*/

	// find the video window and stuff it in our window
	CComQIPtr< IVideoWindow, &IID_IVideoWindow > pWindow = cur_gcap->pFg;           //VideoWindow接口
	if (!pWindow)
	{
		MessageBox(TEXT("Could not get video window interface"));
		return E_FAIL;
	}
	// set up the preview window to be in our dialog instead of floating popup


	GetDlgItem(DIV_ID, &hwndPreview);//获得预览窗口控件的Hwnd,只能显示一个摄像头
	RECT rc;
	::GetWindowRect(hwndPreview, &rc);

	hr = pWindow->put_Owner((OAHWND)hwndPreview);
	hr = pWindow->put_Left(0);
	hr = pWindow->put_Top(0);
	hr = pWindow->put_Width(rc.right - rc.left);
	hr = pWindow->put_Height(rc.bottom - rc.top);
	hr = pWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
	hr = pWindow->put_Visible(OATRUE);

	// run the filter graph，IMediaControl接口提供了数据在Filer Graph中的流向
	CComQIPtr< IMediaControl, &IID_IMediaControl > pControl = cur_gcap->pFg;
	hr = pControl->Run();
	if (FAILED(hr))
	{
		MessageBox(TEXT("Could not run ToPreview"));
		return hr;
	}
	return hr;
}


//选择摄像头并初始化
BOOL CScanDlg::selectDevice(_capstuff *cur_gcap, int index){

	//初始化参数
	HRESULT hr = S_OK;
	BOOL f;
	WCHAR wszVideo[1024], wszAudio[1024];
	IBindCtx *lpBC = 0;
	IMoniker *pmVideo = 0, *pmAudio = 0;
	wszVideo[1023] = wszAudio[1023] = 0;

	//判断当前摄像头是否已构建成功
	f = MakeBuilder(cur_gcap);
	if (!f)
	{
		MessageBox(TEXT("Cannot instantiate graph builder"));
		return FALSE;
	}


	//绑定驱动
	hr = CreateBindCtx(0, &lpBC);
	if (SUCCEEDED(hr))
	{
		DWORD dwEaten;
		hr = MkParseDisplayName(lpBC, wszVideo, &dwEaten, &pmVideo);
		hr = MkParseDisplayName(lpBC, wszAudio, &dwEaten, &pmAudio);
		lpBC->Release();
	}

	//选择的驱动
	pmVideo = gcap.rgpmVideoMenu[index];//device
	cur_gcap->pmVideo = pmVideo;
	//gcap.pmAudio = pmAudio;

	if (cur_gcap->pmVideo != pmVideo){
		pmVideo->AddRef();
	}

	cur_gcap->pVCap = NULL;

	//CString str1;
	//str1.Format(_T("init success,错误码:%d"), gcap.pmVideo);
	//MessageBox(str1);



	if (cur_gcap->pmVideo != 0)
	{
		IPropertyBag *pBag;
		cur_gcap->wachFriendlyName[0] = 0;
		hr = cur_gcap->pmVideo->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if (SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR)
			{
				hr = StringCchCopyW(cur_gcap->wachFriendlyName, sizeof(cur_gcap->wachFriendlyName) / sizeof(cur_gcap->wachFriendlyName[0]), var.bstrVal);
				SysFreeString(var.bstrVal);
			}
			pBag->Release();
		}
		hr = cur_gcap->pmVideo->BindToObject(0, 0, IID_IBaseFilter, (void**)&cur_gcap->pVCap);
	}
	if (cur_gcap->pVCap == NULL)
	{
		CString str;
		str.Format(_T("无法创建视频捕获筛选器,错误码:%x"), hr);
		MessageBox(str);
		return false;
	}
	f = MakeGraph(cur_gcap);
	if (!f)
	{
		MessageBox(TEXT("无法实例化筛选器图形"));
		return FALSE;
	}
	hr = cur_gcap->pBuilder->SetFiltergraph(cur_gcap->pFg);
	if (hr != NOERROR)
	{
		MessageBox(TEXT("Cannot give graph to builder"));
		return FALSE;
	}
	// Add the video capture filter to the graph with its friendly name
	hr = cur_gcap->pFg->AddFilter(cur_gcap->pVCap, cur_gcap->wachFriendlyName);
	if (hr != NOERROR)
	{
		CString str;
		str.Format(_T("Error %x: Cannot add vidcap to filtergraph"), hr);
		MessageBox(str);
		return FALSE;
	}

	hr = cur_gcap->pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Interleaved, cur_gcap->pVCap,
		IID_IAMVideoCompression, (void **)&cur_gcap->pVC);
	if (hr != S_OK)
	{
		hr = cur_gcap->pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
			&MEDIATYPE_Video, cur_gcap->pVCap,
			IID_IAMVideoCompression, (void **)&cur_gcap->pVC);
	}

	// !!! What if this interface isn't supported?
	// we use this interface to set the frame rate and get the capture size
	hr = cur_gcap->pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Interleaved,
		cur_gcap->pVCap, IID_IAMStreamConfig, (void **)&cur_gcap->pVSC);

	if (hr != NOERROR)
	{
		hr = cur_gcap->pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
			&MEDIATYPE_Video, cur_gcap->pVCap,
			IID_IAMStreamConfig, (void **)&cur_gcap->pVSC);
		if (hr != NOERROR)
		{
			// this means we can't set frame rate (non-DV only)
			MessageBox(TEXT("Cannot find VCapture:IAMStreamConfig"));
		}
	}

	cur_gcap->fCapAudioIsRelevant = TRUE;

	//注册媒体控制事件
	hr = cur_gcap->pFg->QueryInterface(IID_IMediaEventEx, (LPVOID*)&cur_gcap->m_pMediaEvent);
	if (FAILED(hr))
		return hr;

	return TRUE;

}




//选择设备
BOOL CScanDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	//列举摄像头驱动
	GetAllCapDevices();

	//声明摄像头结构体
	//_capstuff gcap_1 = { sizeof(_capstuff) };
	//_capstuff gcap_2 = { sizeof(_capstuff) };
	selectDevice(&gcap_1,0);
	selectDevice(&gcap_2,1);
	


	//将视频数据显示到mfc界面上，传入容器的id
	ToPreview(IDC_PREVIEW_AVI, &gcap_2);
	ToPreview(IDC_PREVIEW_AVI2, &gcap_1);
	
	


	MessageBox(TEXT("打开摄像头成功"));
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

/*
void save(){
long evCode = 0;
long lBufferSize = 0;
BYTE *p;
gcap_1.m_pSampGrabber->GetCurrentBuffer(&lBufferSize, NULL);
p = new BYTE[lBufferSize];
gcap_1.m_pSampGrabber->GetCurrentBuffer(&lBufferSize, (LONG*)p);// get Current buffer
g_sampleGrabberCB.SaveBitmap(p, lBufferSize); //save bitmap
delete[] p;
p = NULL;

CString str;
str.Format(_T("点击拍照啦！！！，获取到的长度为%d"), lBufferSize);
MessageBox(str);
}
*/



void CScanDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	//摄像机名称
	StringCchCat(gcap_1.g_sampleGrabberCB.m_cameraName, 50, TEXT("system"));
	gcap_1.g_sampleGrabberCB.m_bGetPicture = TRUE;

	//摄像机2的名称
	StringCchCat(gcap_2.g_sampleGrabberCB.m_cameraName, 50, TEXT("buy"));
	gcap_2.g_sampleGrabberCB.m_bGetPicture = TRUE;


	CString str;
	str.Format(_T("点击拍照啦！！"));

	MessageBox(str);




	/*HRESULT hr;
	long evCode = 0;
	long lBufferSize = 0;
	BYTE *p;
	hr = m_pMediaEvent->WaitForCompletion(INFINITE, &evCode);
	if (SUCCEEDED(hr))
	{
		switch (evCode)
		{
		case EC_COMPLETE:
			m_pSampGrabber->GetCurrentBuffer(&lBufferSize, NULL);
			p = new BYTE[lBufferSize];
			m_pSampGrabber->GetCurrentBuffer(&lBufferSize, (LONG*)p);// get Current buffer
			g_sampleGrabberCB.SaveBitmap(p, lBufferSize); //save bitmap
			delete[] p;
			p = NULL;
			break;
		default:
			break;
		}
	}*/
	
}
