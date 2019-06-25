// ScanDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "zshu.h"
#include "ScanDlg.h"
#include "afxdialogex.h"
#include "Amcap/amcap.h"
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

BOOL CScanDlg::MakeBuilder()
{
	// we have one already
	if (gcap.pBuilder)
		return TRUE;

	gcap.pBuilder = new ISampleCaptureGraphBuilder();
	if (NULL == gcap.pBuilder)
	{
		return FALSE;
	}

	return TRUE;
}

// Make a graph object we can use for capture graph building
//
BOOL CScanDlg::MakeGraph()
{
	// we have one already
	if (gcap.pFg)
		return TRUE;

	HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
		IID_IGraphBuilder, (LPVOID *)&gcap.pFg);

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
				MessageBox(strDeviceName);

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





HRESULT CScanDlg::InitAndStartPreview()
{
	/*if (NUMELMS(gcap.rgpmVideoMenu)>0)
	{
		IPropertyBag *m_pVcap;
		HRESULT hr = gcap.rgpmVideoMenu[0]->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_pVcap);//将选择的设备绑定到源滤镜
		if (!m_pVcap)
		{
			MessageBox(TEXT("There is NO USB/Web Camera for video capture!Please check!"));
			return E_FAIL;
		}

		
		hr = CoCreateInstance(CLSID_FilterGraph);
		if (!m_pCaptureGraph)
		{
			MessageBox(TEXT("Could not create filter graph"));
			return E_FAIL;
		}
		hr = m_pCaptureGraph->AddFilter(m_pVcap, L"VideoPreview");
		if (FAILED(hr))
		{
			MessageBox(TEXT("Could not put capture device in graph"));
			return E_FAIL;
		}

		ICaptureGraphBuilder2 *pBuilder;
		hr = CoCreateInstance((REFCLSID)CLSID_CaptureGraphBuilder2,
			NULL, CLSCTX_INPROC, (REFIID)IID_ICaptureGraphBuilder2,
			(void **)&pBuilder);

		//hr = m_pBuilder.CoCreateInstance(CLSID_CaptureGraphBuilder2);
		if (!pBuilder)
		{
			MessageBox(TEXT("Could not create capture graph builder2"));
			return hr;
		}

		hr = pBuilder->SetFiltergraph(m_pCaptureGraph);                    //
		if (hr != NOERROR)
		{
			MessageBox(TEXT("Cannot give graph to builder"));
			return E_FAIL;
		}
		//连接视频捕捉图像的Filters
		hr = pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Interleaved, m_pVcap, NULL, NULL);
		if (FAILED(hr))
		{
			hr = pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_pVcap, NULL, NULL);
			if (FAILED(hr))
			{
				hr = pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_pVcap, NULL, NULL);
			}
		}
		// find the video window and stuff it in our window
		CComQIPtr< IVideoWindow, &IID_IVideoWindow > pWindow = m_pCaptureGraph;           //VideoWindow接口
		if (!pWindow)
		{
			MessageBox(TEXT("Could not get video window interface"));
			return E_FAIL;
		}
		// set up the preview window to be in our dialog instead of floating popup
		HWND hwndPreview = NULL;
		GetDlgItem(IDC_PREVIEW_AVI, &hwndPreview);//获得预览窗口控件的Hwnd
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
		CComQIPtr< IMediaControl, &IID_IMediaControl > pControl = m_pCaptureGraph;
		hr = pControl->Run();

		if (FAILED(hr))
		{
			MessageBox(TEXT("Could not run graph"));
			return hr;
		}

	}*/
	return 0;
}

HRESULT CScanDlg::InitCaptureGraphBuilder()
{
	HRESULT hr;

	// 创建IGraphBuilder接口
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&m_pGB);
	// 创建ICaptureGraphBuilder2接口
	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC,
		IID_ICaptureGraphBuilder2, (void **)&m_pCapture);
	if (FAILED(hr))return hr;
	m_pCapture->SetFiltergraph(m_pGB);
	hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);
	if (FAILED(hr))return hr;
	hr = m_pGB->QueryInterface(IID_IVideoWindow, (LPVOID *)&m_pVW);
	if (FAILED(hr))return hr;
	return hr;
}

bool CScanDlg::BindFilter(int deviceId, IBaseFilter **pFilter)
{
	if (deviceId < 0)
		return false;

	// enumerate all video capture devices
	CComPtr<ICreateDevEnum> pCreateDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	if (hr != NOERROR)
	{
		return false;
	}
	CComPtr<IEnumMoniker> pEm;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
	if (hr != NOERROR)
	{
		return false;
	}
	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;
	int index = 0;
	while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK, index <= deviceId)
	{
		IPropertyBag *pBag;
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if (SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR)
			{
				if (index == deviceId)
				{
					pM->BindToObject(0, 0, IID_IBaseFilter, (void**)pFilter);
				}
				SysFreeString(var.bstrVal);
			}
			pBag->Release();
		}
		pM->Release();
		index++;
	}
	return true;
}

//选择设备
BOOL CScanDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//选择驱动设备

	

	//打开摄像头


	//列举摄像头
	GetAllCapDevices();

	

	WCHAR wszVideo[1024], wszAudio[1024];
	BOOL f;
	HRESULT hr = S_OK;
	IBindCtx *lpBC = 0;
	f = MakeBuilder();
	IMoniker *pmVideo = 0, *pmAudio = 0;
	wszVideo[1023] = wszAudio[1023] = 0;
	if (!f)
	{
		MessageBox(TEXT("Cannot instantiate graph builder"));
		return FALSE;
	}
	//列举摄像头
	//GetAllCapDevices();
	hr = CreateBindCtx(0, &lpBC);
	if (SUCCEEDED(hr))
	{
		DWORD dwEaten;
		hr = MkParseDisplayName(lpBC, wszVideo, &dwEaten, &pmVideo);
		hr = MkParseDisplayName(lpBC, wszAudio, &dwEaten, &pmAudio);
		lpBC->Release();
	}
	

	pmVideo = gcap.rgpmVideoMenu[1];//device

	gcap.pmVideo = pmVideo;
	//gcap.pmAudio = pmAudio;

	if(gcap.pmVideo != pmVideo){
		pmVideo->AddRef();
	}

	gcap.pVCap = NULL;

	//CString str1;
	//str1.Format(_T("init success,错误码:%d"), gcap.pmVideo);
	//MessageBox(str1);
	if (gcap.pmVideo != 0)
	{
		MessageBox(TEXT("aaaaa"));
		IPropertyBag *pBag;
		gcap.wachFriendlyName[0] = 0;
		hr = gcap.pmVideo->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if (SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR)
			{
				hr = StringCchCopyW(gcap.wachFriendlyName, sizeof(gcap.wachFriendlyName) / sizeof(gcap.wachFriendlyName[0]), var.bstrVal);
				SysFreeString(var.bstrVal);
			}
			pBag->Release();
		}
		hr = gcap.pmVideo->BindToObject(0, 0, IID_IBaseFilter, (void**)&gcap.pVCap);
	}
	if (gcap.pVCap == NULL)
	{
		CString str;
		str.Format(_T("无法创建视频捕获筛选器,错误码:%x"), hr);
		MessageBox(str);
		return false;
	}
	f = MakeGraph();
	if (!f)
	{
		MessageBox(TEXT("无法实例化筛选器图形"));
		return FALSE;
	}
	hr = gcap.pBuilder->SetFiltergraph(gcap.pFg);
	if (hr != NOERROR)
	{
		MessageBox(TEXT("Cannot give graph to builder"));
		return FALSE;
	}
	// Add the video capture filter to the graph with its friendly name
	hr = gcap.pFg->AddFilter(gcap.pVCap, gcap.wachFriendlyName);
	if (hr != NOERROR)
	{
		CString str;
		str.Format(_T("Error %x: Cannot add vidcap to filtergraph"), hr);
		MessageBox(str);
		return FALSE;
	}

	hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Interleaved, gcap.pVCap,
		IID_IAMVideoCompression, (void **)&gcap.pVC);
	if (hr != S_OK)
	{
		hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
			&MEDIATYPE_Video, gcap.pVCap,
			IID_IAMVideoCompression, (void **)&gcap.pVC);
	}

	// !!! What if this interface isn't supported?
	// we use this interface to set the frame rate and get the capture size
	hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Interleaved,
		gcap.pVCap, IID_IAMStreamConfig, (void **)&gcap.pVSC);

	if (hr != NOERROR)
	{
		hr = gcap.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
			&MEDIATYPE_Video, gcap.pVCap,
			IID_IAMStreamConfig, (void **)&gcap.pVSC);
		if (hr != NOERROR)
		{
			// this means we can't set frame rate (non-DV only)
			MessageBox(TEXT("Cannot find VCapture:IAMStreamConfig"));
		}
	}

	gcap.fCapAudioIsRelevant = TRUE;


	//连接视频捕捉图像的Filters
	hr = gcap.pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Interleaved, gcap.pVCap, NULL, NULL);
	if (FAILED(hr))
	{
		hr = gcap.pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, gcap.pVCap, NULL, NULL);
		if (FAILED(hr))
		{
			hr = gcap.pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, gcap.pVCap, NULL, NULL);
		}
	}
	// find the video window and stuff it in our window
	CComQIPtr< IVideoWindow, &IID_IVideoWindow > pWindow = gcap.pFg;           //VideoWindow接口
	if (!pWindow)
	{
		MessageBox(TEXT("Could not get video window interface"));
		return E_FAIL;
	}
	// set up the preview window to be in our dialog instead of floating popup
	HWND hwndPreview = NULL;
	GetDlgItem(IDC_PREVIEW_AVI, &hwndPreview);//获得预览窗口控件的Hwnd
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
	CComQIPtr< IMediaControl, &IID_IMediaControl > pControl = gcap.pFg;
	hr = pControl->Run();

	if (FAILED(hr))
	{
		MessageBox(TEXT("Could not run graph"));
		return hr;
	}
	MessageBox(TEXT("打开摄像头成功"));
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CScanDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	
}
