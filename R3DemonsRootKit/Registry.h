#pragma once
#include "Data.h"
#include <strsafe.h>
#include "AddKey.h"

// Register 对话框

class Registry : public CDialogEx
{
	DECLARE_DYNAMIC(Registry)

public:
	Registry(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~Registry();
	void InitListColumn();				// 初始化列
	void InitTreeTab();					// 初始化树控件

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REGISTRY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CListCtrl	m_list;
	CTreeCtrl	m_tree;
	CString		m_strFileName;

public:
	afx_msg void ClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void RClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void DeleteReg();										// 删除子项
	afx_msg void CreateReg();										// 创建子项
	afx_msg void FlushRegistry();									// 刷新
};
