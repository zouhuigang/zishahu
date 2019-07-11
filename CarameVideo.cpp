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

	//����
	/*hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER,
	IID_ICaptureGraphBuilder2, (LPVOID*) & pFg);
	if (FAILED(hr))
	return hr;*/
	//TRACE("==================================MakeGraph:%p\n", pFg);

	return (hr == NOERROR) ? TRUE : FALSE;
}

//��Ƶ�ص�
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


//�鿴��Ƶ�豸
//https://blog.csdn.net/laoyi_grace/article/details/6231429
//https://github.com/asbelldk/bsam_work/blob/08ec8da9aa09714a2ac636eeb1a74aa4ab10337e/DKProject/DirectShow/DlgWebcam.cpp
//https://www.cnblogs.com/nemolog/archive/2005/11/03/268353.html
void CarameVideo::GetAllCapDevices()
{
	UINT uIndex = 0;
	//�ͷ���Ƶ�豸�б�
	for (int i = 0; i<NUMELMS(rgpmVideoMenu); i++){
		IMonRelease(rgpmVideoMenu[i]);
	}

	//ö�����е���Ƶ��׽�豸
	ICreateDevEnum *pCreateDevEnum = 0;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void**)&pCreateDevEnum);//��������ϵͳ�豸ö��
	if (hr != NOERROR)
	{
		//MessageBox(TEXT("Error Creatint Device Enumerator"));
		return;
	}
	IEnumMoniker *pEm = 0;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);//ɸѡ����Ƶ�����豸
	if (hr != NOERROR)
	{
		//MessageBox(TEXT("Sorry, you have no video capture hardware./r/n/r/n"));
	}
	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;

	while ((hr = pEm->Next(1, &pM, &cFetched)) == S_OK)//ѭ��ֱ�����һ���豸Ϊֹ
	{
		IPropertyBag *pBag = 0;
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);//�Ѷ���󶨵���Ӧ�����Զ���
		if (SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR)
			{
				//����������ͷ�豸�洢��rgpmVideoMenu����
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

	//����ͷ����
	carameCount = uIndex;
}






//������ͷ���ݣ������mfc��picture�ؼ���,��ʼ������ͷDIV_IDΪ����id==IDC_PREVIEW_AVI
HRESULT CarameVideo::ToPreview(){
	HRESULT hr = S_OK;//�����Ƿ�ɹ�
	pSampleGrabberFilter = NULL;
	m_pSampGrabber = NULL;


	//�����ͼ����
	/*
	��ͼ�������
	ͨ������SampleGrabber Filter����Ӧ��ID ΪCLSID_SampleGrabber��
	���ص�Ӧ�ó���ʵ�֣���Ҫ����ISampleGrabberCB ��ĳ�Ա����BufferCB��BufferCB �� һ���ɻ�ȡ��ǰͼ��Sample ָ��Ļص�����
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


	//CLSID_FilterGraph��ΪCLSID_SampleGrabber
	hr = pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber, (LPVOID*)&m_pSampGrabber);
	if (FAILED(hr)){
		//MessageBox(TEXT("IID_ISampleGrabber QueryInterface Fail?"));
		TRACE("IID_ISampleGrabber QueryInterface Fail?");
		return hr;
	}

	//������Ƶ��ʽ
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


	//ֻ������SetOneShotΪTRUE�� ��Ϊʹ��SetPositions ����ʼ�շ��� E_NOTIMPL��Method is not supported.���Ϊfalse
	//WaitForCompletion(INFINITE, &evCode)������һֱ�ȴ���ȥ��
	hr = m_pSampGrabber->SetOneShot(FALSE);

	hr = m_pSampGrabber->SetBufferSamples(FALSE);
	//���ûص�
	hr = m_pSampGrabber->SetCallback(g_sampleGrabberCB, 1);

	// find the video window and stuff it in our window

	hr = pFg->QueryInterface(IID_IVideoWindow, (LPVOID *)&pVW);

	if (FAILED(hr)){
		//MessageBox(TEXT("videoWindows fail"));
		TRACE("videoWindows fail");
		return hr;
	}

	TRACE("׼����ʾ��Ƶ�ɹ�\n");
	
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
		TRACE("==================================��ִ�д���,hr=%ld\n", hr);
		break;
	case E_FAIL:
		TRACE("==================================�﷨����,hr=%ld\n", hr);
		break;
	case E_INVALIDARG:
		TRACE("==================================һ������������Ч,hr=%ld\n", hr);
		break;
	case E_NOINTERFACE:
		TRACE("==================================��֧�ִ˽ӿ�,hr=%ld\n", hr);
		break;
	case E_NOTIMPL:
		TRACE("==================================δʵ��,hr=%ld\n", hr);
		break;
	case E_OUTOFMEMORY:
		TRACE("==================================δ�ܷ���������ڴ�,hr=%ld\n", hr);
		break;
	case E_POINTER:
		TRACE("==================================��Ч��ָ��,hr=%ld\n", hr);
		break;
	case E_UNEXPECTED:
		TRACE("==================================δ֪����,hr=%ld\n", hr);
		break;
	case E_HANDLE:
		TRACE("==================================��Ч���,hr=%ld\n", hr);
		break;
	case E_ABORT:
		TRACE("==================================ִ�б�ȡ��,hr=%ld\n", hr);
		break;
	case E_ACCESSDENIED:
		TRACE("==================================��ֹ����,hr=%ld\n", hr);
		break;
	case E_PENDING:
		TRACE("==================================ִ�б�����,hr=%ld\n", hr);
		break;
	default:
		TRACE("==================================default,hr=%ld\n", hr);
	}
	return;
}


//ѡ������ͷ����ʼ��
BOOL CarameVideo::selectDevice(int index){

	//��ʼ������
	HRESULT hr = S_OK;
	BOOL f;
	BOOL fb;
	WCHAR wszVideo[1024], wszAudio[1024];
	IBindCtx *lpBC = 0;
	IMoniker *tmp_pmVideo = 0, *pmAudio = 0;
	wszVideo[1023] = wszAudio[1023] = 0;

	//�жϵ�ǰ����ͷ�Ƿ��ѹ����ɹ�
	f = MakeBuilder();
	if (!f)
	{
		//MessageBox(TEXT("Cannot instantiate graph builder"));
		return FALSE;
	}

	//�����ص�
	fb = MakeCallback();
	if (!fb)
	{
		//MessageBox(TEXT("������Ƶ�ص�ʧ��"));
		return FALSE;
	}


	//������
	hr = CreateBindCtx(0, &lpBC);
	if (SUCCEEDED(hr))
	{
		DWORD dwEaten;
		hr = MkParseDisplayName(lpBC, wszVideo, &dwEaten, &pmVideo);
		hr = MkParseDisplayName(lpBC, wszAudio, &dwEaten, &pmAudio);
		lpBC->Release();
	}

	//ѡ�������
	tmp_pmVideo = rgpmVideoMenu[index];//device
	pmVideo = tmp_pmVideo;
	//gcap.pmAudio = pmAudio;

	if (pmVideo != tmp_pmVideo){
		pmVideo->AddRef();
	}

	pVCap = NULL;

	//CString str1;
	//str1.Format(_T("init success,������:%d"), gcap.pmVideo);
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
		//str.Format(_T("�޷�������Ƶ����ɸѡ��,������:%x"), hr);
		//MessageBox(str);
		return false;
	}
	f = MakeGraph();
	if (!f)
	{
		//MessageBox(TEXT("�޷�ʵ����ɸѡ��ͼ��"));
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

	//ע��ý������¼�
	/*hr = pFg->QueryInterface(IID_IMediaEventEx, (LPVOID*)&m_pMediaEvent);
	if (FAILED(hr))
	return hr;*/

	//״̬ת��
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


//����
void CarameVideo::Run(int index){
	GetAllCapDevices();
	selectDevice(index);
	ToPreview();
}
