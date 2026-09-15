#pragma once

#include "MyCtrl.h"

class CSgSweep : public CDialogAF
{
public:
	CSgSweep(CWnd* pParent = NULL);   // 標準のコンストラクタ

protected:
	enum { IDD = IDD_SG_SWEEP };

	CMyButton	m_cLoop;
	CMyEdit	m_cSweepTime;
	CMyEdit	m_cStartFreq;
	CMyEdit	m_cEndFreq;
	CMyEdit m_cStartLevel;
	CMyEdit m_cEndLevel;
	CComboBox m_cWaveForm;

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnChangeStartFreq();
	afx_msg void OnChangeEndFreq();
	afx_msg void OnChangeSweepSpeed();
	afx_msg void OnLoop();
	afx_msg void OnSinwave();
	afx_msg void OnTriangle();
	afx_msg void OnSquare();
	afx_msg void OnSaw();
	afx_msg void OnCbnSelchangeWaveForm();
	afx_msg void OnEnChangeStartLevel();
	afx_msg void OnEnChangeEndLevel();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
