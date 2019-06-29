#include "zkfinger/zkfpengx.h"

#pragma once


// CSignUp �Ի���

class CSignUp : public CDialogEx
{
	DECLARE_DYNAMIC(CSignUp)

public:
	CSignUp(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSignUp();

// �Ի�������
	enum { IDD = IDD_SIGNUP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
	afx_msg void ConnectionFingerprint();//����ָ����
	afx_msg void RegisteredFingerprint();//ע��ָ��
public:
	DECLARE_EVENTSINK_MAP()
	void OnOnimagereceivedZkfpengx1(BOOL* AImageValid);
	void OnOncaptureZkfpengx1(BOOL ActionResult, const VARIANT& ATemplate);
	void OnOnenrollZkfpengx1(BOOL ActionResult, const VARIANT& ATemplate);
	void OnOnfeatureinfoZkfpengx1(long AQuality);
private:
	CString m_mobile;
	BOOL m_startRegistered;//��ʼע��ָ��
	CString m_msg;
public:
	void OnOnfingertouchingZkfpengx1();
	void OnOnfingerleavingZkfpengx1();
	afx_msg void OnClose();
};
