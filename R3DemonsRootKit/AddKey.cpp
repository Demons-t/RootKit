// AddKey.cpp: 实现文件
//

#include "pch.h"
#include "R3DemonsRootKit.h"
#include "AddKey.h"
#include "afxdialogex.h"


// AddKey 对话框

IMPLEMENT_DYNAMIC(AddKey, CDialogEx)

AddKey::AddKey(CString strKeyName, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ADDKEY, pParent)
	, m_strKeyName(strKeyName)
	, m_editAddKey(_T(""))
{

}

AddKey::~AddKey()
{
}

void AddKey::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_editAddKey);
}


BEGIN_MESSAGE_MAP(AddKey, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &AddKey::ClickedAddKey)
END_MESSAGE_MAP()


// AddKey 消息处理程序


void AddKey::ClickedAddKey()
{
	// TODO: 在此添加控件通知处理程序代码

	UpdateData(TRUE);

	WCHAR wPath[256] = { 0 };
	CString strLastPath;
	strLastPath = m_strKeyName + _T("\\") + m_editAddKey + _T("\\");
	
	wcscpy_s(wPath, (strLastPath.GetLength() + 1 * 2), strLastPath.GetBuffer());
	int nLen = wcslen(wPath) * 2 + 2;
	DWORD dwSize = 0;
	DeviceIoControl(g_hDeviceHandle, NEW_REGISTRY, wPath, nLen, NULL, NULL, &dwSize, NULL);

	CDialogEx::OnOK();
}


void AddKey::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	// CDialogEx::OnOK();
}


BOOL AddKey::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}


	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL AddKey::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
