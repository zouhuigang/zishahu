#include "zkfinger/zkfpengx.h"
#include "database.h"
#pragma once

class CSignIn : public CDialogEx
{
	DECLARE_DYNAMIC(CSignIn)

public:
	CSignIn(CWnd* pParent = NULL);

	enum { IDD = IDD_SIGNIN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

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
	void ConnectionFingerprint();
	CString toCString(string name);
	CZKFPEngX m_zkfp_1;
	long conZKFP=2;
public:
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	void OnOnfeatureinfoZkfpengx1(long AQuality);
	void OnOnimagereceivedZkfpengx1(BOOL* AImageValid);
	void OnOncaptureZkfpengx1(BOOL ActionResult, const VARIANT& ATemplate);
	afx_msg void OnClose();
};
