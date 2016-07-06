
// winioMFCDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "winioMFC.h"
#include "winioMFCDlg.h"
#include "afxdialogex.h"
#include "..\WinIo\Source\Dll\winio.h"
#include "time.h"
#include <Winsock2.h>
#pragma comment(lib,"ws2_32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void parse(int &x, int &y, int &z, char *num);
void sensor(int ix, int iy, int iz);

// int KEYUPAndDOWN = 0;
// int KEYLEFTAndRIGHT = 0;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
int keyUp;		int oldKeyUp;
int keyDown;	int oldKeyDown;
int keyLeft;	int oldKeyLeft;
int keyRight;	int oldKeyRight;

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


// CwinioMFCDlg 对话框




CwinioMFCDlg::CwinioMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CwinioMFCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
//	m_edit = 0;
}

void CwinioMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
//	DDX_Text(pDX, IDC_EDIT1, m_edit);
}

BEGIN_MESSAGE_MAP(CwinioMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CwinioMFCDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_SETDELAY, &CwinioMFCDlg::OnBnClickedButtonSetdelay)
END_MESSAGE_MAP()


// CwinioMFCDlg 消息处理程序

BOOL CwinioMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CwinioMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CwinioMFCDlg::OnPaint()
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
HCURSOR CwinioMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


typedef bool (_stdcall *bRet1)();
typedef bool (_stdcall *bRet2)(WORD wPortAddr,DWORD dwPortVal,BYTE bSize);
typedef bool (_stdcall *bRet0)(PWSTR pszWinIoDriverPath, bool IsDemandLoaded);
typedef bool (_stdcall *bRet4)(WORD wPortAddr, PDWORD pdwPortVal, BYTE bSize);
typedef void (_stdcall *bVoid)();


#define KBC_CMD 0x64
#define KBC_DATA 0x60
void KEY_UP(int vk_in);
void KEY_DOWN(int vk_in);
void KBCWait4IBE();

bRet2 SPV = NULL;
bRet4 GPV = NULL;
bVoid shut = NULL;
UINT       WorkForce(LPVOID lpParameter);
UINT       UDPThread(LPVOID lpParameter);//线程函数声明


int GAMEDELAY = 200;
CRITICAL_SECTION g_cs;
HANDLE hMutex;

UINT       WorkForce(LPVOID lpParameter){
	HMODULE h1 = ::LoadLibrary("WinIo32.dll");
	if (h1 ==NULL)
	{
		MessageBox(NULL,"failed load  WinIo32.dll","Title(标题)",MB_OK); 
		return 0;
	}
	//初始化程序
	bRet1 iz=(bRet1)GetProcAddress(h1,"InitializeWinIo");
	if(!iz)
	{
		return 0;
	}	
	bool biz = iz();
	if(biz){
		MessageBox(NULL,"InitializeWinIo成功","Title(标题)",MB_OK); 	
	}
	else{
		return 0;
	}


	
	SPV =(bRet2)GetProcAddress(h1,"SetPortVal");
	if (!SPV)
	{
		return 0;
	}	
	GPV=(bRet4)GetProcAddress(h1,"GetPortVal");
	if (!SPV)
	{
		return 0;
	}	
	shut = (bVoid)GetProcAddress(h1,"ShutdownWinIo");
	if(!shut){
	}
	int n = 500;
	while(1) {
		WaitForSingleObject(hMutex, AF_INET);
		if(keyUp != oldKeyUp) {
			KEY_UP(oldKeyUp);
			KEY_DOWN(keyUp);
			oldKeyUp = keyUp;
		}
		if(keyDown != oldKeyDown) {
			KEY_UP(oldKeyDown);
			KEY_DOWN(keyDown);
			oldKeyDown = keyDown;
		}
		if(keyLeft != oldKeyLeft) {
			KEY_UP(oldKeyLeft);
			KEY_DOWN(keyLeft);
			oldKeyLeft = keyLeft;
		}
		if(keyRight != oldKeyRight) {
			KEY_UP(oldKeyRight);
			KEY_DOWN(keyRight);
			oldKeyRight = keyRight;
		}
		
		ReleaseMutex(hMutex);
	}
	shut();
	return 0;
}
void CwinioMFCDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	InitializeCriticalSection(&g_cs);
	hMutex = CreateMutex(NULL, false, NULL);
	CWinThread       *pMyFirstWorker, *pMySecondWorker;
	LPVOID       pParam = NULL;
	int       nPriority = THREAD_PRIORITY_ABOVE_NORMAL;//默认为THREAD_PRIORITY_NORMAL
	UINT       nStackSize = 0;//与创建它的线程堆栈大小相同
	DWORD       dwCreateFlags = 0;//创建后立即执行
	LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL ;//与创建它的线程安全属性相同

	pMyFirstWorker=AfxBeginThread( (AFX_THREADPROC)WorkForce, pParam, nPriority , nStackSize,
		dwCreateFlags , lpSecurityAttrs);

	pMySecondWorker=AfxBeginThread( (AFX_THREADPROC)UDPThread, pParam, nPriority , nStackSize,
		dwCreateFlags , lpSecurityAttrs);
	////////////////////////////////
	
}



void parse(int &x, int &y, int &z, char *num) {
	int counts = 0;
	int i;
	int j;
	int a[3];
	int d=1;
	memset(a, 0 ,sizeof(a));
	i=0;j=0;
	while(i<3){
		if(num[j]=='-'){
			j++;
			d=-1;
		}else{
			if(num[j]!=';'){
				int temp = num[j] - '0';
				a[i] = a[i]*10;
				a[i] += temp;
				j++;}
			else{
				a[i]=a[i]*d;
				d=1;
				j++;
				i++;
			}
		} 
	}
	x=a[0];
	y=a[1];
	z=a[2];
}
UINT UDPThread(LPVOID lpParameter){
	WORD wVersionRequested; 
	WSADATA wsaData; 
	int err; 

	wVersionRequested=MAKEWORD(1,1); 

	err = WSAStartup(wVersionRequested, &wsaData); 
	if (err != 0) 
	{ 

		return 0; 
	} 

	if (LOBYTE(wsaData.wVersion)!=1 || 
		HIBYTE(wsaData.wVersion)!=1) 
	{ 
		WSACleanup(); 
		return 0; 
	} 

	SOCKET sockSrv=socket(AF_INET,SOCK_DGRAM,0);
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(1234);

	bind(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	SOCKADDR_IN addrClient;
	int len=sizeof(SOCKADDR);
	char recvBuf[100];

	while(1) {
		recvfrom(sockSrv,recvBuf,100,0,(SOCKADDR*)&addrClient,&len);
		WaitForSingleObject(hMutex, AF_INET);
		keyUp		= (recvBuf[0]-'0')*10 + (recvBuf[1]-'0');
		keyDown		= (recvBuf[2]-'0')*10 + (recvBuf[3]-'0');
		keyLeft		= (recvBuf[4]-'0')*10 + (recvBuf[5]-'0');
		keyRight	= (recvBuf[6]-'0')*10 + (recvBuf[7]-'0');
		ReleaseMutex(hMutex);
	}
	closesocket(sockSrv);
	WSACleanup();
	return 1;
}



void KBCWait4IBE()
{
	DWORD   dwRegVal=0; 
	do 
	{   
		GPV(0x64,&dwRegVal,1); 
	} 
	while(dwRegVal   &   0x00000001); 

}
void KEY_DOWN(int vk_in)
{
	long   byScancode   =   MapVirtualKey(vk_in,0);   
	KBCWait4IBE();//等待键盘缓冲区为空   
	if(SPV(0x64,0xD2,1)==false) {}//cout < < "发送键盘写入命令失败! " < <endl;//发送键盘写入命令 

	KBCWait4IBE();//等待键盘缓冲区为空   
	if(SPV(0x60,(ULONG)byScancode,1)==false) {}//cout < < "按下键失败! " < <endl;//写入按键信息,按下
}
void KEY_UP(int vk_in)
{
	long   byScancode   =   MapVirtualKey(vk_in,0);   
	KBCWait4IBE();//等待键盘缓冲区为空   
	if(SPV(0x64,0xD2,1)==false) {} //cout < < "发送键盘写入命令失败! " < <endl;//发送键盘写入命令 

	KBCWait4IBE();//等待键盘缓冲区为空   
	if(SPV(0x60,(ULONG)(byScancode   |   0x80),1)==false) {}//cout < < "释放键失败! " < <endl;//写入按键信息，释放键 

}



void CwinioMFCDlg::OnBnClickedButtonSetdelay()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
//	GAMEDELAY = m_edit;
}
