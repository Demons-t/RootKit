// File.cpp: 实现文件
//

#include "pch.h"
#include "R3DemonsRootKit.h"
#include "File.h"
#include "afxdialogex.h"


// File 对话框

IMPLEMENT_DYNAMIC(File, CDialogEx)

File::File(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILE, pParent)
{

}

File::~File()
{
}

void File::InitListColumn()
{
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
	m_list.InsertColumn(0, _T("文件名"), LVCFMT_LEFT, 200);
	m_list.InsertColumn(1, _T("创建时间"), LVCFMT_LEFT, 160);
	m_list.InsertColumn(2, _T("修改时间"), LVCFMT_LEFT, 160);
	m_list.InsertColumn(3, _T("访问时间"), LVCFMT_LEFT, 160);
}


void File::InitTreeTab()
{
	WCHAR szName[MAX_PATH] = { 0 };
	GetLogicalDriveStrings(MAX_PATH, szName);
	WCHAR rootPath[10] = { 0 };
	WCHAR driveType[21] = { 0 };
	DWORD dwType = 0;
	for (char ch = 'A'; ch <= 'Z'; ch++)
	{
		wsprintf(rootPath, _T("%c:\\"), ch);
		dwType = GetDriveType(rootPath);
		if (dwType == DRIVE_FIXED)	// 硬盘
		{
			CString strBuff;
			strBuff.Format(_T("%c:"), ch);

			// 将所有系统盘符设置到树中
			HTREEITEM hItem = m_tree.InsertItem(strBuff, NULL);
			wchar_t* pBuff = _wcsdup(strBuff.GetBuffer());
			m_tree.SetItemData(hItem, (DWORD_PTR)pBuff);
		}
	}
}

DWORD WINAPI ThreadProcFile(_In_ LPVOID lpParameter)
{
	Sleep(300);
	HWND hWnd = (HWND)lpParameter;
	SendMessage(hWnd, WM_FILE, 0, 0);
	return 0;
}

void File::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE, m_list);
	DDX_Control(pDX, IDC_TREE1, m_tree);
}


BEGIN_MESSAGE_MAP(File, CDialogEx)
	ON_MESSAGE(WM_FILE, &File::EnumFile)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &File::ClickTree)
	ON_NOTIFY(NM_RCLICK, IDC_FILE, &File::RClickFile)
	ON_CONTROL(0, 0x1000, &File::FlushFile)
	ON_CONTROL(0, 0x1001, &File::DeleteFile)
END_MESSAGE_MAP()


// File 消息处理程序


void File::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	// CDialogEx::OnOK();
}


BOOL File::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL File::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	InitTreeTab();
	InitListColumn();
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcFile, this->m_hWnd, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

LRESULT File::EnumFile(WPARAM wParam, LPARAM lParam)
{
	m_list.DeleteAllItems();

	FILE_INFO fpi = { 0 };
	FILETIME time = { 0 };
	SYSTEMTIME sysTime = { 0 };
	DWORD dwSize = 0;
	const wchar_t* pStr = _T("\\??\\C:\\Windows");
	int nInputSize = wcslen(pStr) * 2 + 2;

	// 遍历文件
	DeviceIoControl(g_hDeviceHandle, FILE_FIRST, (LPVOID)pStr, nInputSize, &fpi, sizeof(fpi), &dwSize, NULL);

	CString strBuff;
	int nIndex = 0;

	if (dwSize == sizeof(fpi))
	{
		do
		{
			strBuff = fpi.cFileName;

			// 判断是否为空
			if (strBuff.IsEmpty())
			{
				continue;
			}

			m_list.InsertItem(nIndex, _T(""));

			// 文件名
			m_list.SetItemText(nIndex, 0, fpi.cFileName);

			// 创建时间
			FileTimeToLocalFileTime((PFILETIME)&fpi.ftCreationTime, &time);
			FileTimeToSystemTime(&time, &sysTime);
			strBuff.Format(_T("%4d-%02d-%02d %02d:%02d:%02d"), 
				sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
			m_list.SetItemText(nIndex, 1, strBuff);

			// 修改时间
			FileTimeToLocalFileTime((PFILETIME)&fpi.ftLastWriteTime, &time);
			FileTimeToSystemTime(&time, &sysTime);
			strBuff.Format(_T("%4d-%02d-%02d %02d:%02d:%02d"), 
				sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
			m_list.SetItemText(nIndex, 2, strBuff);

			// 访问时间
			FileTimeToLocalFileTime((PFILETIME)&fpi.ftLastAccessTime, &time);
			FileTimeToSystemTime(&time, &sysTime);
			strBuff.Format(_T("%4d-%02d-%02d %02d:%02d:%02d"), 
				sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
			m_list.SetItemText(nIndex, 3, strBuff);

			DeviceIoControl(g_hDeviceHandle,
				FILE_NEXT,
				(LPVOID)&fpi,		/*后续查找,传入查找句柄*/
				sizeof(FILE_INFO),
				&fpi,				/*保存输出的文件信息*/
				sizeof(FILE_INFO),
				&dwSize,
				NULL);

			if (dwSize != sizeof(fpi))
				break;

			++nIndex;

		} while (true);
	}
	
	return 0;
}

// 单击树控件获取目录
void File::ClickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码

	*pResult = 0;
}

// 单击鼠标右键
void File::RClickFile(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	
	CString Buffer = m_list.GetItemText(pNMItemActivate->iItem, 0);
	m_fileName = Buffer;

	//获取鼠标坐标
	CPoint pos;
	GetCursorPos(&pos);

	CMenu menu;

	// 1. 通过代码来创建弹出菜单
	menu.CreatePopupMenu();

	// 1.1 给弹出菜单添加菜单项
	menu.AppendMenu(MF_STRING, 0x1000, _T("刷新"));
	menu.AppendMenu(MF_STRING, 0x1001, _T("删除文件"));

	// 弹出整个菜单.
	menu.TrackPopupMenu(0, pos.x, pos.y, this);

	*pResult = 0;
}

// 刷新
void File::FlushFile()
{
	SendMessage(WM_FILE, 0, 0);
}

// 删除文件
void File::DeleteFile()
{
	// 拼接
	// 获取到列表名字
	CString LastPath;
	LastPath = _T("\\??\\C:\\Windows\\") + m_fileName;
	WCHAR wPath[256] = { 0 };
	memset(wPath, 0, 256);
	wcscpy_s(wPath, LastPath.GetLength() * 2, LastPath.GetBuffer());
	int len = wcslen(wPath) * 2 + 2;
	DWORD dwSize = 0;
	DeviceIoControl(g_hDeviceHandle, DELETE_FILE, wPath, len, NULL, NULL, &dwSize, NULL);
}
