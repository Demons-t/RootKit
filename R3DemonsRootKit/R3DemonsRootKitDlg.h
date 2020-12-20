
// R3DemonsRootKitDlg.h: 头文件
//

#pragma once
#include "File.h"
#include "Driver.h"
#include "Process.h"
#include "Idt.h"
#include "Gdt.h"
#include "SSDT.h"
#include "Registry.h"
#include <winsvc.h>

// CR3DemonsRootKitDlg 对话框
class CR3DemonsRootKitDlg : public CDialogEx
{
// 构造
public:
	CR3DemonsRootKitDlg(CWnd* pParent = nullptr);	// 标准构造函数
	void InitTab();				// 初始化 tab 页

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_R3DEMONSROOTKIT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual void OnOK();

public:
	virtual BOOL	PreTranslateMessage(MSG* pMsg);					// 禁用回车和esc
	afx_msg void	ChangeTab(NMHDR* pNMHDR, LRESULT* pResult);		// 切换 tab 页 
	void			LoadDriver();									// 加载服务

private:
	CTabCtrl			m_tab;
	int					m_nCurSelTab;
	File				m_file;
	Driver				m_driver;
	Process				m_process;
	Idt					m_idt;
	Gdt					m_gdt;
	Ssdt				m_ssdt;
	Registry			m_registry;
	CDialog*			m_pDialog[6];		//用来保存对话框对象指针
	SC_HANDLE			m_hSCManager;
public:
	virtual BOOL DestroyWindow();
};
