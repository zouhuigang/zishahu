// SignUp.cpp : 实现文件
//

#include "stdafx.h"
#include "zshu.h"
#include "SignUp.h"
#include "afxdialogex.h"
#include <string>
#include "utils/HttpClient.h"
#include "json/json.h"
using namespace std;

/*
1. 是否有重复声明的地方？
2. 在头文件里加上#pragma once试试。
*/
// CSignUp 对话框
IMPLEMENT_DYNAMIC(CSignUp, CDialogEx)

CSignUp::CSignUp(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSignUp::IDD, pParent)
	, m_mobile(_T(""))
	, m_msg(_T(""))
	, m_code(_T(""))
	, m_memo(_T(""))
	, m_realname(_T(""))
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
	DDX_Control(pDX, IDC_START_SIGNUP, m_startSignUp);
	DDX_Control(pDX, IDC_UNSIGNUP, m_unSignUp);

	DDX_Text(pDX, IDC_EDIT3, m_code);
	DDX_Text(pDX, IDC_EDIT4, m_memo);
	DDX_Text(pDX, IDC_EDIT1, m_realname);
}


BEGIN_MESSAGE_MAP(CSignUp, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &CSignUp::RegisteredFingerprint)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_START_SIGNUP, &CSignUp::OnBnClickedStartSignup)
	ON_BN_CLICKED(IDC_UNSIGNUP, &CSignUp::OnBnClickedUnsignup)
	ON_BN_CLICKED(IDC_BUTTON1, &CSignUp::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_GETCODE, &CSignUp::OnBnClickedGetcode)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CSignUp 消息处理程序
//有问题，算不出长度
//https://bbs.csdn.net/topics/340158422
template<class T>
int length(T *a){
	return (int)(sizeof(a) / sizeof(a[0]));
}

BOOL CSignUp::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//指纹库初始化
	FingerCount = 0;
	fpcHandle = m_zkfp.CreateFPCacheDB();
	VariantInit(&FRegTemplate);

	tplList = ldb.LoadFingerprintList();
	CString strTemp;
	for (int i = 0; i < ldb.FingerCount; i++) {
		//TRACE("=====================================new mobile = %s,id=%d\n", tplList[i].mobile, tplList[i].id);
		strTemp = tplList[i].template_10;
		//AddRegTemplateStrToFPCacheDB(gnFPCHandle,指纹ID(我一般用员工代码+手指号),指纹字段2)
		m_zkfp.AddRegTemplateStrToFPCacheDB(fpcHandle, tplList[i].id, (LPCTSTR)strTemp);
	}
	free(tplList);

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
	
	if (m_mobile.Compare(TEXT("")) == 0 || m_code.Compare(TEXT("")) == 0){
		MessageBox(_T("手机号和验证码不能为空!!!"));
		return;
	}else{
		//核对手机号和验证码

		//请求网络
		std::string mobile(CW2A(m_mobile.GetString()));
		std::string ver_code(CW2A(m_code.GetString()));
		string strCallback = "";
		HttpClient http;
		string jsonParm = "";
		jsonParm = "{";
		jsonParm += "\"mobile\" : \"" + mobile + "\",";
		jsonParm += "\"ver_code\" : \"" + ver_code + "\"";
		jsonParm += "}";

		if (http.Post("http://c3.ab.51tywy.com/api/v1.0/zsh/fingerprint/ver", jsonParm, NULL, strCallback) == CURLE_OK){
			Json::Reader reader;
			Json::Value value;

			//CString temp;
			if (reader.parse(strCallback.c_str(), value))
			{
				//temp = value["info"].asCString();
				string info = value["info"].asString();
				CString outName = toCString(info);
				//statusCode
				int statusCode = value["status"].asInt();
				if (statusCode == 501){
					MessageBox(outName);
				}
				else{
					//可以录制指纹
					//开始登记指纹，登记结束后发生 OnEnroll 事件
					m_zkfp.BeginEnroll();
					str.Format(_T("开始录入指纹!"));
					//因为已经开始登记指纹了，所以不需要再开始登记注册了
					m_startRegistered = FALSE;
					//设置按钮不能点击
					m_startSignUp.EnableWindow(0);
					m_unSignUp.EnableWindow(1);

					m_msg = str;
					UpdateData(FALSE);
				}

			}

		}
		else{
			MessageBox(TEXT("网络请求失败，请检查网络连接.."));
			return;
		}


		
	}
	
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

	//MessageBox(TEXT("获取到了图像"));//m_zkfp.GetImageWidth() m_zkfp.GetImageHeight()
	m_zkfp.PrintImageAt(long(hdc), 310, 60,350 ,335);
}


void CSignUp::OnOncaptureZkfpengx1(BOOL ActionResult, const VARIANT& ATemplate)
{
	/*
	取到用于比对的指纹验证模板，模版保存到文件中，文件名称为属性 VerTplFileName 设置，
	ActionResult =true 表示成功取到指纹模版；
	False 表 示失败, 如果 VerTplFileName 没有设置或者为空，
	则不产生保存文件，但本 事件仍然会触发。 
	*/
	//MessageBox(TEXT("OnOncaptureZkfpengx1"));
	
	long fi;
	long Score, ProcessNum;
	BOOL RegChanged;
	char buffer[500];
	CString sTemp;
	BSTR bTemp;

	FMatchType = 2;

	sTemp = m_zkfp.GetTemplateAsString();
	m_msg = "获取的指纹模板";
	if (FMatchType == 1){
		bTemp = sRegTemplate.AllocSysString();
		//if (m_zkfp.VerFinger(&FRegTemplate, ATemplate, FALSE, &RegChanged))
		if (m_zkfp.VerFingerFromStr(&bTemp, (LPCTSTR)sTemp, FALSE, &RegChanged)){
			MessageBox(TEXT("验证成功"));
		}else{
			MessageBox(TEXT("验证失败"));
		}
			
	}
	else if (FMatchType == 2){  //1:N
		Score = 8;
		//fi = m_zkfp.IdentificationInFPCacheDB(fpcHandle, ATemplate, &Score, &ProcessNum);
		//当识别成功时返回值指纹标识，失败返回-1
		fi = m_zkfp.IdentificationFromStrInFPCacheDB(fpcHandle, (LPCTSTR)sTemp, &Score, &ProcessNum);
		if (fi == -1)
			MessageBox(TEXT("验证失败"));
		else{
			FingerTpl *user = ldb.GetUserInfo(fi);
			sprintf(buffer, "验证成功,手机号 = %s 分数 = %d  进程号 = %d,注册 = %s", user->GetMobile(), Score, ProcessNum, user->GetPushTime());
			m_msg = buffer;
			//delete user
			delete user;
			user = NULL;
			UpdateData(FALSE);
		}
	}
}



void CSignUp::OnOnenrollZkfpengx1(BOOL ActionResult, const VARIANT& ATemplate)
{
	/* 用户登记指纹结束时调用该事件， 
	ActionResult =true 表示成功登记，
	用 pTemplate 属性可取得指纹特征模版；False 表示失败*/
	//MessageBox(TEXT("指纹登记结束，触发事件:OnOnenrollZkfpengx1"));
	VARIANT pTemplate;

	if (!ActionResult)
		MessageBox(TEXT("指纹登记失败,请重试!!!"));
	else{
		//MessageBox(TEXT("指纹登记成功"));

		VariantClear(&FRegTemplate);
		pTemplate = m_zkfp.GetTemplate();

		sRegTemplate = m_zkfp.GetTemplateAsString();

		s_zkfp = m_zkfp.EncodeTemplate1(pTemplate);

		VariantCopy(&FRegTemplate, &pTemplate);

		//m_zkfp.SaveTemplate(_T("c:\\fingerprint.tpl"), ATemplate); 

		//调用接口存储指纹

		//请求网络
		std::string ver_code(CW2A(m_code.GetString()));
		std::string strStr(CW2A(m_mobile.GetString()));       //CString 转string
		std::string strStr1(CW2A(sRegTemplate.GetString()));       //CString 转string

		string strCallback = "";
		HttpClient http;
		string jsonParm = "";
		jsonParm = "{";
		jsonParm += "\"mobile\" : \"" + strStr + "\",";
		jsonParm += "\"ver_code\" : \"" + ver_code + "\",";
		jsonParm += "\"template_10\" : \"" + strStr1 + "\"";
		jsonParm += "}";


		if (http.Post("http://c3.ab.51tywy.com/api/v1.0/zsh/fingerprint/add", jsonParm, NULL, strCallback) == CURLE_OK){
			Json::Reader reader;
			Json::Value value;

			//CString temp;
			if (reader.parse(strCallback.c_str(), value))
			{
				//temp = value["info"].asCString();
				string info = value["info"].asString();
				CString outName = toCString(info);
				//statusCode
				int statusCode = value["status"].asInt();
				if (statusCode == 501){
					MessageBox(outName);
				}
				else{

					string push_time = value["data"]["push_time"].asString();
					string sign = value["data"]["sign"].asString();
					//插入本地数据库
					//database a;
					int autoid = ldb.AddFingerprint(strStr, "1", strStr1, push_time,sign);
					if (autoid>0){
						m_zkfp.AddRegTemplateStrToFPCacheDB(fpcHandle, autoid, (LPCTSTR)sRegTemplate);
					}
					MessageBox(TEXT("指纹录入成功"));
				}

			}

		}
		else{
			MessageBox(TEXT("网络请求失败，请检查网络连接.."));
			return;
		}


		
		//TRACE("=====================================autoid=%d\n", autoid);
		

		//UpdateData(TRUE);

		//CString->const char *
		/*char str[1024];
		wsprintfA(str, "%ls", m_mobile);
		strcpy(FFingerNames[FingerCount], str);
		FingerCount = FingerCount + 1;*/
	}
	
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
	KillTimer(1);//关闭定时器1
	CDialogEx::OnClose();
}


void CSignUp::OnBnClickedStartSignup()
{
	//点击开始登记指纹
	if (m_startRegistered){//开始注册啦
		RegisteredFingerprint();
	}
}




void CSignUp::OnBnClickedUnsignup()
{
	// 取消登记指纹
	//CancelEnroll() 取消当前的指纹登记状态，即由 BeginEnroll 开始的操作可由此函数中断。 
	if (!m_startRegistered){
		m_zkfp.CancelEnroll();
		m_startRegistered = TRUE;
		//设置按钮不能点击
		m_startSignUp.EnableWindow(1);
		m_unSignUp.EnableWindow(0);
	}
}



void CSignUp::OnBnClickedButton1()
{
	// 请求网络
	UpdateData(TRUE);
	if (m_realname.IsEmpty()){
		MessageBox(TEXT("真实姓名不能为空"));
		return;
	}
	if (m_mobile.IsEmpty()){
		MessageBox(TEXT("手机号不能为空"));
		return;
	}

	if (m_code.IsEmpty()){
		MessageBox(TEXT("验证码不能为空"));
		return;
	}

	
	USES_CONVERSION;
	std::string ver_code(CW2A(m_code.GetString()));
	std::string mobile(CW2A(m_mobile.GetString()));       //CString 转string
	std::string memo(CW2A(m_memo.GetString()));       //CString 转string
	std::string realname(CW2A(m_realname.GetString()));       //CString 转string
	
	

	string strCallback = "";
	HttpClient http;
	string jsonParm = "";
	jsonParm = "{";
	jsonParm += "\"mobile\" : \"" + mobile + "\",";
	jsonParm += "\"ver_code\" : \"" + ver_code + "\",";
	//jsonParm += "\"realname\" : \"aasd中文\",";
	jsonParm += "\"realname\" : \"" + realname + "\",";
	jsonParm += "\"memo\" : \"" + memo + "\"";
	jsonParm += "}";


	if (http.Post("http://c3.ab.51tywy.com/api/v1.0/zsh/user/signup", jsonParm, NULL, strCallback) == CURLE_OK){
		Json::Reader reader;
		Json::Value value;

		//CString temp;
		if (reader.parse(strCallback.c_str(), value))
		{
			//temp = value["info"].asCString();
			string info = value["info"].asString();
			CString outName = toCString(info);
			//statusCode
			int statusCode = value["status"].asInt();
			if (statusCode == 501){
				MessageBox(outName);
			}
			else{
				MessageBox(TEXT("注册成功"));
			}

		}

	}
	else{
		MessageBox(TEXT("网络请求失败，请检查网络连接.."));
		return;
	}
}

//String->CString
CString CSignUp::toCString(string name){
	//解决中文转码问题
	int len = strlen(name.c_str()) + 1;
	char outch[MAX_PATH];
	WCHAR * wChar = new WCHAR[len];
	wChar[0] = 0;
	MultiByteToWideChar(CP_UTF8, 0, name.c_str(), len, wChar, len);
	WideCharToMultiByte(CP_ACP, 0, wChar, len, outch, len, 0, 0);
	delete[] wChar;
	char* pchar = (char*)outch;

	len = strlen(pchar) + 1;
	WCHAR outName[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, pchar, len, outName, len);
	return outName;
}

void CSignUp::OnBnClickedGetcode()
{
	// TODO:  在此添加控件通知处理程序代码
	
	//database a;
	//a.new_table();
	//初始化指纹
	//将本地数据库中的指纹读取进高速缓存中
	//database a;
	/*tplList = ldb.LoadFingerprintList();
	CString strTemp;
	for (int i = 0; i < ldb.FingerCount; i++) {
		//TRACE("=====================================new mobile = %s,id=%d\n", tplList[i].mobile, tplList[i].id);
		strTemp = tplList[i].template_10;
		//AddRegTemplateStrToFPCacheDB(gnFPCHandle,指纹ID(我一般用员工代码+手指号),指纹字段2)
		m_zkfp.AddRegTemplateStrToFPCacheDB(fpcHandle, tplList[i].id, (LPCTSTR)strTemp);
	}
	


	free(tplList);

	MessageBox(TEXT("初始化数据库")); */

	//获取最新数据
	UpdateData(TRUE);
	if (m_mobile.IsEmpty()){
		MessageBox(TEXT("手机号不能为空"));
		return;
	}

	//请求网络
	std::string mobile(CW2A(m_mobile.GetString()));

	if (mobile.length() != 11)
	{
		MessageBox(TEXT("请输入正确的手机号码!"));
		return;
	}


	//发送网络请求
	string strCallback = "";
	HttpClient http;
	string jsonParm = "";
	jsonParm = "{";
	jsonParm += "\"mobile\" : \"" + mobile + "\"";
	jsonParm += "}";

	if (http.Post("http://c3.ab.51tywy.com/api/v1.0/zsh/user/sms/send", jsonParm, NULL, strCallback) == CURLE_OK){
		Json::Reader reader;
		Json::Value value;

		//CString temp;
		if (reader.parse(strCallback.c_str(), value))
		{
			//temp = value["info"].asCString();
			string info = value["info"].asString();
			CString outName = toCString(info);
			//statusCode
			int statusCode = value["status"].asInt();
			if (statusCode == 501){
				MessageBox(outName);
			}
			else{
				//开启倒计时
				GetIdentCodeTimeLimit();
				MessageBox(TEXT("短信发送成功,请注意查收!"));
			}

		}

	}
	else{
		MessageBox(TEXT("网络请求失败，请检查网络连接.."));
		return;
	}

}

//验证码倒计时
void CSignUp::GetIdentCodeTimeLimit()
{
	GetDlgItem(IDC_GETCODE)->EnableWindow(0);
	m_nGetIdentCodeTime = 60;
	SetTimer(1, 1000, NULL);      //参数：定时器标号，定时时间（ms）。启动定时器1，每隔1s刷新一次
}


//消息里面添加定时消息
void CSignUp::OnTimer(UINT_PTR nIDEvent)
{
	CString str;
	switch (nIDEvent)
	{
	case 1:   //定时器1处理函数，定时发送数据进行更新
	{
				  if (m_nGetIdentCodeTime == 0)
				  {
					  GetDlgItem(IDC_GETCODE)->EnableWindow(1);
					  GetDlgItem(IDC_GETCODE)->SetWindowText(_T("获取验证码"));
					  KillTimer(1);//关闭定时器1
				  }
				  else
				  {
					  m_nGetIdentCodeTime--;
					  str.Format(_T("%d"), m_nGetIdentCodeTime);
					  GetDlgItem(IDC_GETCODE)->SetWindowText(str);
				  }
				  break;
	}
	default:
		break;
	}
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CDialogEx::OnTimer(nIDEvent);
}
