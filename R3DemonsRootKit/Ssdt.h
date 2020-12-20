#pragma once
#include "Data.h"

// Ssdt 对话框

class Ssdt : public CDialogEx
{
	DECLARE_DYNAMIC(Ssdt)

public:
	Ssdt(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~Ssdt();
	void InitListColumn();				// 初始化列

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SSDT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CListCtrl m_list;

public:
	afx_msg void RClickSsdt(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg LRESULT EnumSsdt(WPARAM wParam, LPARAM lParam);		// 遍历SSDT
	afx_msg void FlushSsdt();	// 刷新
};
