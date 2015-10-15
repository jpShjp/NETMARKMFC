#pragma once


// KEY_INFO 对话框

class KEY_INFO : public CDialogEx
{
	DECLARE_DYNAMIC(KEY_INFO)

public:
	KEY_INFO(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~KEY_INFO();

// 对话框数据
	enum { IDD = IDD_KEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChangeEditShipName();
	afx_msg void OnClickedShipNameWrite();
	afx_msg void OnClickedShipNameRead();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	BOOL PreTranslateMessage(MSG* pMsg);
	CString V_Key_ShipName;
};
