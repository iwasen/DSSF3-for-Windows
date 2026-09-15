#pragma once

#include "MyCtrl.h"

class CSgSynth : public CDialogAF
{
public:
	CSgSynth(CWnd* pParent = NULL);   // 標準コンストラクタ

protected:
	enum { IDD = IDD_SG_SYNTH };

	CMySliderCtrl m_cFreqSlider;
	CComboBox m_cWaveForm;
	CMyEdit m_cCompFreq[N_SG_COMPONENT];
	CMyEdit m_cCompLevel[N_SG_COMPONENT];
	CSliderCtrl m_cLevelSlider[N_SG_COMPONENT];
	CMyEdit m_cSynthFreq;
	CSpinButtonCtrl m_cSynthFreqSpin;

	void SetFreqRange();
	int CalcSliderPos(double fFreq);
	void SetSynthFreq(double fFreq);
	double CalcFreq(int pos);
	void SetCompFreq(int nIndex);
	void SetCompLevel(int nIndex);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeCompFreq1();
	afx_msg void OnEnChangeCompFreq2();
	afx_msg void OnEnChangeCompFreq3();
	afx_msg void OnEnChangeCompFreq4();
	afx_msg void OnEnChangeCompFreq5();
	afx_msg void OnEnChangeCompFreq6();
	afx_msg void OnEnChangeCompFreq7();
	afx_msg void OnEnChangeCompFreq8();
	afx_msg void OnEnChangeCompLevel1();
	afx_msg void OnEnChangeCompLevel2();
	afx_msg void OnEnChangeCompLevel3();
	afx_msg void OnEnChangeCompLevel4();
	afx_msg void OnEnChangeCompLevel5();
	afx_msg void OnEnChangeCompLevel6();
	afx_msg void OnEnChangeCompLevel7();
	afx_msg void OnEnChangeCompLevel8();
	afx_msg void OnEnChangeSynthFreq();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCbnSelchangeWaveForm();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
