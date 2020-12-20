// Thread.cpp: 实现文件
//

#include "pch.h"
#include "R3DemonsRootKit.h"
#include "Thread.h"
#include "afxdialogex.h"


// Thread 对话框

IMPLEMENT_DYNAMIC(Thread, CDialogEx)

Thread::Thread(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_THREAD, pParent)
{

}

Thread::~Thread()
{
}

// 初始化列
void Thread::InitListColumn()
{
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
	m_list.InsertColumn(0, _T("序号"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(1, _T("TID"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(2, _T("优先级"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(3, _T("线程起始地址"), LVCFMT_LEFT, 150);
	m_list.InsertColumn(4, _T("状态"), LVCFMT_LEFT, 100);
}

WCHAR* NumToStatus(int nNum)
{
	WCHAR* Buffer = _T("NULL");
	switch (nNum)
	{
	case 0:
		Buffer = _T("已初始化");
		break;
	case 1:
		Buffer = _T("准备");
		break;
	case 2:
		Buffer = _T("运行中");
		break;
	case 3:
		Buffer = _T("Standby");
		break;
	case 4:
		Buffer = _T("终止");
		break;
	case 5:
		Buffer = _T("等待");
		break;
	case 6:
		Buffer = _T("Transition");
		break;
	case 7:
		Buffer = _T("DeferredReady");
		break;
	case 8:
		Buffer = _T("GateWait");
		break;
	default:
		break;
	}

	return Buffer;
}


DWORD WINAPI ThreadProcThread(_In_ LPVOID lpParameter)
{
	Sleep(300);
	HWND hWnd = (HWND)lpParameter;
	SendMessage(hWnd, WM_THREAD, 0, 0);
	return 0;
}

void Thread::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_THREAD, m_list);
}


BEGIN_MESSAGE_MAP(Thread, CDialogEx)
	ON_MESSAGE(WM_THREAD, &Thread::EnumThread)
END_MESSAGE_MAP()


// Thread 消息处理程序


BOOL Thread::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	InitListColumn(); 
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcThread, this->m_hWnd, 0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void Thread::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	// CDialogEx::OnOK();
}


BOOL Thread::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

// 遍历线程
LRESULT Thread::EnumThread(WPARAM wParam, LPARAM lParam)
{
	m_list.DeleteAllItems();
	PTHREAD_INFO thread;
	DWORD dwSize = 0;

	DeviceIoControl(g_hDeviceHandle, ENUM_THREAD, &m_dwPID, sizeof(DWORD), &thread, sizeof(THREAD_INFO), &dwSize, NULL);

	// 根据返回来的大小重新申请空间
	PTHREAD_INFO pThread = new THREAD_INFO[dwSize]();
	DeviceIoControl(g_hDeviceHandle, ENUM_THREAD, &m_dwPID, dwSize, pThread, dwSize, &dwSize, NULL);

	int nCOunt = dwSize / sizeof(THREAD_INFO);
	int i = 0;
	int nIndex = 0;
	CString strTemp;

	while (nCOunt)
	{
		--nCOunt;
		m_list.InsertItem(nIndex, _T(""));

		// 序号
		strTemp.Format(_T("%d"), nIndex + 1);
		m_list.SetItemText(nIndex, 0, strTemp);

		// 线程ID
		strTemp.Format(_T("%d"), pThread[i].ulTID);
		m_list.SetItemText(nIndex, 1, strTemp);

		// BasePriority
		strTemp.Format(_T("%d"), pThread[i].ulBasePriority);
		m_list.SetItemText(nIndex, 2, strTemp);

		// 起始地址
		strTemp.Format(_T("0x%08X"), pThread[i].ulStartAddress);
		m_list.SetItemText(nIndex, 3, strTemp);

		// 状态
		m_list.SetItemText(nIndex, 4, NumToStatus(pThread[i].ulStatus));

		i++;
		nIndex++;
	}

	delete[] pThread;
	return 0;
}
