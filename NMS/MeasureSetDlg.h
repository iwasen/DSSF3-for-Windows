#pragma once

class CMeasureSetDlg : public CDialogAF
{
public:
	CMeasureSetDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

protected:
	enum { IDD = IDD_MEASURE_SET };

	CButton	m_cStartCurrentTime;
	CButton	m_cEndCurrentTime;
	CDateTimeCtrl	m_cStartTime;
	CDateTimeCtrl	m_cStartDate;
	CDateTimeCtrl	m_cEndTime;
	CDateTimeCtrl	m_cEndDate;
	CEdit	m_cIdentBeforeStep;
	CEdit	m_cIdentAfterStep;
	CComboBox	m_cSamplingRate;
	CComboBox	m_cFreqWeighting;
	double	m_dIntegrationTime;
	double	m_dMaxDelayTime;
	double	m_dRunningStep;
	double	m_dSamplingLevel;
	double	m_dTaueEndLevel;
	double	m_dTaueEndTime;
	double	m_dTaueInterval;
	double	m_dTimeConstant;
	BOOL	m_bTaueOrigin;
	int		m_iIdentMode;
	int		m_iIdentAfterStep;
	int		m_iIdentBeforeStep;
	CTime	m_tEndDate;
	CTime	m_tEndTime;
	CTime	m_tStartDate;
	CTime	m_tStartTime;
	BOOL	m_bEndDateTime;
	BOOL	m_bStartDateTime;
	int		m_iSamplingTiming;
	double	m_dSamplingTime1;
	double	m_dSamplingTime2;
	BOOL	m_bRelativeLevel;

	void EnableControl();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnIdentSpl();
	afx_msg void OnIdentTaue();
	afx_msg void OnStartDateTime();
	afx_msg void OnEndDateTime();
	afx_msg void OnStartCurrentTime();
	afx_msg void OnEndCurrentTime();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
