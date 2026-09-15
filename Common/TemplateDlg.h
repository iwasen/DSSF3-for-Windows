#pragma once

#include "MyCtrl.h"
#include "TempAcf.h"
#include "TempIAcf.h"

class CTemplateDlg : public CDialogAF
{
public:
	CTemplateDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

protected:
	enum { IDD = IDD_TEMPLATE };

	CMyEdit	m_cNoiseSource;
	CListBox	m_cTemplateList;
	CMyTab	m_cTab;
	CTempAcf m_TempAcf;
	CTempIAcf m_TempIAcf;

	void SetNsTmpList(long nID);
	void SaveNoiseTemplate(long nID);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDeleteTemplate();
	afx_msg void OnSelchangeTemplateList();
	afx_msg void OnUpdateTemplate();
	afx_msg void OnImport();
	afx_msg void OnExport();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
