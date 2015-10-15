// Trawl.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Ais_Net.h"
#include "Trawl.h"
#include "afxdialogex.h"
#include "WSC.H"

extern CWnd * pMainDlg;
extern char icom;
extern int commport;
extern int IsSize;


void Trawl::DisablePage2Item(void)
{
	pMainDlg->GetDlgItem(IDC_OpenCom)->EnableWindow(FALSE);
	GetDlgItem(IDC_ShipNameWrite)->EnableWindow(FALSE);
	GetDlgItem(IDC_ShipNameRead)->EnableWindow(FALSE);
	GetDlgItem(IDC_MMSIWrite)->EnableWindow(FALSE);
	GetDlgItem(IDC_MMSIRead)->EnableWindow(FALSE);
	GetDlgItem(IDC_Encryp)->EnableWindow(FALSE);
	GetDlgItem(IDC_Encode)->EnableWindow(FALSE);
	GetDlgItem(IDC_LOCAT_WRITE)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_NetNum)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MMSI)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_Code)->EnableWindow(FALSE);

	GetDlgItem(IDC_LOCAT_LENGTH)->EnableWindow(FALSE);
	GetDlgItem(IDC_LOCAT_WIDTH)->EnableWindow(FALSE);
}

// Trawl �Ի���

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
END_MESSAGE_MAP()


// Trawl ��Ϣ�������


void Trawl::OnChangeEditShipname()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	icom = 0;

	V_Trawl_ShipName="";
	V_Trawl_NetNum="";	
	UpdateData(FALSE);

	BYTE m_Array[18]; //����0x04����
	memset(m_Array,0,sizeof(m_Array));
	m_Array[0] = 0x24; m_Array[1] = 0x04;
	SioPuts(commport,(LPSTR)(m_Array), 18);

	SetTimer(1, 2000, NULL); //��ʱ2��
	DisablePage2Item();
}



void Trawl::OnClickedShipnamewrite()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
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

	//UpdateData(TRUE); //��ȡ�༭������

	BYTE m_Array[18]; //ʶ����λ���Ƿ���������
	memset(m_Array,0,sizeof(m_Array));
	m_Array[0] = 0x24; 
	m_Array[1] = 0x01;
	L1 = V_Trawl_ShipName.GetLength();
	L2 = V_Trawl_NetNum.GetLength();
	m_Array[2] = (L1+L2+1)*6; //m_Array.SetAt(2,(L1+L2+1)*6); //�����ܳ���

	ch1 = ( char *)malloc(20*sizeof(char));
	ch2 = ( char *)malloc(2*sizeof(char));
	ch = ( char *)malloc(40*sizeof(char));
	ch1 = (LPSTR)(LPCTSTR)V_Trawl_ShipName; //cstring��char��ת��

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

	DisablePage2Item();

}


void Trawl::OnChangeEditMmsi()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//UpdateData(TRUE); //��ȡ�༭������

	icom = 0;

	unsigned long rev = 0;

	rev = atol(V_Trawl_MMSI);

	BYTE m_Array[18]; //����0x02����
	memset(m_Array,0,sizeof(m_Array));
	m_Array[0] = 0x24; 
	m_Array[1] = 0x02;
	m_Array[2] = (unsigned char)(rev>>24); 
	m_Array[3] = (unsigned char)(rev>>16); 
	m_Array[4] = (unsigned char)(rev>>8); 
	m_Array[5] = (unsigned char)rev;
	SioPuts(commport,(LPSTR)(m_Array), 18);
	SetTimer (1,2000,NULL); 

	DisablePage2Item();
}


void Trawl::OnClickedMmsiread()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	icom = 0;
	V_Trawl_MMSI="";	
	UpdateData(FALSE);


	BYTE m_Array[18]; //����0x05����
	memset(m_Array,0,sizeof(m_Array));
	m_Array[0] = 0x24;
	m_Array[1] = 0x05;
	SioPuts(commport,(LPSTR)(m_Array), 18);
	SetTimer (1,2000,NULL); 
	
	DisablePage2Item();
}


void Trawl::OnChangeEditCode()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	static CString code1,code2;
	static char page2_encryp_time; //��������Ĵ���
	char i;
	icom = 0;

	if (page2_encryp_time == 0) // ��һ�μ���
	{
		code1 = ""+V_Trawl_code+"";
		GetDlgItem(IDC_EDIT_Code)->SetWindowText("");
		MessageBox("��������һ������","��ʾ"); 
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
			BYTE m_Array[18]; //����0x0A����
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
			DisablePage2Item();
		}
		else
		{
			MessageBox ("�����������,����������!","��ʾ");
		}
		page2_encryp_time = 0;
		V_Trawl_code=""; 
		UpdateData(FALSE);
	}
}


void Trawl::OnClickedEncode()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	char i;
	icom = 0;
	BYTE m_Array[18]; //����0x0B����
	memset(m_Array,0,sizeof(m_Array));
	m_Array[0] = 0x24;
	m_Array[1] = 0x0B;

	char *ch;
	ch = (LPSTR)(LPCTSTR)V_Trawl_code; //cstring��char��ת��
	for(i=0;i<6;i++)
	{	m_Array[i+2] = ch[i];

	}
	
	SioPuts(commport,(LPSTR)(m_Array), 18);

	V_Trawl_code=""; 
	UpdateData(FALSE);
	SetTimer (1,2000,NULL); 

	DisablePage2Item();
}


void Trawl::OnClickedLocatWrite()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	unsigned long boatlength,boatwidth;
	icom = 0;

	BYTE m_Array[18];
	memset(m_Array,0,sizeof(m_Array));
	m_Array[0] = 0x24; 
	m_Array[1] = 0x31;
	boatlength = atol(V_Trawl_Length);
	boatwidth = atol(V_Trawl_Width);  
				
	m_Array[2] = (unsigned char)(boatlength >> 8);
	m_Array[3] = (unsigned char)(boatlength);
	m_Array[4] = (unsigned char)(boatwidth);

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

	DisablePage2Item();

}


void Trawl::OnChangeLocatLength()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_LOCAT_LENGTH);
	pEdit->LimitText(3);
	UpdateData(TRUE);
}


void Trawl::OnChangeLocatWidth()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_LOCAT_WIDTH);
	pEdit->LimitText(3);
	UpdateData(TRUE);
}


void Trawl::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
