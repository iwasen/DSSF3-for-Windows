#pragma once

#include "BaseDlg.h"
#include "fft.h"
#include "Acf3dd.h"
#include "RecWnd.h"
#include "MyCtrl.h"
#include "WaveNotify.h"
#include "AcfRealtimeDlg.h"

class CAcfDlg : public CBaseDlg, public IWaveNotify
{
// コンストラクション
public:
	CAcfDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ
	~CAcfDlg();

protected:
	enum { IDD = IDD_ACF };

	void DispSelectArea(double fSelectStart, double fSelectEnd);
	BOOL AllocBuffers();
	void FreeBuffers();
	BOOL WaveInData(LPWAVENOTIFY pWaveNotify);
	int WaveOutData(LPWAVENOTIFY pWaveNotify);
	void SetRingBuf(const double *pData, int nData);
	void CalcAcf();
	void DispAcfGraph();
	void RedrawAcfGraph();
	static void CALLBACK GraphCallBack(WPARAM code, LPARAM data, double param1, double param2);
	void DispWaveGraph();
	void InitDispTime();
	void SetFilter();
	void EnableScaleCtrl();
	void SetMeasureParam();
	void LoadACFData(long nAcfID);
	void LoadWaveFile(LPCTSTR pFileName);
	void EnableRedraw();
	void EnableButton(BOOL bEnable = TRUE);
	void SetTimeDataNum(double fTime);
	void EnableDispRch();
	void EnableControl(BOOL bEnable);
	virtual void ChangeWaveDevice(int nInOut);
	void ChangeSamplingRate(int nSamplingRate);
	int OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify);
	BOOL SaveACF(HWAVEDATA hWaveData);

	CMyEdit	m_cZoomH;
	CScrollBar	m_cGraphScroll;
	CButton	m_cChannelLch;
	CButton	m_cChannelRch;
	CButton	m_cScale_Liner;
	CButton	m_cScale_Log;
	CButton	m_cTimeFwd;
	CButton	m_cTimeBwd;
	CButton	m_cChannelMono;
	CButton	m_cChannelStereo;
	CMyEdit	m_cSelectTime;
	CButton	m_cSelectAll;
	CMyEdit	m_cSelectStart;
	CMyEdit	m_cSelectEnd;
	CButton	m_cReplay;
	CButton	m_cCut;
	CButton	m_cFileSave;
	CButton	m_cRedraw;
	CButton	m_cLaunchSA;
	CSpinButtonCtrl	m_cTimeDataNumSpin;
	CSpinButtonCtrl	m_cMeasureTimeSpin;
	CMyEdit	m_cMrTime;
	CMyEdit	m_cMrSampling;
	CMyEdit	m_cMrMtime;
	CMyEdit	m_cScaleLevel;
	CMyButton	m_cScaleAbs;
	CComboBox	m_cFreqFilter;
	CMyEdit	m_cMeasureTime;
	CRecWnd	m_cGraphWave;
	CMyButton	m_cGraphNorm;
	CAcf3dd	m_cGraphAcf;
	CMyEdit	m_cTimeDataNum;
	CMyEdit	m_cMaxTau;
	CMyEdit	m_cCalcCycle;
	CButton	m_cBtnStop;
	CButton	m_cBtnStart;
	CMyEdit	m_cIntegrationTime;
	CComboBox	m_cSamplingRate;
	CButton m_cFileLoad;
	CButton m_cExit;
	CMyButton m_cAcfRealtimeGraph;

	int m_iTimeDir;
	int m_iScaleType;
	int m_iChannel;
	int m_iDispChannel;
	CToolBar m_tbZoomH;
	BOOL m_bInitialized;
	double *m_pAcfBuf;
	int m_nSamplingRate;
	int m_nChannel;
	int m_nSample;
	int m_nRingBufSize;
	int m_nRingBufPtr;
	BOOL m_bRingBufFull;
	double *m_pRingBuf;
	double *m_pWaveBuf;
	int m_nWaveBufSize;
	int m_nWaveBufCount;
	int m_nAcfSize;
	int m_nAcfBufSize;
	double *m_pAcfBuf1;
	CRFFT m_oRFFT;
	double m_fDataTime;
	double m_fTotalTime, m_fStartTime, m_fDispTime;
	int m_nScrollSize;
	int m_nScrollPos;
	double m_fMaxData;
	double *m_pFilterTbl;
	CString m_sTime;
	int m_nWaveOutCount;
	int m_nWaveOutEndCount;
	double m_fSelectStart;
	double m_fSelectEnd;
	BOOL m_bDispSelectArea;
	int m_nDispChannel;
	int m_nReplayPos;
	BOOL m_bOpenWaveOut;
	class CWaveInEx *m_pWaveIn;
	class CWaveOutEx *m_pWaveOut;
	int m_nBitsPerSample;
	CAcfRealtimeDlg *m_pAcfRealtimeDlg;

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnDestroy();
	afx_msg void OnChangeCalcCycle();
	afx_msg void OnChangeTimeDataNum();
	afx_msg void OnChannelMono();
	afx_msg void OnChannelStereo();
	afx_msg void OnSelchangeSamplingRate();
	afx_msg void OnChangeIntegrationTime();
	afx_msg void OnChangeMaxTau();
	afx_msg void OnTimeBwd();
	afx_msg void OnTimeFwd();
	afx_msg void OnChangeMeasureTime();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnZoomFullH();
	afx_msg void OnZoomInH();
	afx_msg void OnZoomOutH();
	afx_msg void OnGraphNorm();
	afx_msg void OnScaleAbs();
	afx_msg void OnChangeScaleLevel();
	afx_msg void OnScaleLiner();
	afx_msg void OnScaleLog();
	afx_msg void OnSelchangeFreqFilter();
	afx_msg void OnFileLoad();
	afx_msg void OnRedraw();
	afx_msg void OnFileSave();
	afx_msg void OnReplay();
	afx_msg void OnCut();
	afx_msg void OnSelectAll();
	afx_msg void OnChangeSelectStart();
	afx_msg void OnChangeSelectEnd();
	afx_msg void OnChannelLch();
	afx_msg void OnChannelRch();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedLaunchSa();
	afx_msg LRESULT OnCloseAcfRealtimeDialog(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedAcfRealtimeGraph();
};
