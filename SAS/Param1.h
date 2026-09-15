#pragma once

#include "MyCtrl.h"

class CParam1 : public CDialogAF
{
public:
	CParam1(CWnd* pParent = NULL);   // 標準コンストラクタ

	CMyEdit	m_cPrefSPL;
	CMyEdit	m_cPrefTauE;
	CMyEdit	m_cIACCWLevel;
	CMyEdit	m_cDT1MinTime;
	CMyEdit	m_cTsubNoise;
	CMyEdit	m_cTsubEnd;
	CMyEdit	m_cSplRefLevel;
	CMyEdit	m_cSplRefData;
	int		m_iFreqBand;
	BOOL	m_bTsubAuto;
	BOOL	m_bAFilter;
	CString	m_sStartFreq;
	CString	m_sEndFreq;

protected:
	enum { IDD = IDD_PARAM1 };

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
};
