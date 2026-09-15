#pragma once

#include "SgDlg.h"
#include "WaveEditManual.h"
#include "WaveEditFM.h"

// CWaveEditDlg ダイアログ

class CWaveEditDlg : public CDialogAF
{
public:
	CWaveEditDlg(CWnd* pParent = NULL);   // 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_WAVE_EDIT };

	WaveForm *m_pWaveForm;
	CSgDlg *m_pSgDlg;

protected:
	CMyTab m_cTabWaveEdit;

	CWaveEditManual m_wndWaveEditManual;
	CWaveEditFM m_wndWaveEditFM;

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedOk();
	afx_msg void OnDestroy();
	afx_msg void OnTcnSelchangeTabWaveEdit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
