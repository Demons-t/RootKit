#pragma once
#include "Data.h"

// Thread 对话框

class Thread : public CDialogEx
{
	DECLARE_DYNAMIC(Thread)

public:
	Thread(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~Thread();
	void InitListColumn();				// 初始化列

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_THREAD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CListCtrl	m_list;
	ULONG		m_eProcess;

public:
	DWORD		m_dwPID;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg LRESULT EnumThread(WPARAM wParam, LPARAM lParam);		// 遍历线程
};
