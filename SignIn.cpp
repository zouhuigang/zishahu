// SignIn.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "zshu.h"
#include "SignIn.h"
#include "afxdialogex.h"
#include "curl/curl.h"
#include "json/json.h"

// CSignIn �Ի���

IMPLEMENT_DYNAMIC(CSignIn, CDialogEx)

CSignIn::CSignIn(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSignIn::IDD, pParent)
	, m_mobile(_T(""))
{

}

CSignIn::~CSignIn()
{
}

void CSignIn::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_mobile);
	DDX_Control(pDX, IDC_ZKFPENGX1, m_zkfp);
}


BEGIN_MESSAGE_MAP(CSignIn, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CSignIn::OnBnClickedButton1)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CSignIn ��Ϣ�������


void CSignIn::OnBnClickedButton1()
{
	UpdateData(TRUE);
	CString strValue = _T("{\"key1\":\"value1\"}");
	Json::Reader reader;
	Json::Value value;

	CString temp;
	USES_CONVERSION;
	char* cJson = T2A(strValue.GetBuffer(0));
	strValue.ReleaseBuffer();

	if (reader.parse(cJson, value))
	{
		temp = value["key1"].asCString();
		MessageBox(temp);
	}


	// click sign in
	if (m_mobile.IsEmpty() || m_sign.IsEmpty() || m_push_time.IsEmpty()){
		MessageBox(TEXT("ָ����Ϣ����Ϊ��"));
		return;
	}





}


BOOL CSignIn::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	fpcHandle = m_zkfp.CreateFPCacheDB();
	VariantInit(&FRegTemplate);

	tplList = ldb.LoadFingerprintList();
	CString strTemp;
	for (int i = 0; i < ldb.FingerCount; i++) {
		strTemp = tplList[i].template_10;
		m_zkfp.AddRegTemplateStrToFPCacheDB(fpcHandle, tplList[i].id, (LPCTSTR)strTemp);
		TRACE("=====================================new mobile = %s,id=%d\n", tplList[i].mobile, tplList[i].id);
	}
	free(tplList);

	//�Զ�����ָ����
	ConnectionFingerprint();


	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


//����ָ����
void CSignIn::ConnectionFingerprint()
{
	char buffer[10];

	if (m_zkfp.InitEngine() == 0){
		m_SN = m_zkfp.GetSensorSN();
		ltoa(m_zkfp.GetSensorIndex(), buffer, 10);
		m_Used = buffer;
		ltoa(m_zkfp.GetSensorCount(), buffer, 10);
		m_COUNT = buffer;
	}
	else
		MessageBox(TEXT("ָ���ǳ�ʼ��ʧ�ܣ�����ָ�����Ƿ���..."));
}BEGIN_EVENTSINK_MAP(CSignIn, CDialogEx)
ON_EVENT(CSignIn, IDC_ZKFPENGX1, 5, CSignIn::OnOnfeatureinfoZkfpengx1, VTS_I4)
ON_EVENT(CSignIn, IDC_ZKFPENGX1, 8, CSignIn::OnOnimagereceivedZkfpengx1, VTS_PBOOL)
ON_EVENT(CSignIn, IDC_ZKFPENGX1, 10, CSignIn::OnOncaptureZkfpengx1, VTS_BOOL VTS_VARIANT)
END_EVENTSINK_MAP()


void CSignIn::OnOnfeatureinfoZkfpengx1(long AQuality)
{
	// TODO:  �ڴ˴������Ϣ����������
}


void CSignIn::OnOnimagereceivedZkfpengx1(BOOL* AImageValid)
{
	HDC hdc;

	hdc = this->GetDC()->m_hDC;

	//MessageBox(TEXT("��ȡ����ͼ��"));//m_zkfp.GetImageWidth() m_zkfp.GetImageHeight()
	m_zkfp.PrintImageAt(long(hdc), 100, 136, 270, 160);
}


void CSignIn::OnOncaptureZkfpengx1(BOOL ActionResult, const VARIANT& ATemplate)
{
	// TODO:  �ڴ˴������Ϣ����������
	long fi;
	long Score, ProcessNum;
	BOOL RegChanged;
	CString sTemp;
	Score = 8;

	//��ȡ����ָ��
	sTemp = m_zkfp.GetTemplateAsString();

	fi = m_zkfp.IdentificationFromStrInFPCacheDB(fpcHandle, (LPCTSTR)sTemp, &Score, &ProcessNum);
	if (fi == -1){
		m_mobile = "";//��ձ���
		m_sign = "";
		m_push_time = "";
		UpdateData(FALSE);
		//MessageBox(TEXT("ָ����֤ʧ��"));
	}else{
		FingerTpl *user = ldb.GetUserInfo(fi);
		//sprintf(buffer, "��֤�ɹ�,�ֻ��� = %s ���� = %d  ���̺� = %d,ע�� = %s", user->GetMobile(), Score, ProcessNum, user->GetPushTime());
		m_mobile = user->GetMobile();
		m_sign = user->GetSign();
		m_push_time = user->GetPushTime();
		UpdateData(FALSE);

		//delete user
		//MessageBox(TEXT("��֤�ɹ�"));
		delete user;
		user = NULL;
	}
}


void CSignIn::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_zkfp.EndEngine();//ע��ָ��
	CDialog *pdlg = (CDialog *)AfxGetMainWnd();
	pdlg->DestroyWindow();
	CDialogEx::OnClose();
}
