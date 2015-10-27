
// Ais_NetDlg.h : 头文件
//

#pragma once
#include "DriftNet.h"
#include "trawl.h"
#include "key_info.h"
#include "afxwin.h"

// CAis_NetDlg 对话框
class CAis_NetDlg : public CDialogEx
{
// 构造
public:
	CAis_NetDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_AIS_NET_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpenCom();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void getcomm(void);
	void EnableItem(void);
	void PortSet(void);
	void ItemInit(void);
	void EditEmpty(void);	
	CStatic static1;
	CButton OpenCom;
	CFont My_StaticFont;	
	CTabCtrl m_tabCtrl;
	DriftNet page1;
	Trawl page2;	
	KEY_INFO page3;
	HCURSOR m_hCursor;
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	CStatic m_Picture;
};
