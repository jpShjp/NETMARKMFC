
// Ais_NetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Ais_Net.h"
#include "Ais_NetDlg.h"
#include "afxdialogex.h"
#include <io.h>
#include <fcntl.h>
#include "WSC.H"
#define NetSel 0 // 0为帆张网、流刺网； 1为拖网

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CAis_NetDlg *pDlg;
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
char IsSize = 0;  //是否写入船舶尺寸标记变量
extern char AllWrite;    //全部注入标记
extern char AllReadFlag;   //全部读取标记
extern char AllWrite; //判断全部注入是否成功
static char ThreadRun; //线程是否在跑

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


// 串口设置
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

// 设置鼠标图标
BOOL CAis_NetDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// 当鼠标移动到LOGO上更换鼠标图案
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//  单击图片打开网页
	CRect rect;
	m_Picture.GetWindowRect(&rect);
	ScreenToClient(&rect);
	
	if (rect.PtInRect(point))
		ShellExecute(AfxGetMainWnd()->m_hWnd,_T("open"),
			_T("http://www.sandemarine.com/"),_T(""),NULL,0);
	CDialogEx::OnLButtonDown(nFlags, point);
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
	m_hCursor = AfxGetApp()->LoadCursor(IDC_CURSOR1); //载入鼠标图标
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
	if (NetSel == 0)
	{
		m_tabCtrl.InsertItem(0,_T("流刺网/帆张网网位仪信息"));
	   m_tabCtrl.InsertItem(2,_T("钥匙信息"));
	}
	else if (NetSel == 1)
		m_tabCtrl.InsertItem(1,_T("拖网网位仪信息"));
	
    
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
	if (NetSel == 0)
	{
		page1.ShowWindow(true);
		page3.ShowWindow(false);
	}
	else if (NetSel == 1)
	{
		page2.ShowWindow(true);
	}
	

	//设置默认的选项卡
	m_tabCtrl.SetCurSel(0);

	SetTimer(1,20,NULL); // 每20ms 读取一次串口信息

	// 变量初始化
	IsConnect = 0;
	siostate = 0;
	Timeflag = 0;
	TimeIndex = 0;

	// 控件状态初始化
	ItemInit();
	((CButton*)(page1.GetDlgItem(IDC_RADIO_1min)))->SetCheck(TRUE); // 默认为1分钟发送间隔
	((CButton*)(page1.GetDlgItem(IDC_KIND1)))->SetCheck(TRUE);		//默认选择流刺网
	((CButton*)(page1.GetDlgItem(IDC_SIZE_NO)))->SetCheck(TRUE);		//默认不输入尺寸
	page1.V_Drift_Interval = 2;  
	page1.kind = 0;
	IsSize = 0;


 	static1.SetWindowText("网位仪未连接");
	My_StaticFont.CreatePointFont(150, "宋体");
	page2.my_font.CreatePointFont(110,"宋体");
	static1.SetFont(&My_StaticFont);   
	pDlg = this;
	pMainDlg = this;
	PortSet();

	// 获取串口返回的数据
	hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadGetCom,NULL,0,&ThreadID);
	//SuspendThread(hThread);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


//
//获取串口返回的信息
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


void CAis_NetDlg::OnBnClickedOpenCom()
{
	// TODO: 在此添加控件通知处理程序代码
	icom = 0;
	if(siostate == 0)  //打开串口
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
			SetTimer(2,15000,NULL); //连接失败处理
			SetTimer(3,1000,NULL);  //等待连接

			OpenCom.SetWindowText(_T("关闭串口"));
			OpenCom.EnableWindow(FALSE);
			static1.SetWindowText(_T("网位仪连接中."));
			ThreadRun = 1;
		}
	}
	else  // 关闭串口
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
		OpenCom.SetWindowText(_T("打开串口"));
		static1.SetWindowText(_T("网位仪未连接         "));

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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case 1:
		//getcomm();
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
			if (TimeIndex == 3)
			{
				OpenCom.EnableWindow(TRUE);
			}
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


void CAis_NetDlg::ItemInit(void) // 控件状态初始化
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

	//全部注入读取
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

void CAis_NetDlg::EnableItem(void)  //启用控件
{
		GetDlgItem(IDC_OpenCom)->EnableWindow(TRUE);
		// 流刺网、帆张网
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
		// 拖网
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
		//钥匙
		page3.GetDlgItem(IDC_EDIT_ShipName)->EnableWindow(TRUE);
		page3.GetDlgItem(IDC_ShipNameRead)->EnableWindow(TRUE);
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
	if (pWnd->GetDlgCtrlID() == IDC_STATIC2)
		pDC->SelectObject(&(page2.my_font));  // 设置字体

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CAis_NetDlg::OnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
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
							   pDlg->static1.SetWindowText("网位仪已连接      ");
							   pDlg->KillTimer(3);
							   pDlg->KillTimer(2);
							   TimeIndex = 0;
							   pDlg->EnableItem();
						   }
						   else
						   {
						   }
						   break;

	 				  case CloseSerial: //关闭串口
						   break;
						   
					  case WriteShipName: //船名注入
						  {
							  pDlg->page1.KillTimer(1);
							  pDlg->page2.KillTimer(1);
							  if(AllWrite == 0) //单个注入
							  {
								 if(rxdata[2] == 0x01)
								 {
									 PostMessage(hMain,MY_MESSAGEBOX,1,NULL);
									 //pDlg->MessageBox(_T("船名注入成功!"),_T("提示"));
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
							  else  //全部注入
							  {
									if(rxdata[2] == 0x01)
									{
										//AllWrite++;  
										printf ("船名注入成功\n");
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
						printf ("读取船名\n");
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
								c = (unsigned char )((b[i]>>(18 - j*6)) & 0x3F); //转换成6位ASCII码
								if(c <= 31)   c += 0x40; //转换成8位ASCII码

								shipname[k-1] = c;
								printf ("ccc = %d\n",c);
							}
						}

						// 找到最后一个‘-’的位置
						for (shipindex = 0; shipindex<=rxdatatemp; shipindex++ )
						{
							if (shipname[shipindex ]== '-')
								netNumFlag = shipindex;
						}
						printf ("netflg = %d\n",netNumFlag);

						for (i = 0; i<netNumFlag; i++) //船名
						{
							if (NetSel == 0)
								pDlg->page1.V_Drift_shipName.Insert(i,shipname[i]);
							else 
								pDlg->page2.V_Trawl_ShipName.Insert(i,shipname[i]);
							printf ("shipn = %d\n",shipname[i]);
						}
						//更新数据
						if (NetSel == 0)
							pDlg->page1.UpdateData(FALSE);
						else 
							pDlg->page2.UpdateData(FALSE);			
						
						for(i++;i<rxdatatemp;i++)  //网号
						{
							if (NetSel == 0)
								pDlg->page1.V_Drift_NetNum.Insert(i,shipname[i]);
							else 
								pDlg->page2.V_Trawl_NetNum.Insert(i,shipname[i]);
							printf ("net = %d\n",shipname[i]);
						}						
						//更新数据
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
						

						if (AllReadFlag == 0) //屏蔽全部读取
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

				case WriteMMSI:  //MMSI注入
						{	
							pDlg->page1.KillTimer(1);
							pDlg->page2.KillTimer(1);
							printf ("MMSI注入\n");
							if(AllWrite == 0)	 //单个注入
							{
								if(rxdata[2] == 0x01)  
									PostMessage(hMain,MY_MESSAGEBOX,4,NULL);
								else if(rxdata[2] == 0x00) 
									PostMessage(hMain,MY_MESSAGEBOX,5,NULL);
								else if(rxdata[2] == 0x02)  
									PostMessage(hMain,MY_MESSAGEBOX,3,NULL);

								pDlg->EnableItem();
							}
							else //全部注入
							{
								if(rxdata[2] == 0x01)
								{
									//AllWrite++;
									pDlg->page1.IntervalWrite();
									icom = 17;
									printf ("MMSI注入成功");
								}
							}

						}
					break;

					case ReadMMSI: //MMSII读取
					{
						printf ("MMSI读取\n");
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

						if (AllReadFlag == 0 || IsSize == 0) //屏蔽全部读取
						{
							pDlg->EnableItem();
							icom = 0;
							AllReadFlag = 0;
						}
						else if (AllReadFlag == 1 || IsSize == 1)//全部读取
						{
							pDlg->page1.sizeRead();
							icom = 17;
						}
					}
					break;
					case Encryp:  //加密
						{
							printf ("加密\n");
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

				case Encode:  //解密
						{
								printf ("解密\n");
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

				case KindWrite: //功能注入
					{   
						pDlg->page1.KillTimer(1);
						printf ("功能注入\n");
							if(AllWrite == 0) //单个注入
							{


								if(rxdata[2] == 0x01)  //注入成功
								{
									if (pDlg->page1.kind == 0)
										PostMessage(hMain,MY_MESSAGEBOX,13,NULL);
									else if(pDlg->page1.kind == 1) 
										PostMessage(hMain,MY_MESSAGEBOX,14,NULL);
									pDlg->EnableItem();
								}
								else if(rxdata[2] == 0x00)   // 注入失败
									PostMessage(hMain,MY_MESSAGEBOX,15,NULL);
							}
							else // 全部注入
							{					
								if (IsSize) // 判断是否注入船舶尺寸
								{
									if(rxdata[2] == 0x01)
									{
										pDlg->page1.sizeWrite();
										icom = 17;
										printf ("功能注入成功");
									}
								}
								else //不注入船舶尺寸
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

				case Interval:  // 发射间隔
					{
						pDlg->page1.KillTimer(1);
						printf ("发送间隔注入\n");
						if(AllWrite == 0)	// 单个注入
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
						else // 全部注入
						{
							printf ("jg2");
							if(rxdata[2] == 1)
							{
								pDlg->page1.kindWrite();
								icom = 17;
								printf ("间隔注入成功");
							}
						}	

						pDlg->page1.UpdateData(FALSE);	
					}
					break;
					
				case SizeWrite:  //船舶尺寸注入
					{
						pDlg->page1.KillTimer(1);
						printf ("船舶尺寸注入\n");
						if (AllWrite == 0) // 单个注入
						{
							if(rxdata[2] == 0x01)
								PostMessage(hMain,MY_MESSAGEBOX,20,NULL);
							else if(rxdata[2] == 0x00) 
								PostMessage(hMain,MY_MESSAGEBOX,21,NULL);
						}
						else // 全部注入
						{
							if(rxdata[2] == 0x01)
							{
								pDlg->page1.KillTimer(2);
								AllWrite = 0;
								PostMessage(hMain,MY_MESSAGEBOX,16,NULL);
								printf ("船舶尺寸注入成功");
							}
						}
						pDlg->page1.UpdateData(FALSE);
					
						pDlg->EnableItem();
						icom = 0;
					}	
					break;
				
				case SizeRead: //尺寸读取
					{
						 char *aa = (char *)malloc(20*sizeof(char));
						 int boatlong;
						 char boatwidth;
				 printf("尺寸读取");
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

				case KeyWrite:  //钥匙注入
					{
						printf ("钥匙注入\n");
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

				case 0x06:  //读取钥匙
					{
						printf ("读取钥匙\n");
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
								if(k <= rxdatatemp-1) //船名不含网号部分
								{
									//printf("%d ",k);
									c = (char )((b[i]>>(18 - j*6)) & 0x3F); //转换成6位ASCII码
									if(c <= 31)   c += 0x40; //转换成8位ASCII码//////////////////
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

				case OffSet_Write: //拖网位置注入
					{
						printf ("拖网位置注入\n");
						if(rxdata[2] == 0x01)        PostMessage(hMain,MY_MESSAGEBOX,22,NULL); 
						else if(rxdata[2] == 0x00)   PostMessage(hMain,MY_MESSAGEBOX,23,NULL); 
						pDlg->page2.KillTimer(1);
						pDlg->EnableItem();
						icom = 0;
					}
					break;
				
				case OffSet_Read: //拖网的位置读取
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
			
						printf("拖网位置读取\n");
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
	    MessageBox("船名注入成功","提示");
	    break;
    case 2:
		MessageBox("船名注入失败!","提示");
		break;
	case 3:
		MessageBox("请先解密!","提示");
		break;
	case 4:
		MessageBox("MMSI号注入成功!","提示");
		break;
	case 5:
		MessageBox("MMSI号注入失败!","提示");
		break;
	case 6:
		MessageBox("加密成功!","提示");
		break;
	case 7:
		MessageBox("请勿重复写码!","提示");
		break;
	case 8:
		MessageBox("加密失败!","提示");
		break;
	case 9:
		MessageBox("解密成功!","提示");
		break;
	case 10:
		MessageBox("密码错误!","提示");
		break;
	case 11:
		MessageBox("解密失败!","提示");
		break;
	case 12:
		MessageBox("尚未设置密码!","提示");
		break;
	case 13:
		MessageBox("流刺网注入成功!","提示");
		break;
	case 14:
		MessageBox("帆张网注入成功!","提示");
		break;
	case 15:
		MessageBox("注入失败!","提示");
		break;
	case 16:
		MessageBox("全部注入成功!","提示");
		break;
	case 17:
		MessageBox("间隔30秒，注入成功!","提示");
		break;
	case 18:
		MessageBox("间隔1分钟，注入成功!","提示");
		break;
	case 19:
		MessageBox("间隔3分钟，注入成功!","提示");
		break;
	case 20:
		MessageBox("船舶长宽注入成功!","提示");
		break;
	case 21:
		MessageBox("船舶长宽注入失败!","提示");
		break;
	case 22:
		MessageBox("网位仪位置注入成功!","提示");
		break;
	case 23:
		MessageBox("网位仪位置注入失败!","提示");
		break;

	};
	return 0;
}
