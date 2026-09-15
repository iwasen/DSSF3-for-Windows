#pragma once

#include "BaseDlg.h"
#include "SG.h"
#include "SgTone.h"
#include "SgNoise.h"
#include "SgSweep.h"
#include "SgPulse.h"
#include "SgSynth.h"
#include "SgScale.h"
#include "SgWave.h"
#include "WaveNotify.h"

class CSgDlg : public CBaseDlg, public IWaveNotify
{
public:
	CSgDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ
	~CSgDlg();

	void StartStop(BOOL bStart);
	CSG m_oSG;

protected:
	enum { IDD = IDD_SG };

	CMyEdit	m_cSgTime;
	CMyEdit	m_cSgRLevel;
	CMyEdit	m_cSgLLevel;
	CMyButton	m_cRendou;
	CMyEdit	m_cSgDLevel;
	CSliderCtrl	m_cSgDSlider;
	CButton	m_cSgStart;
	CComboBox	m_cSamplingRate;
	CSliderCtrl	m_cSgLSlider;
	CSliderCtrl	m_cSgRSlider;
	CMyTab	m_cSgTab;
	CSgTone m_SgTone;
	CSgNoise m_SgNoise;
	CSgSweep m_SgSweep;
	CSgPulse m_SgPulse;
	CSgSynth m_SgSynth;
	CSgScale m_SgScale;
	CSgWave m_SgWave;
	int m_nChannel;
	BOOL m_bMove;
	double m_fTimeCount;
	double m_fBlockTime;
	int m_nTimeCount;
	class CWaveOutEx *m_pWaveOut;

	void SetLLevel(int nLevel);
	void SetLLevel2(int nLevel);
	void SetRLevel(int nLevel);
	void SetRLevel2(int nLevel);
	void SetDLevel(int nLevel);
	BOOL Start();
	void Stop();
	void SetVolume();
	int GetChannel();
	virtual void ChangeWaveDevice(int nInOut);
	int OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify);
	void EnableLevel();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnStart();
	afx_msg void OnSelchangeSgTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeSgLlevel();
	afx_msg void OnChangeSgRlevel();
	afx_msg void OnSelchangeSamplingRate();
	afx_msg LRESULT OnSgChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRendou();
	afx_msg void OnDestroy();
	afx_msg void OnChangeSgDlevel();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMove(int x, int y);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
