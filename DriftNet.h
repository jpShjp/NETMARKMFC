#pragma once
#include "afxwin.h"

// DriftNet 对话框

class DriftNet : public CDialog
{
	DECLARE_DYNAMIC(DriftNet)

public:
	DriftNet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DriftNet();

// 对话框数据
	enum { IDD = IDD_DriftNet };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnClickedShipNameWrite();
	afx_msg void OnClickedShipnameread();
	afx_msg void OnClickedMmsiread();
	afx_msg void OnClickedMmsiwrite();
	afx_msg void OnClickedIntervalwrite();
	afx_msg void OnClickedEncryp();
	afx_msg void OnClickedEncode();
	afx_msg void OnClickedSizeWrite();
	afx_msg void OnClickedRadio30s();
	afx_msg void OnClickedRadio1min();
	afx_msg void OnClickedRadio3min();
	afx_msg void OnClickedSizeNo();
	afx_msg void OnClickedSizeYes();

	afx_msg void OnChangeEditShipname();
	afx_msg void OnChangeEditNetnum();
	afx_msg void OnChangeEditMmsi();
	afx_msg void OnChangeEditCode();
	afx_msg void OnChangeSizeLength();
	afx_msg void OnChangeSizeWidth();

	CString V_Drift_shipName;
	CString V_Drift_NetNum;
	CString V_Drift_MMSI;
	CString V_Drift_Code;
	CString V_Size_Width;
	CString V_Size_Length;

	int V_Drift_Interval;
	void DisablePage1Item(void);
	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClickedKind1();
	afx_msg void OnClickedKind2();
	afx_msg void OnClickedKindWrite();
	char kind;
	CButton bt_shipname_write;
	CButton bt_mmsi_write;
	CButton bt_size_write;
	CButton bt_intervalwrite;
	CButton bt_kindwrite;
	void shipNameWrite(void);
	void shipNameRead(void);
	void MMSIWrite(void);
	void MMSIRead (void);
	void EnCode(void);
	void EnCryp(void);
	void IntervalWrite(void);
	void kindWrite(void);
	void sizeWrite(void);
	afx_msg void OnBnClickedAllwrite();
	afx_msg void OnBnClickedAllread();
};