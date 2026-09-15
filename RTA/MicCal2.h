#pragma once

#include "MyCtrl.h"
#include "FreqWnd.h"
#include "FileIO.h"
#include "FFT.h"
#include "MakeFilter.h"
#include "WaveInEx.h"
#include "WaveOutEx.h"

#define N_MEAN	5
#define WAVEBUF_SAMPLESPERBUFFER	8192
#define WAVEBUF_NUM		4
#define SAMPLING_RATE	48000

class CMicCal2 : public CDialogAF
{
public:
	CMicCal2(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CMicCal2();

	BOOL m_bUpdate;
	CString	m_sMicInfo;
	CString	m_sMicAmpInfo;
	CString	m_sComment;

	void ReadMicCalData(int nMicCalID);
	int RegistMicCalData(int nMicCalID = 0);
	void ChangeMicCalData(int nMicCalID);
	void WaveInData(double *pData);

protected:
	enum { IDD = IDD_MICCAL2 };

	CComboBox	m_cFilter;
	CScrollBar	m_cLevelBar;
	CMyEdit	m_cInputLevel;
	CFreqWnd	m_cFreqResponse;
	CListCtrl	m_cFreqList;
	double	m_fFreq;
	double	m_fLevel;
	double *m_pFreqData;
	double *m_pFilterTbl;
	CRFFT m_oRFFT;
	double m_fMeanBuf[N_MEAN];
	int m_nMeanBufPtr;
	double m_fInputSens;
	CFont m_Font;
	int m_nMicCalID;

	void InitFreqList();
	static int CALLBACK CompareFreq(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	void DispFreqWindow();
	int GetFreqData(FilterData *pFilterData);
	void SetFilter(const FilterData *pFilterData, int nFilterData);
	void GetMicCalData(DbMicCalRec *pDbMicCalRec);
	void MakeFreqData();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	afx_msg void OnFreqRegist();
	afx_msg void OnFreqDelete();
	afx_msg void OnChangeMicinfo();
	afx_msg void OnChangeMicampinfo();
	afx_msg void OnItemchangedFreqList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnInsertitemFreqList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitemFreqList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelchangeFilter();
	afx_msg void OnDestroy();
	afx_msg void OnEnChangeComment();
};
