// ScanDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "zshu.h"
#include "ScanDlg.h"
#include "afxdialogex.h"

// CScanDlg �Ի���

IMPLEMENT_DYNAMIC(CScanDlg, CDialogEx)

CScanDlg::CScanDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScanDlg::IDD, pParent)
{

	//��ʼ��
	//_CrtSetBreakAlloc(646); //����ڴ�й©

}

CScanDlg::~CScanDlg()
{
	//free mem
	TRACE("==================================������������\n");

	if (NULL != cur_gcap){
		delete cur_gcap;
	}

	if (NULL != cur_gcap2){
		delete cur_gcap2;
	}

	delete[] gcapList;
	CoUninitialize();
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


// CScanDlg ��Ϣ�������


void CScanDlg::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
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

	//����
	/*hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER,
		IID_ICaptureGraphBuilder2, (LPVOID*) & cur_gcap->pFg);
	if (FAILED(hr))
		return hr;*/
	//TRACE("==================================MakeGraph:%p\n", cur_gcap->pFg);

	return (hr == NOERROR) ? TRUE : FALSE;
}

//��Ƶ�ص�
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


//�鿴��Ƶ�豸
//https://blog.csdn.net/laoyi_grace/article/details/6231429
//https://github.com/asbelldk/bsam_work/blob/08ec8da9aa09714a2ac636eeb1a74aa4ab10337e/DKProject/DirectShow/DlgWebcam.cpp
//https://www.cnblogs.com/nemolog/archive/2005/11/03/268353.html
void CScanDlg::GetAllCapDevices()
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
		MessageBox(TEXT("Error Creatint Device Enumerator"));
		return;
	}
	IEnumMoniker *pEm = 0;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);//ɸѡ����Ƶ�����豸
	if (hr != NOERROR)
	{
		MessageBox(TEXT("Sorry, you have no video capture hardware./r/n/r/n"));
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

void CScanDlg::IMonRelease(IMoniker *&pm)
{
	if (pm)
	{
		pm->Release();
		pm = 0;
	}
}






//������ͷ���ݣ������mfc��picture�ؼ���,��ʼ������ͷDIV_IDΪ����id==IDC_PREVIEW_AVI
HRESULT CScanDlg::ToPreview(int DIV_ID, Carame* cur_gcap){
	HRESULT hr = S_OK;//�����Ƿ�ɹ�
	cur_gcap->pSampleGrabberFilter = NULL;
	cur_gcap->m_pSampGrabber = NULL;


	//�����ͼ����
	/*
	��ͼ�������
	ͨ������SampleGrabber Filter����Ӧ��ID ΪCLSID_SampleGrabber��
	���ص�Ӧ�ó���ʵ�֣���Ҫ����ISampleGrabberCB ��ĳ�Ա����BufferCB��BufferCB �� һ���ɻ�ȡ��ǰͼ��Sample ָ��Ļص�����
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


	//CLSID_FilterGraph��ΪCLSID_SampleGrabber
	hr = cur_gcap->pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber, (LPVOID*)&cur_gcap->m_pSampGrabber);
	if (FAILED(hr)){
		MessageBox(TEXT("IID_ISampleGrabber QueryInterface Fail?"));
		return hr;
	}

	//������Ƶ��ʽ
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
	

	//ֻ������SetOneShotΪTRUE�� ��Ϊʹ��SetPositions ����ʼ�շ��� E_NOTIMPL��Method is not supported.���Ϊfalse
	//WaitForCompletion(INFINITE, &evCode)������һֱ�ȴ���ȥ��
	hr = cur_gcap->m_pSampGrabber->SetOneShot(FALSE);

	hr = cur_gcap->m_pSampGrabber->SetBufferSamples(FALSE);
	//���ûص�
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


//�����ָ������
HRESULT CScanDlg::ShowVideo(Carame* cur_gcap,int DIV_ID){
	HWND hwndPreview = NULL;//Ԥ������
	HRESULT hr = S_OK;//�����Ƿ�ɹ�
	// set up the preview window to be in our dialog instead of floating popup
	GetDlgItem(DIV_ID, &hwndPreview);//���Ԥ�����ڿؼ���Hwnd,ֻ����ʾһ������ͷ
	RECT rc;
	::GetWindowRect(hwndPreview, &rc);
	hr = cur_gcap->pVW->put_Owner((OAHWND)hwndPreview);
	hr = cur_gcap->pVW->put_Left(0);
	hr = cur_gcap->pVW->put_Top(0);
	hr = cur_gcap->pVW->put_Width(rc.right - rc.left);
	hr = cur_gcap->pVW->put_Height(rc.bottom - rc.top);
	hr = cur_gcap->pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
	hr = cur_gcap->pVW->put_Visible(OATRUE);
	//��Ӧ�ó����˳�ǰ��ֹͣͼ��������Ƶ����ΪNULL�����򣬴�����Ϣ���ܱ�����ķ��͸�����Ĵ��ڣ��Ӷ����´�����
	// run the filter graph��IMediaControl�ӿ��ṩ��������Filer Graph�е�����
	//OAFilterState pfs;
	//hr = cur_gcap->m_pMC->GetState(100, &pfs);//����Run֮ǰ
	//TRACE("==================================hr=%d\n", hr);
	hr = cur_gcap->m_pMC->Run();

	//hr = cur_gcap->m_pMC->GetState(100, &pfs);//����Run֮ǰ

	//TRACE("==================================pfs = %d,hr=%d\n", pfs, hr);
	DWORD r = GetLastError();
	TRACE("==================================���� = %d,hr=%d\n", r, hr);

	/*if (hr != S_OK)
	{
		FILTER_STATE fs;
		hr = g_pMC->GetState(1000, (OAFilterState*)&fs);
		switch (fs)
		{
		case State_Stopped:
			MessageBox(NULL, "Graph Stopped", "Info", MB_OK);
			break;
		case State_Paused:
			MessageBox(NULL, "Graph Paused", "Info", MB_OK);
			break;
		case State_Running:
			MessageBox(NULL, "Graph Running", "Info", MB_OK);
			break;
		}
	}*/
	if (FAILED(hr))
	{
		MessageBox(TEXT("��Ƶ����Ԥ��"));
		return hr;
	}
	//�����߳�
	//AfxBeginThread((AFX_THREADPROC)WaitProc, this);
	return hr;
}

void  CScanDlg::GetComExceptionMessage(HRESULT hr)
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
BOOL CScanDlg::selectDevice(Carame* cur_gcap, int index){

	//��ʼ������
	HRESULT hr = S_OK;
	BOOL f;
	BOOL fb;
	WCHAR wszVideo[1024], wszAudio[1024];
	IBindCtx *lpBC = 0;
	IMoniker *pmVideo = 0, *pmAudio = 0;
	wszVideo[1023] = wszAudio[1023] = 0;

	//�жϵ�ǰ����ͷ�Ƿ��ѹ����ɹ�
	f = MakeBuilder(cur_gcap);
	if (!f)
	{
		MessageBox(TEXT("Cannot instantiate graph builder"));
		return FALSE;
	}

	//�����ص�
	fb = MakeCallback(cur_gcap);
	if (!fb)
	{
		MessageBox(TEXT("������Ƶ�ص�ʧ��"));
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
	pmVideo = rgpmVideoMenu[index];//device
	cur_gcap->pmVideo = pmVideo;
	//gcap.pmAudio = pmAudio;

	if (cur_gcap->pmVideo != pmVideo){
		pmVideo->AddRef();
	}

	cur_gcap->pVCap = NULL;

	//CString str1;
	//str1.Format(_T("init success,������:%d"), gcap.pmVideo);
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
		str.Format(_T("�޷�������Ƶ����ɸѡ��,������:%x"), hr);
		MessageBox(str);
		return false;
	}
	f = MakeGraph(cur_gcap);
	if (!f)
	{
		MessageBox(TEXT("�޷�ʵ����ɸѡ��ͼ��"));
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

	//ע��ý������¼�
	/*hr = cur_gcap->pFg->QueryInterface(IID_IMediaEventEx, (LPVOID*)&cur_gcap->m_pMediaEvent);
	if (FAILED(hr))
		return hr;*/

	//״̬ת��
	hr = cur_gcap->pFg->QueryInterface(IID_IMediaControl, (void **)&cur_gcap->m_pMC);
	if (FAILED(hr))return hr;


	return TRUE;

}


//������ͷ
/*UINT ChildThread1(LPVOID Param){

	return 0;
}
UINT  ChildThread2(LPVOID Param){

	return 0;
}*/


//ѡ���豸
BOOL CScanDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	//��֧��com����ľͻ�ʧ��!
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)){
		MessageBox(TEXT("��֧��COM����ĳ�ʼ�������¶���δ������"));
	}
	cur_gcap = new CarameVideo();
	cur_gcap->Run(0);
	HWND hwndPreview = NULL;//Ԥ������
	GetDlgItem(IDC_PREVIEW_AVI_1, &hwndPreview);
	RECT rc;
	::GetWindowRect(hwndPreview, &rc);
	hr = cur_gcap->pVW->put_Owner((OAHWND)hwndPreview);
	hr = cur_gcap->pVW->put_Left(0);
	hr = cur_gcap->pVW->put_Top(0);
	hr = cur_gcap->pVW->put_Width(rc.right - rc.left);
	hr = cur_gcap->pVW->put_Height(rc.bottom - rc.top);
	hr = cur_gcap->pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
	hr = cur_gcap->pVW->put_Visible(OATRUE);
	hr = cur_gcap->m_pMC->Run();
	if (FAILED(hr))
	{
		MessageBox(TEXT("��Ƶ����Ԥ��"));
		return hr;
	}

	cur_gcap2 = new CarameVideo();
	cur_gcap2->Run(0);
	HWND hwndPreview2 = NULL;//Ԥ������
	GetDlgItem(IDC_PREVIEW_AVI_2, &hwndPreview2);
	RECT rc2;
	::GetWindowRect(hwndPreview2, &rc2);
	hr = cur_gcap2->pVW->put_Owner((OAHWND)hwndPreview2);
	hr = cur_gcap2->pVW->put_Left(0);
	hr = cur_gcap2->pVW->put_Top(0);
	hr = cur_gcap2->pVW->put_Width(rc.right - rc.left);
	hr = cur_gcap2->pVW->put_Height(rc.bottom - rc.top);
	hr = cur_gcap2->pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
	hr = cur_gcap2->pVW->put_Visible(OATRUE);
	hr = cur_gcap2->m_pMC->Run();
	if (FAILED(hr))
	{
		MessageBox(TEXT("��Ƶ����Ԥ��"));
		return hr;
	}
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
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
		//�����2������
		takeAPicture(&gcapList[i],i);
	
	}


	CString str;
	str.Format(_T("�������������"));

	MessageBox(str);
	
}


void CScanDlg::PostNcDestroy()
{
	// TODO:  �ڴ����ר�ô����/����û���
	CDialogEx::PostNcDestroy();
}
