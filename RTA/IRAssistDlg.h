#pragma once

#include "MSeq.h"
#include "fft.h"
#include "IRAssistWnd.h"

class CIRAssistDlg : public CDialogAF, public IWaveNotify
{
public:
	CIRAssistDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ
	~CIRAssistDlg();

protected:
	friend class CImpulseDlg;

	enum { IDD = IDD_IR_ASSIST };

	CMyEdit	m_cDeviation;
	CMyEdit	m_cResult3;
	CMyEdit	m_cResult2;
	CMyEdit	m_cResult1;
	CIRAPositionWnd	m_cPositionRight;
	CIRAPositionWnd	m_cPositionLeft;
	CMyEdit	m_cSNRatio;
	CMyEdit	m_cMeasuringTime;
	CIRALevelWnd	m_cLevelWnd;
	CButton	m_cSwitch1;
	CButton	m_cSwitch2;
	CButton	m_cSwitch3;
	CMyEdit	m_cDistance;
	CImpulseDlg *m_pImpulseDlg;
	CMSeq m_MSeq;
	int m_nData;
	double *m_pCcfBuf1;
	double *m_pCcfBuf2;
	CRFFT m_oRFFT;
	int m_nSwitchNo;
	BOOL m_bStart;
	int m_nTimeSave;
	double m_fLevel;
	double m_fPosition;
	int m_nPosition;
	CFont m_Font;
	class CWaveInEx *m_pWaveIn;
	class CWaveOutEx *m_pWaveOut;

	void StartAdjust(int nSwitchNo);
	BOOL StartAdjustLevel();
	BOOL StartAdjustPosition();
	BOOL StartAdjustTime();
	void StartMeasuringIR();
	void StopAdjust();
	void WaveAdjustLevel(LPWAVENOTIFY pWaveNotify);
	void WaveAdjustPosition(LPWAVENOTIFY pWaveNotify);
	void EndIR();
	void FreeBuffers();
	void SetSwitch(int nSwitchNo);
	int OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSwitch1();
	afx_msg void OnSwitch2();
	afx_msg void OnSwitch3();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
