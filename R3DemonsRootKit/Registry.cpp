// Register.cpp: 实现文件
//

#include "pch.h"
#include "R3DemonsRootKit.h"
#include "Registry.h"
#include "afxdialogex.h"


// Register 对话框

IMPLEMENT_DYNAMIC(Registry, CDialogEx)

Registry::Registry(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REGISTRY, pParent)
{

}

Registry::~Registry()
{
}

void Registry::InitListColumn()
{
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
	m_list.InsertColumn(0, _T("名称"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(1, _T("类型"), LVCFMT_LEFT, 150);
	m_list.InsertColumn(2, _T("数据"), LVCFMT_LEFT, 150);
}

void Registry::InitTreeTab()
{
	WCHAR buffer[256] = {};
	memset(buffer, 0, 256);
	CString strRoot = _T("\\Registry");
	memcpy(buffer, strRoot, strRoot.GetLength() * 2);
	DWORD dwSize = 0;
	DeviceIoControl(g_hDeviceHandle, ENUM_REGISTRY, buffer, wcslen(strRoot.GetBuffer()) * 2 + 2, NULL, NULL, &dwSize, NULL);
	int nCount = dwSize / sizeof(REGISTRY);
	PREGISTRY pReg = new REGISTRY[nCount]();
	DeviceIoControl(g_hDeviceHandle, ENUM_REGISTRY, buffer, wcslen(strRoot.GetBuffer()) * 2 + 2, pReg, dwSize, &dwSize, NULL);

	for (int i = 0; i < nCount; i++)
	{
		// 根据 TYPE 来给控件分配
		// 子项
		if (pReg[i].uType == 0)
		{
			CString strBuff = pReg[i].wKeyName;
			CString strPath;

			if (strBuff == _T("MACHINE"))
			{
				strPath = _T("\\Registry\\Machine");
				// strBuff = _T("\\HKEY_LOCAL_MACHINE");
				strBuff.Format(_T("%s"), _T("HKEY_LOCAL_MAXHINE"));
			}
			else if (strBuff == _T("USER"))
			{
				strPath = _T("\\Registry\\user");
				// strBuff = _T("HKEY_USERS");
				strBuff.Format(_T("%s"), _T("HKEY_USERS"));
			}

			// 将所有系统盘符设置到树控件里
			HTREEITEM hItem = m_tree.InsertItem(strBuff, NULL);
			wchar_t* pBuff = _wcsdup(strPath.GetBuffer());
			m_tree.SetItemData(hItem, (DWORD_PTR)pBuff);
		}
	}

	delete[] pReg;
}

DWORD WINAPI ThreadProcRegistry(_In_ LPVOID lpParameter)
{
	Sleep(300);
	HWND hWnd = (HWND)lpParameter;
	SendMessage(hWnd, WM_REGISTER, 0, 0);
	return 0;
}

void Registry::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REGISTRY, m_list);
	DDX_Control(pDX, IDC_TREE1, m_tree);
}


BEGIN_MESSAGE_MAP(Registry, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &Registry::ClickTree)
	ON_NOTIFY(NM_RCLICK, IDC_TREE1, &Registry::RClickTree)
	ON_CONTROL(0, 0x1000, &Registry::FlushRegistry)
	ON_CONTROL(0, 0x1001, &Registry::CreateReg)
	ON_CONTROL(0, 0x1002, &Registry::DeleteReg)
END_MESSAGE_MAP()


// Register 消息处理程序



void Registry::ClickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码

	DWORD dwChild = 0;
	CPoint pos = {};
	GetCursorPos(&pos);
	ScreenToClient(&pos);
	HTREEITEM hItem = m_tree.HitTest(pos);

	// 判断是否有值
	if (!hItem)
		return;

	// 获取选中的树控件的项的数据
	CString strPath = (wchar_t*)m_tree.GetItemData(hItem);
	if (strPath.IsEmpty() || strPath == _T("\\Registry"))
		return;

	HTREEITEM hChild = m_tree.GetChildItem(hItem);
	HTREEITEM hTemp;

	while (hChild)
	{
		hTemp = hChild;
		hChild = m_tree.GetNextSiblingItem(hChild);	// 得到下一个子项
		m_tree.DeleteItem(hTemp);
	}

	m_list.DeleteAllItems();

	REGISTRY reg;
	DWORD dwSize = 0;

	// 先发出一次请求，返回需要的大小
	WCHAR wPath[256] = { 0 };
	memset(wPath, 0, 256);
	CString strLastPath;
	strLastPath = strPath;
	wcscpy_s(wPath, strLastPath.GetLength() * 2, strLastPath.GetBuffer());
	int nLen = wcslen(wPath) * 2 + 2;
	DeviceIoControl(g_hDeviceHandle, ENUM_REGISTRY, wPath, nLen, &reg, sizeof(REGISTRY), &dwSize, NULL);
	PREGISTRY pReg = new REGISTRY[dwSize]();

	// 发送盘符
	DeviceIoControl(g_hDeviceHandle, ENUM_REGISTRY, wPath, nLen, pReg, dwSize, &dwSize, NULL);
	int nNum = dwSize / sizeof(REGISTRY);
	int nIndex = 0;
	for (int i = 0; i < nNum; i++)
	{
		// 判断类型
		// 子项
		if (pReg[i].uType == 0 && !dwChild)
		{
			CString strBuff = pReg[i].wKeyName;
			HTREEITEM hItem2 = m_tree.InsertItem(strBuff, hItem);
			WCHAR* szFullPath = new WCHAR[MAX_PATH];
			
			// 拼接
			StringCbPrintf(szFullPath, MAX_PATH, _T("%s\\%s"), strPath, strBuff);
			wchar_t* pBuff = _wcsdup(szFullPath);
			m_tree.SetItemData(hItem2, (DWORD_PTR)szFullPath);
		}
		// 键
		else if (pReg[i].uType == 1)
		{
			CString strBuff;
			CString buff = pReg[i].wValueName;
			m_list.InsertItem(nIndex, _T(""));

			// 值名
			if (buff == _T(""))
			{
				buff = _T("默认");
			}

			m_list.SetItemText(nIndex, 0, buff);	// 文件名

			if (pReg[i].dwValueType == REG_SZ)		// 以 NULL 结尾的字符串
			{
				buff = _T("REG_SZ");
				strBuff.Format(_T("%s"), pReg[i].uValue);
			}
			else if (pReg[i].dwValueType == REG_MULTI_SZ)
			{
				buff = _T("REG_MULTI_SZ");
				strBuff.Format(_T("%s"), pReg[i].uValue);
			}
			else if (pReg[i].dwValueType == REG_DWORD)
			{
				buff = _T("REG_DWORD");
				strBuff.Format(_T("0x%08X"), pReg[i].uValue);
			}
			else if (pReg[i].dwValueType == REG_BINARY)		// 二进制数据
			{
				buff = _T("REG_BINARY");
				int nSize = strlen((char*)pReg[i].uValue);
				strBuff = _T("");
				CString strTemp;
				for (int j = 0; j < nSize; j++)
				{
					strBuff += _T("");
					strTemp.Format(_T("%02X"), (unsigned char)pReg[i].uValue[j]);
					strBuff += strTemp;
				}
			}
			m_list.SetItemText(nIndex, 1, buff);	// 文件名
			m_list.SetItemText(nIndex, 2, strBuff);	// 数据
			nIndex++;
		}
	}

	*pResult = 0;
}


void Registry::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	// CDialogEx::OnOK();
}


BOOL Registry::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL Registry::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	InitListColumn();
	InitTreeTab();
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcRegistry, this->m_hWnd, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void Registry::RClickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码

	CPoint pos = {};
	GetCursorPos(&pos);
	ScreenToClient(&pos);
	HTREEITEM hItem = m_tree.HitTest(pos);

	// 判断是否有值
	if (!hItem)
		return;

	CString strPath = (wchar_t*)m_tree.GetItemData(hItem);
	if (strPath.IsEmpty())
		return;

	m_strFileName = strPath;

	GetCursorPos(&pos);

	CMenu menu;

	// 1. 通过代码来创建弹出菜单
	menu.CreatePopupMenu();

	// 1.1 给弹出菜单添加菜单项
	menu.AppendMenu(MF_STRING, 0x1000, _T("刷新"));
	menu.AppendMenu(MF_STRING, 0x1001, _T("创建子项"));
	menu.AppendMenu(MF_STRING, 0x1002, _T("删除子项")); 

	// 弹出整个菜单.
	menu.TrackPopupMenu(0, pos.x, pos.y, this);

	*pResult = 0;
}

void Registry::DeleteReg()
{
	WCHAR wPath[256] = { 0 };
	CString strLastPath;
	strLastPath = m_strFileName + _T("\\");
	wcscpy_s(wPath, strLastPath.GetLength() * 2, strLastPath.GetBuffer());
	int nLen = wcslen(wPath) * 2 + 2;
	DWORD dwSize = 0;
	
	DeviceIoControl(g_hDeviceHandle, DELETE_REGISTRY_KEY, wPath, nLen, NULL, NULL, &dwSize, NULL);
}

void Registry::CreateReg()
{
	AddKey addKey(m_strFileName);

	addKey.DoModal();
}

void Registry::FlushRegistry()
{
	SendMessage(WM_REGISTER, 0, 0);
}
