#include "zkfinger/zkfpengx.h"

#pragma once


// CSignUp 对话框

class CSignUp : public CDialogEx
{
	DECLARE_DYNAMIC(CSignUp)

public:
	CSignUp(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSignUp();

// 对话框数据
	enum { IDD = IDD_SIGNUP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

private:
	VARIANT FRegTemplate;
	long FingerCount;
	long fpcHandle;

	char FFingerNames[100][20];
	long FMatchType;

	CString s_zkfp;
	CString sRegTemplate;
	CString	m_Used;
	CString	m_SN;
	CString	m_COUNT;
	CString	m_Regname;
	CString	m_Hint;
	CZKFPEngX m_zkfp;
public:
	afx_msg void ConnectionFingerprint();//连接指纹仪
	afx_msg void RegisteredFingerprint();//注册指纹
public:
	DECLARE_EVENTSINK_MAP()
	void OnOnimagereceivedZkfpengx1(BOOL* AImageValid);
	void OnOncaptureZkfpengx1(BOOL ActionResult, const VARIANT& ATemplate);
	void OnOnenrollZkfpengx1(BOOL ActionResult, const VARIANT& ATemplate);
	void OnOnfeatureinfoZkfpengx1(long AQuality);
private:
	CString m_mobile;
	BOOL m_startRegistered;//开始注册指纹
	CString m_msg;
public:
	void OnOnfingertouchingZkfpengx1();
	void OnOnfingerleavingZkfpengx1();
	afx_msg void OnClose();
};
