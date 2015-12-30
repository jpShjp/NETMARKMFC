// Trawl.cpp : 实现文件
//

#include "stdafx.h"
#include "Ais_Net.h"
#include "Trawl.h"
#include "afxdialogex.h"
#include "WSC.H"
#include <math.h>

extern CWnd * pMainDlg;
extern char icom;
extern int commport;
extern char IsSize;

// 实现编辑框的冒泡提示  
void Trawl::ShowBalloonTip(LPWSTR strTitile,LPWSTR strtext,UINT uStyle,int id)  
{  
    EDITBALLOONTIP ebt;  
    ebt.cbStruct = sizeof(EDITBALLOONTIP);  
    ebt.pszText = strtext;  
    ebt.pszTitle = strTitile;  
    ebt.ttiIcon = uStyle;  
    Edit_ShowBalloonTip(((CEdit*)GetDlgItem(id))->GetSafeHwnd(),&ebt);  
}

// 禁用小控件
void Trawl::DisableTrawlItem(void)
{
	pMainDlg->GetDlgItem(IDC_OpenCom)->EnableWindow(FALSE);
	GetDlgItem(IDC_ShipNameWrite)->EnableWindow(FALSE);
	GetDlgItem(IDC_ShipNameRead)->EnableWindow(FALSE);
	GetDlgItem(IDC_MMSIWrite)->EnableWindow(FALSE);
	GetDlgItem(IDC_MMSIRead)->EnableWindow(FALSE);
	GetDlgItem(IDC_Encryp)->EnableWindow(FALSE);
	GetDlgItem(IDC_Encode)->EnableWindow(FALSE);
	GetDlgItem(IDC_LOCAT_WRITE)->EnableWindow(FALSE);
	GetDlgItem(IDC_LOCAT_READ)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_NetNum)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MMSI)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_Code)->EnableWindow(FALSE);

	GetDlgItem(IDC_LOCAT_LENGTH)->EnableWindow(FALSE);
	GetDlgItem(IDC_LOCAT_WIDTH)->EnableWindow(FALSE);
}

// Trawl 对话框

IMPLEMENT_DYNAMIC(Trawl, CDialogEx)

Trawl::Trawl(CWnd* pParent /*=NULL*/)
	: CDialogEx(Trawl::IDD, pParent)
{

	V_Trawl_code = _T("");
	V_Trawl_MMSI = _T("");
	V_Trawl_NetNum = _T("");
	V_Trawl_ShipName = _T("");
	V_Trawl_Length = _T("");
	V_Trawl_Width = _T("");
}

Trawl::~Trawl()
{
}

void Trawl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_Code, V_Trawl_code);
	DDV_MaxChars(pDX, V_Trawl_code, 6);
	DDX_Text(pDX, IDC_EDIT_MMSI, V_Trawl_MMSI);
	DDV_MaxChars(pDX, V_Trawl_MMSI, 9);
	DDX_Text(pDX, IDC_EDIT_NetNum, V_Trawl_NetNum);
	DDV_MaxChars(pDX, V_Trawl_NetNum, 2);
	DDX_Text(pDX, IDC_EDIT_ShipName, V_Trawl_ShipName);
	DDV_MaxChars(pDX, V_Trawl_ShipName, 17);
	DDX_Text(pDX, IDC_LOCAT_LENGTH, V_Trawl_Length);
	DDX_Text(pDX, IDC_LOCAT_WIDTH, V_Trawl_Width);
}


BEGIN_MESSAGE_MAP(Trawl, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT_ShipName, &Trawl::OnChangeEditShipname)
	ON_EN_CHANGE(IDC_EDIT_NetNum, &Trawl::OnChangeEditNetnum)
	ON_BN_CLICKED(IDC_ShipNameWrite, &Trawl::OnClickedShipnamewrite)
	ON_EN_CHANGE(IDC_EDIT_MMSI, &Trawl::OnChangeEditMmsi)
	ON_BN_CLICKED(IDC_MMSIWrite, &Trawl::OnClickedMmsiwrite)
	ON_BN_CLICKED(IDC_MMSIRead, &Trawl::OnClickedMmsiread)
	ON_EN_CHANGE(IDC_EDIT_Code, &Trawl::OnChangeEditCode)
	ON_BN_CLICKED(IDC_Encryp, &Trawl::OnClickedEncryp)
	ON_BN_CLICKED(IDC_Encode, &Trawl::OnClickedEncode)
	ON_BN_CLICKED(IDC_LOCAT_WRITE, &Trawl::OnClickedLocatWrite)
	ON_EN_CHANGE(IDC_LOCAT_LENGTH, &Trawl::OnChangeLocatLength)
	ON_EN_CHANGE(IDC_LOCAT_WIDTH, &Trawl::OnChangeLocatWidth)
	ON_BN_CLICKED(IDC_ShipNameRead, &Trawl::OnClickedShipnameread)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_LOCAT_READ, &Trawl::OnBnClickedLocatRead)
END_MESSAGE_MAP()


// Trawl 消息处理程序


void Trawl::OnChangeEditShipname()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ShipName);
	pEdit->LimitText(17);
	UpdateData(TRUE);
	if(!V_Trawl_ShipName.IsEmpty() && !V_Trawl_NetNum.IsEmpty()) 
		GetDlgItem(IDC_ShipNameWrite)->EnableWindow(TRUE);
	else 
		GetDlgItem(IDC_ShipNameWrite)->EnableWindow(FALSE);
}


void Trawl::OnChangeEditNetnum()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_NetNum);
	pEdit->LimitText(2);
	UpdateData(TRUE);

	if(!V_Trawl_ShipName.IsEmpty() && !V_Trawl_NetNum.IsEmpty()) 
		GetDlgItem(IDC_ShipNameWrite)->EnableWindow(TRUE);
	else 
		GetDlgItem(IDC_ShipNameWrite)->EnableWindow(FALSE);
}

void Trawl::OnClickedShipnameread()
{
	// TODO: 在此添加控件通知处理程序代码

	icom = 0;

	V_Trawl_ShipName="";
	V_Trawl_NetNum="";	
	UpdateData(FALSE);

	BYTE m_Array[18]; //发送0x04命令
	memset(m_Array,0,sizeof(m_Array));
	m_Array[0] = 0x24; m_Array[1] = 0x04;
	SioPuts(commport,(LPSTR)(m_Array), 18);

	SetTimer(1, 2000, NULL); //定时2秒
	DisableTrawlItem();
}



void Trawl::OnClickedShipnamewrite()
{
	// TODO: 在此添加控件通知处理程序代码
	
	char L1,L2;
	char temp;
	char *ch1=NULL;
	char *ch2=NULL;
	char *ch=NULL; 
	char i,j,k;
	unsigned long b[5];
	icom = 0;
	for(i=0;i<5;i++) 
		b[i]=0;

	//UpdateData(TRUE); //读取编辑框内容

	BYTE m_Array[18]; //识别网位仪是否插入的命令
	memset(m_Array,0,sizeof(m_Array));
	m_Array[0] = 0x24; 
	m_Array[1] = 0x01;
	L1 = V_Trawl_ShipName.GetLength();
	L2 = V_Trawl_NetNum.GetLength();
	m_Array[2] = (L1+L2+1)*6; //m_Array.SetAt(2,(L1+L2+1)*6); //船名总长度

	ch1 = ( char *)malloc(20*sizeof(char));
	ch2 = ( char *)malloc(2*sizeof(char));
	ch = ( char *)malloc(40*sizeof(char));
	ch1 = (LPSTR)(LPCTSTR)V_Trawl_ShipName; //cstring向char型转换

	for(i=0;i < L1;i++)
	{	
		ch[i] = ch1[i]; 
		ch[i] &= 0x3F;printf("i = %d,ch1[i] = %d \n",i,ch1[i]);
	}

	ch[i] = '-'; ch[i] &= 0x3F;
	ch2 = (char*)(LPCTSTR)V_Trawl_NetNum;
	k = 0;

	for(i=L1+1;i<20;i++)
	{	
		k++;
		if(i < L1+L2+1)
		{
			ch[i] = ch2[k-1]; 
			ch[i] &= 0x3F;
		}
		else
		{
			ch[i] = 0;printf("%c \n",ch[i]);
		}
	}

	for(i=0;i<5;i++)
	{	
		b[i] = ch[i*4]<<18; 
		b[i] += ch[i*4+1]<<12;
		b[i] += ch[i*4+2]<<6; 
		b[i] += ch[i*4+3];  
	}

	k=0;
	temp = (L1+L2+1)*6/8;

	if((L1+L2+1)*6%8 != 0)   
		temp++;
	for(i=0;i<5;i++)
	{
		for(j=0;j<3;j++)
		{
			if(k < temp)
			{	
				m_Array[3+k] = (unsigned char)(b[i]>>(16-j*8)); 
			}
			k++;
		}
	}

	SioPuts(commport,(LPSTR)(m_Array), 18); 
	SetTimer (1,2000,NULL); 

	DisableTrawlItem();

}


void Trawl::OnChangeEditMmsi()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_MMSI);
	pEdit->LimitText(9);
	UpdateData(TRUE);
	if (V_Trawl_MMSI.GetLength() == 9)
	{
		GetDlgItem(IDC_MMSIWrite)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_MMSIWrite)->EnableWindow(FALSE);
	}
}


void Trawl::OnClickedMmsiwrite()
{
	// TODO: 在此添加控件通知处理程序代码

	//UpdateData(TRUE); //读取编辑框内容

	icom = 0;

	unsigned long rev = 0;

	rev = atol(V_Trawl_MMSI);

	BYTE m_Array[18]; //发送0x02命令
	memset(m_Array,0,sizeof(m_Array));
	m_Array[0] = 0x24; 
	m_Array[1] = 0x02;
	m_Array[2] = (unsigned char)(rev>>24); 
	m_Array[3] = (unsigned char)(rev>>16); 
	m_Array[4] = (unsigned char)(rev>>8); 
	m_Array[5] = (unsigned char)rev;
	SioPuts(commport,(LPSTR)(m_Array), 18);
	SetTimer (1,2000,NULL); 

	DisableTrawlItem();
}


void Trawl::OnClickedMmsiread()
{
	// TODO: 在此添加控件通知处理程序代码

	icom = 0;
	V_Trawl_MMSI="";	
	UpdateData(FALSE);


	BYTE m_Array[18]; //发送0x05命令
	memset(m_Array,0,sizeof(m_Array));
	m_Array[0] = 0x24;
	m_Array[1] = 0x05;
	SioPuts(commport,(LPSTR)(m_Array), 18);
	SetTimer (1,2000,NULL); 
	
	DisableTrawlItem();
}


void Trawl::OnChangeEditCode()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_Code);
	pEdit->LimitText(6);
	UpdateData(TRUE);

	if (V_Trawl_code.GetLength() == 6)
	{
		GetDlgItem(IDC_Encryp)->EnableWindow(TRUE);
		GetDlgItem(IDC_Encode)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_Encryp)->EnableWindow(FALSE);
		GetDlgItem(IDC_Encode)->EnableWindow(FALSE);
	}
}


void Trawl::OnClickedEncryp()
{
	// TODO: 在此添加控件通知处理程序代码
	static CString code1,code2;
	static char page2_encryp_time; //密码输入的次数
	char i;
	icom = 0;

	if (page2_encryp_time == 0) // 第一次加密
	{
		code1 = ""+V_Trawl_code+"";
		GetDlgItem(IDC_EDIT_Code)->SetWindowText("");
		MessageBox("请再输入一遍密码","提示"); 
		page2_encryp_time = 1; 
	}
	else if (page2_encryp_time == 1 )
	{
		char *ch;
		ch = (char*)malloc(6*sizeof (char));
		code2 = ""+V_Trawl_code+"";
		ch = (LPSTR)(LPCTSTR)V_Trawl_code;
		 
		if (strcmp (code1,code2) == 0)
		{
			BYTE m_Array[18]; //发送0x0A命令
			memset(m_Array,0,sizeof(m_Array));
			m_Array[0] = 0x24;
			m_Array[1] = 0x0A;

			for(i=0;i<6;i++)
			{
				m_Array[i+2] = ch[i];//{m_Array.SetAt(i+2,ch[i]);  printf("%c \n",ch[i]);}
			}
			SioPuts(commport,(LPSTR)(m_Array), 18);//g_mainwnd->m_comm1.put_Output(COleVariant(m_Array));  
			ch = NULL;
			free(ch);
			SetTimer (1,2000,NULL); 
			DisableTrawlItem();
		}
		else
		{
			MessageBox ("密码输入错误,请重新输入!","提示");
		}
		page2_encryp_time = 0;
		V_Trawl_code=""; 
		UpdateData(FALSE);
	}
}


void Trawl::OnClickedEncode()
{
	// TODO: 在此添加控件通知处理程序代码
	char i;
	icom = 0;
	BYTE m_Array[18]; //发送0x0B命令
	memset(m_Array,0,sizeof(m_Array));
	m_Array[0] = 0x24;
	m_Array[1] = 0x0B;

	char *ch;
	ch = (LPSTR)(LPCTSTR)V_Trawl_code; //cstring向char型转换
	for(i=0;i<6;i++)
	{	m_Array[i+2] = ch[i];

	}
	
	SioPuts(commport,(LPSTR)(m_Array), 18);

	V_Trawl_code=""; 
	UpdateData(FALSE);
	SetTimer (1,2000,NULL); 

	DisableTrawlItem();
}


void Trawl::OnClickedLocatWrite()
{
	// TODO: 在此添加控件通知处理程序代码
	if (V_Trawl_Length == "")
	{
		ShowBalloonTip(L"请输入",L"数值范围1～500",TTI_ERROR,IDC_LOCAT_LENGTH);
		//MessageBox ("请先输入网位仪在船后方的\n距离,然后点击“注入”","提示");
	}
	else
	{
		int boatlength,boatwidth;
		icom = 0;

		BYTE m_Array[18];
		memset(m_Array,0,sizeof(m_Array));
		m_Array[0] = 0x24;
		m_Array[1] = 0x31;
		boatlength = atol(V_Trawl_Length);
		boatwidth = atol(V_Trawl_Width);

		//横偏移量 m_Array[2],m_Array[3]
		m_Array[2] = (boatlength >> 8); //高位
		m_Array[3] = (boatlength);//低位
		
		// 正负：m_Array[4]
		//纵偏移量 m_Array[5],m_Array[6]
		if (boatwidth >0)
			m_Array[4] = 1;
		else {m_Array[4] = 0;}
		boatwidth = abs(boatwidth);

		m_Array[5] = boatwidth>>8;
		m_Array[6] = boatwidth;
		printf ("length = %d\n",boatlength);
		printf ("width = %d\n",boatwidth);
		printf ("m_Array[2] = %d\n",m_Array[2]);
		printf ("m_Array[3] = %d\n",m_Array[3]);
		printf ("m_Array[4] = %d\n",m_Array[4]);
		printf ("m_Array[5] = %d\n",m_Array[5]);
		printf ("m_Array[6] = %d\n",m_Array[6]);
		//if(boatlength > 1021)
		//{
		//	m_Array[2] = 0x03; 
		//	m_Array[3] = 0xfe;
		//}

		//if(boatwidth > 125)
		//{
		//	m_Array[4] = 0x7e;
		//}

		SioPuts(commport,(LPSTR)(m_Array), 18);
		SetTimer (1,2000,NULL); 

		DisableTrawlItem();
	}
}

void Trawl::OnBnClickedLocatRead()
{
	// TODO: 在此添加控件通知处理程序代码
	icom = 0;
	
	BYTE m_Array[18]; 
	memset(m_Array,0,sizeof(m_Array));
	m_Array[0] = 0x24;
	m_Array[1] = 0x32;
	SioPuts(commport,(LPSTR)(m_Array), 18);
	SetTimer (1,2000,NULL); 
	DisableTrawlItem();
}



void Trawl::OnChangeLocatLength()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_LOCAT_LENGTH);
	pEdit->LimitText(4);
	UpdateData(TRUE);
	if (atol(V_Trawl_Length) > 5000)
	{
		V_Trawl_Length  = "5000";
		UpdateData(FALSE);
		pEdit->SetSel(V_Trawl_Length.GetLength(),V_Trawl_Length.GetLength()); //移动光标
		//ShowBalloonTip(L"只能输入1-5000",L"输入",TTI_ERROR);
		ShowBalloonTip(L"数值范围",L"1～5000",TTI_ERROR,IDC_LOCAT_LENGTH);
	}
	else if (atol(V_Trawl_Length) == 0)
	{
		V_Trawl_Length  = "";
		UpdateData(FALSE);
		pEdit->SetSel(V_Trawl_Length.GetLength(),V_Trawl_Length.GetLength()); //移动光标
		ShowBalloonTip(L"数值范围",L"1～5000",TTI_ERROR,IDC_LOCAT_LENGTH);
	}
}


void Trawl::OnChangeLocatWidth()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_LOCAT_WIDTH);
	if (V_Trawl_Width[0] == '-')
		pEdit->LimitText(5);
	else pEdit->LimitText(4);
	UpdateData(TRUE);
	//printf ("length = %d\n",V_Trawl_Width.GetLength());
	//printf ("find = %d\n",V_Trawl_Width.Find("-"));
	//printf ("WIDTH = %d\n",V_Trawl_Width[V_Trawl_Width.GetLength()-1]);
	if (V_Trawl_Width.GetLength()>0) // 只允许输入数字，第一位允许输入‘-’
	{
		if (V_Trawl_Width[V_Trawl_Width.GetLength()-1]<'0'||V_Trawl_Width[V_Trawl_Width.GetLength()-1]>'9') 
		{
			if (V_Trawl_Width[V_Trawl_Width.GetLength()-1] != '-' || V_Trawl_Width.GetLength()>1 )
			{
				V_Trawl_Width.Delete((int)V_Trawl_Width.GetLength()-1,1);
				UpdateData(FALSE);
				pEdit->SetSel(V_Trawl_Width.GetLength(),V_Trawl_Width.GetLength()); //移动光标
			}
		}
	}

	// 现在最大输入范围为-5000到5000 之间
	if (atol(V_Trawl_Width) > 5000)
	{
		V_Trawl_Width = "5000";
		UpdateData(FALSE);
		pEdit->SetSel(V_Trawl_Width.GetLength(),V_Trawl_Width.GetLength()); //移动光标
		ShowBalloonTip(L"数值范围",L"-5000～5000",TTI_ERROR,IDC_LOCAT_WIDTH);
	}
	else if ( atol(V_Trawl_Width) < -5000)
	{
		V_Trawl_Width = "-5000";
		UpdateData(FALSE);
		pEdit->SetSel(V_Trawl_Width.GetLength(),V_Trawl_Width.GetLength()); //移动光标
		ShowBalloonTip(L"数值范围",L"-5000～5000",TTI_ERROR,IDC_LOCAT_WIDTH);
	}
	
}


void Trawl::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case 1:
		KillTimer(1);
		pMainDlg->GetDlgItem(IDC_OpenCom)->EnableWindow(TRUE);
		GetDlgItem(IDC_ShipNameRead)->EnableWindow(TRUE);
		GetDlgItem(IDC_MMSIRead)->EnableWindow(TRUE);
		GetDlgItem(IDC_LOCAT_WRITE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_NetNum)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_MMSI)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_Code)->EnableWindow(TRUE);
		GetDlgItem(IDC_LOCAT_LENGTH)->EnableWindow(TRUE);
		GetDlgItem(IDC_LOCAT_WIDTH)->EnableWindow(TRUE);
		GetDlgItem(IDC_LOCAT_READ)->EnableWindow(TRUE);

		if(!V_Trawl_ShipName.IsEmpty() && !V_Trawl_NetNum.IsEmpty())
			GetDlgItem(IDC_ShipNameWrite)->EnableWindow(TRUE);
		else 
			GetDlgItem(IDC_ShipNameWrite)->EnableWindow(FALSE);
		
		if((V_Trawl_MMSI.GetLength()) == 9)
			GetDlgItem(IDC_MMSIWrite)->EnableWindow(TRUE);
		else 
			GetDlgItem(IDC_MMSIWrite)->EnableWindow(FALSE);

		if((V_Trawl_code.GetLength()) == 6)
		{
			GetDlgItem(IDC_Encryp)->EnableWindow(TRUE);
			GetDlgItem(IDC_Encode)->EnableWindow(TRUE);
		}
		else 
		{
			GetDlgItem(IDC_Encryp)->EnableWindow(FALSE);
			GetDlgItem(IDC_Encode)->EnableWindow(FALSE);
		}
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}



BOOL Trawl::PreTranslateMessage(MSG *pMsg)
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



HBRUSH Trawl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetDlgCtrlID() ==	IDC_STATIC )
	{
		pDC->SetTextColor(RGB(255,0,0)); //红色
		pDC->SelectObject(&my_font);
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


//void Trawl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//
//	ShowBalloonTip(L"SDF",L"dfd",TTI_ERROR);
//
//	CDialogEx::OnChar(nChar, nRepCnt, nFlags);
//}
