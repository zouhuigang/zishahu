// SignUp.cpp : 实现文件
//

#include "stdafx.h"
#include "zshu.h"
#include "SignUp.h"
#include "afxdialogex.h"


// CSignUp 对话框

IMPLEMENT_DYNAMIC(CSignUp, CDialogEx)

CSignUp::CSignUp(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSignUp::IDD, pParent)
	, m_mobile(_T(""))
	, m_msg(_T(""))
{
	m_startRegistered = TRUE;//进入页面就可以登记指纹啦。
}

CSignUp::~CSignUp()
{
}

void CSignUp::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	//指纹仪

	DDX_Control(pDX, IDC_ZKFPENGX1, m_zkfp);
	DDX_Text(pDX, IDC_EDIT2, m_mobile);
	DDV_MaxChars(pDX, m_mobile, 11);
	DDX_Text(pDX, IDC_EDIT5, m_msg);
}


BEGIN_MESSAGE_MAP(CSignUp, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &CSignUp::RegisteredFingerprint)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CSignUp 消息处理程序


BOOL CSignUp::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//指纹库初始化
	FingerCount = 0;
	fpcHandle = m_zkfp.CreateFPCacheDB();
	VariantInit(&FRegTemplate);

	//自动连接指纹仪
	ConnectionFingerprint();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


//连接指纹仪
void CSignUp::ConnectionFingerprint()
{
		char buffer[10];

		//CheckRadioButton(IDC_RADIOBMP, IDC_RADIOJPG, IDC_RADIOBMP);
		// TODO: Add your control notification handler code here
		if (m_zkfp.InitEngine() == 0){
			m_SN = m_zkfp.GetSensorSN();
			ltoa(m_zkfp.GetSensorIndex(), buffer, 10);
			m_Used = buffer;
			ltoa(m_zkfp.GetSensorCount(), buffer, 10);
			m_COUNT = buffer;
			//SetDlgItemText(IDC_EDUsed, m_Used);
			//SetDlgItemText(IDC_EDCount, m_COUNT);
			//SetDlgItemText(IDC_EDSN, m_SN);
			//MessageBox(TEXT("Initial Succeed"));
		}
		else
			MessageBox(TEXT("指纹仪初始化失败，请检查指纹仪是否插好..."));
		FMatchType = 0;
}

//注册指纹
void CSignUp::RegisteredFingerprint(){
	UpdateData(TRUE);//同步控件数据
	CString str;
	
	if (m_mobile.Compare(TEXT("")) == 0){
		str.Format(_T("请先填写手机号，再录入指纹!"));
	}else{
		//开始登记指纹，登记结束后发生 OnEnroll 事件
		m_zkfp.BeginEnroll();
		str.Format(_T("开始录入指纹!"));
		//因为已经开始登记指纹了，所以不需要再开始登记注册了
		m_startRegistered = FALSE;
	}
	
	m_msg = str;

	UpdateData(FALSE);
	return;
	//CancelEnroll() 取消当前的指纹登记状态，即由 BeginEnroll 开始的操作可由此函数中断。 
	//SetDlgItemText(IDC_EDHint, "start register");

}

BEGIN_EVENTSINK_MAP(CSignUp, CDialogEx)
	//注册指纹仪事件
	ON_EVENT(CSignUp, IDC_ZKFPENGX1, 8, CSignUp::OnOnimagereceivedZkfpengx1, VTS_PBOOL)
	ON_EVENT(CSignUp, IDC_ZKFPENGX1, 10, CSignUp::OnOncaptureZkfpengx1, VTS_BOOL VTS_VARIANT)
	ON_EVENT(CSignUp, IDC_ZKFPENGX1, 9, CSignUp::OnOnenrollZkfpengx1, VTS_BOOL VTS_VARIANT)
	ON_EVENT(CSignUp, IDC_ZKFPENGX1, 5, CSignUp::OnOnfeatureinfoZkfpengx1, VTS_I4)
	ON_EVENT(CSignUp, IDC_ZKFPENGX1, 1, CSignUp::OnOnfingertouchingZkfpengx1, VTS_NONE)
	ON_EVENT(CSignUp, IDC_ZKFPENGX1, 3, CSignUp::OnOnfingerleavingZkfpengx1, VTS_NONE)
END_EVENTSINK_MAP()


void CSignUp::OnOnimagereceivedZkfpengx1(BOOL* AImageValid)
{
	/*
	设备取到指纹图像或者通过AddImageFile和AddBitmap加入指纹图像时 
	调用该事件，AImageValid 表示是否进行模板提取，
	设置为 False 后，
	系统在 取到指纹图像后返回，不进行模板提取
	*/
	HDC hdc;

	hdc = this->GetDC()->m_hDC;

	MessageBox(TEXT("获取到了图像"));//m_zkfp.GetImageWidth() m_zkfp.GetImageHeight()
	m_zkfp.PrintImageAt(long(hdc), 310, 60,350 ,250);
}


void CSignUp::OnOncaptureZkfpengx1(BOOL ActionResult, const VARIANT& ATemplate)
{
	/*
	取到用于比对的指纹验证模板，模版保存到文件中，文件名称为属性 VerTplFileName 设置，
	ActionResult =true 表示成功取到指纹模版；
	False 表 示失败, 如果 VerTplFileName 没有设置或者为空，
	则不产生保存文件，但本 事件仍然会触发。 
	*/
	MessageBox(TEXT("OnOncaptureZkfpengx1"));
	/*
	long fi;
	long Score, ProcessNum;
	BOOL RegChanged;
	char buffer[80];
	CString sTemp;
	BSTR bTemp;

	sTemp = m_zkfp.GetTemplateAsString();
	SetDlgItemText(IDC_EDHint, "Acquired Fingerprint template");
	if (FMatchType == 1){
		bTemp = sRegTemplate.AllocSysString();
		//if (m_zkfp.VerFinger(&FRegTemplate, ATemplate, FALSE, &RegChanged))
		if (m_zkfp.VerFingerFromStr(&bTemp, (LPCTSTR)sTemp, FALSE, &RegChanged))
			MessageBox("Verify Succeed");
		else
			MessageBox("Verify Failed");

	}
	else if (FMatchType == 2){  //1:N
		Score = 8;
		//fi = m_zkfp.IdentificationInFPCacheDB(fpcHandle, ATemplate, &Score, &ProcessNum);
		fi = m_zkfp.IdentificationFromStrInFPCacheDB(fpcHandle, (LPCTSTR)sTemp, &Score, &ProcessNum);
		if (fi == -1)
			MessageBox("Identify Failed");
		else{
			sprintf(buffer, "Identify Failed Name = %s Score = %d  Processed Number = %d", FFingerNames[fi], Score, ProcessNum);
			MessageBox(buffer);
		}
	}*/
}


void CSignUp::OnOnenrollZkfpengx1(BOOL ActionResult, const VARIANT& ATemplate)
{
	/* 用户登记指纹结束时调用该事件， 
	ActionResult =true 表示成功登记，
	用 pTemplate 属性可取得指纹特征模版；False 表示失败*/
	MessageBox(TEXT("指纹登记结束，触发事件:OnOnenrollZkfpengx1"));
	/*VARIANT pTemplate;

	if (!ActionResult)
		MessageBox(TEXT("指纹登记失败"));
	else{
		MessageBox(TEXT("指纹登记成功"));

		VariantClear(&FRegTemplate);
		pTemplate = m_zkfp.GetTemplate();

		sRegTemplate = m_zkfp.GetTemplateAsString();

		s_zkfp = m_zkfp.EncodeTemplate1(pTemplate);

		VariantCopy(&FRegTemplate, &pTemplate);

		m_zkfp.SaveTemplate(_T("c:\\fingerprint.tpl"), ATemplate);

		m_zkfp.AddRegTemplateStrToFPCacheDB(fpcHandle, FingerCount, (LPCTSTR)sRegTemplate);

		UpdateData(TRUE);

		//CString->const char *
		char str[1024];
		wsprintfA(str, "%ls", m_mobile);
		strcpy(FFingerNames[FingerCount], str);
		FingerCount = FingerCount + 1;
	}*/
	
}


void CSignUp::OnOnfeatureinfoZkfpengx1(long AQuality)
{
	/*
	取得指纹初始特征，Quality 表示该指纹特征的质量，有如下可能值：
	0: 好的指纹特征 
	1: 特征点不够 
	2: 其它原因导致不能取到指纹特征 
	-1: 可疑指纹
	*/
	//MessageBox(TEXT("OnOnfeatureinfoZkfpengx1"));
	
	if (m_zkfp.GetIsRegister()){
		CString sTemp;
		char buffer[10];
		sTemp = "";

		ltoa(m_zkfp.GetEnrollIndex(), buffer, 10);
		//unicode环境下格式化ansi字符串要用 %S
	
		//ltoa(AQuality, buffer, 10);
		if (AQuality != 0){
			sTemp.Format(_T("还需再按%S次,当前采集的指纹质量较差，返回码:%ld"), buffer, AQuality);
		}
		else{
			sTemp.Format(_T("还需再按%S次,当前指纹质量很好，返回码:%ld"), buffer, AQuality);
		}

		m_msg = sTemp;
		UpdateData(FALSE);
	}

	
}


void CSignUp::OnOnfingertouchingZkfpengx1()
{
	//当手指按压指纹取像设备时调用该事件。 

	//MessageBox(TEXT("开始按压指纹"));
	if (m_startRegistered){//开始注册啦
		RegisteredFingerprint();
	}
	
	return;
}


void CSignUp::OnOnfingerleavingZkfpengx1()
{
	//当手指移开指纹取像设备时调用该事件
}


void CSignUp::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_zkfp.EndEngine();
	CDialogEx::OnClose();
}
