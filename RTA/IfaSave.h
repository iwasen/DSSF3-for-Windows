#pragma once

#include "ImpWnd.h"
#include "fft.h"
#include "FreqWnd.h"
#include "MyCtrl.h"

class CIfaSave : public CDialogAF
{
// コンストラクション
public:
	CIfaSave(CWnd* pParent = NULL);   // 標準のコンストラクタ

	double *m_pIRData;
	int m_nStage;
	int m_iSampling;

protected:
	enum { IDD = IDD_IFA_SAVE };

	CMyEdit	m_cMaxAdjLevel;
	CMyEdit	m_cStartPos;
	CMyEdit	m_cEndPos;
	CToolBar m_tbZoomH;
	CFreqWnd	m_cFreq;
	CScrollBar	m_cScroll;
	CImpWnd	m_cImpulse;
	CString	m_sComment;
	CString	m_sTitle;
	BOOL	m_bPhaseAdj;
	int m_nData;
	double m_fTotalTime;
	double m_fDispTime;
	double	m_fStartTime;
	int m_nScrollPos;
	int m_nScrollSize;
	int m_nStartPos;
	int m_nEndPos;
	double *m_pFreqData;
	double *m_pPhaseData;
	CRFFT m_oRFFT;
	double m_fMinLevel;
	CMyEdit m_cZoomH;

	void DispIRWindow();
	void DispFreqWindow();
	void CalcPowerSpectrum(double start, double end);
	static void CALLBACK GraphCallBack(WPARAM code, LPARAM data, double param1, double param2);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnZoomInH();
	afx_msg void OnZoomOutH();
	afx_msg void OnZoomFullH();
	afx_msg void OnSave();
	afx_msg void OnDestroy();
	afx_msg void OnChangeMaxAdjLevel();
	afx_msg void OnPhaseAdj();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
};
