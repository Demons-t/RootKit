
// R3DemonsRootKitDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "R3DemonsRootKit.h"
#include "R3DemonsRootKitDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CR3DemonsRootKitDlg 对话框



CR3DemonsRootKitDlg::CR3DemonsRootKitDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_R3DEMONSROOTKIT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

// 初始化 tab 页
void CR3DemonsRootKitDlg::InitTab()
{
	//为Tab Control增加两个页面
	m_tab.InsertItem(0, _T("进程"));
	m_tab.InsertItem(1, _T("驱动"));
	m_tab.InsertItem(2, _T("文件"));
	m_tab.InsertItem(3, _T("注册表"));
	m_tab.InsertItem(4, _T("IDT"));
	m_tab.InsertItem(5, _T("GDT"));
	m_tab.InsertItem(6, _T("SSDT"));
	
	//创建两个对话框
	m_process.Create(IDD_PROCESS, &m_tab);
	m_driver.Create(IDD_DRIVER, &m_tab);
	m_file.Create(IDD_FILE, &m_tab);
	m_registry.Create(IDD_REGISTRY, &m_tab);
	m_idt.Create(IDD_IDT, &m_tab);
	m_gdt.Create(IDD_GDT, &m_tab);
	m_ssdt.Create(IDD_SSDT, &m_tab);

	//设定在Tab内显示的范围
	CRect rc;
	m_tab.GetClientRect(rc);
	rc.top += 20;
	rc.bottom -= 0;
	rc.left += 0;
	rc.right -= 0;
	m_process.MoveWindow(&rc);
	m_driver.MoveWindow(&rc);
	m_file.MoveWindow(&rc);
	m_registry.MoveWindow(&rc);
	m_idt.MoveWindow(&rc);
	m_gdt.MoveWindow(&rc);
	m_ssdt.MoveWindow(&rc);

	//把对话框对象指针保存起来
	m_pDialog[0] = &m_process;
	m_pDialog[1] = &m_driver;
	m_pDialog[2] = &m_file;
	m_pDialog[3] = &m_registry;
	m_pDialog[4] = &m_idt;
	m_pDialog[5] = &m_gdt;
	m_pDialog[6] = &m_ssdt;

	//显示初始页面
	m_pDialog[0]->ShowWindow(SW_SHOW);
	m_pDialog[1]->ShowWindow(SW_HIDE);
	m_pDialog[2]->ShowWindow(SW_HIDE);
	m_pDialog[3]->ShowWindow(SW_HIDE);
	m_pDialog[4]->ShowWindow(SW_HIDE);
	m_pDialog[5]->ShowWindow(SW_HIDE);
	m_pDialog[6]->ShowWindow(SW_HIDE);

	//保存当前选择
	m_nCurSelTab = 0;
}

void CR3DemonsRootKitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
}

BEGIN_MESSAGE_MAP(CR3DemonsRootKitDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CR3DemonsRootKitDlg::ChangeTab)
END_MESSAGE_MAP()


// CR3DemonsRootKitDlg 消息处理程序

BOOL CR3DemonsRootKitDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	// 加载驱动
	LoadDriver();

	// 使用 CreateFile 打开一个设备对象，要求管理员权限
	g_hDeviceHandle = CreateFile(
		L"\\??\\augen", GENERIC_ALL, NULL, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (g_hDeviceHandle == INVALID_HANDLE_VALUE)
	{
		CString Buffer;
		Buffer.Format(L"[3环程序]打开设备失败\n");
		MessageBox(Buffer);
		ExitProcess(0);
	}

	// 发送PID
	int nPID = _getpid();
	DWORD dwSize = 0;
	DeviceIoControl(g_hDeviceHandle, GETPID, &nPID, sizeof(int), NULL, NULL, &dwSize, NULL);

	InitTab();

	//CloseHandle(g_hDeviceHandle);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CR3DemonsRootKitDlg::OnPaint()
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
HCURSOR CR3DemonsRootKitDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CR3DemonsRootKitDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnOK();
}

// 禁用回车和esc
BOOL CR3DemonsRootKitDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

// 切换 tab 页 
void CR3DemonsRootKitDlg::ChangeTab(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码

	//把当前的页面隐藏起来
	m_pDialog[m_nCurSelTab]->ShowWindow(SW_HIDE);
	//得到新的页面索引
	m_nCurSelTab = m_tab.GetCurSel();
	//把新的页面显示出来
	m_pDialog[m_nCurSelTab]->ShowWindow(SW_SHOW);

	*pResult = 0;
}

// 加载服务
void CR3DemonsRootKitDlg::LoadDriver()
{
	//1.打开服务管理器
	auto hServiceMgr{ OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS) };

	//获取当前程序所在路径
	WCHAR pszFileName[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, pszFileName, MAX_PATH);
	//获取当前程序所在目录
	(wcsrchr(pszFileName, '\\'))[0] = 0;
	//拼接驱动文件路径
	WCHAR pszDriverName[MAX_PATH] = { 0 };
	swprintf_s(pszDriverName, L"%s\\%s", pszFileName, L"DemonsRootKit.sys");

	//2.创建服务
	m_hSCManager = CreateService(
		hServiceMgr,    //SMC句柄
		L"MyService",    //驱动服务名称
		L"MyDriver",    //驱动服务显示名称
		SERVICE_ALL_ACCESS,    //权限（所有访问权限）
		SERVICE_KERNEL_DRIVER,  //服务类型（驱动程序）
		SERVICE_DEMAND_START,  //启动方式
		SERVICE_ERROR_IGNORE,  //错误控制
		pszDriverName,    //驱动文件路径
		NULL,  //加载组命令
		NULL,  //TagId(指向一个加载顺序的标签值)
		NULL,  //依存关系
		NULL,  //服务启动名
		NULL  //密码
	);
	//2.1判断服务是否存在
	if (GetLastError() == ERROR_SERVICE_EXISTS) {
		//如果服务存在，只要打开
		m_hSCManager = OpenService(hServiceMgr, L"MyService", SERVICE_ALL_ACCESS);
	}

	//3.启动服务
	StartService(m_hSCManager, NULL, NULL);
}


BOOL CR3DemonsRootKitDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类

	//关闭设备
	CloseHandle(g_hDeviceHandle);
	SERVICE_STATUS status = { 0 };
	ControlService(m_hSCManager, SERVICE_CONTROL_STOP, &status);
	//删除服务
	DeleteService(m_hSCManager);
	//关闭服务句柄
	CloseServiceHandle(m_hSCManager);

	return CDialogEx::DestroyWindow();
}
