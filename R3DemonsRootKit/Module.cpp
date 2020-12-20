// Module.cpp: 实现文件
//

#include "pch.h"
#include "R3DemonsRootKit.h"
#include "Module.h"
#include "afxdialogex.h"


// Module 对话框

IMPLEMENT_DYNAMIC(Module, CDialogEx)

Module::Module(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MODULE, pParent)
{

}

Module::~Module()
{
}

void Module::InitListColumn()
{
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
	m_list.InsertColumn(0, _T("序号"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(1, _T("基地址"), LVCFMT_LEFT, 200);
	m_list.InsertColumn(2, _T("大小"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(3, _T("模块路径"), LVCFMT_LEFT, 300);
}

DWORD WINAPI ThreadProcModule(_In_ LPVOID lpParameter)
{
	Sleep(300);
	HWND hWnd = (HWND)lpParameter;
	SendMessage(hWnd, WM_MODULE, 0, 0);
	return 0;
}


void Module::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_list);
}


BEGIN_MESSAGE_MAP(Module, CDialogEx)
	ON_MESSAGE(WM_MODULE, &Module::EnumModule)
END_MESSAGE_MAP()


// Module 消息处理程序


void Module::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	// CDialogEx::OnOK();
}


BOOL Module::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitListColumn();
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcModule, this->m_hWnd, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL Module::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

// 遍历模块
LRESULT Module::EnumModule(WPARAM wParam, LPARAM lParam)
{
	m_list.DeleteAllItems();

	PMODULE module;
	DWORD dwSize = 0;

	// 先出一次请求，返回需要的大小
	DeviceIoControl(g_hDeviceHandle, ENUM_MODULE, &m_dwPID, sizeof(DWORD), &module, sizeof(MODULE), &dwSize, NULL);

	// 根据返回的大小重新申请空间
	PMODULE pModule = new MODULE[dwSize]();
	DeviceIoControl(g_hDeviceHandle, ENUM_MODULE, &m_dwPID, dwSize, pModule, dwSize, &dwSize, NULL);

	// 所有项数
	int nCount = dwSize / sizeof(MODULE);
	int i = 0;
	int nIndex = 0;
	while (nCount)
	{
		--nCount;

		m_list.InsertItem(nIndex, _T(""));
		CString strTemp;
		strTemp.Format(_T("%d"), nIndex + 1);

		// 序号
		m_list.SetItemText(nIndex, 0, strTemp);

		// 基地址
		strTemp.Format(_T("0x%08X"), pModule[i].dwStartAddress);
		m_list.SetItemText(nIndex, 1, strTemp);

		// 大小
		strTemp.Format(_T("0x%08X"), pModule[i].dwSize);
		m_list.SetItemText(nIndex, 2, strTemp);

		// 路径
		m_list.SetItemText(nIndex, 3, pModule[i].wFullName);

		i++;
		nIndex++;

	}

	delete[] pModule;
	return 0;
}
