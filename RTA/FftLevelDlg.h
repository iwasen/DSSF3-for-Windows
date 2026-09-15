#pragma once

#include "FftDlg.h"

// CFftLevelDlg ダイアログ

class CFftLevelDlg : public CDialogAF
{
public:
	CFftLevelDlg(CWnd* pParent = NULL);   // 標準コンストラクタ

	CFftDlg *m_pFftDlg;
	int m_nMaxRange;
	int m_nMinRange;
	int m_nMaxLevel;
	int m_nMinLevel;

// ダイアログ データ
	enum { IDD = IDD_FFT_LEVEL };

protected:
	CSliderCtrl m_cFftMaxLevel;
	CSliderCtrl m_cFftMinLevel;

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void PostNcDestroy();
};
