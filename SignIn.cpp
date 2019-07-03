// SignIn.cpp : 实现文件
//

#include "stdafx.h"
#include "zshu.h"
#include "SignIn.h"
#include "afxdialogex.h"
#include "curl/curl.h"
#include "json/json.h"

// CSignIn 对话框

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


// CSignIn 消息处理程序


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
		MessageBox(TEXT("指纹信息不能为空"));
		return;
	}





}


BOOL CSignIn::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

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

	//自动连接指纹仪
	ConnectionFingerprint();


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


//连接指纹仪
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
		MessageBox(TEXT("指纹仪初始化失败，请检查指纹仪是否插好..."));
}BEGIN_EVENTSINK_MAP(CSignIn, CDialogEx)
ON_EVENT(CSignIn, IDC_ZKFPENGX1, 5, CSignIn::OnOnfeatureinfoZkfpengx1, VTS_I4)
ON_EVENT(CSignIn, IDC_ZKFPENGX1, 8, CSignIn::OnOnimagereceivedZkfpengx1, VTS_PBOOL)
ON_EVENT(CSignIn, IDC_ZKFPENGX1, 10, CSignIn::OnOncaptureZkfpengx1, VTS_BOOL VTS_VARIANT)
END_EVENTSINK_MAP()


void CSignIn::OnOnfeatureinfoZkfpengx1(long AQuality)
{
	// TODO:  在此处添加消息处理程序代码
}


void CSignIn::OnOnimagereceivedZkfpengx1(BOOL* AImageValid)
{
	HDC hdc;

	hdc = this->GetDC()->m_hDC;

	//MessageBox(TEXT("获取到了图像"));//m_zkfp.GetImageWidth() m_zkfp.GetImageHeight()
	m_zkfp.PrintImageAt(long(hdc), 100, 136, 270, 160);
}


void CSignIn::OnOncaptureZkfpengx1(BOOL ActionResult, const VARIANT& ATemplate)
{
	// TODO:  在此处添加消息处理程序代码
	long fi;
	long Score, ProcessNum;
	BOOL RegChanged;
	CString sTemp;
	Score = 8;

	//获取到的指纹
	sTemp = m_zkfp.GetTemplateAsString();

	fi = m_zkfp.IdentificationFromStrInFPCacheDB(fpcHandle, (LPCTSTR)sTemp, &Score, &ProcessNum);
	if (fi == -1){
		m_mobile = "";//清空变量
		m_sign = "";
		m_push_time = "";
		UpdateData(FALSE);
		//MessageBox(TEXT("指纹验证失败"));
	}else{
		FingerTpl *user = ldb.GetUserInfo(fi);
		//sprintf(buffer, "验证成功,手机号 = %s 分数 = %d  进程号 = %d,注册 = %s", user->GetMobile(), Score, ProcessNum, user->GetPushTime());
		m_mobile = user->GetMobile();
		m_sign = user->GetSign();
		m_push_time = user->GetPushTime();
		UpdateData(FALSE);

		//delete user
		//MessageBox(TEXT("验证成功"));
		delete user;
		user = NULL;
	}
}


void CSignIn::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_zkfp.EndEngine();//注销指纹
	CDialog *pdlg = (CDialog *)AfxGetMainWnd();
	pdlg->DestroyWindow();
	CDialogEx::OnClose();
}
