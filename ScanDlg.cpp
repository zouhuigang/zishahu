// ScanDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "zshu.h"
#include "ScanDlg.h"
#include "afxdialogex.h"
#include <windows.h>

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


// CScanDlg ��Ϣ�������


void CScanDlg::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//CDialog *pdlg = (CDialog *)AfxGetMainWnd();
	//pdlg->DestroyWindow();
	CDialogEx::OnClose();
}


//������ͷ
/*UINT ChildThread1(LPVOID Param){

	CScanDlg* pc1 = (CScanDlg*)Param;//��ȡ���Ի�����

	if (pc1 != NULL)
	{
		//Sleep(4000);
		::PostMessage(pc1->m_hWnd, WM_UPDATE_STATIC, 0, 0);
	}
	
	return 0;
}
UINT  ChildThread2(LPVOID Param){


	CScanDlg* pc2 = (CScanDlg*)Param;//��ȡ���Ի�����

	if (pc2 != NULL)
	{
			::PostMessage(pc2->m_hWnd, WM_UPDATE_STATIC_2, 0, 0);
		
	}
	return 0;
}*/


//�����߳��е�����
LRESULT CScanDlg::OnUpdateStatic(WPARAM wParam, LPARAM lParam)
{

	

	/*
	cur_gcap = new CarameVideo();
	cur_gcap->Run(0);
	Sleep(100);
	HRESULT hr = S_OK;
	//HWND hwndPreview = NULL;//Ԥ������
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
		MessageBox(TEXT("��Ƶ����Ԥ��1"));
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
	//HWND hwndPreview = NULL;//Ԥ������
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
		MessageBox(TEXT("��Ƶ����Ԥ��2"));
		return hr;
	}
	*/
	return 0;
}

//ѡ���豸
BOOL CScanDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//��֧��com����ľͻ�ʧ��!
	/*HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)){
		MessageBox(TEXT("��֧��COM����ĳ�ʼ�������¶���δ������"));
	}*/

	//�������߳�,ò��ûʲô��
	//OnUpdateStatic(0, 0);
	//OnUpdateStatic2(0,0);

	m_cap.EnumDevices(m_listCtrl.GetSafeHwnd());
	m_listCtrl.SetCurSel(0);

	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
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
		str.Format(_T("���ճɹ�����"));

		MessageBox(str);
	}else{
			MessageBox(_T(" ץͼʧ�� !"));
	}*/

	
	if (m_cap.TakeAPicture())
	{
		CString str;
		str.Format(_T("���ճɹ�����"));

		MessageBox(str);
	}
	else{
		MessageBox(_T(" ץͼʧ�� !"));
	}

	
	
}


void CScanDlg::PostNcDestroy()
{
	// TODO:  �ڴ����ר�ô����/����û���
	CDialogEx::PostNcDestroy();
}


void CScanDlg::OnBnClickedButton2()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	/*HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)){
		MessageBox(TEXT("��֧��COM����ĳ�ʼ�������¶���δ������"));
	}

	if (NULL != cur_gcap){
		delete cur_gcap;
		cur_gcap = NULL;
		TRACE("�ر�����ͷ1\n");
	}
	if (NULL != cur_gcap2){
		delete cur_gcap2;
		cur_gcap2 = NULL;
		TRACE("�ر�����ͷ2\n");
	}

	OnUpdateStatic(0, 0);
	OnUpdateStatic2(0, 0);

	CoUninitialize();
	*/
	HWND hVWindow = m_preview_1.GetSafeHwnd();  // ��ȡ��Ƶ��ʾ���ڵľ��
	//int id = m_listCtrl.GetCurSel();          // ��ȡ��ǰѡ�е���Ƶ�豸
	m_cap.PreviewImages(0, hVWindow);     // ��ʼԤ����Ƶ


	//HWND hVWindow2 = m_preview_2.GetSafeHwnd();  // ��ȡ��Ƶ��ʾ���ڵľ��
	//m_cap2.PreviewImages(1, hVWindow2);     // ��ʼԤ����Ƶ
	
}


void CScanDlg::OnBnClickedButton3()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_cap.Pause();
}


void CScanDlg::OnBnClickedButton4()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_cap.Play();
}
