// Idt.cpp: 实现文件
//

#include "pch.h"
#include "R3DemonsRootKit.h"
#include "Idt.h"
#include "afxdialogex.h"


// Idt 对话框

IMPLEMENT_DYNAMIC(Idt, CDialogEx)

Idt::Idt(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IDT, pParent)
{

}

Idt::~Idt()
{
}

void Idt::InitListColumn()
{
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
	m_list.InsertColumn(0, _T("中断地址"), LVCFMT_LEFT, 200);
	m_list.InsertColumn(1, _T("中断号"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(2, _T("段选择子"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(3, _T("类型"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(4, _T("特权等级"), LVCFMT_LEFT, 100);
}

DWORD WINAPI ThreadProcIdt(_In_ LPVOID lpParameter)
{
	Sleep(300);
	HWND hWnd = (HWND)lpParameter;
	SendMessage(hWnd, WM_IDT, 0, 0);
	return 0;
}

void Idt::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IDT, m_list);
}


BEGIN_MESSAGE_MAP(Idt, CDialogEx)
	ON_MESSAGE(WM_IDT, &Idt::EnumIdt)
	ON_NOTIFY(NM_RCLICK, IDC_IDT, &Idt::RClickIdt)
	ON_CONTROL(0, 0x1000, &Idt::FlushIdt)
END_MESSAGE_MAP()


// Idt 消息处理程序


void Idt::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	// CDialogEx::OnOK();
}


BOOL Idt::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL Idt::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitListColumn();
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcIdt, this->m_hWnd, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// 遍历 IDT
LRESULT Idt::EnumIdt(WPARAM wParam, LPARAM lParam)
{
	m_list.DeleteAllItems();
	PIDT_ENTRY pIdt = new IDT_ENTRY[0x100]();
	DWORD dwSize = 0;
	DeviceIoControl(g_hDeviceHandle, ENUM_IDT, NULL, NULL, pIdt, 0x100 * sizeof(IDT_ENTRY), &dwSize, NULL);

	CString strTemp;

	for (int i = 0; i < 0x100; i++)
	{
		m_list.InsertItem(i, _T(""));
		
		// 中断地址
		ULONG IdtAddress = MAKELONG(pIdt[i].uOffsetLow, pIdt[i].uOffsetHigh);
		strTemp.Format(_T("0x%08X"), IdtAddress);
		m_list.SetItemText(i, 0, strTemp);

		// 中断号
		strTemp.Format(_T("%d"), i);
		m_list.SetItemText(i, 1, strTemp);

		// 段选择子
		strTemp.Format(_T("%d"), pIdt[i].uSelector);
		m_list.SetItemText(i, 2, strTemp);

		// 类型
		strTemp.Format(_T("%d"), pIdt[i].GateType);
		m_list.SetItemText(i, 3, strTemp);

		// 特权级
		strTemp.Format(_T("%d"), pIdt[i].DPL);
		m_list.SetItemText(i, 4, strTemp);
	}

	delete[] pIdt;
	return 0;
}

// 单机右键
void Idt::RClickIdt(NMHDR* pNMHDR, LRESULT* pResult)
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

// 刷新
void Idt::FlushIdt()
{
	SendMessage(WM_IDT, 0, 0);
}
