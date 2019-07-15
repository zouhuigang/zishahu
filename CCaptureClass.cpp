#include "CCaptureClass.h"

/* �๹�캯��ʵ�� */
CCaptureClass::CCaptureClass()
{
	CoInitialize(NULL);  //COM  ���ʼ��
	m_hWnd = NULL;    // ��Ƶ��ʾ���ڵľ��
	m_pVW = NULL;    // ��Ƶ���ڽӿ�ָ�����
	m_pMC = NULL;    // ý����ƽӿ�ָ�����
	m_pGB = NULL;   // �˲�������������ӿ�ָ�����
	m_pBF = NULL;     // �����˲����ӿ�ָ�����  
	pBasicVideo = NULL;// ������Ƶ�ӿ�ָ�����
	pGrabberF = NULL; // �����˲����ӿ�ָ�����
	pNull = NULL; // ��Ⱦ�˲����ӿ����
	pGrabber = NULL; //
	pEvent = NULL; // ý���¼��ӿ�ָ�����
	m_pCapture = NULL;   // ��ǿ�Ͳ����˲�������������ӿ�ָ�����

}
/* ��������ʵ�� */
CCaptureClass::~CCaptureClass()
{
	if (m_pMC)  m_pMC->Stop();   // ����ֹͣý��
	if (m_pVW) {
		m_pVW->put_Visible(OAFALSE); // ��Ƶ���ڲ��ɼ�
		m_pVW->put_Owner(NULL);  // ��Ƶ���ڵĸ��������
	}
	srelease(m_pCapture);       // �ͷ���ǿ�Ͳ����˲�������������ӿ� srelease(pBasicVideo);
	srelease(pGrabber);
	srelease(pGrabberF);
	srelease(pNull);
	srelease(pEvent);
	srelease(m_pMC);       // �ͷ�ý����ƽӿ�
	srelease(m_pGB);       // �ͷ��˲�������������ӿ�
	srelease(m_pBF);      // �ͷŲ����˲����ӿ�

	CoUninitialize();     // ж�� COM ��
}
// ��ָ���ɼ��豸���˲�������
/*

�ú����Ĵ�������ǲɼ��豸�������źͲ����豸���˲�����
���������Ų�ѯϵͳ�е���Ƶ�����豸��
���Ѻ����֣� FriendlyName ���ķ�ʽ��ȡ��ѡ�豸����Ϣ��
Ȼ��Ѳ�ѯ�ɹ����豸�봫����˲�������
���ز����豸���˲�����
*/
bool CCaptureClass::BindFilter(int deviceId, IBaseFilter **pFilter)
{
	if (deviceId < 0) return false;
	// ö�����е���Ƶ�����豸
	ICreateDevEnum *pCreateDevEnum;
	// �����豸ö���� pCreateDevEnum
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
		CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum,
		(void**)&pCreateDevEnum);
	if (hr != NOERROR)  return false;
	IEnumMoniker *pEm;
	// ������Ƶ�����豸��ö����
	hr = pCreateDevEnum->CreateClassEnumerator
		(CLSID_VideoInputDeviceCategory,
		&pEm, 0);
	if (hr != NOERROR) return false;
	pEm->Reset();         // ��λ���豸
	ULONG cFetched;
	IMoniker *pM;
	int index = 0;
	// ��ȡ�豸
	while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK, index <= deviceId)
	{
		IPropertyBag *pBag;
		// ��ȡ���豸�����Լ�
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if (SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;     // ������Ƕ����Ƶ�����
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			// ��ȡ FriendlyName ��ʽ����Ϣ
			if (hr == NOERROR)
			{
				// �ɼ��豸�벶���˲�������
				if (index == deviceId) pM->BindToObject(0, 0,
					IID_IBaseFilter, (void**)pFilter);
				SysFreeString(var.bstrVal); // �ͷŶ�����������Դ�������ͷ�



			}
			pBag->Release();
		}
		pM->Release();
		index++;
	}
	return true;
}



/* ������Ƶ��ʾ���ڵ����� */
HRESULT CCaptureClass::SetupVideoWindow()
{
	HRESULT hr;
	//m_hWnd Ϊ�� CCaptureClass �ĳ�Ա��������ʹ�øú���ǰ���ʼ��
	hr = m_pVW->put_Visible(OAFALSE);                  // ��Ƶ���ڲ��ɼ�
	hr = m_pVW->put_Owner((OAHWND)m_hWnd);      // ���������ߣ�����Ĵ��ھ��
	if (FAILED(hr)) return hr;
	hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);// ���ô�������
	if (FAILED(hr)) return hr;
	ResizeVideoWindow();                           // ���Ĵ��ڴ�С
	hr = m_pVW->put_Visible(OATRUE);              // ��Ƶ���ڿɼ�
	return hr;
}
/* ������Ƶ���ڴ�С */
void CCaptureClass::ResizeVideoWindow()
{
	if (m_pVW) {
		// ��ͼ���������ָ������
		CRect rc;
		::GetClientRect(m_hWnd, &rc);    // ��ȡ��ʾ���ڵĿͻ���
		m_pVW->SetWindowPosition(0, 0, rc.right, rc.bottom);

		
		// ������Ƶ��ʾ���ڵ�λ��
	}
}


/*�����˲����������������ѯ����ֿ��ƽӿ�*/
HRESULT CCaptureClass::InitCaptureGraphBuilder()
{
	HRESULT hr;
	//����IGraphBuilder�ӿ�
	hr = CoCreateInstance(CLSID_FilterGraph, NULL,CLSCTX_INPROC_SERVER,IID_IGraphBuilder, (void **)&m_pGB);
	TRACE("InitCaptureGraphBuilder init:0x%x\n", hr);
	if (FAILED(hr)) return hr;
	//����ICaptureGraphBuilder2�ӿ�
	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL,
		CLSCTX_INPROC,
		IID_ICaptureGraphBuilder2, (void **)&m_pCapture);
	TRACE("InitCaptureGraphBuilder0:0x%x\n", hr);
	if (FAILED(hr)) return hr;
	//��ʼ���˲������������IGraphBuilder
	m_pCapture->SetFiltergraph(m_pGB);
	//��ѯý����ƽӿ�
	hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);
	TRACE("InitCaptureGraphBuilder1:0x%x\n", hr);
	if (FAILED(hr)) return hr;
	//��ѯ��Ƶ���ڽӿ�
	hr = m_pGB->QueryInterface(IID_IVideoWindow, (LPVOID *)&m_pVW);
	TRACE("InitCaptureGraphBuilder2:0x%x\n", hr);
	if (FAILED(hr)) return hr;
	return hr;
}

/* ��ʼԤ����Ƶ���� */
HRESULT CCaptureClass::PreviewImages(int iDeviceID, HWND hWnd)
{
	HRESULT hr;

	//  ��ʼ����Ƶ�����˲������������
	hr = InitCaptureGraphBuilder();
	if (FAILED(hr))
	{
		AfxMessageBox(_T("Failed to get video interfaces!"));
		return  hr;
	}
	//  ��ָ���ɼ��豸���˲�������
	if (!BindFilter(iDeviceID, &m_pBF))
		return  S_FALSE;
	//  ���˲�����ӵ��˲���������
	hr = m_pGB->AddFilter(m_pBF, L"Capture Filter");
	if (FAILED(hr))
	{
		AfxMessageBox(_T("Can �� t add the capture filter"));
		return  hr;
	}


	// Create the Sample Grabber.
	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (void **)&pGrabberF);
	if (FAILED(hr))
	{
		AfxMessageBox(_T("Can �� t create the grabber"));
		return  hr;
	}

	

	hr = pGrabberF->QueryInterface(IID_ISampleGrabber, (void **)&pGrabber);
	//  ���˲�����ӵ��˲���������


	hr = m_pGB->AddFilter(pGrabberF, L"Sample Grabber");
	if (FAILED(hr))
	{
		AfxMessageBox(_T("Can �� t add the grabber"));
		return  hr;
	}

	
	// Add the Null Renderer filter to the graph.
	hr = CoCreateInstance(CLSID_VideoRenderer, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (void **)&pNull);
	hr = m_pGB->AddFilter(pNull, L"VideoRender");
	if (FAILED(hr))
	{
		AfxMessageBox(_T("Can �� t add the VideoRender"));
		return  hr;
	}

	
	//  ��Ⱦý�壬���������˲�����������
	hr = m_pCapture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_pBF, pGrabberF, pNull);
	if (FAILED(hr))
	{
		AfxMessageBox(_T("Can �� t build the graph"));
		return  hr;
	}

	
	// ������Ƶ��ʾ����
	m_hWnd = hWnd;         // ��ʼ�����ھ��
	SetupVideoWindow();   // ������ʾ����



	hr = m_pMC->Run();   // ��ʼ�ɼ���Ԥ����Ƶ����ָ��������ʾ��Ƶ
	TRACE("��������ͼ:0x%x\n",hr);
	if (FAILED(hr)) {
		AfxMessageBox(_T("Couldn't run the graph!"));
		return hr;
	}
	return S_OK;
}


//�Ѳ������Ƶ�� AVI ��ʽд�ļ���ע������ǰֹͣ�����˲�������������ɺ�����������
/* ���ò�����Ƶ���ļ�����ʼ��׽��Ƶ����д�ļ� */
HRESULT CCaptureClass::CaptureImages(CString inFileName)
{

	HRESULT hr = 0;
	m_pMC->Stop();      // ��ֹͣ��Ƶ
	// �����ļ�����ע��ڶ�������������
	hr = m_pCapture->SetOutputFileName(&MEDIASUBTYPE_Avi,

		inFileName.AllocSysString(), &pMux, NULL);
	// ��Ⱦý�壬���������˲���
	hr = m_pCapture->RenderStream(&PIN_CATEGORY_CAPTURE,

		&MEDIATYPE_Video, m_pBF, NULL, pMux);
	pMux->Release();
	m_pMC->Run();     // �ظ���Ƶ
	return hr;
}


BOOL CCaptureClass::CaptureBitmap(const   char  * outFile) //const char * outFile)
{
	HRESULT hr = 0;
	// ȡ�õ�ǰ������ý�������
	AM_MEDIA_TYPE mt;
	hr = pGrabber->GetConnectedMediaType(&mt);
	// Examine the format block.
	VIDEOINFOHEADER *pVih;
	if ((mt.formattype == FORMAT_VideoInfo) &&
		(mt.cbFormat >= sizeof (VIDEOINFOHEADER)) &&
		(mt.pbFormat != NULL))
	{
		pVih = (VIDEOINFOHEADER*)mt.pbFormat;
	}
	else
	{
		// Wrong format. Free the format block and return an error.
		return  VFW_E_INVALIDMEDIATYPE;
	}
	// Set one-shot mode and buffering.
	hr = pGrabber->SetOneShot(TRUE);

	if (SUCCEEDED(pGrabber->SetBufferSamples(TRUE)))
	{
		bool  pass = false;
		m_pMC->Run();
		long  EvCode = 0;
		hr = pEvent->WaitForCompletion(INFINITE, &EvCode);
		//find the required buffer size
		long  cbBuffer = 0;
		if (SUCCEEDED(pGrabber->GetCurrentBuffer(&cbBuffer, NULL)))
		{
			//Allocate the array and call the method a second time to copy the buffer:
			char  *pBuffer = new   char[cbBuffer];
			if (!pBuffer)
			{
				// Out of memory. Return an error code.
				AfxMessageBox(_T("Out of Memory"));
			}
			hr = pGrabber->GetCurrentBuffer(&cbBuffer, (long *)(pBuffer));
			// д�� BMP �ļ���
			HANDLE hf = CreateFile(LPCTSTR(outFile), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
			if (hf == INVALID_HANDLE_VALUE)
			{
				return  0;
			}
			// Write the file header.
			BITMAPFILEHEADER bfh;
			ZeroMemory(&bfh, sizeof (bfh));
			bfh.bfType = 'MB';  // Little-endian for "MB".
			bfh.bfSize = sizeof (bfh)+cbBuffer + sizeof (BITMAPINFOHEADER);
			bfh.bfOffBits = sizeof (BITMAPFILEHEADER)+sizeof (BITMAPINFOHEADER);
			DWORD dwWritten = 0;
			WriteFile(hf, &bfh, sizeof (bfh), &dwWritten, NULL);
			// Write the bitmap format
			BITMAPINFOHEADER bih;
			ZeroMemory(&bih, sizeof (bih));
			bih.biSize = sizeof (bih);

			bih.biWidth = pVih->bmiHeader.biWidth;
			bih.biHeight = pVih->bmiHeader.biHeight;
			bih.biPlanes = pVih->bmiHeader.biPlanes;
			bih.biBitCount = pVih->bmiHeader.biBitCount;
			dwWritten = 0;
			WriteFile(hf, &bih, sizeof (bih), &dwWritten, NULL);
			//write the bitmap bits
			dwWritten = 0;
			WriteFile(hf, pBuffer, cbBuffer, &dwWritten, NULL);
			CloseHandle(hf);
			pass = true;
		}
		return  pass;
	}
	hr = pGrabber->SetOneShot(FALSE);
	return true;
}
/*
ϵͳ�豸ö����Ϊ���ǰ�����ö����ע����ϵͳ�е�Fitler�ṩ��ͳһ�ķ������������ܹ����ֲ�ͬ��Ӳ���豸��������ͬһ��Filter֧�����ǡ������Щʹ��Windows����ģ�ͺ�KSProxy Filter���豸��˵�Ƿǳ����õġ�ϵͳ�豸ö���������ǰ���ͬ���豸ʵ�����жԴ�����ע����Ȼ����֧����ͬFilter����
��������������ϵͳ�豸ö������ѯ�豸��ʱ��ϵͳ�豸ö����Ϊ�ض����͵��豸���磬��Ƶ�������Ƶѹ����������һ��ö�ٱ�Enumerator��������ö������Category enumerator��Ϊÿ���������͵��豸����һ��Moniker������ö�����Զ���ÿһ�ּ��弴�õ��豸�������ڡ�
  ���������µĲ���ʹ��ϵͳ�豸ö������
	����1�� ���÷���CoCreateInstance����ϵͳ�豸ö���������ʶ��CLSID��ΪCLSID_SystemDeviceEnum��
	  ����2�� ����ICreateDevEnum::CreateClassEnumerator������������ö����������Ϊ����Ҫ�õ������͵�CLSID���÷�������һ��IEnumMoniker�ӿ�ָ�룬���ָ�������ͣ��ǿյģ��򲻴��ڣ�����ICreateDevEnum::CreateClassEnumerator������S_FALSE�����Ǵ�����룬ͬʱIEnumMonikerָ�루��ע��ͨ���������أ�Ҳ�ǿյģ����Ҫ�������ڵ���CreateClassEnumerator��ʱ����ȷ��S_OK���бȽ϶�����ʹ�ú�SUCCEEDED��
		����3�� ʹ��IEnumMoniker::Next�������εõ�IEnumMonikerָ���е�ÿ��moniker���÷�������һ��IMoniker�ӿ�ָ�롣��Next����ö�ٵĵײ������ķ���ֵ��Ȼ��S_FALSE��������������Ҫ��S_OK�����м��顣
		  ����4�� ��Ҫ�õ����豸��Ϊ�Ѻõ����ƣ�������Ҫ���û������н�����ʾ��������IMoniker::BindToStorage������
			����5�� �����Ҫ���ɲ���ʼ��������豸��Filter����3����ָ���IMonitor::BindToObject����������������IFilterGraph::AddFilter�Ѹ�Filter��ӵ���ͼ�С�
			  */

int CCaptureClass::EnumDevices(HWND hList)
{
	if (!hList)	return -1;
	int id = 0;

	//ö�ٲ����豸
	ICreateDevEnum *pCreateDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
		CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum,
		(void**)&pCreateDevEnum);

	if (hr != NOERROR) 	return -1;

	IEnumMoniker *pEm;
	//������Ƶ����ö����
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
	//������Ƶ������
	//hr = pCreateDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory,&pEm, 0);

	if (hr != NOERROR) return -1;
	//����ö������λ
	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;
	while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK)
	{
		IPropertyBag *pBag;
		//�豸����ҳ
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if (SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR)
			{
				id++;
				::SendMessage(hList, CB_ADDSTRING, 0, (LPARAM)var.bstrVal);
				SysFreeString(var.bstrVal);
			}
			pBag->Release();
		}
		pM->Release();
	}
	return id;
}


/*
IPin *pPin=FindPin(m_pBF,"����");

IPin * pInputPinU1=m_pSmartTee->GetInputPin();
pPin->Connect(pInputPinU1,0);

m_pSmartTee->GetPreviewPin()->Connect(m_pAVIDecompressor->GetInputPin(),0);
m_pAVIDecompressor->GetOutputPin()->Connect(m_pHQGrabber->GetInputPin(),0);
m_pHQGrabber->GetOutputPin()->Connect(m_pVideoRenderer->GetInputPin(),0);
*/
IPin * CCaptureClass::FindPin(IBaseFilter *inFilter, char *inFilterName)
{
	HRESULT hr;
	IPin *pin = NULL;
	IPin * pFoundPin = NULL;
	IEnumPins *pEnumPins = NULL;
	hr = inFilter->EnumPins(&pEnumPins);
	if (FAILED(hr))
		return pFoundPin;
	ULONG fetched = 0;
	while (SUCCEEDED(pEnumPins->Next(1, &pin, &fetched)) && fetched)
	{
		PIN_INFO pinfo;
		pin->QueryPinInfo(&pinfo);
		pinfo.pFilter->Release();
		CString str(pinfo.achName);
		if (str == CString(inFilterName))
		{
			pFoundPin = pin;
			pin->Release();
			break;
		}
		pin->Release();
	}
	pEnumPins->Release();
	return pFoundPin;
}




