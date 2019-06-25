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


	//连接视频捕捉图像的Filters
	hr = cur_gcap->pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Interleaved, cur_gcap->pVCap, NULL, NULL);
	if (FAILED(hr))
	{
		hr = cur_gcap->pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, cur_gcap->pVCap, NULL, NULL);
		if (FAILED(hr))
		{
			hr = cur_gcap->pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, cur_gcap->pVCap, NULL, NULL);
		}
	}
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

	return TRUE;

}




//选择设备
BOOL CScanDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	//列举摄像头驱动
	GetAllCapDevices();

	//声明摄像头结构体
	_capstuff gcap_1 = { sizeof(_capstuff) };
	_capstuff gcap_2 = { sizeof(_capstuff) };
	selectDevice(&gcap_1,0);
	selectDevice(&gcap_2,1);
	


	//将视频数据显示到mfc界面上，传入容器的id
	ToPreview(IDC_PREVIEW_AVI2, &gcap_1);

	ToPreview(IDC_PREVIEW_AVI, &gcap_2);



	MessageBox(TEXT("打开摄像头成功"));
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CScanDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	
}
