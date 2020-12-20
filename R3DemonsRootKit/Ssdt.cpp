// Ssdt.cpp: 实现文件
//

#include "pch.h"
#include "R3DemonsRootKit.h"
#include "Ssdt.h"
#include "afxdialogex.h"


// Ssdt 对话框

IMPLEMENT_DYNAMIC(Ssdt, CDialogEx)

Ssdt::Ssdt(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SSDT, pParent)
{

}

Ssdt::~Ssdt()
{
}

void Ssdt::InitListColumn()
{
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
	m_list.InsertColumn(0, _T("系统调用号"), LVCFMT_LEFT, 200);
	m_list.InsertColumn(1, _T("地址"), LVCFMT_LEFT, 200);
}

DWORD WINAPI ThreadProcSsdt(_In_ LPVOID lpParameter)
{
	Sleep(300);
	HWND hWnd = (HWND)lpParameter;
	SendMessage(hWnd, WM_SSDT, 0, 0);
	return 0;
}

void Ssdt::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SSDT, m_list);
}


BEGIN_MESSAGE_MAP(Ssdt, CDialogEx)
	ON_MESSAGE(WM_SSDT, &Ssdt::EnumSsdt)
	ON_NOTIFY(NM_RCLICK, IDC_SSDT, &Ssdt::RClickSsdt)
	ON_CONTROL(0, 0x1000, &Ssdt::FlushSsdt)
END_MESSAGE_MAP()


// Ssdt 消息处理程序


void Ssdt::RClickSsdt(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	//获取鼠标坐标
	CPoint pos;
	GetCursorPos(&pos);

	CMenu menu;

	// 1. 通过代码来创建弹出菜单
	menu.CreatePopupMenu();

	// 1.1 给弹出菜单添加菜单项
	menu.AppendMenu(MF_STRING, 0x1000, _T("刷新"));

	// 弹出整个菜单.
	menu.TrackPopupMenu(0, pos.x, pos.y, this);

	*pResult = 0;
}


void Ssdt::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	// CDialogEx::OnOK();
}


BOOL Ssdt::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL Ssdt::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	InitListColumn();
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcSsdt, this->m_hWnd, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// 遍历 ssdt
LRESULT Ssdt::EnumSsdt(WPARAM wParam, LPARAM lParam)
{
	m_list.DeleteAllItems();
	SSDT ssdt;
	DWORD dwSize = 0;
	DeviceIoControl(g_hDeviceHandle, ENUM_SSDT, NULL, NULL, &ssdt, sizeof(SSDT), &dwSize, NULL);

	int nNum = dwSize / sizeof(SSDT);
	PSSDT pSsdt = new SSDT[nNum]();
	DeviceIoControl(g_hDeviceHandle, ENUM_SSDT, NULL, NULL, pSsdt, dwSize, &dwSize, NULL);

	CString strBuff;

	for (int i = 0; i < nNum; i++)
	{
		// 系统调用号
		m_list.InsertItem(i, _T(""));
		strBuff.Format(_T("0x%X"), pSsdt[i].uSysCallIndex);
		m_list.SetItemText(i, 0, strBuff);

		// 地址
		strBuff.Format(_T("0x%08X"), pSsdt[i].uAddress);
		m_list.SetItemText(i, 1, strBuff);
	}

	delete[] pSsdt;
	return 0;
}

// 刷新
void Ssdt::FlushSsdt()
{
	SendMessage(WM_SSDT, 0, 0);
}
