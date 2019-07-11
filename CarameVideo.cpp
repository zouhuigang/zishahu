#include "stdafx.h"
#include "CarameVideo.h"


CarameVideo::CarameVideo()
{
	m_pMC = NULL;
	pVW = NULL;
	m_pSampGrabber = NULL;
	m_pMediaEvent = NULL;
}



BOOL CarameVideo::MakeBuilder()
{
	// we have one already
	if (pBuilder)
		return TRUE;

	pBuilder = new ISampleCaptureGraphBuilder();
	if (NULL == pBuilder)
	{
		return FALSE;
	}

	//TRACE("==================================MakeBuilder:%p\n", pBuilder);
	return TRUE;
}

// Make a graph object we can use for capture graph building
//
BOOL CarameVideo::MakeGraph()
{
	// we have one already
	if (pFg)
		return TRUE;
	HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
		IID_IGraphBuilder, (LPVOID *)&pFg);

	//新增
	/*hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER,
	IID_ICaptureGraphBuilder2, (LPVOID*) & pFg);
	if (FAILED(hr))
	return hr;*/
	//TRACE("==================================MakeGraph:%p\n", pFg);

	return (hr == NOERROR) ? TRUE : FALSE;
}

//视频回调
BOOL CarameVideo::MakeCallback()
{

	if (g_sampleGrabberCB)
		return TRUE;

	g_sampleGrabberCB = new SampleGrabberCallback();
	if (NULL == g_sampleGrabberCB)
	{
		return FALSE;
	}

	return TRUE;
}

void CarameVideo::IMonRelease(IMoniker *&pm)
{
	if (pm)
	{
		pm->Release();
		pm = 0;
	}
}


//查看视频设备
//https://blog.csdn.net/laoyi_grace/article/details/6231429
//https://github.com/asbelldk/bsam_work/blob/08ec8da9aa09714a2ac636eeb1a74aa4ab10337e/DKProject/DirectShow/DlgWebcam.cpp
//https://www.cnblogs.com/nemolog/archive/2005/11/03/268353.html
void CarameVideo::GetAllCapDevices()
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
		//MessageBox(TEXT("Error Creatint Device Enumerator"));
		return;
	}
	IEnumMoniker *pEm = 0;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);//筛选出视频输入设备
	if (hr != NOERROR)
	{
		//MessageBox(TEXT("Sorry, you have no video capture hardware./r/n/r/n"));
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






//将摄像头数据，输出到mfc的picture控件中,初始化摄像头DIV_ID为容器id==IDC_PREVIEW_AVI
HRESULT CarameVideo::ToPreview(){
	HRESULT hr = S_OK;//运行是否成功
	pSampleGrabberFilter = NULL;
	m_pSampGrabber = NULL;


	//插入截图程序
	/*
	截图处理程序
	通过插入SampleGrabber Filter（对应的ID 为CLSID_SampleGrabber）
	并回调应用程序实现，主要在于ISampleGrabberCB 类的成员函数BufferCB。BufferCB 是 一个可获取当前图像Sample 指针的回调函数
	.CoCreateInstance( CLSID_SampleGrabber )
	*/
	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (LPVOID*)&pSampleGrabberFilter);
	if (FAILED(hr)){
		//MessageBox(TEXT("Fail to create SampleGrabber, maybe qedit.dll is not registered?"));
		TRACE("Fail to create SampleGrabber, maybe qedit.dll is not registered?");
		return hr;
	}

	hr = pFg->AddFilter(pSampleGrabberFilter, L"Grabber");
	if (FAILED(hr))
		return hr;


	//CLSID_FilterGraph改为CLSID_SampleGrabber
	hr = pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber, (LPVOID*)&m_pSampGrabber);
	if (FAILED(hr)){
		//MessageBox(TEXT("IID_ISampleGrabber QueryInterface Fail?"));
		TRACE("IID_ISampleGrabber QueryInterface Fail?");
		return hr;
	}

	//设置视频格式
	//set media type
	CMediaType VideoType;
	VideoType.SetType(&MEDIATYPE_Video);
	VideoType.SetSubtype(&MEDIASUBTYPE_RGB24);
	hr = m_pSampGrabber->SetMediaType(&VideoType); // shouldn't fail
	if (FAILED(hr))
	{
		//MessageBox(TEXT("Could not set media type"));
		TRACE("Could not set media type");
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

	//hr = m_pSampGrabber->SetMediaType(&mediaType);
	/*VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*)mediaType.pbFormat;
	g_sampleGrabberCB.m_lWidth = vih->bmiHeader.biWidth;
	g_sampleGrabberCB.m_lHeight = vih->bmiHeader.biHeight;*/
	//FreeMediaType(mediaType);

	hr = pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
		pVCap, pSampleGrabberFilter, NULL);
	if (FAILED(hr))
		return hr;



	hr = m_pSampGrabber->GetConnectedMediaType(&mediaType);
	if (FAILED(hr)){
		//MessageBox(TEXT(""));
		TRACE("Failt to read the connected media type");
		return hr;
	}

	VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*)mediaType.pbFormat;
	g_sampleGrabberCB->m_lWidth = vih->bmiHeader.biWidth;
	g_sampleGrabberCB->m_lHeight = vih->bmiHeader.biHeight;


	//只能设置SetOneShot为TRUE， 因为使用SetPositions 函数始终返回 E_NOTIMPL：Method is not supported.如果为false
	//WaitForCompletion(INFINITE, &evCode)函数会一直等待下去。
	hr = m_pSampGrabber->SetOneShot(FALSE);

	hr = m_pSampGrabber->SetBufferSamples(FALSE);
	//设置回调
	hr = m_pSampGrabber->SetCallback(g_sampleGrabberCB, 1);

	// find the video window and stuff it in our window

	hr = pFg->QueryInterface(IID_IVideoWindow, (LPVOID *)&pVW);

	if (FAILED(hr)){
		//MessageBox(TEXT("videoWindows fail"));
		TRACE("videoWindows fail");
		return hr;
	}

	TRACE("准备显示视频成功\n");
	
	return hr;
}


void CarameVideo::takeAPicture(int index){
	StringCchPrintf(g_sampleGrabberCB->m_cameraName, 50, TEXT("carame_") + index);
	g_sampleGrabberCB->m_bGetPicture = TRUE;
}

void  CarameVideo::GetComExceptionMessage(HRESULT hr)
{
	switch (hr)
	{
	case S_OK:
	case S_FALSE:
		TRACE("==================================无执行错误,hr=%ld\n", hr);
		break;
	case E_FAIL:
		TRACE("==================================语法错误,hr=%ld\n", hr);
		break;
	case E_INVALIDARG:
		TRACE("==================================一个或多个参数无效,hr=%ld\n", hr);
		break;
	case E_NOINTERFACE:
		TRACE("==================================不支持此接口,hr=%ld\n", hr);
		break;
	case E_NOTIMPL:
		TRACE("==================================未实现,hr=%ld\n", hr);
		break;
	case E_OUTOFMEMORY:
		TRACE("==================================未能分配所需的内存,hr=%ld\n", hr);
		break;
	case E_POINTER:
		TRACE("==================================无效的指针,hr=%ld\n", hr);
		break;
	case E_UNEXPECTED:
		TRACE("==================================未知错误,hr=%ld\n", hr);
		break;
	case E_HANDLE:
		TRACE("==================================无效句柄,hr=%ld\n", hr);
		break;
	case E_ABORT:
		TRACE("==================================执行被取消,hr=%ld\n", hr);
		break;
	case E_ACCESSDENIED:
		TRACE("==================================禁止访问,hr=%ld\n", hr);
		break;
	case E_PENDING:
		TRACE("==================================执行被挂起,hr=%ld\n", hr);
		break;
	default:
		TRACE("==================================default,hr=%ld\n", hr);
	}
	return;
}


//选择摄像头并初始化
BOOL CarameVideo::selectDevice(int index){

	//初始化参数
	HRESULT hr = S_OK;
	BOOL f;
	BOOL fb;
	WCHAR wszVideo[1024], wszAudio[1024];
	IBindCtx *lpBC = 0;
	IMoniker *tmp_pmVideo = 0, *pmAudio = 0;
	wszVideo[1023] = wszAudio[1023] = 0;

	//判断当前摄像头是否已构建成功
	f = MakeBuilder();
	if (!f)
	{
		//MessageBox(TEXT("Cannot instantiate graph builder"));
		return FALSE;
	}

	//构建回调
	fb = MakeCallback();
	if (!fb)
	{
		//MessageBox(TEXT("构建视频回调失败"));
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
	tmp_pmVideo = rgpmVideoMenu[index];//device
	pmVideo = tmp_pmVideo;
	//gcap.pmAudio = pmAudio;

	if (pmVideo != tmp_pmVideo){
		pmVideo->AddRef();
	}

	pVCap = NULL;

	//CString str1;
	//str1.Format(_T("init success,错误码:%d"), gcap.pmVideo);
	//MessageBox(str1);



	if (pmVideo != 0)
	{
		IPropertyBag *pBag;
		wachFriendlyName[0] = 0;
		hr = pmVideo->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if (SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR)
			{
				hr = StringCchCopyW(wachFriendlyName, sizeof(wachFriendlyName) / sizeof(wachFriendlyName[0]), var.bstrVal);
				SysFreeString(var.bstrVal);
			}
			pBag->Release();
		}
		hr = pmVideo->BindToObject(0, 0, IID_IBaseFilter, (void**)&pVCap);
	}
	if (pVCap == NULL)
	{
		//CString str;
		//str.Format(_T("无法创建视频捕获筛选器,错误码:%x"), hr);
		//MessageBox(str);
		return false;
	}
	f = MakeGraph();
	if (!f)
	{
		//MessageBox(TEXT("无法实例化筛选器图形"));
		return FALSE;
	}
	hr = pBuilder->SetFiltergraph(pFg);
	if (hr != NOERROR)
	{
		//MessageBox(TEXT("Cannot give graph to builder"));
		return FALSE;
	}
	// Add the video capture filter to the graph with its friendly name
	hr = pFg->AddFilter(pVCap, wachFriendlyName);
	if (hr != NOERROR)
	{
		//CString str;
		//str.Format(_T("Error %x: Cannot add vidcap to filtergraph"), hr);
		//MessageBox(str);
		return FALSE;
	}

	hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Interleaved, pVCap,
		IID_IAMVideoCompression, (void **)&pVC);
	if (hr != S_OK)
	{
		hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
			&MEDIATYPE_Video, pVCap,
			IID_IAMVideoCompression, (void **)&pVC);
	}

	// !!! What if this interface isn't supported?
	// we use this interface to set the frame rate and get the capture size
	hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Interleaved,
		pVCap, IID_IAMStreamConfig, (void **)&pVSC);

	if (hr != NOERROR)
	{
		hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
			&MEDIATYPE_Video, pVCap,
			IID_IAMStreamConfig, (void **)&pVSC);
		if (hr != NOERROR)
		{
			// this means we can't set frame rate (non-DV only)
			//MessageBox(TEXT("Cannot find VCapture:IAMStreamConfig"));
		}
	}

	fCapAudioIsRelevant = TRUE;

	//注册媒体控制事件
	/*hr = pFg->QueryInterface(IID_IMediaEventEx, (LPVOID*)&m_pMediaEvent);
	if (FAILED(hr))
	return hr;*/

	//状态转换
	hr = pFg->QueryInterface(IID_IMediaControl, (void **)&m_pMC);
	if (FAILED(hr))return hr;

	

	return TRUE;

}

CarameVideo::~CarameVideo()
{

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


//运行
void CarameVideo::Run(int index){
	GetAllCapDevices();
	selectDevice(index);
	ToPreview();
}
