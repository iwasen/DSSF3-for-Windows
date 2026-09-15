#pragma once

#include "ImpWnd.h"
#include "fft.h"
#include "FreqWnd.h"
#include "MyCtrl.h"

class CIfaSet : public CDialogAF
{
public:
	CIfaSet(CWnd* pParent = NULL);   // 標準のコンストラクタ
	~CIfaSet();

	long m_nIFilterID;
	int m_nStartPos;
	int m_nEndPos;
	int m_nMaxAdjLevel;
	BOOL	m_bPhaseAdj;
	int m_iSampling;
	double *m_pIRData;
	CString	m_sTitle;

protected:
	enum { IDD = IDD_IFA_SET };

	CMyEdit	m_cMaxAdjLevel;
	CMyEdit	m_cStartPos;
	CFreqWnd	m_cFreq;
	CMyEdit	m_cEndPos;
	CButton	m_cSet;
	CButton	m_cDelete;
	CButton	m_cChange;
	CScrollBar	m_cScroll;
	CImpWnd	m_cImpulse;
	CListCtrl	m_cIRList;
	CButton	m_cZoomOut;
	CButton	m_cZoomIn;
	CButton	m_cFull;
	CString	m_sComment;
	CToolBar m_tbZoomH;
	int m_nData;
	double m_fTotalTime;
	double m_fDispTime;
	double m_fStartTime;
	int m_nScrollPos;
	int m_nScrollSize;
	double *m_pFreqData;
	double *m_pPhaseData;
	CRFFT m_oRFFT;
	double m_fMinLevel;
	int m_nSortItem;
	int m_nSortDir;
	CMyEdit m_cZoomH;

	void DispIRWindow();
	void DispFreqWindow();
	void ReadIRData();
	void SetIRList();
	void CalcPowerSpectrum(double start, double end);
	static void CALLBACK GraphCallBack(WPARAM code, LPARAM data, double param1, double param2);
	static int CALLBACK CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnZoomInH();
	afx_msg void OnZoomOutH();
	afx_msg void OnZoomFullH();
	afx_msg void OnSet();
	afx_msg void OnChange();
	afx_msg void OnDelete();
	afx_msg void OnChangeMaxAdjLevel();
	afx_msg void OnItemchangedIrList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPhaseAdj();
	afx_msg void OnColumnclickIrList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkIrList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
};
