### 对比视频


	CComQIPtr< IVideoWindow, &IID_IVideoWindow > pWindow = cur_gcap->pFg;           //VideoWindow接口
	if (!pWindow)
	{
		MessageBox(TEXT("Could not get video window interface"));
		return E_FAIL;
	}


	GetDlgItem(DIV_ID, &hwndPreview);//获得预览窗口控件的Hwnd,只能显示一个摄像头
	RECT rc;
	::GetWindowRect(hwndPreview, &rc);

	hr = pWindow->put_Owner((OAHWND)hwndPreview);
	hr = pWindow->put_Left(0);
	hr = pWindow->put_Top(0);
	hr = pWindow->put_Width(rc.right - rc.left);
	hr = pWindow->put_Height(rc.bottom - rc.top);
	hr = pWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
	hr = pWindow->put_Visible(OATRUE);


相当于:

	//IVideoWindow* m_pVW=NULL;
	hr = cur_gcap->pFg->QueryInterface(IID_IVideoWindow, (LPVOID *)&cur_gcap->pVW);

	if (FAILED(hr))return hr;


	// set up the preview window to be in our dialog instead of floating popup


	GetDlgItem(DIV_ID, &hwndPreview);//获得预览窗口控件的Hwnd,只能显示一个摄像头
	RECT rc;
	::GetWindowRect(hwndPreview, &rc);

	hr = cur_gcap->pVW->put_Owner((OAHWND)hwndPreview);
	hr = cur_gcap->pVW->put_Left(0);
	hr = cur_gcap->pVW->put_Top(0);
	hr = cur_gcap->pVW->put_Width(rc.right - rc.left);
	hr = cur_gcap->pVW->put_Height(rc.bottom - rc.top);
	hr = cur_gcap->pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
	hr = cur_gcap->pVW->put_Visible(OATRUE);



---



		CComQIPtr< IMediaControl, &IID_IMediaControl > pControl = cur_gcap->pFg;
		
		hr = pControl->Run();
		if (FAILED(hr))
		{
			MessageBox(TEXT("Could not run ToPreview"));
			return hr;
		}