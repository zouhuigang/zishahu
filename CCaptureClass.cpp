#include "CCaptureClass.h"

/* 类构造函数实现 */
CCaptureClass::CCaptureClass()
{
	CoInitialize(NULL);  //COM  库初始化
	m_hWnd = NULL;    // 视频显示窗口的句柄
	m_pVW = NULL;    // 视频窗口接口指针清空
	m_pMC = NULL;    // 媒体控制接口指针清空
	m_pGB = NULL;   // 滤波器链表管理器接口指针清空
	m_pBF = NULL;     // 捕获滤波器接口指针清空  
	pBasicVideo = NULL;// 基类视频接口指针清空
	pGrabberF = NULL; // 采样滤波器接口指针清空
	pNull = NULL; // 渲染滤波器接口清空
	pGrabber = NULL; //
	pEvent = NULL; // 媒体事件接口指针清空
	m_pCapture = NULL;   // 增强型捕获滤波器链表管理器接口指针清空

}
/* 析构函数实现 */
CCaptureClass::~CCaptureClass()
{
	if (m_pMC)  m_pMC->Stop();   // 首先停止媒体
	if (m_pVW) {
		m_pVW->put_Visible(OAFALSE); // 视频窗口不可见
		m_pVW->put_Owner(NULL);  // 视频窗口的父窗口清空
	}
	srelease(m_pCapture);       // 释放增强型捕获滤波器链表管理器接口 srelease(pBasicVideo);
	srelease(pGrabber);
	srelease(pGrabberF);
	srelease(pNull);
	srelease(pEvent);
	srelease(m_pMC);       // 释放媒体控制接口
	srelease(m_pGB);       // 释放滤波器链表管理器接口
	srelease(m_pBF);      // 释放捕获滤波器接口

	CoUninitialize();     // 卸载 COM 库
}
// 把指定采集设备与滤波器捆绑
/*

该函数的传入参数是采集设备的索引号和捕获设备的滤波器。
根据索引号查询系统中的视频捕获设备。
以友好名字（ FriendlyName ）的方式获取所选设备的信息，
然后把查询成功的设备与传入的滤波器捆绑，
返回捕获设备的滤波器。
*/
bool CCaptureClass::BindFilter(int deviceId, IBaseFilter **pFilter)
{
	if (deviceId < 0) return false;
	// 枚举所有的视频捕获设备
	ICreateDevEnum *pCreateDevEnum;
	// 生成设备枚举器 pCreateDevEnum
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
		CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum,
		(void**)&pCreateDevEnum);
	if (hr != NOERROR)  return false;
	IEnumMoniker *pEm;
	// 创建视频输入设备类枚举器
	hr = pCreateDevEnum->CreateClassEnumerator
		(CLSID_VideoInputDeviceCategory,
		&pEm, 0);
	if (hr != NOERROR) return false;
	pEm->Reset();         // 复位该设备
	ULONG cFetched;
	IMoniker *pM;
	int index = 0;
	// 获取设备
	while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK, index <= deviceId)
	{
		IPropertyBag *pBag;
		// 获取该设备的属性集
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if (SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;     // 保存的是二进制的数据
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			// 获取 FriendlyName 形式的信息
			if (hr == NOERROR)
			{
				// 采集设备与捕获滤波器捆绑
				if (index == deviceId) pM->BindToObject(0, 0,
					IID_IBaseFilter, (void**)pFilter);
				SysFreeString(var.bstrVal); // 释放二进制数据资源，必须释放



			}
			pBag->Release();
		}
		pM->Release();
		index++;
	}
	return true;
}



/* 设置视频显示窗口的特性 */
HRESULT CCaptureClass::SetupVideoWindow()
{
	HRESULT hr;
	//m_hWnd 为类 CCaptureClass 的成员变量，在使用该函数前须初始化
	hr = m_pVW->put_Visible(OAFALSE);                  // 视频窗口不可见
	hr = m_pVW->put_Owner((OAHWND)m_hWnd);      // 窗口所有者：传入的窗口句柄
	if (FAILED(hr)) return hr;
	hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);// 设置窗口类型
	if (FAILED(hr)) return hr;
	ResizeVideoWindow();                           // 更改窗口大小
	hr = m_pVW->put_Visible(OATRUE);              // 视频窗口可见
	return hr;
}
/* 更改视频窗口大小 */
void CCaptureClass::ResizeVideoWindow()
{
	if (m_pVW) {
		// 让图像充满整个指定窗口
		CRect rc;
		::GetClientRect(m_hWnd, &rc);    // 获取显示窗口的客户区
		m_pVW->SetWindowPosition(0, 0, rc.right, rc.bottom);

		
		// 设置视频显示窗口的位置
	}
}


/*创建滤波器链表管理器，查询其各种控制接口*/
HRESULT CCaptureClass::InitCaptureGraphBuilder()
{
	HRESULT hr;
	//创建IGraphBuilder接口
	hr = CoCreateInstance(CLSID_FilterGraph, NULL,CLSCTX_INPROC_SERVER,IID_IGraphBuilder, (void **)&m_pGB);
	TRACE("InitCaptureGraphBuilder init:0x%x\n", hr);
	if (FAILED(hr)) return hr;
	//创建ICaptureGraphBuilder2接口
	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL,
		CLSCTX_INPROC,
		IID_ICaptureGraphBuilder2, (void **)&m_pCapture);
	TRACE("InitCaptureGraphBuilder0:0x%x\n", hr);
	if (FAILED(hr)) return hr;
	//初始化滤波器链表管理器IGraphBuilder
	m_pCapture->SetFiltergraph(m_pGB);
	//查询媒体控制接口
	hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);
	TRACE("InitCaptureGraphBuilder1:0x%x\n", hr);
	if (FAILED(hr)) return hr;
	//查询视频窗口接口
	hr = m_pGB->QueryInterface(IID_IVideoWindow, (LPVOID *)&m_pVW);
	TRACE("InitCaptureGraphBuilder2:0x%x\n", hr);
	if (FAILED(hr)) return hr;
	return hr;
}

/* 开始预览视频数据 */
HRESULT CCaptureClass::PreviewImages(int iDeviceID, HWND hWnd)
{
	HRESULT hr;

	//  初始化视频捕获滤波器链表管理器
	hr = InitCaptureGraphBuilder();
	if (FAILED(hr))
	{
		AfxMessageBox(_T("Failed to get video interfaces!"));
		return  hr;
	}
	//  把指定采集设备与滤波器捆绑
	if (!BindFilter(iDeviceID, &m_pBF))
		return  S_FALSE;
	//  把滤波器添加到滤波器链表中
	hr = m_pGB->AddFilter(m_pBF, L"Capture Filter");
	if (FAILED(hr))
	{
		AfxMessageBox(_T("Can ’ t add the capture filter"));
		return  hr;
	}


	// Create the Sample Grabber.
	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (void **)&pGrabberF);
	if (FAILED(hr))
	{
		AfxMessageBox(_T("Can ’ t create the grabber"));
		return  hr;
	}

	

	hr = pGrabberF->QueryInterface(IID_ISampleGrabber, (void **)&pGrabber);
	//  把滤波器添加到滤波器链表中


	hr = m_pGB->AddFilter(pGrabberF, L"Sample Grabber");
	if (FAILED(hr))
	{
		AfxMessageBox(_T("Can ’ t add the grabber"));
		return  hr;
	}

	
	// Add the Null Renderer filter to the graph.
	hr = CoCreateInstance(CLSID_VideoRenderer, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (void **)&pNull);
	hr = m_pGB->AddFilter(pNull, L"VideoRender");
	if (FAILED(hr))
	{
		AfxMessageBox(_T("Can ’ t add the VideoRender"));
		return  hr;
	}

	
	//  渲染媒体，把链表中滤波器连接起来
	hr = m_pCapture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_pBF, pGrabberF, pNull);
	if (FAILED(hr))
	{
		AfxMessageBox(_T("Can ’ t build the graph"));
		return  hr;
	}

	
	// 设置视频显示窗口
	m_hWnd = hWnd;         // 初始化窗口句柄
	SetupVideoWindow();   // 设置显示窗口



	hr = m_pMC->Run();   // 开始采集、预览视频，在指定窗口显示视频
	TRACE("不能运行图:0x%x\n",hr);
	if (FAILED(hr)) {
		AfxMessageBox(_T("Couldn't run the graph!"));
		return hr;
	}
	return S_OK;
}


//把捕获的视频以 AVI 格式写文件。注意设置前停止调用滤波器链表，设置完成后再运行链表。
/* 设置捕获视频的文件，开始捕捉视频数据写文件 */
HRESULT CCaptureClass::CaptureImages(CString inFileName)
{

	HRESULT hr = 0;
	m_pMC->Stop();      // 先停止视频
	// 设置文件名，注意第二个参数的类型
	hr = m_pCapture->SetOutputFileName(&MEDIASUBTYPE_Avi,

		inFileName.AllocSysString(), &pMux, NULL);
	// 渲染媒体，链接所有滤波器
	hr = m_pCapture->RenderStream(&PIN_CATEGORY_CAPTURE,

		&MEDIATYPE_Video, m_pBF, NULL, pMux);
	pMux->Release();
	m_pMC->Run();     // 回复视频
	return hr;
}


BOOL CCaptureClass::CaptureBitmap(const   char  * outFile) //const char * outFile)
{
	HRESULT hr = 0;
	// 取得当前所连接媒体的类型
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
			// 写到 BMP 文件中
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
系统设备枚举器为我们按类型枚举已注册在系统中的Fitler提供了统一的方法。而且它能够区分不同的硬件设备，即便是同一个Filter支持它们。这对那些使用Windows驱动模型和KSProxy Filter的设备来说是非常有用的。系统设备枚举器对它们按不同的设备实例进行对待（译注：虽然它们支持相同Filter）。
　　当我们利用系统设备枚举器查询设备的时候，系统设备枚举器为特定类型的设备（如，音频捕获和视频压缩）生成了一张枚举表（Enumerator）。类型枚举器（Category enumerator）为每个这种类型的设备返回一个Moniker，类型枚举器自动把每一种即插即用的设备包含在内。
  　　按如下的步骤使用系统设备枚举器：
	　　1． 调用方法CoCreateInstance生成系统设备枚举器。类标识（CLSID）为CLSID_SystemDeviceEnum。
	  　　2． 调用ICreateDevEnum::CreateClassEnumerator方法生成类型枚举器，参数为你想要得到的类型的CLSID，该方法返回一个IEnumMoniker接口指针，如果指定的类型（是空的）或不存在，函数ICreateDevEnum::CreateClassEnumerator将返回S_FALSE而不是错误代码，同时IEnumMoniker指针（译注：通过参数返回）也是空的，这就要求我们在调用CreateClassEnumerator的时候明确用S_OK进行比较而不是使用宏SUCCEEDED。
		　　3． 使用IEnumMoniker::Next方法依次得到IEnumMoniker指针中的每个moniker。该方法返回一个IMoniker接口指针。当Next到达枚举的底部，它的返回值仍然是S_FALSE，这里我们仍需要用S_OK来进行检验。
		  　　4． 想要得到该设备较为友好的名称（例如想要在用户界面中进行显示），调用IMoniker::BindToStorage方法。
			　　5． 如果想要生成并初始化管理该设备的Filter调用3返回指针的IMonitor::BindToObject方法，接下来调用IFilterGraph::AddFilter把该Filter添加到视图中。
			  */

int CCaptureClass::EnumDevices(HWND hList)
{
	if (!hList)	return -1;
	int id = 0;

	//枚举捕获设备
	ICreateDevEnum *pCreateDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
		CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum,
		(void**)&pCreateDevEnum);

	if (hr != NOERROR) 	return -1;

	IEnumMoniker *pEm;
	//创建视频类型枚举器
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
	//创建音频捕获类
	//hr = pCreateDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory,&pEm, 0);

	if (hr != NOERROR) return -1;
	//类型枚举器复位
	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;
	while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK)
	{
		IPropertyBag *pBag;
		//设备属性页
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
IPin *pPin=FindPin(m_pBF,"捕获");

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




