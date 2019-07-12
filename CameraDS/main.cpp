//////////////////////////////////////////////////////////////////////+
// Video Capture using DirectShow
// Author: Shiqi Yu (shiqi.yu@gmail.com)
// Thanks to:
//		HardyAI@OpenCV China
//		flymanbox@OpenCV China (for his contribution to function CameraName, and frame width/height setting)
// Last modification: April 9, 2009
//
// ʹ��˵����
//   1. ��CameraDS.h CameraDS.cpp�Լ�Ŀ¼DirectShow���Ƶ������Ŀ��
//   2. �˵� Project->Settings->Settings for:(All configurations)->C/C++->Category(Preprocessor)->Additional include directories
//      ����Ϊ DirectShow/Include
//   3. �˵� Project->Settings->Settings for:(All configurations)->Link->Category(Input)->Additional library directories
//      ����Ϊ DirectShow/Lib
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "camerads.h"

#include <highgui.h>

const char *g_szTitle = "Camera";

int main()
{
	// 1�����ǵ��Ѿ���������ʾͼ��Ĵ��ڣ��ǾͲ����ٴ���������ͷ�ˣ���������������Ҳ������ͷ�ѱ�ռ�á�
	if(IsWindowVisible(FindWindow(NULL, g_szTitle)))
	{
		return (-1);
	}

	//������ȡ����ͷ��Ŀ
	int m_iCamCount = CCameraDS::CameraCount();
	printf("There are %d cameras.\n", m_iCamCount);

	if(m_iCamCount == 0)
	{
		return (-1);
	}

	CCameraDS m_CamDS;

	//��ȡ��������ͷ������
	for(int i = 0; i < m_iCamCount; i++)
	{
		char szCamName[1024];

		int retval = m_CamDS.CameraName(i, szCamName, sizeof(szCamName));

		if(retval >0)
		{
			printf("Camera #%d's Name is '%s'.\n", i, szCamName);
		}
		else
		{
			printf("Can not get Camera #%d's name.\n", i);
		}
	}

	// 2�����ǵ�����ж������ͷ��������������ĳ����ĳ�������ڱ���������ռ�У�����Ҫ���������
	// ֱ���ҵ����õ�Ϊֹ��
	int m_iCamNum = 0; // ����ͷ���

	IplImage *pFrame = NULL;

	while(m_iCamNum < m_iCamCount)
	{
		if((! m_CamDS.OpenCamera(m_iCamNum, false, 320, 240)) || ((pFrame = m_CamDS.QueryFrame()) == NULL))
		{
			m_iCamNum++;
		}
		else
		{ // �ҵ����ʵ�����ͷ���˳�ѭ����
			break;
		}

		// �ر�����ͷ������Ҫ�رգ���Ϊ����Ҫ������һ�εļ�⣬���ǰҪ��յ�ǰ��ռ�ÿռ䡣
		m_CamDS.CloseCamera();
	}

	if(m_iCamNum == m_iCamCount)
	{
		fprintf(stderr, "Can not open camera or is used by another app.\n");

		return (-1);
	}

	cvNamedWindow(g_szTitle);
	//��ʾ
	cvShowImage(g_szTitle, pFrame);

	while(1)
	{
		//��ȡһ֡
		pFrame = m_CamDS.QueryFrame();

		//��ʾ
		cvShowImage(g_szTitle, pFrame);

		if (cvWaitKey(20) == 'q')
		{
			break;
		}
	}

	m_CamDS.CloseCamera(); //�ɲ����ô˺�����CCameraDS����ʱ���Զ��ر�����ͷ

	cvDestroyWindow(g_szTitle);

	return 0;
}
