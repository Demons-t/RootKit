// ProcessAndThread.cpp: 实现文件
//

#include "pch.h"
#include "R3DemonsRootKit.h"
#include "Process.h"
#include "afxdialogex.h"


// ProcessAndThread 对话框

IMPLEMENT_DYNAMIC(Process, CDialogEx)

Process::Process(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROCESS, pParent)
{

}

Process::~Process()
{
}

// 初始化列
void Process::InitListColumn()
{
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
	m_list.InsertColumn(0, _T("序号"), LVCFMT_LEFT, 50);
	m_list.InsertColumn(1, _T("名称"), LVCFMT_LEFT, 200);
	m_list.InsertColumn(2, _T("进程ID"), LVCFMT_LEFT, 70);
	m_list.InsertColumn(3, _T("父进程ID"), LVCFMT_LEFT, 70);
	m_list.InsertColumn(4, _T("路径"), LVCFMT_LEFT, 500);
}

DWORD WINAPI ThreadProcProcess(_In_ LPVOID lpParameter)
{
	Sleep(300);
	HWND hWnd = (HWND)lpParameter;
	SendMessage(hWnd, WM_PROCESS, 0, 0);
	return 0;
}

void Process::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROCESS, m_list);
}


BEGIN_MESSAGE_MAP(Process, CDialogEx)
	ON_MESSAGE(WM_PROCESS, &Process::EnumProcess)
	ON_NOTIFY(NM_RCLICK, IDC_PROCESS, &Process::RClickProcess)
	ON_CONTROL(0, 0x1000, &Process::FlushProcess)
	ON_CONTROL(0, 0x1001, &Process::HideProcess)
	ON_CONTROL(0, 0x1002, &Process::TerminateProcess)
	ON_CONTROL(0, 0x1003, &Process::ShowThread)
	ON_CONTROL(0, 0x1004, &Process::ShowModule)
END_MESSAGE_MAP()


// ProcessAndThread 消息处理程序


void Process::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	// CDialogEx::OnOK();
}


BOOL Process::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL Process::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	InitListColumn();
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcProcess, this->m_hWnd, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// 遍历进程
LRESULT Process::EnumProcess(WPARAM wParam, LPARAM lParam)
{
	m_list.DeleteAllItems();

	// 遍历进程.
	PROCESS_INFO fpi = { 0 };
	DWORD dwSize = 0;

	// 先发出一次请求，返回需要的大小
	DeviceIoControl(g_hDeviceHandle,
		PROCESS_FIRST,
		0,
		0,
		&fpi,
		sizeof(PROCESS_INFO),
		&dwSize,
		NULL);

	// 根据返回的大小重新申请空间
	PPROCESS_INFO pProcess = new PROCESS_INFO[dwSize]();
	DeviceIoControl(g_hDeviceHandle,
		PROCESS_NEXT,
		pProcess,
		dwSize,
		pProcess,
		dwSize*sizeof(PROCESS_INFO),
		&dwSize,
		NULL);
	
	// 所有数量
	int nCount = dwSize / sizeof(PROCESS_INFO);
	int i = 0;
	int nIndex = 0;
	CString strBuff;
	CString strTemp;
	
	while (nCount)
	{
		--nCount;
		USES_CONVERSION;
		strBuff = A2W(pProcess[i].wName);

		// 判断是否为空
		if (strBuff.IsEmpty())
		{
			++i;
			continue;
		}
		m_list.InsertItem(nIndex, _T(""));
		
		CString strName{ pProcess[i].cFullDllName };

		// 序号
		strTemp.Format(_T("%d"), nIndex + 1);
		m_list.SetItemText(nIndex, 0, strTemp);

		// 名称
		PathStripPath(strName.GetBuffer());
		m_list.SetItemText(nIndex, 1, strName.GetBuffer());

		// ID
		strTemp.Format(_T("%d"), pProcess[i].uPid);
		m_list.SetItemText(nIndex, 2, strTemp);

		// 父进程ID
		strTemp.Format(_T("%d"), pProcess[i].uPPid);
		m_list.SetItemText(nIndex, 3, strTemp);

		// 路径
		strTemp.Format(_T("%wS"), pProcess[i].cFullDllName);

		// 判断名称是否为 System
		if (strBuff == _T("System"))
			m_list.SetItemText(nIndex, 4, _T("System"));
		else
			m_list.SetItemText(nIndex, 4, strTemp);
		++i;
		++nIndex;
	}

	delete[] pProcess;
	return 0;
}

// 单机鼠标右键
void Process::RClickProcess(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	CString Buffer = m_list.GetItemText(pNMItemActivate->iItem, 2);
	m_dwPID = _wtoi(Buffer.GetBuffer());

	//获取鼠标坐标
	CPoint pos;
	GetCursorPos(&pos);

	CMenu menu;

	// 1. 通过代码来创建弹出菜单
	menu.CreatePopupMenu();

	// 1.1 给弹出菜单添加菜单项
	menu.AppendMenu(MF_STRING, 0x1000, _T("刷新"));
	menu.AppendMenu(MF_STRING, 0x1001, _T("隐藏进程"));
	menu.AppendMenu(MF_STRING, 0x1002, _T("结束进程"));
	menu.AppendMenu(MF_STRING, 0x1003, _T("查看线程"));
	menu.AppendMenu(MF_STRING, 0x1004, _T("查看模块"));

	// 弹出整个菜单.
	menu.TrackPopupMenu(0, pos.x, pos.y, this);

	*pResult = 0;
}

// 刷新进程
void Process::FlushProcess()
{
	SendMessage(WM_PROCESS, 0, 0);
}

// 隐藏进程
void Process::HideProcess()
{
	DWORD dwSize = 0;
	DeviceIoControl(g_hDeviceHandle, HIDE_PROCESS, &m_dwPID, sizeof(DWORD), NULL, NULL, &dwSize, NULL);
}

// 结束进程
void Process::TerminateProcess()
{
	DWORD dwSize = 0;
	DeviceIoControl(g_hDeviceHandle, TERMINATE_PROCESS, &m_dwPID, sizeof(DWORD), NULL, NULL, &dwSize, NULL);
}

// 查看线程
void Process::ShowThread()
{
	Thread obj;
	obj.m_dwPID = m_dwPID;

	obj.DoModal();
}

// 查看模块
void Process::ShowModule()
{
	Module obj;
	obj.m_dwPID = m_dwPID;

	obj.DoModal();
}
