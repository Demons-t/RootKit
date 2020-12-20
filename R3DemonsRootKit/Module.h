#pragma once
#include "Data.h"

// Module 对话框

class Module : public CDialogEx
{
	DECLARE_DYNAMIC(Module)

public:
	Module(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~Module();
	void InitListColumn();				// 初始化列

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MODULE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CListCtrl m_list;
	virtual void OnOK();

public:
	DWORD		m_dwPID;
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg LRESULT EnumModule(WPARAM wParam, LPARAM lParam);		// 遍历模块
};
