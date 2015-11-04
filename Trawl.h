#pragma once
#include "afxwin.h"


// Trawl 对话框

class Trawl : public CDialogEx
{
	DECLARE_DYNAMIC(Trawl)

public:
	Trawl(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Trawl();

// 对话框数据
	enum { IDD = IDD_TRAWL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnChangeEditShipname();
	afx_msg void OnChangeEditNetnum();
	afx_msg void OnClickedShipnamewrite();
	afx_msg void OnChangeEditMmsi();
	afx_msg void OnClickedMmsiwrite();
	afx_msg void OnClickedMmsiread();
	afx_msg void OnChangeEditCode();
	afx_msg void OnClickedEncryp();
	afx_msg void OnClickedEncode();
	afx_msg void OnClickedLocatWrite();
	afx_msg void OnChangeLocatLength();
	afx_msg void OnChangeLocatWidth();
	afx_msg void OnClickedShipnameread();

	void DisableTrawlItem(void);	
	CString V_Trawl_code;
	CString V_Trawl_MMSI;
	CString V_Trawl_NetNum;
	CString V_Trawl_ShipName;
	CString V_Trawl_Length;
	CString V_Trawl_Width;
	BOOL PreTranslateMessage(MSG *pMsg);
	CFont my_font;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void ShowBalloonTip(LPWSTR strTitile,LPWSTR strtext,UINT uStyle,int ID);
};
