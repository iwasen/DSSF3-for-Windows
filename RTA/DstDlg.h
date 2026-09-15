#pragma once

#include "BaseDlg.h"
#include "DstManual.h"
#include "DstFreq.h"
#include "DstLevel.h"
#include "WaveNotify.h"
#include "ScreenShot.h"

#define MAX_HARMONICS	30
#define MAX_GUARD_CNT	12

class CDstDlg : public CBaseDlg, public IWaveNotify
{
public:
	CDstDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	~CDstDlg();

protected:
	enum { IDD = IDD_DST };

	CComboBox m_cSamplingRate;
	BOOL m_bInitialized;
	int m_nSample;
	int m_nChannel;
	int m_nBufSize;
	int m_nFreq;
	double m_fLevel;
	int m_nWaveOutOffset;
	int m_nWaveInOffset;
	int m_nGuardCnt;
	double *m_pLeftData;
	double *m_pRightData;
	double m_pLeftDst[MAX_HARMONICS];
	double m_pRightDst[MAX_HARMONICS];
	double *m_pSinTable;
	CDstManual m_cDstManual;
	CDstFreq m_cDstFreq;
	CDstLevel m_cDstLevel;
	int m_nTimeCount;
	BOOL m_bRegist;
	BOOL m_bNoRegist;
	class CWaveInEx *m_pWaveIn;
	class CWaveOutEx *m_pWaveOut;
	int m_iChannel;
	CMyTab m_cDstTab;
	CButton m_cBtnStart;
	CComboBox m_cMaxHarmonics;
	int m_iScaleMode;
	CComboBox m_cScaleMax;
	CComboBox m_cScaleMin;
	CMyEdit m_cElapseTime;
	CComboBox m_cGuardTime;
	CButton m_cCsvOutput;
	CScreenShot m_oScreenShot;
	CButton m_cAddDataHold;
	CButton m_cDelDataHold;
	CButton m_cScreenShot;

	void RestoreHoldData();
	void SetMaxHarmonics();
	void SetGuardTime();
	void SetScaleSelectAll();
	void SetScaleSelect(CComboBox &cCombo, int nMax, int nMin, int nDef);
	BOOL Start();
	void Stop();
	void FreeBuffers();
	void MakeSinTable();
	int WaveOutData(double *pData);
	BOOL WaveInData(const double *pData);
	void CalcTHD();
	double GetHarmonic(const double *pData, int nHarmonic);
	void RedrawGraph();
	void EnableOutputFile();
	BOOL CheckDataExist();
	BOOL CheckDataHold();
	int OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify);
	void EnableDataHoldButton();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void ChangeWaveDevice(int nInOut);
	virtual BOOL DestroyWindow();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnBnClickedScalePercent();
	afx_msg void OnBnClickedScaleDb();
	afx_msg void OnCbnSelchangeScaleMax();
	afx_msg void OnCbnSelchangeScaleMin();
	afx_msg void OnBnClickedStart();
	afx_msg void OnDestroy();
	afx_msg void OnCbnSelchangeSamplingRate();
	afx_msg void OnBnClickedChannelMono();
	afx_msg void OnBnClickedChannelStereo();
	afx_msg void OnCbnSelchangeMaxHarmonics();
	afx_msg void OnTcnSelchangeDstTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeGuardTime();
	afx_msg void OnBnClickedOutputFile();
	afx_msg void OnBnClickedScreenShot();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnBnClickedAddDataHold();
	afx_msg void OnBnClickedDelDataHold();
};
