
// Ais_NetDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Ais_Net.h"
#include "Ais_NetDlg.h"
#include "afxdialogex.h"
#include <io.h>
#include <fcntl.h>
#include "WSC.H"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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
int IsSize = 0;  //�Ƿ�д�봬���ߴ��Ǳ���
char OperatePage;//��ǰ������ҳ

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
}

void CAis_NetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, static1);
	DDX_Control(pDX, IDC_OpenCom, OpenCom);
	DDX_Control(pDX, IDC_TAB1, m_tabCtrl);
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
	m_tabCtrl.InsertItem(0,_T("������/��������λ����Ϣ"));
	m_tabCtrl.InsertItem(1,_T("������λ����Ϣ"));
	m_tabCtrl.InsertItem(2,_T("Կ����Ϣ"));
    
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
	page1.ShowWindow(true);
	page2.ShowWindow(false);
	page3.ShowWindow(false);

	//����Ĭ�ϵ�ѡ�
	m_tabCtrl.SetCurSel(0);

	SetTimer(1,20,NULL); // ÿ20ms ��ȡһ�δ�����Ϣ

	// ������ʼ��
	IsConnect = 0;
	siostate = 0;
	Timeflag = 0;
	TimeIndex = 0;
	OperatePage = 0;

	// �ؼ�״̬��ʼ��
	ItemInit();

 	static1.SetWindowText("��λ��δ����");
	My_StaticFont.CreatePointFont(150, "����");
	static1.SetFont(&My_StaticFont);   
	pMainDlg = this;
	PortSet();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

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

void CAis_NetDlg::OnBnClickedOpenCom()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	icom = 0;
	if(siostate == 0)  //�򿪴���
	{
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
			SetTimer(2,10000,NULL); //����ʧ�ܴ���
			SetTimer(3,1000,NULL);  //�ȴ�����

			OpenCom.SetWindowText(_T("�رմ���"));
			OpenCom.EnableWindow(FALSE);
			static1.SetWindowText(_T("��λ��������."));
		}
	}
	else  // �رմ���
	{
		siostate = 0;
		IsConnect = 0;
		OpenCom.SetWindowText(_T("�򿪴���"));
		static1.SetWindowText(_T("��λ��δ����"));

		EditEmpty();
		ItemInit();

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


void CAis_NetDlg::getcomm(void)
{
	int temp;
	if( (temp = SioGetc(commport)) >= 0)
	{
		if(temp == 0x24 && icom == 0)
			com_start = 1;

		if (com_start = 1)
		{
			rxdata[icom] = temp;
			
			if (icom == 17)
			{
				com_start = 0;
				switch (rxdata[1])
				{
				case OpenSerial: //����λ������
					if (rxdata[2] == 1)
					{
						IsConnect = 1;
						OpenCom.EnableWindow(TRUE);
						static1.SetWindowText(_T("��λ��������     "));
						KillTimer(2);
						KillTimer(3);
						TimeIndex = 0;
						EnableItem();

					}
					else 
					{

					}
					break;
				case CloseSerial: //�رմ���
					break;

				case WriteShipName: //����ע��
						if(rxdata[2] == 0x01)
							MessageBox(_T(" ����ע��ɹ�!"),_T("��ʾ"));
						else if (rxdata[2] == 0x00)
							MessageBox(_T("����ע��ʧ��!"),_T("��ʾ"));
						else if(rxdata[2] == 0x02)
							MessageBox(_T("���Ƚ���!"),_T("��ʾ"));

						if (OperatePage == 0)
							page1.KillTimer(1);
						else if (OperatePage == 1)
							page2.KillTimer(1);
						EnableItem();
					icom = 0;
					break;

				case ReadShipName: //����λ�Ƕ�ȡ����������
					{
						unsigned long b[5];
						unsigned char c,d,i,j,k=0;
						unsigned char rxdatatemp;
						CString strtemp1;CString strtemp2;
						unsigned char e = 0;

						strtemp1.Empty();strtemp2.Empty();
						rxdatatemp = rxdata[2]/6;
						for(i=0;i<5;i++)
						{   b[i] = (rxdata[3+i*3]<<16); b[i] += (rxdata[4+i*3]<<8); b[i] += rxdata[5+i*3]; }
						
						for(i=0;i<5;i++)//for(i=0;i<rxdata[1]/6;i++)
						{
							for(j=0;j<4;j++)
							{
								k++;
								if(e == 0)  //�����������Ų���
								{
									c = (unsigned char )((b[i]>>(18 - j*6)) & 0x3F); //ת����6λASCII��
									if(c <= 31)   c += 0x40; //ת����8λASCII��

									if(c == '-')
									{
										e = 1;
									}
									else 
									{
										strtemp1.Format(_T("%c"),c);
										if (OperatePage == 0)
										{
											page1.V_Drift_shipName += strtemp1;
										}
										else if (OperatePage == 1)
										{
											page2.V_Trawl_ShipName += strtemp1;
										}
									}
								}
								else if(e == 1 && k <= rxdatatemp)  //���Ų���
								{
									d = (unsigned char )((b[i]>>(18 - j*6)) & 0x3F); //ת����6λASCII��
									if(d <= 31)   d += 0x40; //ת����8λASCII��
									strtemp2.Format(_T("%c"),d);
									if (OperatePage == 0)
									{
										page1.V_Drift_NetNum += strtemp2; //��ʾ����
									}
									else if (OperatePage == 1)
									{
										page2.V_Trawl_NetNum += strtemp2; //��ʾ����
									}
								}
							}
						}
						icom = 0;

						if (OperatePage == 0)
						{
							page1.UpdateData(FALSE);
							page1.KillTimer(1);
						}
						else if(OperatePage == 1)
						{
							page2.UpdateData(FALSE);
							page2.KillTimer(1);
						}
						EnableItem();
				}
				break;

				case WriteMMSI:  //MMSIע��
						if(rxdata[2] == 0x01)   MessageBox("MMSI��ע��ɹ�!","��ʾ");
						else if(rxdata[2] == 0x00)   MessageBox("MMSI��ע��ʧ��!","��ʾ");
						else if(rxdata[2] == 0x02)   MessageBox("���Ƚ���!","��ʾ");

						if (OperatePage == 0)
						{
							page1.KillTimer(1);
						}
						else if (OperatePage == 1)
						{
							page2.KillTimer(1);
						}
						EnableItem();
				break;

				case ReadMMSI: //MMSIע��
					{
						char *aa = (char *)malloc(20*sizeof(char));
						unsigned long rev;
						rev = rxdata[2] << 24;  rev +=(rxdata[3] << 16); rev +=(rxdata[4] << 8); rev +=rxdata[5];
						my_itoa(rev,aa,10);
						if (OperatePage == 0)
						{
							page1.V_Drift_MMSI = aa;
							switch (strlen(page1.V_Drift_MMSI))
							{
							case 1:
								page1.V_Drift_MMSI.Insert(0,"00000000");
								break;
							case 2:
								page1.V_Drift_MMSI.Insert(0,"0000000");
								break;
							case 3:
								page1.V_Drift_MMSI.Insert(0,"000000");
								break;
							case 4:
								page1.V_Drift_MMSI.Insert(0,"00000");
								break;
							case 5:
								page1.V_Drift_MMSI.Insert(0,"0000");
								break;
							case 6:
								page1.V_Drift_MMSI.Insert(0,"000");
								break;
							case 7:
								page1.V_Drift_MMSI.Insert(0,"00");
								break;
							case 8:
								page1.V_Drift_MMSI.Insert(0,"0");
								break;
							}
							page1.UpdateData(FALSE);
							page1.KillTimer(1);
						}
						else if(OperatePage == 1)
						{
							page2.V_Trawl_MMSI = aa;
							switch (strlen(page2.V_Trawl_MMSI))
							{
							case 1:
								page2.V_Trawl_MMSI.Insert(0,"00000000");
							break;

							case 2:
								page2.V_Trawl_MMSI.Insert(0,"0000000");
							break;
							case 3:
								page2.V_Trawl_MMSI.Insert(0,"000000");
							break;
							case 4:
								page2.V_Trawl_MMSI.Insert(0,"00000");
							break;
							case 5:
								page2.V_Trawl_MMSI.Insert(0,"0000");
							break;
							case 6:
								page2.V_Trawl_MMSI.Insert(0,"000");
							break;
							case 7:
								page2.V_Trawl_MMSI.Insert(0,"00");
							break;
							case 8:
								page2.V_Trawl_MMSI.Insert(0,"0");
							break;
							
							}						
							page2.UpdateData(FALSE);
							page2.KillTimer(1);
						}

						EnableItem();
						icom = 0;
					}

					break;

				case Encryp:  //����
					{
					if(rxdata[2] == 1)   MessageBox("���ܳɹ�!","��ʾ");
					else if(rxdata[2] == 2)   MessageBox("�����ظ�д��!","��ʾ");
					else if(rxdata[2] == 0)   MessageBox("����ʧ��!","��ʾ");

					if (OperatePage == 0)
					{
						page1.KillTimer(1);
					}
					else if (OperatePage == 1)
					{
						page2.KillTimer(1);
					}
					EnableItem();
					icom = 0;
					}
				 break;

				case Encode:  //����
					{
					if(rxdata[2] == 1)   MessageBox("���ܳɹ�!","��ʾ");
					else if(rxdata[2] == 2)   MessageBox("�������!","��ʾ");
					else if(rxdata[2] == 0)   MessageBox("����ʧ��!","��ʾ");
					else if(rxdata[2] == 3)   MessageBox("��δ��������!","��ʾ");

					if (OperatePage == 0)
					{
						page1.KillTimer(1);
					}
					else if (OperatePage == 1)
					{
						page2.KillTimer(1);
					}
					EnableItem();

					icom = 0;
					}
				break;

				case KindWrite:
					{
					if(rxdata[2] == 0x01)   MessageBox("ע��ɹ�!","��ʾ");
					else if(rxdata[2] == 0x00)   MessageBox("ע��ʧ��!","��ʾ");
					page1.KillTimer(1);
					EnableItem();
					icom = 0;
					}
				break;

				case Interval:  // ������
				{
					
					if(rxdata[2] == 1)   
					{
						if(page1.V_Drift_Interval == 1)	{MessageBox("���30�룬ע��ɹ�!","��ʾ"); printf("m_Radio = %d \n",page1.V_Drift_Interval);}
						else if(page1.V_Drift_Interval == 2)	{MessageBox("���1���ӣ�ע��ɹ�!","��ʾ"); printf("m_Radio = %d \n",page1.V_Drift_Interval);}
						else if(page1.V_Drift_Interval == 3)	{MessageBox("���3���ӣ�ע��ɹ�!","��ʾ"); printf("m_Radio = %d \n",page1.V_Drift_Interval);}
					}
					else if(rxdata[2] == 0)   MessageBox("ע��ʧ�ܣ�","��ʾ");

					page1.UpdateData(FALSE);
					page1.KillTimer(1);

					EnableItem();
					icom = 0;
				}
					break;
					
				case SizeWrite:  //�����ߴ�ע��
					{
						if(rxdata[2] == 0x01)   MessageBox("��������ע��ɹ�!","��ʾ");
						else if(rxdata[2] == 0x00)   MessageBox("��������ע��ʧ��!","��ʾ");
						page1.UpdateData(FALSE);
						page1.KillTimer(1);
						EnableItem();
						icom = 0;
					}
					break;

				case KeyWrite:  //Կ��ע��
					{
						if(rxdata[2] == 0x01 )   
						{		
							MessageBox("����ע��ɹ�!","��ʾ");
						}
						else if(rxdata[2] == 0x00)   MessageBox("����ע��ʧ��!","��ʾ");
						else if(rxdata[2] == 0x02)   MessageBox("���Ƚ���!","��ʾ");

						GetDlgItem(IDC_OpenCom)->EnableWindow(TRUE);
						page3.GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(TRUE);
						page3.GetDlgItem(IDC_ShipNameWrite)->EnableWindow(TRUE);
						page3.GetDlgItem(IDC_ShipNameRead)->EnableWindow(TRUE);
						icom = 0;
					}
					break;

				case KeyRead:  //��ȡԿ��
					{
						unsigned long b[5];
						char c,i,j,k=0;
						unsigned char rxdatatemp;
						CString strtemp1;CString strtemp2;

						page3.V_Key_ShipName="";
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
									page3.V_Key_ShipName += strtemp1;
									//printf(" = %s \n",strtemp);

								}
							}
						}
						page3.UpdateData(FALSE);

						GetDlgItem(IDC_OpenCom)->EnableWindow(TRUE);
						page3.GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(TRUE);
						page3.GetDlgItem(IDC_ShipNameWrite)->EnableWindow(TRUE);
						page3.GetDlgItem(IDC_ShipNameRead)->EnableWindow(TRUE);

						icom = 0;
					}
					break;

				case LocatWrite: //����λ��ע��
					{
						if(rxdata[2] == 0x01)   MessageBox("��λ��λ��ע��ɹ�!","��ʾ");
						else if(rxdata[2] == 0x00)   MessageBox("��λ��λ��ע��ɹ�!","��ʾ");
						page2.KillTimer(1);
						EnableItem();
						icom = 0;
					}
					break;
			
			}


			}
			if (icom < 17)
				icom++;
			else
				icom = 0;
		}
	}
}


void CAis_NetDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch (nIDEvent)
	{
	case 1:
		getcomm();
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
void CAis_NetDlg::ItemInit(void)
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
	page1.GetDlgItem(IDC_KIND1)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_KIND2)->EnableWindow(FALSE);
	page1.GetDlgItem(IDC_KIND_WRITE)->EnableWindow(FALSE);


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

	page3.GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(FALSE);
	page3.GetDlgItem(IDC_ShipNameWrite)->EnableWindow(FALSE);
	page3.GetDlgItem(IDC_ShipNameRead)->EnableWindow(FALSE);
}

void CAis_NetDlg::EnableItem(void)
{
		GetDlgItem(IDC_OpenCom)->EnableWindow(TRUE);
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

		page2.GetDlgItem(IDC_ShipNameRead)->EnableWindow(TRUE);
		page2.GetDlgItem(IDC_MMSIRead)->EnableWindow(TRUE);
		page2.GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(TRUE);
		page2.GetDlgItem(IDC_EDIT_NetNum)->EnableWindow(TRUE);
		page2.GetDlgItem(IDC_EDIT_MMSI)->EnableWindow(TRUE);
		page2.GetDlgItem(IDC_EDIT_Code)->EnableWindow(TRUE);
		page2.GetDlgItem(IDC_LOCAT_WRITE)->EnableWindow(TRUE);	
		page2.GetDlgItem(IDC_LOCAT_WIDTH)->EnableWindow(TRUE);
		page2.GetDlgItem(IDC_LOCAT_LENGTH)->EnableWindow(TRUE);		

		page3.GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(TRUE);
		page3.GetDlgItem(IDC_ShipNameRead)->EnableWindow(TRUE);

		if(!page1.V_Drift_shipName.IsEmpty() && !page1.V_Drift_NetNum.IsEmpty())
			page1.GetDlgItem(IDC_ShipNameWrite)->EnableWindow(TRUE);
		else 
			page1.GetDlgItem(IDC_ShipNameWrite)->EnableWindow(FALSE);
		

		if((page1.V_Drift_MMSI.GetLength()) == 9)
			page1.GetDlgItem(IDC_MMSIWrite)->EnableWindow(TRUE);
		else 
			page1.GetDlgItem(IDC_MMSIWrite)->EnableWindow(FALSE);

		

		if((page1.V_Drift_Code.GetLength()) == 6)
		{
			page1.GetDlgItem(IDC_Encryp)->EnableWindow(TRUE);
			page1.GetDlgItem(IDC_Encode)->EnableWindow(TRUE);
		}
		else 
		{
			page1.GetDlgItem(IDC_Encryp)->EnableWindow(FALSE);
			page1.GetDlgItem(IDC_Encode)->EnableWindow(FALSE);
		}

		if (IsSize)
		{
			page1.GetDlgItem(IDC_SIZE_WRITE)->EnableWindow(TRUE);
			page1.GetDlgItem(IDC_SIZE_LONG)->EnableWindow(TRUE);
			page1.GetDlgItem(IDC_SIZE_WIDTH)->EnableWindow(TRUE);
		}
		else
		{
			page1.GetDlgItem(IDC_SIZE_WRITE)->EnableWindow(FALSE);
			page1.GetDlgItem(IDC_SIZE_LONG)->EnableWindow(FALSE);
			page1.GetDlgItem(IDC_SIZE_WIDTH)->EnableWindow(FALSE);		
		}

		if(!page2.V_Trawl_ShipName.IsEmpty()&&!page2.V_Trawl_NetNum.IsEmpty())
		{
			page2.GetDlgItem(IDC_ShipNameWrite)->EnableWindow(TRUE);
		}
		else
		{
			page2.GetDlgItem(IDC_ShipNameWrite)->EnableWindow(FALSE);
		}

		if (!page2.V_Trawl_MMSI.IsEmpty())
		{
			page2.GetDlgItem(IDC_MMSIWrite)->EnableWindow(TRUE);
		}
		else
		{
			page2.GetDlgItem(IDC_MMSIWrite)->EnableWindow(FALSE);
		}

		if (!page2.V_Trawl_code.IsEmpty())
		{
			page2.GetDlgItem(IDC_Encryp)->EnableWindow(TRUE);
			page2.GetDlgItem(IDC_Encode)->EnableWindow(TRUE);
		}
		else
		{
			page2.GetDlgItem(IDC_Encryp)->EnableWindow(FALSE);
			page2.GetDlgItem(IDC_Encode)->EnableWindow(FALSE);		
		}
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

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}


void CAis_NetDlg::OnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int CurSel = m_tabCtrl.GetCurSel();
	OperatePage = CurSel;
    switch(CurSel)
    {
    case 0:
        page1.ShowWindow(true);
        page2.ShowWindow(false);
		page3.ShowWindow(false);
        break;
    case 1:
        page1.ShowWindow(false);
        page2.ShowWindow(true);
		page3.ShowWindow(false);
        break;
	case 2:
		page1.ShowWindow(false);
        page2.ShowWindow(false);
		page3.ShowWindow(true);
		break;
    default:
        ;
    }  
	*pResult = 0;
}

