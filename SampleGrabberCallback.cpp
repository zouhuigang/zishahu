
#include "SampleGrabberCallback.h"
#include "ImageFormatConversion.h"
#include "strsafe.h"

SampleGrabberCallback::SampleGrabberCallback()
{
	//���������
	StringCchCat(m_cameraName, 50, TEXT("camera_"));
	//�Ƿ��ͼ
	m_bGetPicture = FALSE;
	//Get template path
	GetTempPath(MAX_PATH, m_chTempPath);
	StringCchCat(m_chTempPath, MAX_PATH, TEXT("CaptureBmp"));
	CreateDirectory(m_chTempPath, NULL);
}

ULONG STDMETHODCALLTYPE SampleGrabberCallback::AddRef()
{
	return 1;
}

ULONG STDMETHODCALLTYPE SampleGrabberCallback::Release()
{
	return 2;
}

HRESULT STDMETHODCALLTYPE SampleGrabberCallback::QueryInterface(REFIID riid, void** ppvObject)
{
	if (NULL == ppvObject) return E_POINTER;
	if (riid == __uuidof(IUnknown))
	{
		*ppvObject = static_cast<IUnknown*>(this);
		return S_OK;
	}
	if (riid == IID_ISampleGrabberCB)
	{
		*ppvObject = static_cast<ISampleGrabberCB*>(this);
		return S_OK;
	}
	return E_NOTIMPL;

}

HRESULT STDMETHODCALLTYPE SampleGrabberCallback::SampleCB(double Time, IMediaSample *pSample)
{
	return E_NOTIMPL;
}

//��Ƶ����ص������Ȼ���ͼ
HRESULT STDMETHODCALLTYPE SampleGrabberCallback::BufferCB(double Time, BYTE *pBuffer, long BufferLen)
{
	if (FALSE == m_bGetPicture)  //�ж��Ƿ���Ҫ��ͼ
		return S_FALSE;
	if (!pBuffer)
		return E_POINTER;

	//MessageBox(NULL, TEXT("������"), TEXT("Message"), MB_OK);
	SaveBitmap(pBuffer, BufferLen);
	/*char FileName[256];
	sprintf_s(FileName, "capture_%d.yuv", (int)GetTickCount());
	FILE * out = fopen(FileName, "wb");
	fwrite(pBuffer, 1, BufferLen, out);
	fclose(out);*/

	m_bGetPicture = FALSE;
	return S_OK;
}


//����Ϊbmp��ʽͼƬ
BOOL  SampleGrabberCallback::SaveBitmap(BYTE * pBuffer, long lBufferSize)
{
	/*
	BMP�ļ���������4������ɣ��ֱ���λͼ�ļ�ͷ��λͼ��Ϣͷ����ɫ���ͼ������
	λͼ�ļ�ͷ��bitmap-file header��
	λͼ��Ϣͷ��bitmap-information header��
	��ɫ��/��ɫ�壨color table��
	λͼ���ݣ�bitmap-data��
	*/

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	StringCchCopy(m_chSwapStr, MAX_PATH, m_chTempPath);
	StringCchPrintf(m_chDirName, MAX_PATH, TEXT("\\%s_%04i%02i%02i%02i%02i%02i%03i_%ld.bmp"),
		m_cameraName,
		sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour,
		sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds, lBufferSize);
	StringCchCat(m_chSwapStr, MAX_PATH, m_chDirName);

	HANDLE hf = CreateFile(
		m_chSwapStr, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hf == INVALID_HANDLE_VALUE)return 0;

	// д�ļ�ͷ 
	BITMAPFILEHEADER bfh;
	memset(&bfh, 0, sizeof(bfh));
	bfh.bfType = 'MB';
	bfh.bfSize = sizeof(bfh)+lBufferSize + sizeof(BITMAPINFOHEADER);
	bfh.bfOffBits = sizeof(BITMAPINFOHEADER)+sizeof(BITMAPFILEHEADER);
	
	DWORD dwWritten = 0;
	WriteFile(hf, &bfh, sizeof(bfh), &dwWritten, NULL);
	// дλͼ��ʽ
	BITMAPINFOHEADER bih;
	memset(&bih, 0, sizeof(bih));
	bih.biSize = sizeof(bih);
	bih.biWidth = m_lWidth;
	bih.biHeight = m_lHeight;
	bih.biPlanes = 1;
	bih.biBitCount = 24;//ָ��ÿ�����ص���ɫ��ȣ�1��4,8,16,��24��32
	WriteFile(hf, &bih, sizeof(bih), &dwWritten, NULL);
	// дλͼ����
	WriteFile(hf, pBuffer, lBufferSize, &dwWritten, NULL);
	CloseHandle(hf);

	//���Ե���
	TCHAR pszDest[50];
	size_t cchDest = 50;
	StringCchPrintf(pszDest, cchDest, TEXT("m_lWidth:%d,m_lHeight:%d,lBufferSize:%ld"), m_lWidth, m_lHeight, lBufferSize);
	MessageBox(NULL, pszDest, TEXT("Message"), MB_OK);
	return 0;
}

/*
BOOL SampleGrabberCallback::SaveBitmap(BYTE * pBuffer, long lBufferSize)
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	StringCchCopy(m_chSwapStr, MAX_PATH, m_chTempPath);
	StringCchPrintf(m_chDirName, MAX_PATH, TEXT("\\%04i%02i%02i%02i%02i%02i%03ione.bmp"),
		sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour,
		sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
	StringCchCat(m_chSwapStr, MAX_PATH, m_chDirName);
	// %temp%/CaptureBmp/*
	//create picture file
	HANDLE hf = CreateFile(m_chSwapStr, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
		CREATE_ALWAYS, 0, NULL);
	if (hf == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	BITMAPFILEHEADER bfh;  //Set bitmap header
	ZeroMemory(&bfh, sizeof(bfh));
	bfh.bfType = 'MB';
	bfh.bfSize = sizeof(bfh)+lBufferSize + sizeof(BITMAPFILEHEADER);
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPFILEHEADER);
	// Write the file header.
	DWORD dwWritten = 0;
	WriteFile(hf, &bfh, sizeof(bfh), &dwWritten, NULL);
	// Write the file Format
	BITMAPINFOHEADER bih;
	ZeroMemory(&bih, sizeof(bih));
	bih.biSize = sizeof(bih);
	bih.biWidth = m_lWidth;
	bih.biHeight = m_lHeight;
	bih.biPlanes = 1;
	bih.biBitCount = m_iBitCount;  //Specifies the number of bits per pixel (bpp)
	WriteFile(hf, &bih, sizeof(bih), &dwWritten, NULL);
	//Write the file Data
	WriteFile(hf, pBuffer, lBufferSize, &dwWritten, NULL);
	CloseHandle(hf);

	// ͬʱ����jpgͼƬ
	char szSrcFileName[MAX_PATH];
	char szDstFileName[MAX_PATH];
	memset(szSrcFileName, 0, sizeof(char)*(MAX_PATH));
	memset(szDstFileName, 0, sizeof(char)*(MAX_PATH));
#ifdef _UNICODE
	DWORD num = WideCharToMultiByte(CP_ACP, 0, m_chSwapStr, -1, NULL, 0, NULL, 0);
	char *pbuf = NULL;
	pbuf = (char*)malloc(num * sizeof(char)) + 1;
	if (pbuf == NULL)
	{
		free(pbuf);
		return false;
	}
	memset(pbuf, 0, num * sizeof(char)+1);
	WideCharToMultiByte(CP_ACP, 0, m_chSwapStr, -1, pbuf, num, NULL, 0);
#else
	pbuf = (char*)m_chSwapStr;
#endif

	size_t len = strlen(pbuf);
	memcpy(szSrcFileName, pbuf, len);
	memcpy(szDstFileName, pbuf, len);
	memcpy(szDstFileName + len - 3, "jpg", 3);
	CImageFormatConversion	ifc;
	bool bRet = ifc.ToJpg(szSrcFileName, szDstFileName, 100);

	memcpy(szDstFileName + len - 3, "png", 3);
	bRet = ifc.ToPng(szSrcFileName, szDstFileName);

	//���Ե���
	TCHAR pszDest[50];
	size_t cchDest = 50;
	StringCchPrintf(pszDest, cchDest, TEXT("m_lWidth:%d,m_lHeight:%d,lBufferSize:%ld"), m_lWidth, m_lHeight, lBufferSize);
	MessageBox(NULL, pszDest, TEXT("Message"), MB_OK);

	return TRUE;
	}*/