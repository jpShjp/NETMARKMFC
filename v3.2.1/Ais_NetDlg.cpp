
// Ais_NetDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Ais_Net.h"
#include "Ais_NetDlg.h"
#include "afxdialogex.h"
#include <io.h>
#include <fcntl.h>
#include "WSC.H"
#define NetSel 0 // 0Ϊ���������������� 1Ϊ����

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CAis_NetDlg *pDlg;
CWnd * pMainDlg;
char commName[256]; //�Զ����Ҵ��ڵ�����
char comflag;//���ڴ��ڱ�Ǳ���
char comflagnum;//���ڲ�ѯ��������
int commport; //���ں�
char IsConnect;//��λ���Ƿ�����
char icom; //�������ݱ��� ����
static char com_start; //����֡��ʼ����
BYTE rxdata[20];
char siostate; //����״̬ �Ƿ�򿪵ı���
char Timeflag;  //��ʱ����Ǳ���
char TimeIndex; //�ȴ����������Ƕ�ʱ������
char configShipName = 0;
char IsSize = 0;  //�Ƿ�д�봬���ߴ��Ǳ���
extern char AllWrite;    //ȫ��ע����
extern char AllReadFlag;   //ȫ����ȡ���
extern char AllWrite; //�ж�ȫ��ע���Ƿ�ɹ�
static char ThreadRun; //�߳��Ƿ�����

void InitConsoleWindow() //��ʼ������̨
{
    int nCrt = 0;
    FILE* fp;
    AllocConsole();
    nCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
    fp = _fdopen(nCrt, "w");
    *stdout = *fp;
    setvbuf(stdout, NULL, _IONBF, 0);
}


// ��������
void CAis_NetDlg::PortSet(void)
{
	
	HKEY hKey;

	comflag = 0;
	if( ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, "Hardware\\DeviceMap\\SerialComm", NULL, KEY_READ, &hKey) == ERROR_SUCCESS) // �򿪴���ע���
	{
		int i=0;
		char portName[256];
		CString my_com;
		DWORD dwLong,dwSize;

		while(1)  //WIN7 code
		{
			dwLong = dwSize = sizeof(portName);
			if( ::RegEnumValue( hKey, i, portName, &dwLong, NULL, NULL, (PUCHAR)commName, &dwSize ) == ERROR_NO_MORE_ITEMS ) // ö�ٴ���   
			{   
				if (siostate == 1)
				{
					MessageBox("��λ���豸δ����!","��ʾ");
				}
				siostate = 0;
				break;
			}

			if( strncmp("\\Device\\Silabser",portName,16)==0)   //Device\\ProlificSerial0
			{
				comflag = 1;

				if( commName[4] == 0)   //���ڸ���Ϊ��λ��
				{
					comflagnum = 0;
					i = 0;
				}
				else                    //���ڸ���Ϊ˫��
				{
					comflagnum = 1;
					i = 0;
				}

				if(comflagnum == 0)	  commport = commName[3]-0x30-1;
				else if(comflagnum == 1)   commport = (commName[3]-0x30)*10 + commName[4]-0x30-1;

				SioReset(commport,100,100);	
				SioBaud(commport,WSC_Baud9600);//���ò�����
				SioParms(commport, WSC_NoParity ,WSC_OneStopBit, WSC_WordLength8);//���ò���

				break;
			}
			i++;
		}

		if(comflag == 0)
		{
			GetDlgItem(IDC_Com)->ShowWindow(SW_HIDE);
		}
		else if(comflag == 1)  
		{
			GetDlgItem(IDC_Com)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_Com)->SetWindowText(commName);
		}

		RegCloseKey(hKey);
	}
}


//  Edit �������
void CAis_NetDlg::EditEmpty(void)
{
		page1.V_Drift_shipName.Empty();
		page1.V_Drift_NetNum.Empty();
		page1.V_Drift_MMSI.Empty();
		page1.V_Drift_Code.Empty();
		page1.V_Size_Length.Empty();
		page1.V_Size_Width.Empty();
		page1.UpdateData(FALSE);

		page2.V_Trawl_ShipName.Empty();
		page2.V_Trawl_NetNum.Empty();
		page2.V_Trawl_MMSI.Empty();
		page2.V_Trawl_code.Empty();
		page2.V_Trawl_Length.Empty();
		page2.V_Trawl_Width.Empty();
		page2.UpdateData(FALSE);

		page3.V_Key_ShipName.Empty();
		page3.UpdateData(FALSE);
}

// �������ͼ��
BOOL CAis_NetDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	// ������ƶ���LOGO�ϸ������ͼ��
	CRect rect;
	m_Picture.GetWindowRect(&rect);
	ScreenToClient(&rect);

	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);

	if (rect.PtInRect(point) && m_hCursor)
	{
		SetCursor(m_hCursor);
		return TRUE;
	};
	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}


void CAis_NetDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//  ����ͼƬ����ҳ
	CRect rect;
	m_Picture.GetWindowRect(&rect);
	ScreenToClient(&rect);
	
	if (rect.PtInRect(point))
		ShellExecute(AfxGetMainWnd()->m_hWnd,_T("open"),
			_T("http://www.sandemarine.com/"),_T(""),NULL,0);
	CDialogEx::OnLButtonDown(nFlags, point);
}

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAis_NetDlg �Ի���




CAis_NetDlg::CAis_NetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAis_NetDlg::IDD, pParent)
	//, com_Value(_T(""))
	//, IsSize(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	m_hCursor = AfxGetApp()->LoadCursor(IDC_CURSOR1); //�������ͼ��
}

void CAis_NetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, static1);
	DDX_Control(pDX, IDC_OpenCom, OpenCom);
	DDX_Control(pDX, IDC_TAB1, m_tabCtrl);
	DDX_Control(pDX, IDC_STATIC3, m_Picture);
}

BEGIN_MESSAGE_MAP(CAis_NetDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OpenCom, &CAis_NetDlg::OnBnClickedOpenCom)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CAis_NetDlg::OnSelchangeTab1)
ON_WM_SETCURSOR()
ON_WM_LBUTTONDOWN()
ON_MESSAGE(MY_MESSAGEBOX, &CAis_NetDlg::OnMyMessagebox)
END_MESSAGE_MAP()


// CAis_NetDlg ��Ϣ�������

BOOL CAis_NetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//InitConsoleWindow();
	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	//��� Tab Control

	//���ѡ�����
	if (NetSel == 0)
	{
		m_tabCtrl.InsertItem(0,_T("������/��������λ����Ϣ"));
	   m_tabCtrl.InsertItem(2,_T("Կ����Ϣ"));
	}
	else if (NetSel == 1)
		m_tabCtrl.InsertItem(1,_T("������λ����Ϣ"));
	
    
	//����IDC_TAB1Ϊ������
	page1.Create(IDD_DriftNet,GetDlgItem(IDC_TAB1));
	page2.Create(IDD_TRAWL,GetDlgItem(IDC_TAB1));
	page3.Create(IDD_KEY,GetDlgItem(IDC_TAB1));
    
	//���IDC_TABTEST�ͻ�����С
	CRect rc;
	m_tabCtrl.GetClientRect(&rc);
    
	//�����ӶԻ����ڸ������е�λ��
	rc.top += 25;
	rc.bottom -= 1;
	rc.left += 1;
	rc.right -= 2;

	//�����ӶԻ���ߴ粢�ƶ���ָ��λ��
	page1.MoveWindow(&rc);
	page2.MoveWindow(&rc);
	page3.MoveWindow(&rc);

	//�ֱ��������غ���ʾ
	if (NetSel == 0)
	{
		page1.ShowWindow(true);
		page3.ShowWindow(false);
	}
	else if (NetSel == 1)
	{
		page2.ShowWindow(true);
	}
	

	//����Ĭ�ϵ�ѡ�
	m_tabCtrl.SetCurSel(0);

	SetTimer(1,20,NULL); // ÿ20ms ��ȡһ�δ�����Ϣ

	// ������ʼ��
	IsConnect = 0;
	siostate = 0;
	Timeflag = 0;
	TimeIndex = 0;

	// �ؼ�״̬��ʼ��
	ItemInit();
	((CButton*)(page1.GetDlgItem(IDC_RADIO_1min)))->SetCheck(TRUE); // Ĭ��Ϊ1���ӷ��ͼ��
	((CButton*)(page1.GetDlgItem(IDC_KIND1)))->SetCheck(TRUE);		//Ĭ��ѡ��������
	((CButton*)(page1.GetDlgItem(IDC_SIZE_NO)))->SetCheck(TRUE);		//Ĭ�ϲ�����ߴ�
	page1.V_Drift_Interval = 2;  
	page1.kind = 0;
	IsSize = 0;


 	static1.SetWindowText("��λ��δ����");
	My_StaticFont.CreatePointFont(150, "����");
	page2.my_font.CreatePointFont(110,"����");
	static1.SetFont(&My_StaticFont);   
	pDlg = this;
	pMainDlg = this;
	PortSet();

	// ��ȡ���ڷ��ص�����
	hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadGetCom,NULL,0,&ThreadID);
	//SuspendThread(hThread);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}


//
//��ȡ���ڷ��ص���Ϣ
//

void CAis_NetDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAis_NetDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAis_NetDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CAis_NetDlg::OnBnClickedOpenCom()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	icom = 0;
	if(siostate == 0)  //�򿪴���
	{
		TimeIndex = 0;
		IsConnect = 2;
		siostate = 1;		
		PortSet();
		if (comflag == 1)
		{
			BYTE m_Array[18]; 
			memset(m_Array,0,sizeof(m_Array));
			m_Array[0] = 0x24; 
			m_Array[1] = 0x17;
			SioPuts(commport,(LPSTR)(m_Array), 18);
			Invalidate (false);
			SetTimer(2,15000,NULL); //����ʧ�ܴ���
			SetTimer(3,1000,NULL);  //�ȴ�����

			OpenCom.SetWindowText(_T("�رմ���"));
			OpenCom.EnableWindow(FALSE);
			static1.SetWindowText(_T("��λ��������."));
			ThreadRun = 1;
		}
	}
	else  // �رմ���
	{
		KillTimer (2);
		KillTimer (3);

		BYTE m_Array[18]; 
		memset(m_Array,0,sizeof(m_Array));
		m_Array[0] = 0x24; 
		m_Array[1] = 0x18;
		SioPuts(commport,(LPSTR)(m_Array), 18);
		siostate = 0;
		IsConnect = 0;
		OpenCom.SetWindowText(_T("�򿪴���"));
		static1.SetWindowText(_T("��λ��δ����         "));

		EditEmpty();
		ItemInit();
		ThreadRun = 0;
	}
	
}

#define ABS(cond) (cond>0?cond:-cond)
char   *my_itoa(int   value,   char   *string,   int   radix)
{
   // assert(string!=NULL);
    char tmp[32]={'\0'};
    int tmpval=ABS(value);
    int i,j;
    for(i=0;i<32;i++)
    {				 
		tmp[i]=(tmpval%radix)+'0';
		tmpval=tmpval/radix;
		if(tmpval==0)
			break;
	}
    if(value<0)tmp[++i]='-';
    for(j=0;i>=0;i--)
		string[j++]=tmp[i];
    string[j]='\0';
    return string;
}

void CAis_NetDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch (nIDEvent)
	{
	case 1:
		//getcomm();
		break;
	case 2: // ��������ʧ��
		KillTimer(2);
		KillTimer(3);
		TimeIndex = 0;

		OpenCom.EnableWindow(TRUE);
		IsConnect = 0;
		static1.SetWindowText(_T("��λ��δ����      "));
		break;
	case 3: //�ȴ���������
			TimeIndex++;
			if (TimeIndex == 3)
			{
				OpenCom.EnableWindow(TRUE);
			}
			TimeIndex %= 6;
			switch (TimeIndex)
			{
			case 0:
				static1.SetWindowText("��λ��������.");
			break;

			case 1:
				static1.SetWindowText("��λ��������..");
			break;

			case 2:
				static1.SetWindowText("��λ��������...");
			break;

			case 3:
				static1.SetWindowText("��λ��������....");
			break;
			case 4:
				static1.SetWindowText("��λ��������.....");
			break;
			case 5:
				static1.SetWindowText("��λ��������     ");
			break;
			}
			
		break;

	}
	CDialogEx::OnTimer(nIDEvent);
	
}


void CAis_NetDlg::ItemInit(void) // �ؼ�״̬��ʼ��
{
	page1.GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_EDIT_NetNum)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_ShipNameWrite)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_ShipNameRead)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_EDIT_MMSI)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_MMSIWrite)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_MMSIRead)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_EDIT_Code)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_Encryp)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_Encode)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_RADIO_30s)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_RADIO_1min)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_RADIO_3min)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_IntervalWrite)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_SIZE_NO)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_SIZE_YES)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_SIZE_WRITE)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_SIZE_LONG)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_SIZE_WIDTH)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_SIZE_READ)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_KIND1)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_KIND2)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_KIND_WRITE)->EnableWindow(FALSE);

	//ȫ��ע���ȡ
	page1.GetDlgItem(IDC_ALLWRITE)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_ALLREAD)->EnableWindow(FALSE);

	page2.GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(FALSE);
	page2.GetDlgItem(IDC_EDIT_NetNum)->EnableWindow(FALSE);
	page2.GetDlgItem(IDC_ShipNameWrite)->EnableWindow(FALSE);
	page2.GetDlgItem(IDC_ShipNameRead)->EnableWindow(FALSE);
	page2.GetDlgItem(IDC_EDIT_MMSI)->EnableWindow(FALSE);
	page2.GetDlgItem(IDC_MMSIWrite)->EnableWindow(FALSE);
	page2.GetDlgItem(IDC_MMSIRead)->EnableWindow(FALSE);
	page2.GetDlgItem(IDC_EDIT_Code)->EnableWindow(FALSE);
	page2.GetDlgItem(IDC_Encryp)->EnableWindow(FALSE);
	page2.GetDlgItem(IDC_Encode)->EnableWindow(FALSE);
	page2.GetDlgItem(IDC_LOCAT_WRITE)->EnableWindow(FALSE);
	page2.GetDlgItem(IDC_LOCAT_WIDTH)->EnableWindow(FALSE);
	page2.GetDlgItem(IDC_LOCAT_LENGTH)->EnableWindow(FALSE);
	page2.GetDlgItem(IDC_LOCAT_READ)->EnableWindow(FALSE);

	page3.GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(FALSE);
	page3.GetDlgItem(IDC_ShipNameWrite)->EnableWindow(FALSE);
	page3.GetDlgItem(IDC_ShipNameRead)->EnableWindow(FALSE);


}

void CAis_NetDlg::EnableItem(void)  //���ÿؼ�
{
		GetDlgItem(IDC_OpenCom)->EnableWindow(TRUE);
		// ��������������
		if (NetSel == 0){
			page1.GetDlgItem(IDC_ShipNameRead)->EnableWindow(TRUE);
			page1.GetDlgItem(IDC_MMSIRead)->EnableWindow(TRUE);
			page1.GetDlgItem(IDC_IntervalWrite)->EnableWindow(TRUE);
			page1.GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(TRUE);
			page1.GetDlgItem(IDC_EDIT_NetNum)->EnableWindow(TRUE);
			page1.GetDlgItem(IDC_EDIT_MMSI)->EnableWindow(TRUE);
			page1.GetDlgItem(IDC_EDIT_Code)->EnableWindow(TRUE);
			page1.GetDlgItem(IDC_RADIO_30s)->EnableWindow(TRUE);
			page1.GetDlgItem(IDC_RADIO_1min)->EnableWindow(TRUE);
			page1.GetDlgItem(IDC_RADIO_3min)->EnableWindow(TRUE);
			page1.GetDlgItem(IDC_SIZE_YES)->EnableWindow(TRUE);
			page1.GetDlgItem(IDC_SIZE_NO)->EnableWindow(TRUE);
			page1.GetDlgItem(IDC_KIND1)->EnableWindow(TRUE);
			page1.GetDlgItem(IDC_KIND2)->EnableWindow(TRUE);
			page1.GetDlgItem(IDC_KIND_WRITE)->EnableWindow(TRUE);

			page1.GetDlgItem(IDC_ALLREAD)->EnableWindow(TRUE);
			if(!page1.V_Drift_shipName.IsEmpty() && !page1.V_Drift_NetNum.IsEmpty())  
			{
				page1.GetDlgItem(IDC_ShipNameWrite)->EnableWindow(TRUE);
				if((page1.V_Drift_MMSI.GetLength()) == 9)
						page1.GetDlgItem(IDC_ALLWRITE)->EnableWindow(TRUE);
			}

			if((page1.V_Drift_MMSI.GetLength()) == 9)
				page1.GetDlgItem(IDC_MMSIWrite)->EnableWindow(TRUE);


			if((page1.V_Drift_Code.GetLength()) == 6)
			{
				page1.GetDlgItem(IDC_Encryp)->EnableWindow(TRUE);
				page1.GetDlgItem(IDC_Encode)->EnableWindow(TRUE);
			}

			if (IsSize)
			{
				page1.GetDlgItem(IDC_SIZE_WRITE)->EnableWindow(TRUE);
				page1.GetDlgItem(IDC_SIZE_LONG)->EnableWindow(TRUE);
				page1.GetDlgItem(IDC_SIZE_WIDTH)->EnableWindow(TRUE);
				page1.GetDlgItem(IDC_SIZE_READ)->EnableWindow(TRUE);
			}
		}
		else if (NetSel == 1)
		{
		// ����
			page2.GetDlgItem(IDC_ShipNameRead)->EnableWindow(TRUE);
			page2.GetDlgItem(IDC_MMSIRead)->EnableWindow(TRUE);
			page2.GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(TRUE);
			page2.GetDlgItem(IDC_EDIT_NetNum)->EnableWindow(TRUE);
			page2.GetDlgItem(IDC_EDIT_MMSI)->EnableWindow(TRUE);
			page2.GetDlgItem(IDC_EDIT_Code)->EnableWindow(TRUE);
			page2.GetDlgItem(IDC_LOCAT_WRITE)->EnableWindow(TRUE);	
			page2.GetDlgItem(IDC_LOCAT_WIDTH)->EnableWindow(TRUE);
			page2.GetDlgItem(IDC_LOCAT_LENGTH)->EnableWindow(TRUE);		
			page2.GetDlgItem(IDC_LOCAT_READ)->EnableWindow(TRUE);


			if(!page2.V_Trawl_ShipName.IsEmpty()&&!page2.V_Trawl_NetNum.IsEmpty())
			{
				page2.GetDlgItem(IDC_ShipNameWrite)->EnableWindow(TRUE);
			}


			if (!page2.V_Trawl_MMSI.IsEmpty())
			{
				page2.GetDlgItem(IDC_MMSIWrite)->EnableWindow(TRUE);
			}


			if (!page2.V_Trawl_code.IsEmpty())
			{
				page2.GetDlgItem(IDC_Encryp)->EnableWindow(TRUE);
				page2.GetDlgItem(IDC_Encode)->EnableWindow(TRUE);
			}
		}
		//Կ��
		page3.GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(TRUE);
		page3.GetDlgItem(IDC_ShipNameRead)->EnableWindow(TRUE);
}


HBRUSH CAis_NetDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	if(pWnd->GetDlgCtrlID() == IDC_STATIC1)
	{
		if(IsConnect == 0)	
			pDC->SetTextColor(RGB(255,0,0)); //��ɫ
		else if(IsConnect == 1)	
			pDC->SetTextColor(RGB(0,255,0)); //��ɫ
		else if(IsConnect == 2)	
			pDC->SetTextColor(RGB(255,100,0)); //��ɫ

		pDC->SelectObject(&My_StaticFont);  // ��������
		pDC->SetBkColor(RGB(237,233,224));  //���ñ���ɫ
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC2)
		pDC->SelectObject(&(page2.my_font));  // ��������

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}


void CAis_NetDlg::OnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int CurSel = m_tabCtrl.GetCurSel();
	//OperatePage = CurSel;
    switch(CurSel)
    {
    case 0:
		if (NetSel == 0)
		{
			page1.ShowWindow(true);
		}
		else if (NetSel == 1)
			page2.ShowWindow(true);
		page3.ShowWindow(false);
        break;
    case 1:
		if (NetSel == 0)
			page3.ShowWindow(true);
        page1.ShowWindow(false);
        page2.ShowWindow(false);
		
        break;
	//case 2:
	//	page1.ShowWindow(false);
 //       page2.ShowWindow(false);
	//	page3.ShowWindow(true);
	//	break;
    default:
        ;
    }  
	*pResult = 0;
}

void ThreadGetCom()
{
	HWND hMain = pMainDlg->m_hWnd;
	HWND ComIsConnet = GetDlgItem(hMain,IDC_STATIC1);
	int temp;
	ThreadRun = 0;
	while(1)
	{
	if(ThreadRun)
	{
		if( (temp = SioGetc(commport)) >= 0)
		{
			if (temp == 0x24 && icom == 0)
			{
				com_start = 1;
			}
			if (com_start == 1)
			{
				rxdata[icom] = temp;
				printf ("rxd[%d] = %x\n",icom,rxdata[icom]);
				if (icom == 17)
				{
					com_start = 0;
					switch (rxdata[1])
					{
					  case OpenSerial:
						   if (rxdata[2] == 1)
						   {
							   printf ("OpenCom");
							   IsConnect = 1;
							   pDlg->OpenCom.EnableWindow(TRUE);
							   pDlg->static1.SetWindowText("��λ��������      ");
							   pDlg->KillTimer(3);
							   pDlg->KillTimer(2);
							   TimeIndex = 0;
							   pDlg->EnableItem();
						   }
						   else
						   {
						   }
						   break;

	 				  case CloseSerial: //�رմ���
						   break;
						   
					  case WriteShipName: //����ע��
						  {
							  pDlg->page1.KillTimer(1);
							  pDlg->page2.KillTimer(1);
							  if(AllWrite == 0) //����ע��
							  {
								 if(rxdata[2] == 0x01)
								 {
									 PostMessage(hMain,MY_MESSAGEBOX,1,NULL);
									 //pDlg->MessageBox(_T("����ע��ɹ�!"),_T("��ʾ"));
									 //AfxMessageBox("vv",MB_OK,0);
								 }
								 else if (rxdata[2] == 0x00)
									 PostMessage(hMain,MY_MESSAGEBOX,2,NULL);
								 else if(rxdata[2] == 0x02)
								 {
									 PostMessage(hMain,MY_MESSAGEBOX,3,NULL);
								 }
								 pDlg->EnableItem();
								 icom = 0;
							  }
							  else  //ȫ��ע��
							  {
									if(rxdata[2] == 0x01)
									{
										//AllWrite++;  
										printf ("����ע��ɹ�\n");
										pDlg->page1.MMSIWrite();
										icom = 17;
									}
									else if(rxdata[2] == 0x02)
									{
										PostMessage(hMain,MY_MESSAGEBOX,3,NULL);
										AllWrite = 0;
										pDlg->EnableItem();
									}
							  }
						  }

						  break;

					 case ReadShipName: 
					{
						printf ("��ȡ����\n");
						unsigned long b[5];
						unsigned char c,d,i,j,k=0;
						unsigned char rxdatatemp;
						CString strtemp1;CString strtemp2;
						unsigned char e = 0;
						char shipname[20]={0};
						char shipindex = 0;
						char netNumFlag;

						rxdatatemp = rxdata[2]/6;
						for(i=0;i<5;i++)
						{  
							b[i] = (rxdata[3+i*3]<<16); b[i] += (rxdata[4+i*3]<<8); b[i] += rxdata[5+i*3];
						}
						
						for(i=0;i<5;i++)//for(i=0;i<rxdata[1]/6;i++)
						{
							for(j=0;j<4;j++)
							{
								k++;
								c = (unsigned char )((b[i]>>(18 - j*6)) & 0x3F); //ת����6λASCII��
								if(c <= 31)   c += 0x40; //ת����8λASCII��

								shipname[k-1] = c;
								printf ("ccc = %d\n",c);
							}
						}

						// �ҵ����һ����-����λ��
						for (shipindex = 0; shipindex<=rxdatatemp; shipindex++ )
						{
							if (shipname[shipindex ]== '-')
								netNumFlag = shipindex;
						}
						printf ("netflg = %d\n",netNumFlag);

						for (i = 0; i<netNumFlag; i++) //����
						{
							if (NetSel == 0)
								pDlg->page1.V_Drift_shipName.Insert(i,shipname[i]);
							else 
								pDlg->page2.V_Trawl_ShipName.Insert(i,shipname[i]);
							printf ("shipn = %d\n",shipname[i]);
						}
						//��������
						if (NetSel == 0)
							pDlg->page1.UpdateData(FALSE);
						else 
							pDlg->page2.UpdateData(FALSE);			
						
						for(i++;i<rxdatatemp;i++)  //����
						{
							if (NetSel == 0)
								pDlg->page1.V_Drift_NetNum.Insert(i,shipname[i]);
							else 
								pDlg->page2.V_Trawl_NetNum.Insert(i,shipname[i]);
							printf ("net = %d\n",shipname[i]);
						}						
						//��������
						if (NetSel == 0)
						{
							pDlg->page1.UpdateData(FALSE);
							pDlg->page1.KillTimer(1);
						}
						else if(NetSel == 1)
						{
							printf("NS1");
							pDlg->page2.UpdateData(FALSE);
							pDlg->page2.KillTimer(1);
						}
						

						if (AllReadFlag == 0) //����ȫ����ȡ
						{
							pDlg->EnableItem();
							icom = 0;
						}
						else if (AllReadFlag == 1)
						{
							pDlg->page1.KillTimer(2);
							pDlg->page1.MMSIRead();
							icom = 17;
						}
				}
				break;

				case WriteMMSI:  //MMSIע��
						{	
							pDlg->page1.KillTimer(1);
							pDlg->page2.KillTimer(1);
							printf ("MMSIע��\n");
							if(AllWrite == 0)	 //����ע��
							{
								if(rxdata[2] == 0x01)  
									PostMessage(hMain,MY_MESSAGEBOX,4,NULL);
								else if(rxdata[2] == 0x00) 
									PostMessage(hMain,MY_MESSAGEBOX,5,NULL);
								else if(rxdata[2] == 0x02)  
									PostMessage(hMain,MY_MESSAGEBOX,3,NULL);

								pDlg->EnableItem();
							}
							else //ȫ��ע��
							{
								if(rxdata[2] == 0x01)
								{
									//AllWrite++;
									pDlg->page1.IntervalWrite();
									icom = 17;
									printf ("MMSIע��ɹ�");
								}
							}

						}
					break;

					case ReadMMSI: //MMSII��ȡ
					{
						printf ("MMSI��ȡ\n");
						char *aa = (char *)malloc(20*sizeof(char));
						int Length;
						int index;
						unsigned long rev;
						rev = rxdata[2] << 24;  rev +=(rxdata[3] << 16); rev +=(rxdata[4] << 8); rev +=rxdata[5];
						my_itoa(rev,aa,10);
						if (NetSel == 0)
						{
							pDlg->page1.V_Drift_MMSI = aa;
							Length = 9-strlen(pDlg->page1.V_Drift_MMSI);
							for(index = 0;index < Length; index++)
							{
								pDlg->page1.V_Drift_MMSI.Insert(0,"0");
							}
							pDlg->page1.UpdateData(FALSE);
							pDlg->page1.KillTimer(1);
						}
						else if(NetSel == 1)
						{
							pDlg->page2.V_Trawl_MMSI = aa;
							Length = 9-strlen(pDlg->page2.V_Trawl_MMSI);
							for(index = 0;index < Length; index++)
							{
								pDlg->page2.V_Trawl_MMSI.Insert(0,"0");
							}
							pDlg->page2.UpdateData(FALSE);
							pDlg->page2.KillTimer(1);
						}

						if (AllReadFlag == 0 || IsSize == 0) //����ȫ����ȡ
						{
							pDlg->EnableItem();
							icom = 0;
							AllReadFlag = 0;
						}
						else if (AllReadFlag == 1 || IsSize == 1)//ȫ����ȡ
						{
							pDlg->page1.sizeRead();
							icom = 17;
						}
					}
					break;
					case Encryp:  //����
						{
							printf ("����\n");
							if(rxdata[2] == 1)   PostMessage(hMain,MY_MESSAGEBOX,6,NULL); 
							else if(rxdata[2] == 2)   PostMessage(hMain,MY_MESSAGEBOX,7,NULL);
							else if(rxdata[2] == 0)   PostMessage(hMain,MY_MESSAGEBOX,8,NULL); 

							if (NetSel == 0)
							{
								pDlg->page1.KillTimer(1);
							}
							else if (NetSel == 1)
							{
								pDlg->page2.KillTimer(1);
							}
							pDlg->EnableItem();
							icom = 0;
						}
						 break;

				case Encode:  //����
						{
								printf ("����\n");
							if(rxdata[2] == 1)   PostMessage(hMain,MY_MESSAGEBOX,9,NULL); 
							else if(rxdata[2] == 2)   PostMessage(hMain,MY_MESSAGEBOX,10,NULL);
							else if(rxdata[2] == 0)   PostMessage(hMain,MY_MESSAGEBOX,11,NULL); 
							else if(rxdata[2] == 3)   PostMessage(hMain,MY_MESSAGEBOX,12,NULL); 

							if (NetSel == 0)
							{
								pDlg->page1.KillTimer(1);
							}
							else if (NetSel == 1)
							{
								pDlg->page2.KillTimer(1);
							}
							pDlg->EnableItem();

							icom = 0;
						}
						break;

				case KindWrite: //����ע��
					{   
						pDlg->page1.KillTimer(1);
						printf ("����ע��\n");
							if(AllWrite == 0) //����ע��
							{


								if(rxdata[2] == 0x01)  //ע��ɹ�
								{
									if (pDlg->page1.kind == 0)
										PostMessage(hMain,MY_MESSAGEBOX,13,NULL);
									else if(pDlg->page1.kind == 1) 
										PostMessage(hMain,MY_MESSAGEBOX,14,NULL);
									pDlg->EnableItem();
								}
								else if(rxdata[2] == 0x00)   // ע��ʧ��
									PostMessage(hMain,MY_MESSAGEBOX,15,NULL);
							}
							else // ȫ��ע��
							{					
								if (IsSize) // �ж��Ƿ�ע�봬���ߴ�
								{
									if(rxdata[2] == 0x01)
									{
										pDlg->page1.sizeWrite();
										icom = 17;
										printf ("����ע��ɹ�");
									}
								}
								else //��ע�봬���ߴ�
								{
									if(rxdata[2] == 0x01)
									{
										pDlg->page1.KillTimer(2);
										AllWrite = 0;
										PostMessage(hMain,MY_MESSAGEBOX,16,NULL);
										pDlg->EnableItem();
									}
								}
								
							}
						
					}
					break;

				case Interval:  // ������
					{
						pDlg->page1.KillTimer(1);
						printf ("���ͼ��ע��\n");
						if(AllWrite == 0)	// ����ע��
						{
							printf ("jg1");
							if(rxdata[2] == 1) 
							{
								if(pDlg->page1.V_Drift_Interval == 1)
									PostMessage(hMain,MY_MESSAGEBOX,17,NULL);
								else if(pDlg->page1.V_Drift_Interval == 2)
									PostMessage(hMain,MY_MESSAGEBOX,18,NULL);
								else if(pDlg->page1.V_Drift_Interval == 3)
									PostMessage(hMain,MY_MESSAGEBOX,19,NULL);
							}
							else if(rxdata[2] == 0)  
								PostMessage(hMain,MY_MESSAGEBOX,15,NULL);

							pDlg->EnableItem();
							icom = 0;
						}
						else // ȫ��ע��
						{
							printf ("jg2");
							if(rxdata[2] == 1)
							{
								pDlg->page1.kindWrite();
								icom = 17;
								printf ("���ע��ɹ�");
							}
						}	

						pDlg->page1.UpdateData(FALSE);	
					}
					break;
					
				case SizeWrite:  //�����ߴ�ע��
					{
						pDlg->page1.KillTimer(1);
						printf ("�����ߴ�ע��\n");
						if (AllWrite == 0) // ����ע��
						{
							if(rxdata[2] == 0x01)
								PostMessage(hMain,MY_MESSAGEBOX,20,NULL);
							else if(rxdata[2] == 0x00) 
								PostMessage(hMain,MY_MESSAGEBOX,21,NULL);
						}
						else // ȫ��ע��
						{
							if(rxdata[2] == 0x01)
							{
								pDlg->page1.KillTimer(2);
								AllWrite = 0;
								PostMessage(hMain,MY_MESSAGEBOX,16,NULL);
								printf ("�����ߴ�ע��ɹ�");
							}
						}
						pDlg->page1.UpdateData(FALSE);
					
						pDlg->EnableItem();
						icom = 0;
					}	
					break;
				
				case SizeRead: //�ߴ��ȡ
					{
						 char *aa = (char *)malloc(20*sizeof(char));
						 int boatlong;
						 char boatwidth;
				 printf("�ߴ��ȡ");
						 pDlg->page1.KillTimer(1);
						 if (rxdata[2] == 0x01)
						 {
							boatlong = (rxdata[3]<<8) + rxdata[4];
							boatwidth = rxdata[5];
							my_itoa(boatlong,aa,10);
							pDlg->page1.V_Size_Length = aa;
							pDlg->page1.UpdateData(FALSE);

							my_itoa(boatwidth,aa,10);
							pDlg->page1.V_Size_Width = aa;

							pDlg->page1.UpdateData(FALSE);
							printf ("long = %d\n",boatlong);
							printf ("width = %d\n",boatwidth);
						 }
						 pDlg->EnableItem();
						 icom = 0;
						 AllReadFlag = 0;
					}
					break;

				case KeyWrite:  //Կ��ע��
					{
						printf ("Կ��ע��\n");
						if(rxdata[2] == 0x01 )   
						{		
							PostMessage(hMain,MY_MESSAGEBOX,1,NULL);
						}
						else if(rxdata[2] == 0x00)   PostMessage(hMain,MY_MESSAGEBOX,2,NULL); 
						else if(rxdata[2] == 0x02)	 PostMessage(hMain,MY_MESSAGEBOX,3,NULL); 

						pDlg->GetDlgItem(IDC_OpenCom)->EnableWindow(TRUE);
						pDlg->page3.GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(TRUE);
						pDlg->page3.GetDlgItem(IDC_ShipNameWrite)->EnableWindow(TRUE);
						pDlg->page3.GetDlgItem(IDC_ShipNameRead)->EnableWindow(TRUE);
						icom = 0;
					}
					break;

				case 0x06:  //��ȡԿ��
					{
						printf ("��ȡԿ��\n");
						unsigned long b[6];
						char c,i,j,k=0;
						unsigned char rxdatatemp;
						CString strtemp1;CString strtemp2;

						pDlg->page3.V_Key_ShipName="";
						strtemp1.Empty();strtemp2.Empty();
						rxdatatemp = rxdata[2]/6;// printf("rxdatatemp = %d \n",rxdatatemp);
						for(i=0;i<=5;i++)
						{   b[i] = (rxdata[3+i*3]<<16); b[i] += (rxdata[4+i*3]<<8); b[i] += rxdata[5+i*3]; }
						//b[i] = rxdata[2+i*3]<<16 + rxdata[3+i*3]<<8 + rxdata[4+i*3];
						for(i=0;i<5;i++)//for(i=0;i<rxdata[1]/6;i++)
						{
							for(j=0;j<4;j++)
							{
								k++;
								if(k <= rxdatatemp-1) //�����������Ų���
								{
									//printf("%d ",k);
									c = (char )((b[i]>>(18 - j*6)) & 0x3F); //ת����6λASCII��
									if(c <= 31)   c += 0x40; //ת����8λASCII��//////////////////
									//printf(" c= %d \n",c);
									strtemp1.Format(_T("%c"),c);//m_para1.m_nPara3.Format("%c",c);
									pDlg->page3.V_Key_ShipName += strtemp1;
									//printf(" = %s \n",strtemp);

								}
							}
						}
						pDlg->page3.UpdateData(FALSE);
						pDlg->page3.KillTimer(1);
						pDlg->GetDlgItem(IDC_OpenCom)->EnableWindow(TRUE);
						pDlg->page3.GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(TRUE);
						if (!(pDlg->page3.V_Key_ShipName.IsEmpty()))
							pDlg->page3.GetDlgItem(IDC_ShipNameWrite)->EnableWindow(TRUE);
						pDlg->page3.GetDlgItem(IDC_ShipNameRead)->EnableWindow(TRUE);

						icom = 0;
					}
					break;

				case OffSet_Write: //����λ��ע��
					{
						printf ("����λ��ע��\n");
						if(rxdata[2] == 0x01)        PostMessage(hMain,MY_MESSAGEBOX,22,NULL); 
						else if(rxdata[2] == 0x00)   PostMessage(hMain,MY_MESSAGEBOX,23,NULL); 
						pDlg->page2.KillTimer(1);
						pDlg->EnableItem();
						icom = 0;
					}
					break;
				
				case OffSet_Read: //������λ�ö�ȡ
					{
						int width,longth;
						char *aa = (char *)malloc(20*sizeof(char));
						longth = (rxdata[2]<<8) + rxdata[3];
						my_itoa(longth,aa,10);
						pDlg->page2.V_Trawl_Length = aa;
						pDlg->page2.UpdateData(FALSE);
						if (rxdata[4] == 1)
							width = (rxdata[5]<<8)+rxdata[6];
						else width = ((rxdata[5]<<8)+rxdata[6])*-1;

						my_itoa(width,aa,10);
						pDlg->page2.V_Trawl_Width = aa;
						pDlg->page2.UpdateData(FALSE);
			
						printf("����λ�ö�ȡ\n");
						pDlg->page2.KillTimer(1);
						pDlg->EnableItem();
						icom = 0;
					}
					break;
				}
			}

			if (icom < 17)
			{
				icom++;
			}
			else icom = 0;
			
		}
	}
	}
  }
}

afx_msg LRESULT CAis_NetDlg::OnMyMessagebox(WPARAM wParam, LPARAM lParam)
{
	printf ("wParam = %d",wParam);
	switch (wParam)
	{
	 case 1:		
	    MessageBox("����ע��ɹ�","��ʾ");
	    break;
    case 2:
		MessageBox("����ע��ʧ��!","��ʾ");
		break;
	case 3:
		MessageBox("���Ƚ���!","��ʾ");
		break;
	case 4:
		MessageBox("MMSI��ע��ɹ�!","��ʾ");
		break;
	case 5:
		MessageBox("MMSI��ע��ʧ��!","��ʾ");
		break;
	case 6:
		MessageBox("���ܳɹ�!","��ʾ");
		break;
	case 7:
		MessageBox("�����ظ�д��!","��ʾ");
		break;
	case 8:
		MessageBox("����ʧ��!","��ʾ");
		break;
	case 9:
		MessageBox("���ܳɹ�!","��ʾ");
		break;
	case 10:
		MessageBox("�������!","��ʾ");
		break;
	case 11:
		MessageBox("����ʧ��!","��ʾ");
		break;
	case 12:
		MessageBox("��δ��������!","��ʾ");
		break;
	case 13:
		MessageBox("������ע��ɹ�!","��ʾ");
		break;
	case 14:
		MessageBox("������ע��ɹ�!","��ʾ");
		break;
	case 15:
		MessageBox("ע��ʧ��!","��ʾ");
		break;
	case 16:
		MessageBox("ȫ��ע��ɹ�!","��ʾ");
		break;
	case 17:
		MessageBox("���30�룬ע��ɹ�!","��ʾ");
		break;
	case 18:
		MessageBox("���1���ӣ�ע��ɹ�!","��ʾ");
		break;
	case 19:
		MessageBox("���3���ӣ�ע��ɹ�!","��ʾ");
		break;
	case 20:
		MessageBox("��������ע��ɹ�!","��ʾ");
		break;
	case 21:
		MessageBox("��������ע��ʧ��!","��ʾ");
		break;
	case 22:
		MessageBox("��λ��λ��ע��ɹ�!","��ʾ");
		break;
	case 23:
		MessageBox("��λ��λ��ע��ʧ��!","��ʾ");
		break;

	};
	return 0;
}
