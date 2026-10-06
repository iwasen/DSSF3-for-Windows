#pragma once

#include "BaseDlg.h"
#include "ImpWnd.h"
#include "MSeq.h"
#include "TSP.h"
#include "fft.h"
#include "PlaybackBtn.h"
#include "MyCtrl.h"
#include "afxwin.h"

class CImpulseDlg : public CBaseDlg, public IWaveNotify
{
public:
	CImpulseDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ
	~CImpulseDlg();

protected:
	friend class CIRAssistDlg;

	enum { IDD = IDD_IMPULSE };

	CButton	m_cFileLoad;
	CButton	m_cMSeqMethod;
	CButton	m_cTSPMethod;
	CMyButton	m_cAutoRetry;
	CMyEdit	m_cZoomH;
	CButton	m_cIfaSet;
	CButton	m_cChannelMono;
	CButton	m_cChannelStereo;
	CButton	m_cAssist;
	CPlaybackBtn	m_cPlayback;
	CButton m_cLaunchSA;
	CMyButton	m_cAutoLevel;
	CMyEdit	m_cMessage;
	CMyEdit	m_cMeasureNum;
	CMyEdit	m_cIfaMessage;
	CButton	m_cIfaCancel;
	CButton	m_cIfaSave;
	CButton	m_cWaveOut;
	CMyEdit	m_cMrTime;
	CMyEdit	m_cMrSampling;
	CMyEdit	m_cMrMTime;
	CMyEdit	m_cMrMNum;
	CMyEdit	m_cMrChannel;
	CMyEdit	m_cMrBit;
	CButton	m_cFileSave;
	CSpinButtonCtrl	m_cImpMeasureNumSpin;
	CButton	m_cBtnStop;
	CComboBox	m_cMeasureTime;
	CImpWnd	m_cImpulseR;
	CImpWnd	m_cImpulseL;
	CScrollBar	m_cGraphScroll;
	CComboBox	m_cSamplingRate;
	CButton	m_cBtnStart;
	CButton m_cExit;
	CMyEdit m_cEditOffsetTime;
	int m_iChannel;
	int m_iWaveOut;
	int m_iFileFormat;
	int m_iMethod;
	CToolBar m_tbZoomH;
	CMSeq m_oMSeq;
	CTSP m_oTSP;
	CIRMethod *m_pMethod;
	double *m_pWaveOutData;
	double *m_pLeftInputData;
	double *m_pRightInputData;
	double *m_pLeftImpulseData;
	double *m_pRightImpulseData;
	int m_nStage;
	int m_nData;
	int m_nChannel;
	int m_nRate;
	int m_nOutPtr;
	int m_nOutCounter;
	BOOL m_bOutEnd;
	int m_nInPtr;
	int m_nInCounter;
	BOOL m_bInEnd;
	int m_nWaveBufNum;
	double m_fTotalTime;
	double m_fStartTime;
	double m_fDispTime;
	int m_nRetryCounter;
	int m_nMeasureNum;
	CString m_sTime;
	CRFFT m_oRFFT;
	double *m_pIFilter;
	int m_nIFilter;
	long m_nIFilterID;
	int m_nMaxAdjLevel;
	BOOL m_bPhaseAdj;
	int m_nIfaSampling;
	BOOL m_bInitialized;
	int m_nScrollSize;
	int m_nScrollPos;
	double *m_pCheckBuf;
	int m_nCheckCount;
	CIRAssistDlg *m_pAssistDlg;
	int m_nMultiplier;
	BOOL m_bImpulseData;
	int m_nMethod;
	class CWaveInEx *m_pWaveIn;
	class CWaveOutEx *m_pWaveOut;
	BOOL m_bValidData;
	int m_nBitsPerSample;

	void StartMeasuring();
	void SetMeasuringParameters(int nRate, int mseq, int nMeasureNum, int nChannel, int nBit, const CString &currentTime, int nMethod, int nData = 0);
	BOOL Start();
	void CancelMeasuring();
	void Stop();
	void FreeBuffers();
	void EndProcess();
	void ShiftCopy(double *pTo, double *pFrom, int nData, int nShift);
	int TimingCheck(const double *pData1, const double *pData2, int nCheckDataNum);
	BOOL LevelCheck(const double *pData);
	void DispImpulse();
	void SetTimeList();
	void EnableImpWindow();
	void InitDispTime();
	void DispImpWindow();
	void StartStopButton(BOOL bStart);
	void LoadImpulseData(long nImpulseID);
	void LoadWaveFile(LPCSTR pFileName);
	void DispMeasureParam();
	void ValidData(BOOL bValid);
	void InverseConvolution(double *pWaveData, int nWaveData, const double *pImpulseData, int nImpulseData);
	void WaveFileOut();
	void TextFileOut();
	static void CALLBACK GraphCallBack(WPARAM code, LPARAM data, double param1, double param2);
	void EnableControl(BOOL bEnable);
	virtual void ChangeWaveDevice(int nInOut);
	int OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify);
	BOOL SaveImpulse(HWAVEDATA hWaveData);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	afx_msg void OnStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSelchangeSamplingRate();
	afx_msg void OnSelchangeMeasureTime();
	afx_msg void OnChannelMono();
	afx_msg void OnChannelStereo();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnZoomInH();
	afx_msg void OnZoomOutH();
	afx_msg void OnZoomFullH();
	afx_msg void OnStop();
	afx_msg void OnFileSave();
	afx_msg void OnFileLoad();
	afx_msg void OnDestroy();
	afx_msg void OnWaveOut();
	afx_msg void OnIfaSet();
	afx_msg void OnIfaCancel();
	afx_msg void OnIfaSave();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnChangeMeasureNum();
	afx_msg void OnFileText();
	afx_msg void OnFileWave();
	afx_msg void OnWaveIr();
	afx_msg void OnWaveInMseq();
	afx_msg void OnWaveOutMsrq();
	afx_msg void OnAssist();
	afx_msg void OnAutoLevel();
	afx_msg void OnPlayback();
	afx_msg void OnAutoRetry();
	afx_msg void OnMSeqMethod();
	afx_msg void OnTspMethod();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedLaunchSa();
	afx_msg void OnEnChangeEditOffsetTime();
};
