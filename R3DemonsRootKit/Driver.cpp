// Driver.cpp: 实现文件
//

#include "pch.h"
#include "R3DemonsRootKit.h"
#include "Driver.h"
#include "afxdialogex.h"


// Driver 对话框

IMPLEMENT_DYNAMIC(Driver, CDialogEx)

Driver::Driver(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DRIVER, pParent)
{

}

Driver::~Driver()
{
}

void Driver::InitListColumn()
{
	m_listDriver.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
	m_listDriver.InsertColumn(0, _T("序号"), LVCFMT_LEFT, 100);
	m_listDriver.InsertColumn(1, _T("驱动名"), LVCFMT_LEFT, 150);
	m_listDriver.InsertColumn(2, _T("基址"), LVCFMT_LEFT, 150);
	m_listDriver.InsertColumn(3, _T("大小"), LVCFMT_LEFT, 150);
	m_listDriver.InsertColumn(4, _T("路径"), LVCFMT_LEFT, 360);
}

DWORD WINAPI ThreadProcDriver(_In_ LPVOID lpParameter)
{
	Sleep(300);
	HWND hWnd = (HWND)lpParameter;
	SendMessage(hWnd, WM_DRIVER, 0, 0);
	return 0;
}

void Driver::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Driver, m_listDriver);
}


BEGIN_MESSAGE_MAP(Driver, CDialogEx)
	ON_MESSAGE(WM_DRIVER, &Driver::EnumDriver)
	ON_NOTIFY(NM_RCLICK, IDC_Driver, &Driver::RClickDriver)
	ON_CONTROL(0, 0x1000, &Driver::FlushDriver)
	ON_CONTROL(0, 0x1001, &Driver::HideDriver)
END_MESSAGE_MAP()


// Driver 消息处理程序


void Driver::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnOK();
}

BOOL Driver::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL Driver::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	InitListColumn();
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcDriver, this->m_hWnd, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// 遍历驱动
LRESULT Driver::EnumDriver(WPARAM wParam, LPARAM lParam)
{
	m_listDriver.DeleteAllItems();
	DRIVER dTemp;
	DWORD dwSize = 0;
	
	// 先发出一次请求，返回需要的大小
	DeviceIoControl(g_hDeviceHandle, ENUM_DRIVER, NULL, NULL, &dTemp, sizeof(DRIVER), &dwSize, NULL);
	
	// 根据返回的大小重新申请空间
	PDRIVER pDriver = new DRIVER[dwSize]();
	DeviceIoControl(g_hDeviceHandle, ENUM_DRIVER, pDriver, dwSize, pDriver, dwSize, &dwSize, NULL);
	
	// 所有数量
	int nCount = dwSize / sizeof(DRIVER);
	int i = 0;
	int nIndex = 0;
	CString strBuff;
	CString strTemp;

	while (nCount)
	{
		--nCount;
		strBuff = pDriver[i].cName;
	
		// 判断是否为空
		if (strBuff.IsEmpty())
		{
			++i;
			continue;
		}
		
		m_listDriver.InsertItem(nIndex, _T(""));

		// 序号
		strTemp.Format(_T("%d"), nIndex + 1);
		m_listDriver.SetItemText(nIndex, 0, strTemp);

		// 驱动名
		m_listDriver.SetItemText(nIndex, 1, strBuff);

		// 基址
		strTemp.Format(_T("0x%08X"), pDriver[i].dwDllBase);
		m_listDriver.SetItemText(nIndex, 2, strTemp);

		// 大小
		strTemp.Format(_T("0x%08X"), pDriver[i].dwSize);
		m_listDriver.SetItemText(nIndex, 3, strTemp);

		// 路径
		strTemp.Format(_T("%s"), pDriver[i].cFullDllName);
		m_listDriver.SetItemText(nIndex, 4, strTemp);

		++i;
		++nIndex;
	}
	
	delete[] pDriver;
	return 0;
}

// 单机鼠标右键
void Driver::RClickDriver(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	CString Buffer = m_listDriver.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem);
	wcscpy_s(m_wcDriverName, MAX_PATH, Buffer.GetBuffer());

	//获取鼠标坐标
	CPoint pos;
	GetCursorPos(&pos);

	CMenu menu;

	// 1. 通过代码来创建弹出菜单
	menu.CreatePopupMenu();

	// 1.1 给弹出菜单添加菜单项
	menu.AppendMenu(MF_STRING, 0x1000, _T("刷新"));
	menu.AppendMenu(MF_STRING, 0x1001, _T("隐藏驱动"));

	// 弹出整个菜单.
	menu.TrackPopupMenu(0, pos.x, pos.y, this);

	*pResult = 0;
}

// 刷新
void Driver::FlushDriver()
{
	SendMessage(WM_DRIVER, 0, 0);
}

// 隐藏驱动
void Driver::HideDriver()
{
	DWORD dwSize = 0;
	DeviceIoControl(g_hDeviceHandle, HIDE_DRIVER, m_wcDriverName, MAX_PATH, NULL, NULL, &dwSize, NULL);
}
