#pragma once
#include "Data.h"

// Idt 对话框

class Idt : public CDialogEx
{
	DECLARE_DYNAMIC(Idt)

public:
	Idt(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~Idt();
	void InitListColumn();				// 初始化列

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IDT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CListCtrl m_list;
	virtual void OnOK();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg LRESULT EnumIdt(WPARAM wParam, LPARAM lParam);		// 遍历IDT
	afx_msg void RClickIdt(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void FlushIdt();	// 刷新
};
