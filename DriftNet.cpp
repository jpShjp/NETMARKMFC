// DriftNet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "afxdialogex.h"
#include "Ais_Net.h"
#include "DriftNet.h"
#include "WSC.H"


extern CWnd * pMainDlg;

extern char icom;
extern int commport;
extern int IsSize;

// DriftNet �Ի���

IMPLEMENT_DYNAMIC(DriftNet, CDialog)

DriftNet::DriftNet(CWnd* pParent /*=NULL*/)
	: CDialog(DriftNet::IDD, pParent)
	
	, V_Drift_shipName(_T(""))
	, V_Drift_NetNum(_T(""))
	, V_Drift_MMSI(_T(""))
	, V_Drift_Code(_T(""))
	, kind(0)
{

	V_Size_Width = _T("");
	V_Size_Length = _T("");
	//kind = 0;
}

DriftNet::~DriftNet()
{
}

void DriftNet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ShipName, V_Drift_shipName);
	DDX_Text(pDX, IDC_EDIT_NetNum, V_Drift_NetNum);
	DDX_Text(pDX, IDC_EDIT_MMSI, V_Drift_MMSI);
	DDX_Text(pDX, IDC_EDIT_Code, V_Drift_Code);

	DDX_Text(pDX, IDC_SIZE_WIDTH, V_Size_Width);
	DDX_Text(pDX, IDC_SIZE_LONG, V_Size_Length);
}


BEGIN_MESSAGE_MAP(DriftNet, CDialog)
	ON_BN_CLICKED(IDC_ShipNameWrite, &DriftNet::OnClickedShipNameWrite)
	ON_BN_CLICKED(IDC_ShipNameRead, &DriftNet::OnClickedShipnameread)
	ON_BN_CLICKED(IDC_MMSIRead, &DriftNet::OnClickedMmsiread)
	ON_BN_CLICKED(IDC_MMSIWrite, &DriftNet::OnClickedMmsiwrite)
	ON_BN_CLICKED(IDC_IntervalWrite, &DriftNet::OnClickedIntervalwrite)
	ON_BN_CLICKED(IDC_Encryp, &DriftNet::OnClickedEncryp)
	ON_BN_CLICKED(IDC_Encode, &DriftNet::OnClickedEncode)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT_ShipName, &DriftNet::OnChangeEditShipname)
	ON_EN_CHANGE(IDC_EDIT_NetNum, &DriftNet::OnChangeEditNetnum)
	ON_EN_CHANGE(IDC_EDIT_MMSI, &DriftNet::OnChangeEditMmsi)
	ON_EN_CHANGE(IDC_EDIT_Code, &DriftNet::OnChangeEditCode)
	ON_BN_CLICKED(IDC_RADIO_30s, &DriftNet::OnClickedRadio30s)
	ON_BN_CLICKED(IDC_RADIO_1min, &DriftNet::OnClickedRadio1min)
	ON_BN_CLICKED(IDC_RADIO_3min, &DriftNet::OnClickedRadio3min)
	ON_BN_CLICKED(IDC_SIZE_NO, &DriftNet::OnClickedSizeNo)
	ON_BN_CLICKED(IDC_SIZE_YES, &DriftNet::OnClickedSizeYes)
	ON_EN_CHANGE(IDC_SIZE_LENGTH, &DriftNet::OnChangeSizeLength)
	ON_EN_CHANGE(IDC_SIZE_WIDTH, &DriftNet::OnChangeSizeWidth)
	ON_BN_CLICKED(IDC_SIZE_WRITE, &DriftNet::OnClickedSizeWrite)
	ON_BN_CLICKED(IDC_KIND1, &DriftNet::OnClickedKind1)
	ON_BN_CLICKED(IDC_KIND2, &DriftNet::OnClickedKind2)
	ON_BN_CLICKED(IDC_KIND_WRITE, &DriftNet::OnClickedKindWrite)
END_MESSAGE_MAP()




// DriftNet ��Ϣ�������


//  С�ؼ�״̬����
void DriftNet::DisablePage1Item(void)
{
	pMainDlg->GetDlgItem(IDC_OpenCom)->EnableWindow(FALSE);
	GetDlgItem(IDC_ShipNameWrite)->EnableWindow(FALSE);
	GetDlgItem(IDC_ShipNameRead)->EnableWindow(FALSE);
	GetDlgItem(IDC_MMSIWrite)->EnableWindow(FALSE);
	GetDlgItem(IDC_MMSIRead)->EnableWindow(FALSE);
	GetDlgItem(IDC_Encryp)->EnableWindow(FALSE);
	GetDlgItem(IDC_Encode)->EnableWindow(FALSE);
	GetDlgItem(IDC_IntervalWrite)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_NetNum)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MMSI)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_Code)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_30s)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_1min)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_3min)->EnableWindow(FALSE);

	GetDlgItem(IDC_SIZE_YES)->EnableWindow(FALSE);
	GetDlgItem(IDC_SIZE_NO)->EnableWindow(FALSE);
	GetDlgItem(IDC_SIZE_LONG)->EnableWindow(FALSE);
	GetDlgItem(IDC_SIZE_WIDTH)->EnableWindow(FALSE);
	GetDlgItem(IDC_SIZE_WRITE)->EnableWindow(FALSE);
	GetDlgItem(IDC_KIND1)->EnableWindow(FALSE);
	GetDlgItem(IDC_KIND2)->EnableWindow(FALSE);
	GetDlgItem(IDC_KIND_WRITE)->EnableWindow(FALSE);
}

// д����
void DriftNet::OnClickedShipNameWrite() 
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
	L1 = V_Drift_shipName.GetLength();
	L2 = V_Drift_NetNum.GetLength();
	m_Array[2] = (L1+L2+1)*6; //m_Array.SetAt(2,(L1+L2+1)*6); //�����ܳ���

	ch1 = ( char *)malloc(20*sizeof(char));
	ch2 = ( char *)malloc(2*sizeof(char));
	ch = ( char *)malloc(40*sizeof(char));
	ch1 = (LPSTR)(LPCTSTR)V_Drift_shipName; //cstring��char��ת��

	for(i=0;i < L1;i++)
	{	
		ch[i] = ch1[i]; 
		ch[i] &= 0x3F;printf("i = %d,ch1[i] = %d \n",i,ch1[i]);
	}

	ch[i] = '-'; ch[i] &= 0x3F;
	ch2 = (char*)(LPCTSTR)V_Drift_NetNum;
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

	DisablePage1Item();
}

// ������
void DriftNet::OnClickedShipnameread()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	icom = 0;
	V_Drift_shipName = "";
	V_Drift_NetNum = "";
	UpdateData(FALSE);

	BYTE m_Array[18]; //����0x04����
	memset(m_Array,0,sizeof(m_Array));
	m_Array[0] = 0x24; 
	m_Array[1] = 0x04;
	SioPuts(commport,(LPSTR)(m_Array), 18);
	SetTimer (1,2000,NULL); 

	DisablePage1Item();
}

// MMSIע��
void DriftNet::OnClickedMmsiwrite()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//UpdateData(TRUE); //��ȡ�༭������
	icom = 0;
	unsigned long rev = 0;

	rev = atol(V_Drift_MMSI);

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

	DisablePage1Item();
}

// MMSI��ȡ
void DriftNet::OnClickedMmsiread()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	icom = 0;
	V_Drift_MMSI="";	
	UpdateData(FALSE);

	BYTE m_Array[18]; //����0x05����
	memset(m_Array,0,sizeof(m_Array));
	m_Array[0] = 0x24; 
	m_Array[1] = 0x05;
	SioPuts(commport,(LPSTR)(m_Array), 18);
	SetTimer (1,2000,NULL); 
	
	DisablePage1Item();
}

// ����
void DriftNet::OnClickedEncryp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	static CString code1,code2;
	static char page1_encryp_time; //��������Ĵ���
	char i;
	icom = 0;

	if (page1_encryp_time == 0) // ��һ�μ���
	{
		code1 = ""+V_Drift_Code+"";
		GetDlgItem(IDC_EDIT_Code)->SetWindowText("");
		MessageBox("��������һ������","��ʾ"); 
		page1_encryp_time = 1; 
	}
	else if (page1_encryp_time == 1 )
	{
		char *ch;
		ch = (char*)malloc(6*sizeof (char));
		code2 = ""+V_Drift_Code+"";
		ch = (LPSTR)(LPCTSTR)V_Drift_Code;
		 
		if (strcmp (code1,code2) == 0)
		{
			BYTE m_Array[18]; //����0x0A����
			memset(m_Array,0,sizeof(m_Array));
			m_Array[0] = 0x24; m_Array[1] = 0x0A;

			for(i=0;i<6;i++)
			{
				m_Array[i+2] = ch[i];
			}
			SioPuts(commport,(LPSTR)(m_Array), 18); 
			ch = NULL;
			free(ch);
			SetTimer (1,2000,NULL); 
			DisablePage1Item();
		}
		else
		{
			MessageBox ("�����������,����������!","��ʾ");
		}
		page1_encryp_time = 0;
		V_Drift_Code=""; 
		UpdateData(FALSE);
	}
}

// ����
void DriftNet::OnClickedEncode()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	char i;
	icom = 0;
	BYTE m_Array[18]; //����0x0B����
	memset(m_Array,0,sizeof(m_Array));
	m_Array[0] = 0x24; m_Array[1] = 0x0B;

	char *ch;
	ch = (LPSTR)(LPCTSTR)V_Drift_Code; //cstring��char��ת��
	for(i=0;i<6;i++)
	{	
		m_Array[i+2] = ch[i];//m_Array.SetAt(i+2,ch[i]);
	}
	SioPuts(commport,(LPSTR)(m_Array), 18);   //g_mainwnd->m_comm1.put_Output(COleVariant(m_Array));

	V_Drift_Code=""; 
	UpdateData(FALSE);
	SetTimer (1,2000,NULL); 
	DisablePage1Item();
}

void DriftNet::OnClickedKind1() //������
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	kind = 0;
}


void DriftNet::OnClickedKind2()  //������
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	kind = 1;
}


void DriftNet::OnClickedKindWrite()  //����ע��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	icom = 0;
	BYTE m_Array[18];
	memset(m_Array,0,sizeof(m_Array));
	m_Array[0] = 0x24; 
	m_Array[1] = 0x2A;
	m_Array[2] = kind;
	SioPuts(commport,(LPSTR)(m_Array), 18);
	SetTimer(1, 2000, NULL); //��ʱ2��
	DisablePage1Item();
}

void DriftNet::OnClickedRadio30s()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	V_Drift_Interval = 1;
	GetDlgItem(IDC_IntervalWrite)->EnableWindow(TRUE);
}


void DriftNet::OnClickedRadio1min()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	V_Drift_Interval = 2;
	GetDlgItem(IDC_IntervalWrite)->EnableWindow(TRUE);
}


void DriftNet::OnClickedRadio3min()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	V_Drift_Interval = 3;
	GetDlgItem(IDC_IntervalWrite)->EnableWindow(TRUE);
}


// ���ע��
void DriftNet::OnClickedIntervalwrite()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	icom = 0;
	BYTE m_Array[18]; //����0x12����
	memset(m_Array,0,sizeof(m_Array));
	m_Array[0] = 0x24; m_Array[1] = 0x12;

	switch(V_Drift_Interval)
	{
		case 1: //m_Array[2] m_Array[3] ����������Ϣ  ||   m_Array[4] m_Array[5]:���ʱ�� ����λms��  ||    m_Array[6] m_Array[7]:��������̶�ֵ  ||    m_Array[8] m_Array[9]:gps����ʧЧʱ�����ֵ (2min)  ||  m_Array[10] m_Array[11]:gps���ʧЧʱ�����ֵ (15min)
			m_Array[2] = 0x01; m_Array[3] = 0x05; 
			m_Array[4] = 0x04; m_Array[5] = 0xb0; 
			m_Array[6] = 0x00; m_Array[7] = 0x05; 
			m_Array[8] = 0x00; m_Array[9] = 0x78; 
			m_Array[10] = 0x00; m_Array[11] = 0x1E; //30s
			SioPuts(commport,(LPSTR)(m_Array), 18);
			SetTimer (1,2000,NULL); 
		break;
		case 2:
			m_Array[2] = 0x02; m_Array[3] = 0x02; 
			m_Array[4] = 0x09; m_Array[5] = 0x60; 
			m_Array[6] = 0x00; m_Array[7] = 0x02; 
			m_Array[8] = 0x00; m_Array[9] = 0x78; 
			m_Array[10] = 0x00; m_Array[11] = 0x0F;  //1min
			SioPuts(commport,(LPSTR)(m_Array), 18);
			SetTimer (1,2000,NULL); 
		break;
		case 3:
			m_Array[2] = 0x03; m_Array[3] = 0x00; 
			m_Array[4] = 0x1c; m_Array[5] = 0x20; 
			m_Array[6] = 0x00; m_Array[7] = 0x00; 
			m_Array[8] = 0x00; m_Array[9] = 0x78; 
			m_Array[10] = 0x00; m_Array[11] = 0x05;  //3min
			SioPuts(commport,(LPSTR)(m_Array), 18);
			SetTimer (1,2000,NULL); 
		break;

		default:
			m_Array[2] = 0x03; m_Array[3] = 0x00; 
			m_Array[4] = 0x1c; m_Array[5] = 0x20; 
			m_Array[6] = 0x00; m_Array[7] = 0x00; 
			m_Array[8] = 0x00; m_Array[9] = 0x78; 
			m_Array[10] = 0x00; m_Array[11] = 0x05;  //3min
			SioPuts(commport,(LPSTR)(m_Array), 18);
			SetTimer (1,2000,NULL); 
		break;


	}
	DisablePage1Item();
}

// �����ߴ�ע��
void DriftNet::OnClickedSizeWrite()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	unsigned long boatlength,boatwidth;

	UpdateData(TRUE); //��ȡ�༭������

	icom = 0;

	BYTE m_Array[18];
	memset(m_Array,0,sizeof(m_Array));
	m_Array[0] = 0x24; 
	m_Array[1] = 0x2B;
	boatlength = atol(V_Size_Length);
	boatwidth = atol(V_Size_Width);  
				
	m_Array[2] = (unsigned char)(boatlength >> 8);
	m_Array[3] = (unsigned char)(boatlength);
	m_Array[4] = (unsigned char)(boatwidth);

	if(boatlength > 1021)
	{
		m_Array[2] = 0x03; 
		m_Array[3] = 0xfe;
	}

	if(boatwidth > 125)
	{
		m_Array[4] = 0x7e;
	}

	SioPuts(commport,(LPSTR)(m_Array), 18);
	SetTimer (1,2000,NULL); 

	DisablePage1Item();

}

void DriftNet::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch (nIDEvent)
	{
	case 1:
		KillTimer(1);

		pMainDlg->GetDlgItem(IDC_OpenCom)->EnableWindow(TRUE);
		GetDlgItem(IDC_ShipNameRead)->EnableWindow(TRUE);
		GetDlgItem(IDC_MMSIRead)->EnableWindow(TRUE);
		GetDlgItem(IDC_IntervalWrite)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_NetNum)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_MMSI)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_Code)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_30s)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_1min)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_3min)->EnableWindow(TRUE);
		GetDlgItem(IDC_IntervalWrite)->EnableWindow(TRUE);
		GetDlgItem(IDC_SIZE_YES)->EnableWindow(TRUE);
		GetDlgItem(IDC_SIZE_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_SIZE_WRITE)->EnableWindow(TRUE);
		GetDlgItem(IDC_KIND1)->EnableWindow(TRUE);
		GetDlgItem(IDC_KIND2)->EnableWindow(TRUE);
		GetDlgItem(IDC_KIND_WRITE)->EnableWindow(TRUE);


		if(!V_Drift_shipName.IsEmpty() && !V_Drift_NetNum.IsEmpty())
			GetDlgItem(IDC_ShipNameWrite)->EnableWindow(TRUE);
		else 
			GetDlgItem(IDC_ShipNameWrite)->EnableWindow(FALSE);
		

		if((V_Drift_MMSI.GetLength()) == 9)
			GetDlgItem(IDC_MMSIWrite)->EnableWindow(TRUE);
		else 
			GetDlgItem(IDC_MMSIWrite)->EnableWindow(FALSE);

		if((V_Drift_Code.GetLength()) == 6)
		{
			GetDlgItem(IDC_Encryp)->EnableWindow(TRUE);
			GetDlgItem(IDC_Encode)->EnableWindow(TRUE);
		}
		else 
		{
			GetDlgItem(IDC_Encryp)->EnableWindow(FALSE);
			GetDlgItem(IDC_Encode)->EnableWindow(FALSE);
		}
		if (IsSize)
		{
			GetDlgItem(IDC_SIZE_WRITE)->EnableWindow(TRUE);
			GetDlgItem(IDC_SIZE_LONG)->EnableWindow(TRUE);
			GetDlgItem(IDC_SIZE_WIDTH)->EnableWindow(TRUE);
		}
		else 
		{
			GetDlgItem(IDC_SIZE_WRITE)->EnableWindow(FALSE);
			GetDlgItem(IDC_SIZE_LONG)->EnableWindow(FALSE);
			GetDlgItem(IDC_SIZE_WIDTH)->EnableWindow(FALSE);
		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

void DriftNet::OnChangeEditShipname()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ShipName);
	pEdit->LimitText(17);
	UpdateData(TRUE);

	if(!V_Drift_shipName.IsEmpty() && !V_Drift_NetNum.IsEmpty()) 
		GetDlgItem(IDC_ShipNameWrite)->EnableWindow(TRUE);
	else 
		GetDlgItem(IDC_ShipNameWrite)->EnableWindow(FALSE);

}

void DriftNet::OnChangeEditNetnum()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_NetNum);
	pEdit->LimitText(2);
	UpdateData(TRUE);

	if(!V_Drift_shipName.IsEmpty() && !V_Drift_NetNum.IsEmpty()) //g_mainwnd->m_comm1.get_PortOpen())
		GetDlgItem(IDC_ShipNameWrite)->EnableWindow(TRUE);
	else 
		GetDlgItem(IDC_ShipNameWrite)->EnableWindow(FALSE);

}

void DriftNet::OnChangeEditMmsi()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_MMSI);
	pEdit->LimitText(9);
	UpdateData(TRUE);
	
	if (V_Drift_MMSI.GetLength() == 9)
	{
		GetDlgItem(IDC_MMSIWrite)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_MMSIWrite)->EnableWindow(FALSE);
	}
}

void DriftNet::OnChangeEditCode()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_Code);
	pEdit->LimitText(6);
	UpdateData(TRUE);
	if (V_Drift_Code.GetLength() == 6)
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

void DriftNet::OnChangeSizeLength()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_SIZE_LONG);
	pEdit->LimitText(4);
	UpdateData(TRUE);
}

void DriftNet::OnChangeSizeWidth()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_SIZE_WIDTH);
	pEdit->LimitText(3);
	UpdateData(TRUE);
}

void DriftNet::OnClickedSizeNo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	IsSize = 0;
	GetDlgItem(IDC_SIZE_WRITE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SIZE_LONG)->EnableWindow(FALSE);
	GetDlgItem(IDC_SIZE_WIDTH)->EnableWindow(FALSE);
}

void DriftNet::OnClickedSizeYes()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	IsSize = 1;
	GetDlgItem(IDC_SIZE_WRITE)->EnableWindow(TRUE);
	GetDlgItem(IDC_SIZE_LONG)->EnableWindow(TRUE);
	GetDlgItem(IDC_SIZE_WIDTH)->EnableWindow(TRUE);
}


BOOL DriftNet::PreTranslateMessage(MSG* pMsg)
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

