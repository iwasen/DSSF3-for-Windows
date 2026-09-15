#pragma once

#include "FreqWnd.h"
#include "MakeFilter.h"
#include "MyCtrl.h"
#include "FileIO.h"
#include "WaveNotify.h"
#include <mmsystem.h>

#define MAX_CONTROL_ID	20
#define N_MEAN	5

class CInputDeviceDlg : public CDialogAF, public IWaveNotify
{
public:
	CInputDeviceDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ
	~CInputDeviceDlg();

	long m_MicCalID;

protected:
	enum { IDD = IDD_INPUT_DEVICE };

	CButton	m_cChange;
	CButton	m_cSetting;
	CStatic	m_cSeparator;
	CButton	m_cRegist;
	CListCtrl	m_cMicCalList;
	CScrollBar	m_cLevelBar;
	CMyEdit	m_cInputLevel;
	CFreqWnd	m_cFreqResponse;
	CListCtrl	m_cFreqList;
	CComboBox	m_cFilter;
	CButton	m_cDelete;
	CSliderCtrl	m_cInputVolume;
	CComboBox	m_cInputSelector;
	CComboBox	m_cInputDevice;
	CString	m_sComment;
	double	m_fFreq;
	double	m_fLevel;
	CString	m_sMicAmpInfo;
	CString	m_sMicInfo;
	BOOL	m_bSignalOut;
	DWORD m_dwDstControlID;
	DWORD m_cMultipleItems;
	DWORD m_cSrcItems;
	DWORD m_dwSrcRecVolID[MAX_CONTROL_ID];
	DWORD m_dwSrcRecMuteID[MAX_CONTROL_ID];
	int m_nSelectInput;
	int m_nInputDevice;
	int m_nSortItem;
	int m_nSortDir;
	double *m_pFreqData;
	BOOL m_bUpdate;
	CSize m_WindowSize;
	double m_fAngle;
	double *m_pFilterTbl;
	CRFFT m_oRFFT;
	double m_fMeanBuf[N_MEAN];
	int m_nMeanBufPtr;
	double m_fInputSens;
	CFont m_Font;

	void InitInputDevice();
	void SetInputSelect(int nInputDevice);
	DWORD GetInputVolume(DWORD dwControlID);
	void SetInputVolume(DWORD dwControlID, DWORD dwLevel);
	void GetInputDevice(int *pInputDevice, int *pInputVolume);
	void SetInputDevice(int nInputDevice, int nInputVolume);
	void ScrollLevelBar(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void InitMicCalList();
	void InitFreqList();
	void SetMicCalList();
	int GetCurSel();
	void SetCurSel(DWORD lParam);
	void DispFreqWindow();
	static int CALLBACK CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK CompareFreq(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	void MakeFreqData();
	void GetMicCalData(DbMicCalRec *pDbMicCalRec);
	int GetFreqData(FilterData *pFilterData);
	void SaveCheck();
	void WaveOutData(LPWAVENOTIFY pWaveNotify);
	void WaveInData(LPWAVENOTIFY pWaveNotify);
	void SetFilter(FilterData *pFilterData, int nFilterData);
	int OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeInputDevice();
	afx_msg void OnSelchangeInputSelector();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDestroy();
	afx_msg void OnItemchangedMiccalList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedFreqList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRegist();
	afx_msg void OnChange();
	afx_msg void OnDelete();
	afx_msg void OnFreqRegist();
	afx_msg void OnFreqDelete();
	afx_msg void OnColumnclickMiccalList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRead();
	afx_msg void OnDblclkMiccalList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReset();
	afx_msg void OnChangeMicinfo();
	afx_msg void OnChangeMicampinfo();
	afx_msg void OnInsertitemFreqList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitemFreqList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetting();
	afx_msg void OnSignalOut();
	afx_msg void OnSelchangeFilter();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
