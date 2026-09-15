#pragma once

#include "MyCtrl.h"

class CSgTone : public CDialogAF
{
// コンストラクション
public:
	CSgTone(CWnd* pParent = NULL);   // 標準のコンストラクタ

	void SetFreqRange(BOOL bSetSlider);

protected:
	enum { IDD = IDD_SG_TONE };

	CMyButton	m_cRendouF;
	CMySliderCtrl	m_cSFreqR;
	CSliderCtrl	m_cSFreqL;
	CMyEdit	m_cEFreqR;
	CMyEdit	m_cEFreqL;
	CComboBox m_cFreqRange;
	CComboBox m_cWaveForm;
	CButton m_cPreset1;
	CButton m_cPreset2;
	CButton m_cPreset3;
	CButton m_cPreset4;
	CButton m_cPreset;
	CButton m_cPhase0;
	CMySliderCtrl m_cPhaseSlider;
	CMyEdit m_cPhaseValue;
	CSpinButtonCtrl m_cFreqLSpin;
	CSpinButtonCtrl m_cFreqRSpin;
	int m_nMinFreq;
	int m_nMaxFreq;
	int m_nFreqRange;

	int CalcSliderPos(double fFreq);
	double CalcFreq(int pos);
	void SetLFreq(double fFreq);
	void SetLFreq2(double fFreq);
	void SetRFreq(double fFreq);
	void SetRFreq2(double fFreq);
	void SetFreqScale();
	void SetPresetFreq(int nPresetNo);
	void DispPresetFreq(int nPresetNo);
	void DispAllPresetFreq();
	void SetPhaseRange();
	void SetPhase(int nPhase);
	void EnablePhase();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeEfreqL();
	afx_msg void OnChangeEfreqR();
	afx_msg void OnRendouF();
	afx_msg void OnBnClickedPreset1();
	afx_msg void OnBnClickedPreset2();
	afx_msg void OnBnClickedPreset3();
	afx_msg void OnBnClickedPreset4();
	afx_msg void OnEnChangePhaseValue();
	afx_msg void OnBnClickedPhase0();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnCbnSelchangeFreqRange();
	afx_msg void OnCbnSelchangeWaveForm();
};
