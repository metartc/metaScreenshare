
// YangServerManagerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "YangServerManager.h"
#include "YangServerManagerDlg.h"
#include "ServiceInstaller.h"
#include "afxdialogex.h"
#include <string>

#define SERVICE_NAME             "YangScreenService"

// Displayed name of the service
#define SERVICE_DISPLAY_NAME     "YangScreenService"

// Service start options. SERVICE_BOOT_START
#define SERVICE_START_TYPE       SERVICE_AUTO_START //SERVICE_DEMAND_START

// List of service dependencies - "dep1\0dep2\0\0"
#define SERVICE_DEPENDENCIES     ""

// The name of the account under which the service should run
#define SERVICE_ACCOUNT          "NT AUTHORITY\\LocalService"

// The password to the service account name
#define SERVICE_PASSWORD         NULL

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CYangServerManagerDlg 对话框



CYangServerManagerDlg::CYangServerManagerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_YANGSERVERMANAGER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CYangServerManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON2, m_b_start);
	DDX_Control(pDX, IDC_BUTTON1, m_b_stop);
}

BEGIN_MESSAGE_MAP(CYangServerManagerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, &CYangServerManagerDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CYangServerManagerDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CYangServerManagerDlg 消息处理程序

BOOL CYangServerManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	m_b_stop.EnableWindow(false);
	//ShowWindow(SW_MAXIMIZE);

	//ShowWindow(SW_MINIMIZE);

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CYangServerManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CYangServerManagerDlg::OnPaint()
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
HCURSOR CYangServerManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CYangServerManagerDlg::createProcess() {
	char szPath[MAX_PATH] = { 0 };
	char szExePath[260] = { 0 };
	char cmd[260] = { 0 };


	if (GetModuleFileName(NULL, szExePath, ARRAYSIZE(szExePath)) == 0)
	{
		printf("GetModuleFileName failed w/err 0x%08lx\n", GetLastError());
		//goto Cleanup;
	}
	else {
		std::string s(szExePath);
		int pos = s.rfind('\\');

		memcpy(szPath, s.c_str(), pos + 1);
	}

	sprintf(cmd, "\"%sYangScreenService.exe\" -test testserver", szPath);
	STARTUPINFO sti;
	PROCESS_INFORMATION pi;
	ZeroMemory(&sti, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	if (CreateProcess(NULL, cmd,
		NULL, NULL, false, NULL, NULL, NULL,
		&sti, &pi) == 0) {
		
	}
	m_proc = pi.hProcess;
	Sleep(3000);
	TerminateProcess(m_proc,0);
}

void CYangServerManagerDlg::OnBnClickedButton2()
{
	createProcess();
	InstallService(
		SERVICE_NAME,               // Name of service
		SERVICE_DISPLAY_NAME,       // Name to display
		SERVICE_START_TYPE,         // Service start type
		SERVICE_DEPENDENCIES,       // Dependencies
		SERVICE_ACCOUNT,            // Service running account
		SERVICE_PASSWORD            // Password of the account
	);
	m_b_start.EnableWindow(false);
	m_b_stop.EnableWindow(true);
	yang_start_service(SERVICE_NAME);
}


void CYangServerManagerDlg::OnBnClickedButton1()
{
	UninstallService(SERVICE_NAME);
	m_b_start.EnableWindow(true);
	m_b_stop.EnableWindow(false);
}
