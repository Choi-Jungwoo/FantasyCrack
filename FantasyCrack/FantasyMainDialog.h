#pragma once

#include "afxdialogex.h"

// FantasyMainDialog 对话框

class FantasyMainDialog : public CDialogEx
{
	DECLARE_DYNAMIC(FantasyMainDialog)

public:
	FantasyMainDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~FantasyMainDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPackHookButton();
	afx_msg void OnBnClickedUndoPackButton();
};
