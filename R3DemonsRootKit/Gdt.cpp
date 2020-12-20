// Gdt.cpp: 实现文件
//

#include "pch.h"
#include "R3DemonsRootKit.h"
#include "Gdt.h"
#include "afxdialogex.h"


// Gdt 对话框

IMPLEMENT_DYNAMIC(Gdt, CDialogEx)

Gdt::Gdt(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GDT, pParent)
{

}

Gdt::~Gdt()
{
}

void Gdt::InitListColumn()
{
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
	m_list.InsertColumn(0, _T("基址"), LVCFMT_LEFT, 200);
	m_list.InsertColumn(1, _T("界限"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(2, _T("段粒度"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(3, _T("段特权"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(4, _T("类型"), LVCFMT_LEFT, 100);
}

DWORD WINAPI ThreadProcGdt(_In_ LPVOID lpParameter)
{
	Sleep(300);
	HWND hWnd = (HWND)lpParameter;
	SendMessage(hWnd, WM_GDT, 0, 0);
	return 0;
}

void Gdt::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GDT, m_list);
}


BEGIN_MESSAGE_MAP(Gdt, CDialogEx)
	ON_MESSAGE(WM_GDT, &Gdt::EnumGdt)
	ON_NOTIFY(NM_RCLICK, IDC_GDT, &Gdt::RClickGdt)
	ON_CONTROL(0, 0x1000, &Gdt::FlushGdt)
END_MESSAGE_MAP()


// Gdt 消息处理程序


void Gdt::RClickGdt(NMHDR* pNMHDR, LRESULT* pResult)
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


void Gdt::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	// CDialogEx::OnOK();
}


BOOL Gdt::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL Gdt::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	InitListColumn();
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcGdt, this->m_hWnd, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// 遍历 GDT
LRESULT Gdt::EnumGdt(WPARAM wParam, LPARAM lParam)
{
	m_list.DeleteAllItems();
	Gdt gdt;
	DWORD dwSize = 0;

	DeviceIoControl(g_hDeviceHandle, ENUM_GDT, NULL, NULL, &gdt, sizeof(GDT), &dwSize, NULL);

	int nCount = dwSize / sizeof(GDT);
	PGDT pGdt = new GDT[nCount]();
	
	DeviceIoControl(g_hDeviceHandle, ENUM_GDT, NULL, NULL, pGdt, dwSize, &dwSize, NULL);
	int i = 0;

	CString strBuff;

	while (nCount)
	{
		--nCount;
		m_list.InsertItem(i, _T(""));

		// 基址
		UINT64 base = (pGdt[i].Base24_31 << 24) + pGdt[i].base0_23;
		strBuff.Format(_T("0x%08X"), base);
		m_list.SetItemText(i, 0, strBuff);

		// 限长
		UINT64 limit = (pGdt[i].Limit16_19 << 16) + pGdt[i].Limit0_15;
		strBuff.Format(_T("0x%08X"), limit);
		m_list.SetItemText(i, 1, strBuff);

		// 粒度
		if (pGdt[i].G == 0)
			strBuff = _T("Byte");
		else if (pGdt[i].G == 1)
			strBuff = _T("Page");
		m_list.SetItemText(i, 2, strBuff);

		// DPL
		strBuff.Format(_T("%d"), pGdt[i].DPL);
		m_list.SetItemText(i, 3, strBuff);

		// 判断类型
		if (pGdt[i].S == 0)	// 系统段
		{
			switch (pGdt[i].TYPE)
			{
			case 12:
				strBuff = _T("调用门");
			case 14:
				strBuff = _T("中断门");
			case 15:
				strBuff = _T("陷阱门");
			case 5:
				strBuff = _T("任务门");
			default:
				strBuff = _T("系统段");
				break;
			}
		}
		else if (pGdt[i].S == 1)		// 数据段或代码段
		{
			if (pGdt[i].TYPE >= 8)
				strBuff = _T("代码段");
			else if (pGdt[i].TYPE < 8)
				strBuff = _T("数据段");
		}

		// 类型
		m_list.SetItemText(i, 4, strBuff);
		++i;
	}

	delete[] pGdt;
	return 0;
}

// 刷新
void Gdt::FlushGdt()
{
	SendMessage(WM_GDT, 0, 0);
}
