#pragma once

#include "MyCtrl.h"
#include "Dpi.h"

class CTempIAcf : public CDialogAF
{
public:
	CTempIAcf(CWnd* pParent = NULL);   // 標準コンストラクタ

	void SetData(const NsTmpData &nsTmpData);
	void GetData(NsTmpData &nsTmpData);
	void ShowWeightings();

protected:
	enum { IDD = IDD_TEMP_IACF };

	CMyEdit	m_cIACCWeighting;
	CMyEdit	m_cIACCUpperLimit;
	CMyEdit	m_cIACCStandard;
	CMyEdit	m_cIACCLowerLimit;
	CMyEdit	m_cTIACCWeighting;
	CMyEdit	m_cTIACCUpperLimit;
	CMyEdit	m_cTIACCStandard;
	CMyEdit	m_cTIACCLowerLimit;
	CMyEdit	m_cWIACCWeighting;
	CMyEdit	m_cWIACCUpperLimit;
	CMyEdit	m_cWIACCStandard;
	CMyEdit	m_cWIACCLowerLimit;
	CMyButton	m_cIACCUpperCheck;
	CMyButton	m_cIACCLowerCheck;
	CMyButton	m_cTIACCUpperCheck;
	CMyButton	m_cTIACCLowerCheck;
	CMyButton	m_cWIACCUpperCheck;
	CMyButton	m_cWIACCLowerCheck;

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	afx_msg void OnSetWeighting();
	afx_msg void OnIACCLowerCheck();
	afx_msg void OnIACCUpperCheck();
	afx_msg void OnTIACCLowerCheck();
	afx_msg void OnTIACCUpperCheck();
	afx_msg void OnWIACCLowerCheck();
	afx_msg void OnWIACCUpperCheck();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
