
#pragma once

class CzshuDlg : public CDialogEx
{
public:
	CzshuDlg(CWnd* pParent = NULL);
	
	enum { IDD = IDD_ZSHU_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnClose();
};
