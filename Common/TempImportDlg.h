#pragma once

#include "Dpi.h"

class CTempImportDlg : public CDialogAF
{
public:
	CTempImportDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

protected:
	enum { IDD = IDD_TEMP_IMPORT };

	CListBox	m_cTemplateList;
	int		m_iAppendMode;
	CString m_PathName;

	BOOL ReadTemplateFile();
	BOOL ImportTemplate();
	void MergeTemplate(NsTmpData *pNsTmpData1, const NsTmpData *pNsTmpData2);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAllSelect();
	afx_msg void OnAllRemove();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
