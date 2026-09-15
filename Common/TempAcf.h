#pragma once

#include "MyCtrl.h"
#include "Dpi.h"

class CTempAcf : public CDialogAF
{
public:
	CTempAcf(CWnd* pParent = NULL);   // 標準コンストラクタ

	void SetData(const NsTmpData &nsTmpData);
	void GetData(NsTmpData &nsTmpData);
	void ShowWeightings();

protected:
	enum { IDD = IDD_TEMP_ACF };

	CMyEdit	m_cTaueWeighting;
	CMyEdit	m_cTaueUpperLimit;
	CMyEdit	m_cTaueStandard;
	CMyEdit	m_cTaueLowerLimit;
	CMyEdit	m_cTau1Weighting;
	CMyEdit	m_cTau1UpperLimit;
	CMyEdit	m_cTau1Standard;
	CMyEdit	m_cTau1LowerLimit;
	CMyEdit	m_cPhi1Weighting;
	CMyEdit	m_cPhi1UpperLimit;
	CMyEdit	m_cPhi1Standard;
	CMyEdit	m_cPhi1LowerLimit;
	CMyEdit	m_cPhi0Weighting;
	CMyEdit	m_cPhi0UpperLimit;
	CMyEdit	m_cPhi0Standard;
	CMyEdit	m_cPhi0LowerLimit;
	CMyButton	m_cTaueUpperCheck;
	CMyButton	m_cTaueLowerCheck;
	CMyButton	m_cTau1UpperCheck;
	CMyButton	m_cTau1LowerCheck;
	CMyButton	m_cPhi1UpperCheck;
	CMyButton	m_cPhi1LowerCheck;
	CMyButton	m_cPhi0UpperCheck;
	CMyButton	m_cPhi0LowerCheck;

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	afx_msg void OnSetWeighting();
	afx_msg void OnPhi0LowerCheck();
	afx_msg void OnPhi0UpperCheck();
	afx_msg void OnPhi1LowerCheck();
	afx_msg void OnPhi1UpperCheck();
	afx_msg void OnTau1LowerCheck();
	afx_msg void OnTau1UpperCheck();
	afx_msg void OnTaueLowerCheck();
	afx_msg void OnTaueUpperCheck();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
