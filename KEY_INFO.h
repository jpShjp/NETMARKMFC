#pragma once


// KEY_INFO �Ի���

class KEY_INFO : public CDialogEx
{
	DECLARE_DYNAMIC(KEY_INFO)

public:
	KEY_INFO(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~KEY_INFO();

// �Ի�������
	enum { IDD = IDD_KEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChangeEditShipName();
	afx_msg void OnClickedShipNameWrite();
	afx_msg void OnClickedShipNameRead();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	BOOL PreTranslateMessage(MSG* pMsg);
	CString V_Key_ShipName;
};
