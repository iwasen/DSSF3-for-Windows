#pragma once

#include "SetData.h"

class CCalcImpDlg : public CDialogAF
{
public:
	CCalcImpDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

	SetCalcParam m_CalcParam;
	long m_nFolderID;
	int m_iCalcData;

protected:
	enum { IDD = IDD_CALC_IMP };

	CEdit	m_cTsubEnd;
	CEdit	m_cSplRefLevel;
	CComboBox	m_cSplRefData;
	CComboBox	m_cEndFreq;
	CComboBox	m_cStartFreq;
	CButton m_cCalcNo;
	CButton m_cCalcSelect;
	CButton m_cCalcAll;
	CButton m_cCalcMeasurement;
	double	m_dTsubEnd;
	double	m_dTsubNoise;
	double	m_dDT1MinTime;
	int		m_iFreqBand;
	BOOL	m_bTsubAuto;
	double	m_dWIACCLevel;
	double	m_dPrefSPL;
	double	m_dPrefTauE;
	double	m_dSplRefLevel;
	BOOL	m_bAFilter;
	double m_dGRefData;
	double m_dTCustom1;
	double m_dTCustom2;
	double m_dCCustom;

	void SetFreq();
	void SetSplRefData();
	long GetSplRefData();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void On1oct();
	afx_msg void On3oct();
	afx_msg void OnSelchangeStartFreq();
	afx_msg void OnSelchangeEndFreq();
	afx_msg void OnSelchangeSplRefdata();
	afx_msg void OnTsubAuto();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
