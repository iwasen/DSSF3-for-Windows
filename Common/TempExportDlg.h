#pragma once

#include "Dpi.h"

class CTempExportDlg : public CDialogAF
{
public:
	CTempExportDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

	long m_nSelectID;

protected:
	enum { IDD = IDD_TEMP_EXPORT };

	CListBox	m_cTemplateList;

	void SetNsTmpList(long nID);
	BOOL ExportTemplate(LPCSTR pFileName);
	BOOL CheckSelect(long nID);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAllSelect();
	afx_msg void OnAllRemove();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
