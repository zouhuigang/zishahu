// ScanDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "zshu.h"
#include "ScanDlg.h"
#include "afxdialogex.h"
#include <windows.h>

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
	TRACE("==================================调用析构函数\n");
}


void CScanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PREVIEW_AVI_1, m_preview_1);
	DDX_Control(pDX, IDC_PREVIEW_AVI_2, m_preview_2);
	DDX_Control(pDX, IDC_COMBO1, m_listCtrl);
}


BEGIN_MESSAGE_MAP(CScanDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, &CScanDlg::OnBnClickedButton1)
	//ON_MESSAGE(WM_UPDATE_STATIC, &CScanDlg::OnUpdateStatic)
	//ON_MESSAGE(WM_UPDATE_STATIC_2, &CScanDlg::OnUpdateStatic2)
	ON_BN_CLICKED(IDC_BUTTON2, &CScanDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CScanDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CScanDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CScanDlg 消息处理程序


void CScanDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	//CDialog *pdlg = (CDialog *)AfxGetMainWnd();
	//pdlg->DestroyWindow();
	CDialogEx::OnClose();
}


//子摄像头
/*UINT ChildThread1(LPVOID Param){

	CScanDlg* pc1 = (CScanDlg*)Param;//获取主对话框类

	if (pc1 != NULL)
	{
		//Sleep(4000);
		::PostMessage(pc1->m_hWnd, WM_UPDATE_STATIC, 0, 0);
	}
	
	return 0;
}
UINT  ChildThread2(LPVOID Param){


	CScanDlg* pc2 = (CScanDlg*)Param;//获取主对话框类

	if (pc2 != NULL)
	{
			::PostMessage(pc2->m_hWnd, WM_UPDATE_STATIC_2, 0, 0);
		
	}
	return 0;
}*/


//处理线程中的数据
LRESULT CScanDlg::OnUpdateStatic(WPARAM wParam, LPARAM lParam)
{

	

	/*
	cur_gcap = new CarameVideo();
	cur_gcap->Run(0);
	Sleep(100);
	HRESULT hr = S_OK;
	//HWND hwndPreview = NULL;//预览窗口
	//GetDlgItem(IDC_PREVIEW_AVI_1, &hwndPreview);
	CWnd* pWnd = GetDlgItem(IDC_PREVIEW_AVI_1);
	pWnd->ModifyStyle(0, WS_CLIPCHILDREN);
	RECT rc;
	//::GetWindowRect(hwndPreview, &rc);
	pWnd->GetWindowRect(&rc);
	//hr = cur_gcap->pVW->put_Owner((OAHWND)hwndPreview);
	hr = cur_gcap->pVW->put_Owner((OAHWND)pWnd->GetSafeHwnd());
	hr = cur_gcap->pVW->put_Left(0);
	hr = cur_gcap->pVW->put_Top(0);
	hr = cur_gcap->pVW->put_Width(rc.right - rc.left);
	hr = cur_gcap->pVW->put_Height(rc.bottom - rc.top);
	hr = cur_gcap->pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
	hr = cur_gcap->pVW->put_Visible(OATRUE);
	hr = cur_gcap->m_pMC->Run();
	DWORD err=GetLastError();
	TRACE("hr:%ld,error:%d\n", hr, err);
	if (FAILED(hr))
	{
		MessageBox(TEXT("视频不能预览1"));
		return hr;
	}
	*/
	return 0;
}

LRESULT CScanDlg::OnUpdateStatic2(WPARAM wParam, LPARAM lParam)
{
	/*
	cur_gcap2 = new CarameVideo();
	cur_gcap2->Run(1);
	
	HRESULT hr = S_OK;
	//HWND hwndPreview = NULL;//预览窗口
	//GetDlgItem(IDC_PREVIEW_AVI_2, &hwndPreview);
	CWnd* pWnd = GetDlgItem(IDC_PREVIEW_AVI_2);
	pWnd->ModifyStyle(0, WS_CLIPCHILDREN);

	
	RECT rc;
	pWnd->GetWindowRect(&rc);
	hr = cur_gcap2->pVW->put_Owner((OAHWND)pWnd->GetSafeHwnd());
	//::GetWindowRect(hwndPreview, &rc);
	//hr = cur_gcap2->pVW->put_Owner((OAHWND)hwndPreview);
	hr = cur_gcap2->pVW->put_Left(0);
	hr = cur_gcap2->pVW->put_Top(0);
	hr = cur_gcap2->pVW->put_Width(rc.right - rc.left);
	hr = cur_gcap2->pVW->put_Height(rc.bottom - rc.top);
	hr = cur_gcap2->pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
	hr = cur_gcap2->pVW->put_Visible(OATRUE);
	hr = cur_gcap2->m_pMC->Run();
	DWORD err = GetLastError();
	TRACE("hr:%ld,error:%d\n", hr, err);
	if (FAILED(hr))
	{
		MessageBox(TEXT("视频不能预览2"));
		return hr;
	}
	*/
	return 0;
}

//选择设备
BOOL CScanDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//不支持com组件的就会失败!
	/*HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)){
		MessageBox(TEXT("不支持COM组件的初始化，导致动画未被载入"));
	}*/

	//开启多线程,貌似没什么用
	//OnUpdateStatic(0, 0);
	//OnUpdateStatic2(0,0);

	m_cap.EnumDevices(m_listCtrl.GetSafeHwnd());
	m_listCtrl.SetCurSel(0);

	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}



void CScanDlg::OnBnClickedButton1()
{

	//cur_gcap->takeAPicture(0);
	//cur_gcap2->takeAPicture(1);
	/*TCHAR m_chSwapStr[MAX_PATH];
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	StringCchPrintf(m_chSwapStr, MAX_PATH, TEXT("\\%s_%04i%02i%02i%02i%02i%02i%03i.bmp"),
		"zzz",
		sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour,
		sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
	if (m_cap.CaptureBitmap((const   char  *)m_chSwapStr))
	{
		CString str;
		str.Format(_T("拍照成功！！"));

		MessageBox(str);
	}else{
			MessageBox(_T(" 抓图失败 !"));
	}*/

	
	if (m_cap.TakeAPicture())
	{
		CString str;
		str.Format(_T("拍照成功！！"));

		MessageBox(str);
	}
	else{
		MessageBox(_T(" 抓图失败 !"));
	}

	
	
}


void CScanDlg::PostNcDestroy()
{
	// TODO:  在此添加专用代码和/或调用基类
	CDialogEx::PostNcDestroy();
}


void CScanDlg::OnBnClickedButton2()
{
	// TODO:  在此添加控件通知处理程序代码

	/*HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)){
		MessageBox(TEXT("不支持COM组件的初始化，导致动画未被载入"));
	}

	if (NULL != cur_gcap){
		delete cur_gcap;
		cur_gcap = NULL;
		TRACE("关闭摄像头1\n");
	}
	if (NULL != cur_gcap2){
		delete cur_gcap2;
		cur_gcap2 = NULL;
		TRACE("关闭摄像头2\n");
	}

	OnUpdateStatic(0, 0);
	OnUpdateStatic2(0, 0);

	CoUninitialize();
	*/
	HWND hVWindow = m_preview_1.GetSafeHwnd();  // 获取视频显示窗口的句柄
	//int id = m_listCtrl.GetCurSel();          // 获取当前选中的视频设备
	m_cap.PreviewImages(0, hVWindow);     // 开始预览视频


	//HWND hVWindow2 = m_preview_2.GetSafeHwnd();  // 获取视频显示窗口的句柄
	//m_cap2.PreviewImages(1, hVWindow2);     // 开始预览视频
	
}


void CScanDlg::OnBnClickedButton3()
{
	// TODO:  在此添加控件通知处理程序代码
	m_cap.Pause();
}


void CScanDlg::OnBnClickedButton4()
{
	// TODO:  在此添加控件通知处理程序代码
	m_cap.Play();
}
