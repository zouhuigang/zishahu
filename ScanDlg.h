#include <streams.h>

#include <windows.h>
#include <dbt.h>
#include <mmreg.h>
#include <msacm.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <commdlg.h>
#include <atlbase.h>
#include <strsafe.h>
#include "stdafx.h"

#include "CarameVideo.h"


//UINT ChildThread1(LPVOID Param);
//UINT ChildThread2(LPVOID Param);

#pragma once



class CScanDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScanDlg)

public:
	CScanDlg(CWnd* pParent = NULL);
	virtual ~CScanDlg();
	enum { IDD = IDD_SCAN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
	HRESULT SetupVideoWindow();
	HRESULT InitAndStartPreview();
	bool BindFilter(int deviceId, IBaseFilter **pFilter);
public:
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	void IMonRelease(IMoniker *&pm);
private:
	CStatic m_preview_1;
	CStatic m_preview_2;
	void GetAllCapDevices();
	BOOL selectDevice(Carame* cur_gcap, int index);
	BOOL MakeBuilder(Carame* cur_gcap);
	BOOL MakeGraph(Carame* cur_gcap);
	BOOL MakeCallback(Carame* cur_gcap);
	HRESULT ToPreview(int DIV_ID, Carame* cur_gcap);
	int carameCount;
	IMoniker *rgpmVideoMenu[10];
	Carame* gcapList;
	void takeAPicture(Carame* cur_gcap, int index);//≈ƒ’’
	HRESULT ShowVideo(Carame* cur_gcap, int DIV_ID);
	void GetComExceptionMessage(HRESULT hr);
	CarameVideo * cur_gcap;
	CarameVideo * cur_gcap2;

public:
	afx_msg void OnBnClickedButton1();
	virtual void PostNcDestroy();
};
