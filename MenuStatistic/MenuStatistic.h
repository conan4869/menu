
// MenuStatistic.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMenuStatisticApp:
// �йش����ʵ�֣������ MenuStatistic.cpp
//

class CMenuStatisticApp : public CWinApp
{
public:
	CMenuStatisticApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMenuStatisticApp theApp;