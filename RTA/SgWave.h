#pragma once

#include "WaveViewWnd.h"
#include "WaveForm.h"
#include "MyCtrl.h"

class CSgWave : public CDialogAF
{
public:
	CSgWave(CWnd* pParent = NULL);   // 標準コンストラクタ

protected:
	enum { IDD = IDD_SG_WAVE };

	CWaveViewWnd m_cWaveWnd;
	CMySliderCtrl m_cSFreq;
	CMyEdit m_cEFreq;
	CSpinButtonCtrl m_cEFreqSpin;
	int m_iWaveForm;
	WaveForm *m_pWaveForm;

	void SetFreqRange();
	int CalcSliderPos(double fFreq);
	void SetFreq(double fFreq);
	double CalcFreq(int pos);
	void DispWaveView();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnChangeEfreq();
	afx_msg void OnBnClickedEdit();
	afx_msg void OnBnClickedWaveform();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
