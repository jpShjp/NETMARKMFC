
// Ais_Net.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAis_NetApp:
// �йش����ʵ�֣������ Ais_Net.cpp
//

class CAis_NetApp : public CWinApp
{
public:
	CAis_NetApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	afx_msg void OnIdok();
};

extern CAis_NetApp theApp;