#pragma once

#include "SetData.h"

class CCalcAcfDlg : public CDialogAF
{
public:
	CCalcAcfDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

	AcfCondition m_AcfCondition;
	long m_nFolderID;
	int		m_iCalcData;

protected:
	enum { IDD = IDD_CALC_ACF };

	CEdit	m_cPhi0RefLevel;
	CComboBox	m_cPhi0RefData;
	CComboBox	m_cFreqFilter;
	CButton m_cCalcNo;
	CButton m_cCalcSelect;
	CButton m_cCalcAll;
	CButton m_cCalcMeasurement;
	double	m_dIntegrationTime;
	double	m_dMaxDelayTime;
	double	m_dTaueInterval;
	double	m_dPhi0RefLevel;
	double	m_dTaueEndLevel;
	double	m_dTaueEndTime;
	double	m_dRunningStep;
	BOOL	m_bTaueOrigin;
	int		m_iTau1Peak;

	void SetPhi0RefData();
	long GetPhi0RefData();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangePhi0Refdata();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
