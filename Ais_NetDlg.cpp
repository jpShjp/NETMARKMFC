
// Ais_NetDlg.cpp : 实现文件
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
char commName[256]; //自动查找串口的数组
char comflag;//串口存在标记变量
char comflagnum;//串口查询个数变量
int commport; //串口号
char IsConnect;//网位仪是否连接
char icom; //串口数据变量 计数
static char com_start; //串口帧起始变量
BYTE rxdata[20];
char siostate; //串口状态 是否打开的变量
char Timeflag;  //定时器标记变量
char TimeIndex; //等待串口连接是定时器计数
char configShipName = 0;
int IsSize = 0;  //是否写入船舶尺寸标记变量
char OperatePage;//当前操作的页

void InitConsoleWindow() //初始化控制台
{
    int nCrt = 0;
    FILE* fp;
    AllocConsole();
    nCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
    fp = _fdopen(nCrt, "w");
    *stdout = *fp;
    setvbuf(stdout, NULL, _IONBF, 0);
}

//  Edit 数据清空
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

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CAis_NetDlg 对话框




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


// CAis_NetDlg 消息处理程序

BOOL CAis_NetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//InitConsoleWindow();
	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	//添加 Tab Control

	//添加选项卡名称
	m_tabCtrl.InsertItem(0,_T("流刺网/帆张网网位仪信息"));
	m_tabCtrl.InsertItem(1,_T("拖网网位仪信息"));
	m_tabCtrl.InsertItem(2,_T("钥匙信息"));
    
	//设置IDC_TAB1为父窗口
	page1.Create(IDD_DriftNet,GetDlgItem(IDC_TAB1));
	page2.Create(IDD_TRAWL,GetDlgItem(IDC_TAB1));
	page3.Create(IDD_KEY,GetDlgItem(IDC_TAB1));
    
	//获得IDC_TABTEST客户区大小
	CRect rc;
	m_tabCtrl.GetClientRect(&rc);
    
	//调整子对话框在父窗口中的位置
	rc.top += 25;
	rc.bottom -= 1;
	rc.left += 1;
	rc.right -= 2;

	//设置子对话框尺寸并移动到指定位置
	page1.MoveWindow(&rc);
	page2.MoveWindow(&rc);
	page3.MoveWindow(&rc);

	//分别设置隐藏和显示
	page1.ShowWindow(true);
	page2.ShowWindow(false);
	page3.ShowWindow(false);

	//设置默认的选项卡
	m_tabCtrl.SetCurSel(0);

	SetTimer(1,20,NULL); // 每20ms 读取一次串口信息

	// 变量初始化
	IsConnect = 0;
	siostate = 0;
	Timeflag = 0;
	TimeIndex = 0;
	OperatePage = 0;

	// 控件状态初始化
	ItemInit();

 	static1.SetWindowText("网位仪未连接");
	My_StaticFont.CreatePointFont(150, "宋体");
	static1.SetFont(&My_StaticFont);   
	pMainDlg = this;
	PortSet();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAis_NetDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAis_NetDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CAis_NetDlg::PortSet(void)
{
	
	HKEY hKey;

	comflag = 0;
	if( ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, "Hardware\\DeviceMap\\SerialComm", NULL, KEY_READ, &hKey) == ERROR_SUCCESS) // 打开串口注册表
	{
		int i=0;
		char portName[256];
		CString my_com;
		DWORD dwLong,dwSize;

		while(1)  //WIN7 code
		{
			dwLong = dwSize = sizeof(portName);
			if( ::RegEnumValue( hKey, i, portName, &dwLong, NULL, NULL, (PUCHAR)commName, &dwSize ) == ERROR_NO_MORE_ITEMS ) // 枚举串口   
			{   
				if (siostate == 1)
				{
					MessageBox("网位仪设备未连接!","提示");
				}
				siostate = 0;
				break;
			}

			if( strncmp("\\Device\\Silabser",portName,16)==0)   //Device\\ProlificSerial0
			{
				comflag = 1;

				if( commName[4] == 0)   //串口个数为个位数
				{
					comflagnum = 0;
					i = 0;
				}
				else                    //串口个数为双数
				{
					comflagnum = 1;
					i = 0;
				}

				if(comflagnum == 0)	  commport = commName[3]-0x30-1;
				else if(comflagnum == 1)   commport = (commName[3]-0x30)*10 + commName[4]-0x30-1;

				SioReset(commport,100,100);	
				SioBaud(commport,WSC_Baud9600);//设置波特率
				SioParms(commport, WSC_NoParity ,WSC_OneStopBit, WSC_WordLength8);//配置参数

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
	// TODO: 在此添加控件通知处理程序代码
	icom = 0;
	if(siostate == 0)  //打开串口
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
			SetTimer(2,10000,NULL); //连接失败处理
			SetTimer(3,1000,NULL);  //等待连接

			OpenCom.SetWindowText(_T("关闭串口"));
			OpenCom.EnableWindow(FALSE);
			static1.SetWindowText(_T("网位仪连接中."));
		}
	}
	else  // 关闭串口
	{
		siostate = 0;
		IsConnect = 0;
		OpenCom.SetWindowText(_T("打开串口"));
		static1.SetWindowText(_T("网位仪未连接"));

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
				case OpenSerial: //打开网位仪命令
					if (rxdata[2] == 1)
					{
						IsConnect = 1;
						OpenCom.EnableWindow(TRUE);
						static1.SetWindowText(_T("网位仪已连接     "));
						KillTimer(2);
						KillTimer(3);
						TimeIndex = 0;
						EnableItem();

					}
					else 
					{

					}
					break;
				case CloseSerial: //关闭串口
					break;

				case WriteShipName: //船名注入
						if(rxdata[2] == 0x01)
							MessageBox(_T(" 船名注入成功!"),_T("提示"));
						else if (rxdata[2] == 0x00)
							MessageBox(_T("船名注入失败!"),_T("提示"));
						else if(rxdata[2] == 0x02)
							MessageBox(_T("请先解密!"),_T("提示"));

						if (OperatePage == 0)
							page1.KillTimer(1);
						else if (OperatePage == 1)
							page2.KillTimer(1);
						EnableItem();
					icom = 0;
					break;

				case ReadShipName: //向网位仪读取船名（含）
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
								if(e == 0)  //船名不含网号部分
								{
									c = (unsigned char )((b[i]>>(18 - j*6)) & 0x3F); //转换成6位ASCII码
									if(c <= 31)   c += 0x40; //转换成8位ASCII码

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
								else if(e == 1 && k <= rxdatatemp)  //网号部分
								{
									d = (unsigned char )((b[i]>>(18 - j*6)) & 0x3F); //转换成6位ASCII码
									if(d <= 31)   d += 0x40; //转换成8位ASCII码
									strtemp2.Format(_T("%c"),d);
									if (OperatePage == 0)
									{
										page1.V_Drift_NetNum += strtemp2; //显示网号
									}
									else if (OperatePage == 1)
									{
										page2.V_Trawl_NetNum += strtemp2; //显示网号
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

				case WriteMMSI:  //MMSI注入
						if(rxdata[2] == 0x01)   MessageBox("MMSI号注入成功!","提示");
						else if(rxdata[2] == 0x00)   MessageBox("MMSI号注入失败!","提示");
						else if(rxdata[2] == 0x02)   MessageBox("请先解密!","提示");

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

				case ReadMMSI: //MMSI注入
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

				case Encryp:  //加密
					{
					if(rxdata[2] == 1)   MessageBox("加密成功!","提示");
					else if(rxdata[2] == 2)   MessageBox("请勿重复写码!","提示");
					else if(rxdata[2] == 0)   MessageBox("加密失败!","提示");

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

				case Encode:  //解密
					{
					if(rxdata[2] == 1)   MessageBox("解密成功!","提示");
					else if(rxdata[2] == 2)   MessageBox("密码错误!","提示");
					else if(rxdata[2] == 0)   MessageBox("解密失败!","提示");
					else if(rxdata[2] == 3)   MessageBox("尚未设置密码!","提示");

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
					if(rxdata[2] == 0x01)   MessageBox("注入成功!","提示");
					else if(rxdata[2] == 0x00)   MessageBox("注入失败!","提示");
					page1.KillTimer(1);
					EnableItem();
					icom = 0;
					}
				break;

				case Interval:  // 发射间隔
				{
					
					if(rxdata[2] == 1)   
					{
						if(page1.V_Drift_Interval == 1)	{MessageBox("间隔30秒，注入成功!","提示"); printf("m_Radio = %d \n",page1.V_Drift_Interval);}
						else if(page1.V_Drift_Interval == 2)	{MessageBox("间隔1分钟，注入成功!","提示"); printf("m_Radio = %d \n",page1.V_Drift_Interval);}
						else if(page1.V_Drift_Interval == 3)	{MessageBox("间隔3分钟，注入成功!","提示"); printf("m_Radio = %d \n",page1.V_Drift_Interval);}
					}
					else if(rxdata[2] == 0)   MessageBox("注入失败！","提示");

					page1.UpdateData(FALSE);
					page1.KillTimer(1);

					EnableItem();
					icom = 0;
				}
					break;
					
				case SizeWrite:  //船舶尺寸注入
					{
						if(rxdata[2] == 0x01)   MessageBox("船舶长宽注入成功!","提示");
						else if(rxdata[2] == 0x00)   MessageBox("船舶长宽注入失败!","提示");
						page1.UpdateData(FALSE);
						page1.KillTimer(1);
						EnableItem();
						icom = 0;
					}
					break;

				case KeyWrite:  //钥匙注入
					{
						if(rxdata[2] == 0x01 )   
						{		
							MessageBox("船名注入成功!","提示");
						}
						else if(rxdata[2] == 0x00)   MessageBox("船名注入失败!","提示");
						else if(rxdata[2] == 0x02)   MessageBox("请先解密!","提示");

						GetDlgItem(IDC_OpenCom)->EnableWindow(TRUE);
						page3.GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(TRUE);
						page3.GetDlgItem(IDC_ShipNameWrite)->EnableWindow(TRUE);
						page3.GetDlgItem(IDC_ShipNameRead)->EnableWindow(TRUE);
						icom = 0;
					}
					break;

				case KeyRead:  //读取钥匙
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
								if(k <= rxdatatemp-1) //船名不含网号部分
								{
									//printf("%d ",k);
									c = (char )((b[i]>>(18 - j*6)) & 0x3F); //转换成6位ASCII码
									if(c <= 31)   c += 0x40; //转换成8位ASCII码//////////////////
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

				case LocatWrite: //拖网位置注入
					{
						if(rxdata[2] == 0x01)   MessageBox("网位仪位置注入成功!","提示");
						else if(rxdata[2] == 0x00)   MessageBox("网位仪位置注入成功!","提示");
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case 1:
		getcomm();
		break;
	case 2: // 串口连接失败
		KillTimer(2);
		KillTimer(3);
		TimeIndex = 0;

		OpenCom.EnableWindow(TRUE);
		IsConnect = 0;
		static1.SetWindowText(_T("网位仪未连接      "));
		break;
	case 3: //等待串口连接
			TimeIndex++;
			TimeIndex %= 6;
			switch (TimeIndex)
			{
			case 0:
				static1.SetWindowText("网位仪连接中.");
			break;

			case 1:
				static1.SetWindowText("网位仪连接中..");
			break;

			case 2:
				static1.SetWindowText("网位仪连接中...");
			break;

			case 3:
				static1.SetWindowText("网位仪连接中....");
			break;
			case 4:
				static1.SetWindowText("网位仪连接中.....");
			break;
			case 5:
				static1.SetWindowText("网位仪连接中     ");
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

	// TODO:  在此更改 DC 的任何特性
	if(pWnd->GetDlgCtrlID() == IDC_STATIC1)
	{
		if(IsConnect == 0)	
			pDC->SetTextColor(RGB(255,0,0)); //红色
		else if(IsConnect == 1)	
			pDC->SetTextColor(RGB(0,255,0)); //绿色
		else if(IsConnect == 2)	
			pDC->SetTextColor(RGB(255,100,0)); //黄色

		pDC->SelectObject(&My_StaticFont);  // 设置字体
		pDC->SetBkColor(RGB(237,233,224));  //设置背景色
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CAis_NetDlg::OnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
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

