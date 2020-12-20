#pragma once
#include "Data.h"

// File 对话框

class File : public CDialogEx
{
	DECLARE_DYNAMIC(File)

public:
	File(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~File();
	void InitListColumn();				// 初始化列
	void InitTreeTab();					// 初始化树控件

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CListCtrl	m_list;
	CTreeCtrl	m_tree;
	CString		m_strDir;
	CString		m_fileName;

public:
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg LRESULT EnumFile(WPARAM wParam, LPARAM lParam);		// 遍历文件
	afx_msg void ClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void RClickFile(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void FlushFile();										// 刷新
	afx_msg void DeleteFile();										// 删除文件
};
