// SignIn.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "zshu.h"
#include "SignIn.h"
#include "afxdialogex.h"
#include "utils/HttpClient.h"
#include "json/json.h"
#include "ScanDlg.h"
#include <stdio.h>
// CSignIn �Ի���

IMPLEMENT_DYNAMIC(CSignIn, CDialogEx)

CSignIn::CSignIn(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSignIn::IDD, pParent)
	, m_mobile(_T(""))
{
	//_CrtSetBreakAlloc(541);
}


void CSignIn::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_mobile);
	DDX_Control(pDX, IDC_ZKFPENGX1, m_zkfp_1);
}


BEGIN_MESSAGE_MAP(CSignIn, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CSignIn::OnBnClickedButton1)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

//String->CString
CString CSignIn::toCString(string name){
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

	//�ͷ��ڴ�
	free(pchar);
	pchar = NULL;
	return outName;
}


// CSignIn ��Ϣ�������
void CSignIn::OnBnClickedButton1()
{
	
	/*CString strValue = _T("{\"key1\":\"value1\"}");
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
	}*/

	CScanDlg dlg;
	dlg.DoModal();
	EndDialog(0);
	return;

	// click sign in
	UpdateData(TRUE);
	if (m_mobile.IsEmpty() || m_sign.IsEmpty() || m_push_time.IsEmpty()){
		MessageBox(TEXT("ָ����Ϣ����Ϊ��"));
		return;
	}

	//CString->string
	std::string mobile(CW2A(m_mobile.GetString()));
	std::string sign(CW2A(m_sign.GetString()));
	std::string push_time(CW2A(m_push_time.GetString()));

	//������������
	string strCallback="";
	HttpClient http;
	string jsonParm = "";
	jsonParm = "{";
	jsonParm += "\"mobile\" : \"" + mobile + "\",";
	jsonParm += "\"sign\" : \"" + sign + "\",";
	jsonParm += "\"push_time\" :\" " + push_time+"\"";
	jsonParm += "}";

	if (http.Post("http://c3.ab.51tywy.com/api/v1.0/zsh/user/signin", jsonParm, NULL, strCallback) == CURLE_OK){
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
			if (statusCode==501){
				MessageBox(outName);
			}
			else{

				//ShowWindow(SW_HIDE);
				//CScanDlg dlg;
				//dlg.DoModal();
				//EndDialog(0);
				MessageBox(TEXT("����ɹ�"));

			}

		}

	}
	else{
		MessageBox(TEXT("��������ʧ�ܣ�������������.."));
		return;
	}



}


BOOL CSignIn::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	
	

	//�Զ�����ָ����
	TRACE("==================================conZKFP = %d\n", conZKFP);
	if (conZKFP!=0){
		long new_conZKFP = m_zkfp_1.InitEngine();
		TRACE("=====================================ָ�Ƴ�ʼ������ֵ:%d,���ʵ�ַ:%p\n", new_conZKFP, &m_zkfp_1);
		if (new_conZKFP == 0){

			conZKFP = 0;
			//create db cache
			fpcHandle = m_zkfp_1.CreateFPCacheDB();
			VariantInit(&FRegTemplate);

			//���ݿ�д��
			tplList = ldb.LoadFingerprintList();
			CString strTemp;
			for (int i = 0; i < ldb.FingerCount; i++) {
				strTemp = tplList[i].template_10;
				m_zkfp_1.AddRegTemplateStrToFPCacheDB(fpcHandle, tplList[i].id, (LPCTSTR)strTemp);
				//TRACE("=====================================new mobile = %s,id=%d\n", tplList[i].mobile, tplList[i].id);
			}
			free(tplList);
			tplList = NULL;
		}
		else{
			m_zkfp_1.EndEngine();
			MessageBox(TEXT("����ָ�����Ƿ���!!"));
		}
	}
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


//����ָ����
void CSignIn::ConnectionFingerprint()
{

	/*if (m_zkfp.InitEngine() == 0){
		m_SN = m_zkfp.GetSensorSN();
		ltoa(m_zkfp.GetSensorIndex(), buffer, 10);
		m_Used = buffer;
		ltoa(m_zkfp.GetSensorCount(), buffer, 10);
		m_COUNT = buffer
	}
	else{
		m_zkfp.EndEngine();
		MessageBox(TEXT("ָ���ǳ�ʼ��ʧ�ܣ�����ָ�����Ƿ���..."));
	}*/
		
}


BEGIN_EVENTSINK_MAP(CSignIn, CDialogEx)
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
	m_zkfp_1.PrintImageAt(long(hdc), 100, 136, 270, 160);
}


void CSignIn::OnOncaptureZkfpengx1(BOOL ActionResult, const VARIANT& ATemplate)
{
	// TODO:  �ڴ˴������Ϣ����������
	long fi;
	long Score, ProcessNum;
	CString sTemp;
	Score = 8;

	//��ȡ����ָ��
	sTemp = m_zkfp_1.GetTemplateAsString();

	fi = m_zkfp_1.IdentificationFromStrInFPCacheDB(fpcHandle, (LPCTSTR)sTemp, &Score, &ProcessNum);
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

		delete user;
		user = NULL;
	}
}


void CSignIn::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//TRACE("=====================================1111,���ʵ�ַ:%p\n", &m_zkfp_1);
	m_zkfp_1.EndEngine();//ע��ָ��
	//TRACE("=====================================2222,���ʵ�ַ:%p\n", &m_zkfp_1);
	CDialogEx::OnClose();
}
