#pragma once

#include "Indicator.h"
#include "TimeWnd.h"
#include "BaseDlg.h"
#include "WaveNotify.h"

#define MAX_CONTROL_ID	20

class CRtaDlg : public CDialogAF, public IWaveNotify
{
public:
	CRtaDlg(CWnd* pParent = NULL);	// 標準のコンストラクタ

	void OpenRaWindow(int nWindow);
	void CloseRaWindow(int nWindow);
	void CloseRaWindow(CWnd *pWnd);
	void GetInputDevice(int *pInputDevice, int *pInputVolume);
	void SetInputDevice(int nInputDevice, int nInputVolume);
	void UpdownInputVolume(int nUpdown);
	void OpenAutoStartFile(LPCSTR pAutoRecFile);

	CBaseDlg *m_pWndSg;
	CBaseDlg *m_pWndFft;
	CBaseDlg *m_pWndOs;
	CBaseDlg *m_pWndFre;
	CBaseDlg *m_pWndDst;
	CBaseDlg *m_pWndImp;
	CBaseDlg *m_pWndAcf;
	CBaseDlg *m_pWndRec;
	CBaseDlg *m_pWndFilter;
	CTimeWnd *m_pWndTime;

protected:
	enum { IDD = IDD_RTA_DIALOG };

	CToolBar	m_wndToolBar;
	CIndicator	m_cOutputLevel;
	CIndicator	m_cInputLevel;
	CSliderCtrl	m_cInputVolume;
	CComboBox	m_cInputSelect;
	CButton m_cRaSg;
	CButton m_cRaFft;
	CButton m_cRaOs;
	CButton m_cRaFre;
	CButton m_cRaDst;
	CButton m_cRaImp;
	CButton m_cRaAcf;
	CButton m_cRaRec;
	CButton m_cRaFilter;
	CComboBox m_cInputDevice;
	CComboBox m_cOutputDevice;
	HICON m_hIcon;
	HICON m_hIcon16;
	DWORD m_dwDstControlID;
	DWORD m_dwMultipleItems;
	DWORD m_dwSrcItems;
	DWORD m_dwSrcRecVolID[MAX_CONTROL_ID];
	DWORD m_dwSrcRecMuteID[MAX_CONTROL_ID];
	int m_nSelectInput;
	int m_nInputDevice;

	void InitialPreset();
	void OpenPreset(int nPresetID);
	void SetSoundDevice();
	BOOL SetInputSelect();
	void MuteMicAndLineIn();
	DWORD GetInputVolume(DWORD dwControlID);
	void SetInputVolume(DWORD dwControlID, DWORD dwLevel);
	void SelectSoundDevice();
	void ChangeWaveDevice(int nInOut);
	int OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify);
	HDIB CreateDIB(CWnd *pWnd);
	void TransferToMmlib(CWnd *pWnd, const CString &sFolderTitle, const CString &sDataTitle);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV のサポート
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnRaSg();
	afx_msg void OnRaFft();
	afx_msg void OnRaFre();
	afx_msg void OnRaDst();
	afx_msg void OnRaImp();
	afx_msg void OnRaOs();
	afx_msg void OnRaAcf();
	afx_msg void OnRaRec();
	afx_msg void OnRaFilter();
	afx_msg void OnRaPreset();
	afx_msg void OnSelchangeRaInputSelect();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDestroy();
	afx_msg void OnSetting();
	afx_msg void OnCbnSelchangeRaInputDevice();
	afx_msg void OnCbnSelchangeRaOutputDevice();
	afx_msg void OnAboutbox();
	afx_msg void OnDocumentQa();
	afx_msg void OnTechnicalSupport();
	afx_msg void OnOnlineManual();
	afx_msg void OnOnlineUpdate();
	afx_msg void OnReadme();
	afx_msg void OnYmecStore();
	afx_msg void OnLicenseRegist();
	afx_msg void OnUpdateLicenseRegist(CCmdUI *pCmdUI);
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnRaTime();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnHelpFile();
	afx_msg void OnPdfFile();
	afx_msg void OnDataBackup();
	afx_msg void OnDataRestore();
	afx_msg LRESULT OnChangeFontSize(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTransferMmlib();
	afx_msg void OnBnClickedRaAutorec();
	afx_msg LRESULT OnAutoRecFile(WPARAM, LPARAM);
	afx_msg LRESULT OnNotifyDeviceChange(WPARAM, LPARAM);
	afx_msg void OnRaAutorec();
};
