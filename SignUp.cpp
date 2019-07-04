// SignUp.cpp : ʵ���ļ�
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
1. �Ƿ����ظ������ĵط���
2. ��ͷ�ļ������#pragma once���ԡ�
*/
// CSignUp �Ի���
IMPLEMENT_DYNAMIC(CSignUp, CDialogEx)

CSignUp::CSignUp(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSignUp::IDD, pParent)
	, m_mobile(_T(""))
	, m_msg(_T(""))
	, m_code(_T(""))
	, m_memo(_T(""))
	, m_realname(_T(""))
{
	m_startRegistered = TRUE;//����ҳ��Ϳ��ԵǼ�ָ������
}

CSignUp::~CSignUp()
{
}

void CSignUp::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	//ָ����

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


// CSignUp ��Ϣ�������
//�����⣬�㲻������
//https://bbs.csdn.net/topics/340158422
template<class T>
int length(T *a){
	return (int)(sizeof(a) / sizeof(a[0]));
}

BOOL CSignUp::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//ָ�ƿ��ʼ��
	FingerCount = 0;
	fpcHandle = m_zkfp.CreateFPCacheDB();
	VariantInit(&FRegTemplate);

	tplList = ldb.LoadFingerprintList();
	CString strTemp;
	for (int i = 0; i < ldb.FingerCount; i++) {
		//TRACE("=====================================new mobile = %s,id=%d\n", tplList[i].mobile, tplList[i].id);
		strTemp = tplList[i].template_10;
		//AddRegTemplateStrToFPCacheDB(gnFPCHandle,ָ��ID(��һ����Ա������+��ָ��),ָ���ֶ�2)
		m_zkfp.AddRegTemplateStrToFPCacheDB(fpcHandle, tplList[i].id, (LPCTSTR)strTemp);
	}
	free(tplList);

	//�Զ�����ָ����
	ConnectionFingerprint();


	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


//����ָ����
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
			MessageBox(TEXT("ָ���ǳ�ʼ��ʧ�ܣ�����ָ�����Ƿ���..."));
		FMatchType = 0;
}

//ע��ָ��
void CSignUp::RegisteredFingerprint(){
	UpdateData(TRUE);//ͬ���ؼ�����
	CString str;
	
	if (m_mobile.Compare(TEXT("")) == 0 || m_code.Compare(TEXT("")) == 0){
		MessageBox(_T("�ֻ��ź���֤�벻��Ϊ��!!!"));
		return;
	}else{
		//�˶��ֻ��ź���֤��

		//��������
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
					//����¼��ָ��
					//��ʼ�Ǽ�ָ�ƣ��Ǽǽ������� OnEnroll �¼�
					m_zkfp.BeginEnroll();
					str.Format(_T("��ʼ¼��ָ��!"));
					//��Ϊ�Ѿ���ʼ�Ǽ�ָ���ˣ����Բ���Ҫ�ٿ�ʼ�Ǽ�ע����
					m_startRegistered = FALSE;
					//���ð�ť���ܵ��
					m_startSignUp.EnableWindow(0);
					m_unSignUp.EnableWindow(1);

					m_msg = str;
					UpdateData(FALSE);
				}

			}

		}
		else{
			MessageBox(TEXT("��������ʧ�ܣ�������������.."));
			return;
		}


		
	}
	
	return;
	//CancelEnroll() ȡ����ǰ��ָ�ƵǼ�״̬������ BeginEnroll ��ʼ�Ĳ������ɴ˺����жϡ� 
	//SetDlgItemText(IDC_EDHint, "start register");

}

BEGIN_EVENTSINK_MAP(CSignUp, CDialogEx)
	//ע��ָ�����¼�
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
	�豸ȡ��ָ��ͼ�����ͨ��AddImageFile��AddBitmap����ָ��ͼ��ʱ 
	���ø��¼���AImageValid ��ʾ�Ƿ����ģ����ȡ��
	����Ϊ False ��
	ϵͳ�� ȡ��ָ��ͼ��󷵻أ�������ģ����ȡ
	*/
	HDC hdc;

	hdc = this->GetDC()->m_hDC;

	//MessageBox(TEXT("��ȡ����ͼ��"));//m_zkfp.GetImageWidth() m_zkfp.GetImageHeight()
	m_zkfp.PrintImageAt(long(hdc), 310, 60,350 ,335);
}


void CSignUp::OnOncaptureZkfpengx1(BOOL ActionResult, const VARIANT& ATemplate)
{
	/*
	ȡ�����ڱȶԵ�ָ����֤ģ�壬ģ�汣�浽�ļ��У��ļ�����Ϊ���� VerTplFileName ���ã�
	ActionResult =true ��ʾ�ɹ�ȡ��ָ��ģ�棻
	False �� ʾʧ��, ��� VerTplFileName û�����û���Ϊ�գ�
	�򲻲��������ļ������� �¼���Ȼ�ᴥ���� 
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
	m_msg = "��ȡ��ָ��ģ��";
	if (FMatchType == 1){
		bTemp = sRegTemplate.AllocSysString();
		//if (m_zkfp.VerFinger(&FRegTemplate, ATemplate, FALSE, &RegChanged))
		if (m_zkfp.VerFingerFromStr(&bTemp, (LPCTSTR)sTemp, FALSE, &RegChanged)){
			MessageBox(TEXT("��֤�ɹ�"));
		}else{
			MessageBox(TEXT("��֤ʧ��"));
		}
			
	}
	else if (FMatchType == 2){  //1:N
		Score = 8;
		//fi = m_zkfp.IdentificationInFPCacheDB(fpcHandle, ATemplate, &Score, &ProcessNum);
		//��ʶ��ɹ�ʱ����ֵָ�Ʊ�ʶ��ʧ�ܷ���-1
		fi = m_zkfp.IdentificationFromStrInFPCacheDB(fpcHandle, (LPCTSTR)sTemp, &Score, &ProcessNum);
		if (fi == -1)
			MessageBox(TEXT("��֤ʧ��"));
		else{
			FingerTpl *user = ldb.GetUserInfo(fi);
			sprintf(buffer, "��֤�ɹ�,�ֻ��� = %s ���� = %d  ���̺� = %d,ע�� = %s", user->GetMobile(), Score, ProcessNum, user->GetPushTime());
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
	/* �û��Ǽ�ָ�ƽ���ʱ���ø��¼��� 
	ActionResult =true ��ʾ�ɹ��Ǽǣ�
	�� pTemplate ���Կ�ȡ��ָ������ģ�棻False ��ʾʧ��*/
	//MessageBox(TEXT("ָ�ƵǼǽ����������¼�:OnOnenrollZkfpengx1"));
	VARIANT pTemplate;

	if (!ActionResult)
		MessageBox(TEXT("ָ�ƵǼ�ʧ��,������!!!"));
	else{
		//MessageBox(TEXT("ָ�ƵǼǳɹ�"));

		VariantClear(&FRegTemplate);
		pTemplate = m_zkfp.GetTemplate();

		sRegTemplate = m_zkfp.GetTemplateAsString();

		s_zkfp = m_zkfp.EncodeTemplate1(pTemplate);

		VariantCopy(&FRegTemplate, &pTemplate);

		//m_zkfp.SaveTemplate(_T("c:\\fingerprint.tpl"), ATemplate); 

		//���ýӿڴ洢ָ��

		//��������
		std::string ver_code(CW2A(m_code.GetString()));
		std::string strStr(CW2A(m_mobile.GetString()));       //CString תstring
		std::string strStr1(CW2A(sRegTemplate.GetString()));       //CString תstring

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
					//���뱾�����ݿ�
					//database a;
					int autoid = ldb.AddFingerprint(strStr, "1", strStr1, push_time,sign);
					if (autoid>0){
						m_zkfp.AddRegTemplateStrToFPCacheDB(fpcHandle, autoid, (LPCTSTR)sRegTemplate);
					}
					MessageBox(TEXT("ָ��¼��ɹ�"));
				}

			}

		}
		else{
			MessageBox(TEXT("��������ʧ�ܣ�������������.."));
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
	ȡ��ָ�Ƴ�ʼ������Quality ��ʾ��ָ�������������������¿���ֵ��
	0: �õ�ָ������ 
	1: �����㲻�� 
	2: ����ԭ���²���ȡ��ָ������ 
	-1: ����ָ��
	*/
	//MessageBox(TEXT("OnOnfeatureinfoZkfpengx1"));
	
	if (m_zkfp.GetIsRegister()){
		CString sTemp;
		char buffer[10];
		sTemp = "";

		ltoa(m_zkfp.GetEnrollIndex(), buffer, 10);
		//unicode�����¸�ʽ��ansi�ַ���Ҫ�� %S
	
		//ltoa(AQuality, buffer, 10);
		if (AQuality != 0){
			sTemp.Format(_T("�����ٰ�%S��,��ǰ�ɼ���ָ�������ϲ������:%ld"), buffer, AQuality);
		}
		else{
			sTemp.Format(_T("�����ٰ�%S��,��ǰָ�������ܺã�������:%ld"), buffer, AQuality);
		}

		m_msg = sTemp;
		UpdateData(FALSE);
	}

	
}


void CSignUp::OnOnfingertouchingZkfpengx1()
{
	//����ָ��ѹָ��ȡ���豸ʱ���ø��¼��� 

	//MessageBox(TEXT("��ʼ��ѹָ��"));
	
	
	return;
}


void CSignUp::OnOnfingerleavingZkfpengx1()
{
	//����ָ�ƿ�ָ��ȡ���豸ʱ���ø��¼�
}


void CSignUp::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_zkfp.EndEngine();
	KillTimer(1);//�رն�ʱ��1
	CDialogEx::OnClose();
}


void CSignUp::OnBnClickedStartSignup()
{
	//�����ʼ�Ǽ�ָ��
	if (m_startRegistered){//��ʼע����
		RegisteredFingerprint();
	}
}




void CSignUp::OnBnClickedUnsignup()
{
	// ȡ���Ǽ�ָ��
	//CancelEnroll() ȡ����ǰ��ָ�ƵǼ�״̬������ BeginEnroll ��ʼ�Ĳ������ɴ˺����жϡ� 
	if (!m_startRegistered){
		m_zkfp.CancelEnroll();
		m_startRegistered = TRUE;
		//���ð�ť���ܵ��
		m_startSignUp.EnableWindow(1);
		m_unSignUp.EnableWindow(0);
	}
}



void CSignUp::OnBnClickedButton1()
{
	// ��������
	UpdateData(TRUE);
	if (m_realname.IsEmpty()){
		MessageBox(TEXT("��ʵ��������Ϊ��"));
		return;
	}
	if (m_mobile.IsEmpty()){
		MessageBox(TEXT("�ֻ��Ų���Ϊ��"));
		return;
	}

	if (m_code.IsEmpty()){
		MessageBox(TEXT("��֤�벻��Ϊ��"));
		return;
	}

	
	USES_CONVERSION;
	std::string ver_code(CW2A(m_code.GetString()));
	std::string mobile(CW2A(m_mobile.GetString()));       //CString תstring
	std::string memo(CW2A(m_memo.GetString()));       //CString תstring
	std::string realname(CW2A(m_realname.GetString()));       //CString תstring
	
	

	string strCallback = "";
	HttpClient http;
	string jsonParm = "";
	jsonParm = "{";
	jsonParm += "\"mobile\" : \"" + mobile + "\",";
	jsonParm += "\"ver_code\" : \"" + ver_code + "\",";
	//jsonParm += "\"realname\" : \"aasd����\",";
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
				MessageBox(TEXT("ע��ɹ�"));
			}

		}

	}
	else{
		MessageBox(TEXT("��������ʧ�ܣ�������������.."));
		return;
	}
}

//String->CString
CString CSignUp::toCString(string name){
	//�������ת������
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	
	//database a;
	//a.new_table();
	//��ʼ��ָ��
	//���������ݿ��е�ָ�ƶ�ȡ�����ٻ�����
	//database a;
	/*tplList = ldb.LoadFingerprintList();
	CString strTemp;
	for (int i = 0; i < ldb.FingerCount; i++) {
		//TRACE("=====================================new mobile = %s,id=%d\n", tplList[i].mobile, tplList[i].id);
		strTemp = tplList[i].template_10;
		//AddRegTemplateStrToFPCacheDB(gnFPCHandle,ָ��ID(��һ����Ա������+��ָ��),ָ���ֶ�2)
		m_zkfp.AddRegTemplateStrToFPCacheDB(fpcHandle, tplList[i].id, (LPCTSTR)strTemp);
	}
	


	free(tplList);

	MessageBox(TEXT("��ʼ�����ݿ�")); */

	//��ȡ��������
	UpdateData(TRUE);
	if (m_mobile.IsEmpty()){
		MessageBox(TEXT("�ֻ��Ų���Ϊ��"));
		return;
	}

	//��������
	std::string mobile(CW2A(m_mobile.GetString()));

	if (mobile.length() != 11)
	{
		MessageBox(TEXT("��������ȷ���ֻ�����!"));
		return;
	}


	//������������
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
				//��������ʱ
				GetIdentCodeTimeLimit();
				MessageBox(TEXT("���ŷ��ͳɹ�,��ע�����!"));
			}

		}

	}
	else{
		MessageBox(TEXT("��������ʧ�ܣ�������������.."));
		return;
	}

}

//��֤�뵹��ʱ
void CSignUp::GetIdentCodeTimeLimit()
{
	GetDlgItem(IDC_GETCODE)->EnableWindow(0);
	m_nGetIdentCodeTime = 60;
	SetTimer(1, 1000, NULL);      //��������ʱ����ţ���ʱʱ�䣨ms����������ʱ��1��ÿ��1sˢ��һ��
}


//��Ϣ������Ӷ�ʱ��Ϣ
void CSignUp::OnTimer(UINT_PTR nIDEvent)
{
	CString str;
	switch (nIDEvent)
	{
	case 1:   //��ʱ��1����������ʱ�������ݽ��и���
	{
				  if (m_nGetIdentCodeTime == 0)
				  {
					  GetDlgItem(IDC_GETCODE)->EnableWindow(1);
					  GetDlgItem(IDC_GETCODE)->SetWindowText(_T("��ȡ��֤��"));
					  KillTimer(1);//�رն�ʱ��1
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
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CDialogEx::OnTimer(nIDEvent);
}
