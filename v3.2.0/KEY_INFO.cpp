// KEY_INFO.cpp : 实现文件
//

#include "stdafx.h"
#include "Ais_Net.h"
#include "KEY_INFO.h"
#include "afxdialogex.h"
#include "WSC.H"

// KEY_INFO 对话框
extern char icom;
extern int commport;
extern CWnd * pMainDlg;
static CFont font;
IMPLEMENT_DYNAMIC(KEY_INFO, CDialogEx)

KEY_INFO::KEY_INFO(CWnd* pParent /*=NULL*/)
	: CDialogEx(KEY_INFO::IDD, pParent)
{

	V_Key_ShipName = _T("");
}

KEY_INFO::~KEY_INFO()
{
}

void KEY_INFO::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ShipName, V_Key_ShipName);
	DDV_MaxChars(pDX, V_Key_ShipName, 17);
}


BEGIN_MESSAGE_MAP(KEY_INFO, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT_ShipName, &KEY_INFO::OnChangeEditShipName)
	ON_BN_CLICKED(IDC_ShipNameWrite, &KEY_INFO::OnClickedShipNameWrite)
	ON_BN_CLICKED(IDC_ShipNameRead, &KEY_INFO::OnClickedShipNameRead)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// KEY_INFO 消息处理程序


void KEY_INFO::OnChangeEditShipName()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ShipName);
	pEdit->LimitText(17);
	UpdateData(TRUE);

	if(!V_Key_ShipName.IsEmpty())
		GetDlgItem(IDC_ShipNameWrite)->EnableWindow(TRUE);
	else 
		GetDlgItem(IDC_ShipNameWrite)->EnableWindow(FALSE);
}

void KEY_INFO::OnClickedShipNameWrite()
{
	// TODO: 在此添加控件通知处理程序代码
	char L1,L2;
	char *ch1=NULL;char *ch2=NULL;char *ch=NULL; char i,j,k;
	unsigned long b[6];
	icom = 0;
	for(i=0;i<5;i++) b[i]=0;

	BYTE m_Array[18]; //发送0x03命令
	memset(m_Array,0,sizeof(m_Array));
	m_Array[0] = 0x24; m_Array[1] = 0x03;
	L1 = strlen(V_Key_ShipName);
	L2 = 0;
	m_Array[2] = (L1+L2+1)*6; //船名总长度  //m_Array.SetAt(2,(L1+L2+1)*6); 

	ch1 = ( char *)malloc(20*sizeof(char));
	ch2 = ( char *)malloc(2*sizeof(char));
	ch = ( char *)malloc(40*sizeof(char));

	ch1 = (LPSTR)(LPCTSTR)V_Key_ShipName; //cstring向char型转换
	printf("ch1 = %s \n",ch1);

	for(i=0;i < 20;i++)
	{	
		if(i < L1)
		{
			ch[i] = ch1[i]; ch[i] &= 0x3F; 
		}
		else 
			ch[i] = 0;
	}
	ch1[L1] = '-'; ch1[L1] &= 0x3F; ch[L1] = ch1[L1];

	for(i=0;i<5;i++)
	{	b[i] = ch[i*4]<<18; b[i] += ch[i*4+1]<<12; b[i] += ch[i*4+2]<<6; b[i] += ch[i*4+3];   }

	k=0;
	for(i=0;i<5;i++)
		for(j=0;j<3;j++)
		{
			if(k < 15)
			{	m_Array[3+k] = (unsigned char)(b[i]>>(16-j*8)); 
				printf("%d \n",(unsigned char)(b[i]>>(16-j*8)));
			}
			k++;
		}
	SioPuts(commport,(LPSTR)(m_Array), 18);
	SetTimer(1,2000,NULL);
	ch1 = NULL;
	ch2 = NULL;
	ch = NULL;
	free(ch1);
	free(ch2);
	free(ch); 

	pMainDlg->GetDlgItem(IDC_OpenCom)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(FALSE);
	GetDlgItem(IDC_ShipNameWrite)->EnableWindow(FALSE);
	GetDlgItem(IDC_ShipNameRead)->EnableWindow(FALSE);
}


void KEY_INFO::OnClickedShipNameRead()
{
	// TODO: 在此添加控件通知处理程序代码
	icom = 0;
		V_Key_ShipName="";	UpdateData(FALSE);

		BYTE m_Array[18];
		memset(m_Array,0,sizeof(m_Array));
		m_Array[0] = 0x24; m_Array[1] = 0x06;
		SioPuts(commport,(LPSTR)(m_Array), 18);

		SetTimer(1, 2000, NULL); //定时1秒

		pMainDlg->GetDlgItem(IDC_OpenCom)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(FALSE);
		GetDlgItem(IDC_ShipNameWrite)->EnableWindow(FALSE);
		GetDlgItem(IDC_ShipNameRead)->EnableWindow(FALSE);
}


void KEY_INFO::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
		case 1:
			KillTimer(1);
			pMainDlg->GetDlgItem(IDC_OpenCom)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(TRUE);
			if (!(V_Key_ShipName.IsEmpty()))
				GetDlgItem(IDC_ShipNameWrite)->EnableWindow(TRUE);
			GetDlgItem(IDC_ShipNameRead)->EnableWindow(TRUE);
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

BOOL KEY_INFO::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
        {
            return 1;
        }
    }


    return CDialog::PreTranslateMessage(pMsg);
}

HBRUSH KEY_INFO::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	//if(pWnd->GetDlgCtrlID() == IDC_STATIC1)
	//{
	//	if(IsConnect == 0)	
	//		pDC->SetTextColor(RGB(255,0,0)); //红色
	//	else if(IsConnect == 1)	
	//		pDC->SetTextColor(RGB(0,255,0)); //绿色
	//	else if(IsConnect == 2)	
			//pDC->SetTextColor(RGB(255,100,0)); //黄色

	//	pDC->SelectObject(&My_StaticFont);  // 设置字体
	//	pDC->SetBkColor(RGB(237,233,224));  //设置背景色
	//}

	// TODO:  在此更改 DC 的任何特性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
