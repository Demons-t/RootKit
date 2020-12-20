#pragma once
#include "Data.h"

// Driver 对话框

class Driver : public CDialogEx
{
	DECLARE_DYNAMIC(Driver)

public:
	Driver(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~Driver();
	void InitListColumn();				// 初始化列

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DRIVER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg LRESULT EnumDriver(WPARAM wParam, LPARAM lParam);		// 遍历驱动
	afx_msg void RClickDriver(NMHDR* pNMHDR, LRESULT* pResult);		// 单机鼠标右键
	afx_msg void FlushDriver();										// 刷新
	afx_msg void HideDriver();										// 隐藏驱动

private:
	CListCtrl m_listDriver;
	WCHAR m_wcDriverName[MAX_PATH];

};
