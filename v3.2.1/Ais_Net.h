
// Ais_Net.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CAis_NetApp:
// 有关此类的实现，请参阅 Ais_Net.cpp
//

class CAis_NetApp : public CWinApp
{
public:
	CAis_NetApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
	afx_msg void OnIdok();
};

extern CAis_NetApp theApp;