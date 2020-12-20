#pragma once
#include "Data.h"

// AddKey 对话框

class AddKey : public CDialogEx
{
	DECLARE_DYNAMIC(AddKey)

public:
	AddKey(CString strKeyName, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~AddKey();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADDKEY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CString m_editAddKey;
	CString m_strKeyName;

public:
	afx_msg void ClickedAddKey();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	
};
