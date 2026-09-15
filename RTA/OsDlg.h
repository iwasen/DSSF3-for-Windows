#pragma once

#include "BaseDlg.h"
#include "OsWin.h"
#include "ScreenShot.h"
#include "MyCtrl.h"
#include "WaveNotify.h"

class COsDlg : public CBaseDlg, public IWaveNotify
{
// コンストラクション
public:
	COsDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ
	~COsDlg();

protected:
	enum { IDD = IDD_OS };

	CMyButton	m_cOsTrigHighCut;
	CMyButton	m_cOsXY;
	CMyButton	m_cOsTrigSingle;
	CMyButton	m_cOsTrigPos;
	CMyButton	m_cOsTrigLowCut;
	CMyEdit	m_cOsTrigLevel2;
	CMyButton	m_cOsTrigFree;
	CMyButton	m_cOsTrigDisp;
	CMyButton	m_cOsTrigAuto;
	CMyEdit	m_cOsSweep2;
	CMyButton	m_cOsReverseR;
	CMyButton	m_cOsReverseL;
	CMyEdit	m_cOsPosR2;
	CMyEdit	m_cOsPosL2;
	CMyEdit	m_cOsLevelR2;
	CMyEdit	m_cOsLevelL2;
	CMyButton	m_cOsLevelAuto;
	CMyEdit	m_cOsDelay2;
	CComboBox	m_cOsDelayRange;
	CButton	m_cOsTrigReset;
	CButton	m_cBtnStart;
	CSliderCtrl	m_cOsTrigLevel;
	CSliderCtrl	m_cOsDelay;
	CSliderCtrl	m_cOsSweep;
	CComboBox	m_cSamplingRate;
	COsWin	m_cOsScreen;
	CSliderCtrl	m_cOsPosR;
	CSliderCtrl	m_cOsPosL;
	CSliderCtrl	m_cOsLevelR;
	CSliderCtrl	m_cOsLevelL;
	int		m_cOsTrigChannel;
	int		m_cOsTrigSlope;
	CComboBox m_cOverlay;
	CButton m_cScreenShot;
	CMyButton	m_cRendou;
	CMyButton	m_cOsZeroLevel;
	CMyEdit	m_cOsTime;
	CComboBox m_cOsChannel;
	int m_nBlockSize;
	BOOL m_bTrigger;
	double m_fTrigLevel;
	int m_nTrigCounter;
	BOOL m_bTrigSingle;
	BOOL m_bTrigSingleMode;
	double m_fTrigWork;
	double m_fSweepTime;
	int m_nSweepCounter;
	int m_nDelayTime;
	int m_nDelayCounter;
	int m_nLvAdjCounterL;
	int m_nLvAdjCounterR;
	int m_nDelayVal;
	double m_fAvrVal;
	int m_nLevelL;
	int m_nLevelR;
	double m_fLevelL;
	double m_fLevelR;
	double m_fSweepDiv;
	double m_fDelayTime;
	BOOL m_bInitialized;
	double m_fOffset;
	BOOL m_bMax;
	double *m_pInterpolationFilter;
	double *m_pInterpolationBufL;
	double *m_pInterpolationBufR;
	int m_nInterpolationPtr;
	double *m_pLeftData;
	double *m_pRightData;
	double m_fTimeCount;
	int m_nTimeCount;
	double m_fBlockTime;
	BOOL m_bRegist;
	BOOL m_bNoRegist;
	CScreenShot m_oScreenShot;
	class CWaveInEx *m_pWaveIn;
	BOOL m_bLch;
	BOOL m_bRch;
	double m_fLevelMul;
	double m_fLevelRangeMul;

	void SetSweepVal();
	void SetDelayVal();
	BOOL Start();
	void Restart();
	void Stop();
	void GetWaveData(double *pInData);
	BOOL CheckTrigger(int nOffset, double *pOffset);
	void GetTriggerLevel();
	void DispTriggerMark(BOOL bDisp);
	void InitScreen();
	void EnableControls();
	void EnableTrigReset();
	void AutoLevelAdjust(const double *pInData);
	void SetLevelL();
	void SetLevelR();
	void SetPosL();
	void SetPosR();
	void AutoTrigAdjust(const double *pInData);
	void DispInfo();
	void ShowChildWindow(BOOL bShow);
	void MakeInterpolationFilter(int n);
	int Interpolation(const double *pInData);
	void Convolution(double in, double *out, double *pInterpolationBuf);
	void AllocDataBuf();
	void FreeDataBuf();
	virtual void ChangeWaveDevice(int nInOut);
	void ChangeSamplingRate(int nSamplingRate);
	void SetChannelMode();
	int OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify);
	void SetLevelRange();
	CString AddNumberPrefix(double fNumber);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnOsReverseL();
	afx_msg void OnOsReverseR();
	afx_msg void OnSelchangeSamplingRate();
	afx_msg void OnOsTrigPlus();
	afx_msg void OnOsTrigAuto();
	afx_msg void OnOsTrigCh1();
	afx_msg void OnOsTrigCh2();
	afx_msg void OnOsTrigMinus();
	afx_msg void OnOsTrigReset();
	afx_msg void OnOsTrigSingle();
	afx_msg void OnStart();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnOsLevelAuto();
	afx_msg void OnOsTrigFree();
	afx_msg void OnOsTrigHighcut();
	afx_msg void OnOsTrigLowcut();
	afx_msg void OnOsTrigPos();
	afx_msg void OnOsTrigDisp();
	afx_msg void OnOsPosCenter();
	afx_msg void OnOsPosAlt();
	afx_msg void OnSelchangeOsDelayRange();
	afx_msg void OnOsXy();
	afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnCbnSelchangeOsOverlay();
	afx_msg void OnBnClickedScreenShot();
	afx_msg void OnBnClickedRendou();
	afx_msg void OnBnClickedOsZeroLevel();
	afx_msg void OnCbnSelchangeOsChannel();
	afx_msg void OnStnDblclickOsScreen();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnBnClickedOsCalibration();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
