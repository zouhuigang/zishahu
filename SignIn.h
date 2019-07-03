#include "zkfinger/zkfpengx.h"
#include "database.h"
#pragma once


// CSignIn 对话框

class CSignIn : public CDialogEx
{
	DECLARE_DYNAMIC(CSignIn)

public:
	CSignIn(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSignIn();

// 对话框数据
	enum { IDD = IDD_SIGNIN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	database ldb;
	VARIANT FRegTemplate;
	long fpcHandle;
	Tpl *tplList;
	CString m_mobile;
	CString m_push_time;
	CString m_sign;
	CString	m_Used;
	CString	m_SN;
	CString	m_COUNT;
	afx_msg void ConnectionFingerprint();
public:
	afx_msg void OnBnClickedButton1();
private:
	CZKFPEngX m_zkfp;
public:
	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	void OnOnfeatureinfoZkfpengx1(long AQuality);
	void OnOnimagereceivedZkfpengx1(BOOL* AImageValid);
	void OnOncaptureZkfpengx1(BOOL ActionResult, const VARIANT& ATemplate);
	afx_msg void OnClose();
};
