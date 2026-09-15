#pragma once

#include "WaveEditWnd.h"
#include "WaveForm.h"
#include "SgDlg.h"

// CWaveEditFM ダイアログ

class CWaveEditFM : public CDialogAF
{
public:
	CWaveEditFM(CWnd* pParent = NULL);   // 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_WAVE_EDIT_FM };

	WaveFormFM m_oWaveForm;
	CSgDlg *m_pSgDlg;
	CMyButton m_cBtnTest;

protected:
	CWaveViewWnd m_cWaveWnd;
	CSliderCtrl m_cSliderModAmp;
	CMyEdit m_cEditModAmp;
	CSliderCtrl m_cSliderModFreq;
	CMyEdit m_cEditModFreq;

	void InitData();
	void DispGraphWindow();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnChangeEditModAmp();
	afx_msg void OnEnChangeEditModFreq();
	afx_msg void OnBnClickedBtnTest();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
