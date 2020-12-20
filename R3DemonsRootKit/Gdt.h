#pragma once
#include "Data.h"

// Gdt 对话框

class Gdt : public CDialogEx
{
	DECLARE_DYNAMIC(Gdt)

public:
	Gdt(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~Gdt();
	void InitListColumn();				// 初始化列

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GDT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void RClickGdt(NMHDR* pNMHDR, LRESULT* pResult);

private:
	CListCtrl m_list;

public:
	virtual void OnOK(); 
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg LRESULT EnumGdt(WPARAM wParam, LPARAM lParam);		// 遍历GDT
	afx_msg void FlushGdt();										// 刷新
};
