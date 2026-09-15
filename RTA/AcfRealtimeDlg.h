#pragma once

#include "BaseDlg.h"
#include "FactorWnd.h"
#include "FileIO.h"
#include "WaveNotify.h"
#include "AcfSpgWnd.h"
#include "fft.h"
#include <afxmt.h>

// CAcfRealtimeDlg ダイアログ

class CAcfRealtimeDlg : public CBaseDlg, public IWaveNotify
{
public:
	CAcfRealtimeDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CAcfRealtimeDlg();

// ダイアログ データ
	enum { IDD = IDD_ACF_REALTIME };

protected:
	struct FACTOR_GRAPH {
		CFactorWnd *pFactorWnd;
		double *pFactorData;
		CString sAxis;
		double fMinValue;
		double fTotalValue;
	};

	CFactorWnd m_cGraphPhi0;
	CFactorWnd m_cGraphPhi1;
	CFactorWnd m_cGraphTauE;
	CFactorWnd m_cGraphTau1;
	CAcfSpgWnd m_cGraphSpg;
	CListCtrl m_cFactorList;

	HWND m_hAcfDlg;
	int m_nSamplingRate;
	int m_nChannel;
	int m_nMeasureTime;
	CPtrList m_aWaveBufferList;
	double *m_pCalcBufferL;
	double *m_pCalcBufferR;
	CEvent m_oCalcEvent;
	CEvent m_oExitEvent;
	BOOL m_bExitFlag;
	AcfCondition m_oAcfCondition;
	IAcfCondition m_oIAcfCondition;
	int m_nIntegrationSize1;
	int m_nIntegrationSize2;
	int m_nFftSize;
	double *m_pFilterTbl;
	double *m_pFilterTbl2;
	int m_nMaxDelaySize;
	AllFactorData *m_pFactorData;
	int m_nTimeDataNum;
	int m_nWaveBufSize;
	int m_nFactorCount;
	FACTOR_GRAPH m_aFactorGraph[4];
	double m_fStartTime;
	double m_fElapseTime;
	CCriticalSection m_oCriticalSection;
	BOOL m_bInitialized;
	class CWaveInEx *m_pWaveIn;
	double *m_pSpectrim;
	CRFFT m_oRFFT;
	int m_nFftSize2;
	double *m_pWindowFunc;
	int m_nTimeCount;
	int m_nSpgMaxLevel;
	int m_nSpgMinLevel;
	BOOL m_bSpgScale;
	BOOL m_bTaueFixedScale;
	double m_fTaueMinTime;
	double m_fTaueMaxTime;
	BOOL m_bTaueLogScale;

	void SetCalcParams();
	void Start();
	int OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify);
	BOOL WaveInData(LPWAVENOTIFY pWaveNotify);
	void AllocBuffers();
	void FreeBuffers();
	static UINT CalcThreadEntry(LPVOID pParam);
	void CalcThread();
	void CalcAcfFactor(AllFactorData *pFactorData);
	void GetFactor(int nFactorKind, FACTOR_GRAPH &oFactorGraph);
	void CalcSpectrum();
	void InitAcfFactorList();
	void DispAcfFactorList(const AllFactorData *pFactorData);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnCalcConditions();
	afx_msg LRESULT OnDispAcfRealtime(WPARAM wParam, LPARAM lParam);
};
