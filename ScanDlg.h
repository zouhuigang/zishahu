#include <streams.h>

#include <windows.h>
#include <dbt.h>
#include <mmreg.h>
#include <msacm.h>
#include <fcntl.h>
#include <io.h>
#pragma once
#include <stdio.h>
#include <commdlg.h>
#include <atlbase.h>
#include <strsafe.h>
#include "stdafx.h"
#include "CCaptureClass.h"
#include "afxwin.h"
#define WM_UPDATE_STATIC (WM_USER + 100) 
#define WM_UPDATE_STATIC_2 (WM_USER + 101)
UINT ChildThread1(LPVOID Param);
UINT ChildThread2(LPVOID Param);
 



class CScanDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScanDlg)

public:
	CScanDlg(CWnd* pParent = NULL);
	virtual ~CScanDlg();
	enum { IDD = IDD_SCAN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnUpdateStatic(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateStatic2(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	//CarameVideo * cur_gcap;
	//CarameVideo * cur_gcap2;

private:
	CStatic m_preview_1;
	CStatic m_preview_2;
	CComboBox m_listCtrl;
	CCaptureClass m_cap;
	CCaptureClass m_cap2;

public:
	afx_msg void OnBnClickedButton1();
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedButton2();
};
