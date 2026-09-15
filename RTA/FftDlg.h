#pragma once

#include "BaseDlg.h"
#include "FftWnd.h"
#include "ScreenShot.h"
#include "WaveNotify.h"
#include "FftLevelDlg.h"
#include "FftFreqDlg.h"

class CFftDlg : public CBaseDlg, public IWaveNotify
{
public:
	CFftDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ
	~CFftDlg();

	void RestorePage(int nMode);
	void FullScreen();
	void CloseLevelDlg();
	void SetFftMaxLevel(int nMaxLevel);
	void SetFftMinLevel(int nMinLevel);
	void CloseFreqDlg();
	void SetFftMaxFreq(int nMaxFreq);
	void SetFftMinFreq(int nMinFreq);
	void SetCsvConditions(int nMode, int nChannel, int nFftSizeIndex, int nSamplingRateIndex, int nOctBandIndex, int nCorrelation);

protected:
	enum { IDD = IDD_FFT };

	CMyEdit	m_cFftPeakLevelRight;
	CMyEdit	m_cFftPeakLevelLeft;
	CMyEdit	m_cFftPeakFreqRight;
	CMyEdit	m_cFftPeakFreqLeft;
	CMyEdit	m_cFftTime;
	CMyEdit	m_cFftPeakHoldTime;
	CMyEdit	m_cFftMinLevel;
	CMyEdit	m_cFftMinFreq;
	CMyEdit	m_cFftMaxLevel;
	CMyEdit	m_cFftMaxFreq;
	CMyEdit	m_cFftResTime;
	CMyEdit	m_cFftResFreq;
	CMyEdit	m_cFftMessage;
	CMyEdit m_cFftAutoStop;
	CButton	m_cFftPeakReset;
	CButton	m_cFftPeakFreq;
	CButton	m_cFftPeakLevel;
	CMyButton	m_cFftPeakDisp;
	CMyButton	m_cFftPeakHold;
	CMyButton	m_cFftAutoLevel;
	CMyButton	m_cFftAutoFreq;
	CMyButton	m_cRlSplit;
	CMyButton m_cDataRecord;
	CButton	m_cFftStart;
	CComboBox	m_cFftAverage;
	CComboBox	m_cFftTimeRes;
	CComboBox	m_cFftFilter;
	CComboBox	m_cFftTimeDataNum;
	CComboBox	m_cFftWinFunc;
	CComboBox	m_cFftFftSize;
	CComboBox	m_cFftSmoothing;
	CComboBox	m_cSamplingRate;
	CComboBox m_cFftOctBand;
	CComboBox m_cFftTimeRange;
	CComboBox m_cFftChannel;
	CSliderCtrl	m_cFftCfZoom;
	CSpinButtonCtrl m_cFftAutoStopSpin;
	CMyTab	m_cFftTab;
	int		m_iFftCorrelation;
	int		m_iFftScale;
	int		m_iFftTimeDir;
	int		m_iFftPeakHoldMode;
	int		m_iFftColorScale;
	CFftWnd m_wndFft;
	int m_nCurrentMsgNo;
	BOOL m_bInitialized;
	BOOL m_bMax;
	int m_nOrgInputSel;
	int m_nOrgInputVol;
	BOOL m_bModeChange;
	double *m_pWaveBuf;
	int m_nBufSize;
	double m_fTimeCount;
	double m_fBlockTime;
	int m_nTimeCount;
	BOOL m_bRegist;
	BOOL m_bNoRegist;
	class CDataRecordDlg *m_pDataRecord;
	CScreenShot m_oScreenShot;
	class CWaveInEx *m_pWaveIn;
	BOOL m_bEnableCsvOutput;
	class CFftLevelDlg *m_pFftLevelDlg;
	class CFftFreqDlg *m_pFftFreqDlg;

	void EnableFftWindow();
	void DisableFftWindow();
	CRect GetFftWindowRect();
	void SelchangeFftTab();
	void EnableControls();
	void EnableCsvOutput();
	void DispMessage(int msgNo);
	void SetFFT();
	void ChangeDispMode(FFTWINDOW *pFftWindow = NULL);
	void DispResolution();
	BOOL Start();
	void Stop(BOOL bModeChange = FALSE);
	void SetMicCalData();
	void ShowChildWindow(BOOL bShow);
	void GetInputDevice(int *pInputDevice, int *pInputVolume);
	void SetInputDevice(int nInputDevice, int nInputVolume);
	virtual void ChangeWaveDevice(int nInOut);
	void ChangeSamplingRate(int nSamplingRate);
	void OpenDataRecord();
	void CloseDataRecord();
	void ChangeDataRecord();
	void RecordData();
	void DeleteRecordData();
	void ClearPeak();
	void SeparatePage(int nMode, RECT rectWindow);
	int OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify);
	void RedrawSpectrogram();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	LRESULT OnThemeChanged();
	afx_msg LRESULT OnTabDrag(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTabDblClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFreqLevelLeft(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFreqLevelRight(WPARAM wParam, LPARAM lParam);
	afx_msg void OnStart();
	afx_msg void OnSelchangeSamplingRate();
	afx_msg void OnSelchangeFftTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFftAutoFreq();
	afx_msg void OnFftAutoLevel();
	afx_msg void OnSelchangeFftSmoothing();
	afx_msg void OnSelchangeFftFftSize();
	afx_msg void OnFftLinearScale();
	afx_msg void OnFftLogScale();
	afx_msg void OnChangeFftMaxFreq();
	afx_msg void OnChangeFftMaxLevel();
	afx_msg void OnChangeFftMinFreq();
	afx_msg void OnChangeFftMinLevel();
	afx_msg void OnSelchangeFftWinfunc();
	afx_msg void OnFftPeakDisp();
	afx_msg void OnFftPeakHold();
	afx_msg void OnFftTimeFwd();
	afx_msg void OnFftTimeBwd();
	afx_msg void OnSelchangeFftTimeDataNum();
	afx_msg void OnDestroy();
	afx_msg void OnFftCfAuto();
	afx_msg void OnFftCfLateral();
	afx_msg void OnFftCalibration();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSelchangeFftFilter();
//	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnRlSplit();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnFftPeakAuto();
	afx_msg void OnFftPeakManual();
	afx_msg void OnChangeFftPeakTime();
	afx_msg void OnFftPeakReset();
	afx_msg void OnSelchangeFftTimeRes();
	afx_msg void OnSelchangeFftAverage();
	afx_msg LRESULT OnDataRecord(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSelchangeFftTimeRange();
	afx_msg void OnFftColor();
	afx_msg void OnFftGray();
	afx_msg void OnSelchangeFftOctBand();
	afx_msg void OnBnClickedDataRecord();
	afx_msg void OnBnClickedScreenShot();
	afx_msg void OnSelchangeFftChannel();
	afx_msg void OnEnChangeFftAutoStop();
	afx_msg void OnBnClickedCsvOutput();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnMyCtrlRButtonDown(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedCsvInput();
};
