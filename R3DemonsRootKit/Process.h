#pragma once
#include "Data.h"
#include <atlbase.h>
#include <atlconv.h>
#include "Thread.h"
#include "Module.h"

// ProcessAndThread 对话框

class Process : public CDialogEx
{
	DECLARE_DYNAMIC(Process)

public:
	Process(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~Process();
	void InitListColumn();				// 初始化列

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROCESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CListCtrl	m_list;
	DWORD		m_dwPID;

public:
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg LRESULT EnumProcess(WPARAM wParam, LPARAM lParam);		// 遍历进程
	afx_msg void RClickProcess(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void FlushProcess();									// 刷新进程
	afx_msg void HideProcess();										// 隐藏进程
	afx_msg void TerminateProcess();								// 结束进程
	afx_msg void ShowThread();										// 查看线程
	afx_msg void ShowModule();										// 查看模块
};
