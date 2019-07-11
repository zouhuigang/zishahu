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
	//_CrtSetBreakAlloc(646); //检测内存泄漏

}

CScanDlg::~CScanDlg()
{
	//free mem
	CoUninitialize();
	TRACE("==================================调用析构函数\n");
	delete[] gcapList;
}


void CScanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PREVIEW_AVI_1, m_preview_1);
	DDX_Control(pDX, IDC_PREVIEW_AVI_2, m_preview_2);
}


BEGIN_MESSAGE_MAP(CScanDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, &CScanDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CScanDlg 消息处理程序


void CScanDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	//CDialog *pdlg = (CDialog *)AfxGetMainWnd();
	//pdlg->DestroyWindow();
	CDialogEx::OnClose();
}

BOOL CScanDlg::MakeBuilder(Carame* cur_gcap)
{
	// we have one already
	if (cur_gcap->pBuilder)
		return TRUE;

	cur_gcap->pBuilder = new ISampleCaptureGraphBuilder();
	if (NULL == cur_gcap->pBuilder)
	{
		return FALSE;
	}

	//TRACE("==================================MakeBuilder:%p\n", cur_gcap->pBuilder);
	return TRUE;
}

// Make a graph object we can use for capture graph building
//
BOOL CScanDlg::MakeGraph(Carame* cur_gcap)
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
	//TRACE("==================================MakeGraph:%p\n", cur_gcap->pFg);

	return (hr == NOERROR) ? TRUE : FALSE;
}

//视频回调
BOOL CScanDlg::MakeCallback(Carame* cur_gcap)
{
	
	if (cur_gcap->g_sampleGrabberCB)
		return TRUE;

	cur_gcap->g_sampleGrabberCB = new SampleGrabberCallback();
	if (NULL == cur_gcap->g_sampleGrabberCB)
	{
		return FALSE;
	}

	return TRUE;
}


//查看视频设备
//https://blog.csdn.net/laoyi_grace/article/details/6231429
//https://github.com/asbelldk/bsam_work/blob/08ec8da9aa09714a2ac636eeb1a74aa4ab10337e/DKProject/DirectShow/DlgWebcam.cpp
//https://www.cnblogs.com/nemolog/archive/2005/11/03/268353.html
void CScanDlg::GetAllCapDevices()
{
	UINT uIndex = 0;
	//释放视频设备列表
	for (int i = 0; i<NUMELMS(rgpmVideoMenu); i++){
		IMonRelease(rgpmVideoMenu[i]);
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
				//将所有摄像头设备存储到rgpmVideoMenu里面
				SysFreeString(var.bstrVal);
				rgpmVideoMenu[uIndex] = pM;
				pM->AddRef();
			}
			pBag->Release();
		}
		pM->Release();
		uIndex++;
	}
	pEm->Release();

	//摄像头个数
	carameCount = uIndex;
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
HRESULT CScanDlg::ToPreview(int DIV_ID, Carame* cur_gcap){
	HRESULT hr = S_OK;//运行是否成功
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
	
	hr = cur_gcap->pFg->AddFilter(cur_gcap->pSampleGrabberFilter, L"Grabber");
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
	CMediaType VideoType;
	VideoType.SetType(&MEDIATYPE_Video);
	VideoType.SetSubtype(&MEDIASUBTYPE_RGB24);
	hr = cur_gcap->m_pSampGrabber->SetMediaType(&VideoType); // shouldn't fail
	if (FAILED(hr))
	{
		MessageBox(TEXT("Could not set media type"));
		return hr;
	}
	
	AM_MEDIA_TYPE mediaType;
	ZeroMemory(&mediaType, sizeof(AM_MEDIA_TYPE));
	//Find the current bit depth
	/*HDC hdc = GetDC(NULL);
	int iBitDepth = GetDeviceCaps(hdc, BITSPIXEL);
	g_sampleGrabberCB.m_iBitCount = iBitDepth;
	ReleaseDC(NULL, hdc);*/

	mediaType.formattype = FORMAT_VideoInfo;
	mediaType.subtype = MEDIASUBTYPE_RGB24;//MEDIASUBTYPE_RGB32

	//hr = cur_gcap->m_pSampGrabber->SetMediaType(&mediaType);
	/*VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*)mediaType.pbFormat;
	g_sampleGrabberCB.m_lWidth = vih->bmiHeader.biWidth;
	g_sampleGrabberCB.m_lHeight = vih->bmiHeader.biHeight;*/
	//FreeMediaType(mediaType);
	
	hr = cur_gcap->pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
		cur_gcap->pVCap, cur_gcap->pSampleGrabberFilter, NULL);
	if (FAILED(hr))
		return hr;



	hr = cur_gcap->m_pSampGrabber->GetConnectedMediaType(&mediaType);
	if (FAILED(hr)){
		MessageBox(TEXT("Failt to read the connected media type"));
		return hr;
	}
	
	VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*)mediaType.pbFormat;
	cur_gcap->g_sampleGrabberCB->m_lWidth = vih->bmiHeader.biWidth;
	cur_gcap->g_sampleGrabberCB->m_lHeight = vih->bmiHeader.biHeight;
	

	//只能设置SetOneShot为TRUE， 因为使用SetPositions 函数始终返回 E_NOTIMPL：Method is not supported.如果为false
	//WaitForCompletion(INFINITE, &evCode)函数会一直等待下去。
	hr = cur_gcap->m_pSampGrabber->SetOneShot(FALSE);

	hr = cur_gcap->m_pSampGrabber->SetBufferSamples(FALSE);
	//设置回调
	hr = cur_gcap->m_pSampGrabber->SetCallback(cur_gcap->g_sampleGrabberCB, 1);

	// find the video window and stuff it in our window
	
	hr = cur_gcap->pFg->QueryInterface(IID_IVideoWindow, (LPVOID *)&cur_gcap->pVW);

	if (FAILED(hr)){
		MessageBox(TEXT("videoWindows fail"));
		return hr;
	}


	ShowVideo(cur_gcap, DIV_ID);

	return hr;
}


//输出到指定容器
HRESULT CScanDlg::ShowVideo(Carame* cur_gcap,int DIV_ID){
	HWND hwndPreview = NULL;//预览窗口
	HRESULT hr = S_OK;//运行是否成功
	// set up the preview window to be in our dialog instead of floating popup
	GetDlgItem(DIV_ID, &hwndPreview);//获得预览窗口控件的Hwnd,只能显示一个摄像头
	RECT rc;
	::GetWindowRect(hwndPreview, &rc);
	hr = cur_gcap->pVW->put_Owner((OAHWND)hwndPreview);
	hr = cur_gcap->pVW->put_Left(0);
	hr = cur_gcap->pVW->put_Top(0);
	hr = cur_gcap->pVW->put_Width(rc.right - rc.left);
	hr = cur_gcap->pVW->put_Height(rc.bottom - rc.top);
	hr = cur_gcap->pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
	hr = cur_gcap->pVW->put_Visible(OATRUE);
	//在应用程序退出前，停止图表并重置视频窗口为NULL。否则，窗口消息可能被错误的发送给错误的窗口，从而导致错误发生
	// run the filter graph，IMediaControl接口提供了数据在Filer Graph中的流向
	//OAFilterState pfs;
	//hr = cur_gcap->m_pMC->GetState(100, &pfs);//测试Run之前
	hr = cur_gcap->m_pMC->Run();

	DWORD r = GetLastError();
	TRACE("==================================错误 = %d,hr=%d\n", r, hr);
	if (FAILED(hr))
	{
		MessageBox(TEXT("视频不能预览"));
		return hr;
	}
	//创建线程
	//AfxBeginThread((AFX_THREADPROC)WaitProc, this);
	return hr;
}


// 线程调用函数
UINT __cdecl CScanDlg::WaitProc(CScanDlg * pThis)
{
	long evCode;
	//pThis->m_pEvent->WaitForCompletion(INFINITE, &evCode); // Wait until the graph stops
	//AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_ANA_ACTIONFINISHED);
	return 0;
}


//选择摄像头并初始化
BOOL CScanDlg::selectDevice(Carame* cur_gcap, int index){

	//初始化参数
	HRESULT hr = S_OK;
	BOOL f;
	BOOL fb;
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

	//构建回调
	fb = MakeCallback(cur_gcap);
	if (!fb)
	{
		MessageBox(TEXT("构建视频回调失败"));
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
	pmVideo = rgpmVideoMenu[index];//device
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
	/*hr = cur_gcap->pFg->QueryInterface(IID_IMediaEventEx, (LPVOID*)&cur_gcap->m_pMediaEvent);
	if (FAILED(hr))
		return hr;*/

	//状态转换
	hr = cur_gcap->pFg->QueryInterface(IID_IMediaControl, (void **)&cur_gcap->m_pMC);
	if (FAILED(hr))return hr;


	return TRUE;

}




//选择设备
BOOL CScanDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	//不支持com组件的就会失败!
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)){
		MessageBox(TEXT("不支持COM组件的初始化，导致动画未被载入"));
	}

	//声明摄像头
	gcapList = new Carame[2];//摄像头列表,存储2个摄像头
	//列举摄像头驱动
	GetAllCapDevices();
	

	for (int i = 0; i < carameCount; i++){
		if (i>=2){
			break;
		}

		
		//将摄像头显示到对应的mfc容器
		if (i == 0){
			//初始化摄像头
			selectDevice(&gcapList[0], 0);
			ToPreview(IDC_PREVIEW_AVI_1, &gcapList[0]);
		}
		else if (i==1){
			//初始化摄像头
			selectDevice(&gcapList[1], 1);
			ToPreview(IDC_PREVIEW_AVI_2, &gcapList[1]);
		}
		
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CScanDlg::takeAPicture(Carame* cur_gcap,int index){
	StringCchPrintf(cur_gcap->g_sampleGrabberCB->m_cameraName, 50, TEXT("carame_") + index);
	cur_gcap->g_sampleGrabberCB->m_bGetPicture = TRUE;
}


void CScanDlg::OnBnClickedButton1()
{

	for (int i = 0; i < carameCount; i++){
		if (i >= 2){
			break;
		}
		//摄像机2的名称
		takeAPicture(&gcapList[i],i);
	
	}


	CString str;
	str.Format(_T("点击拍照啦！！"));

	MessageBox(str);
	
}


void CScanDlg::PostNcDestroy()
{
	// TODO:  在此添加专用代码和/或调用基类
	CDialogEx::PostNcDestroy();
}
