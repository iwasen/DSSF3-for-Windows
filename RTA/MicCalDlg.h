#pragma once

#include "FileIO.h"
#include "MicCal2.h"
#include "WaveInEx.h"
#include "WaveOutEx.h"

class CMicCalDlg : public CDialogAF, public IWaveNotify
{
public:
	CMicCalDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ
	~CMicCalDlg();

	long m_MicCalID;

protected:
	enum { IDD = IDD_MICCAL };

	CListCtrl m_cMicCalList;
	CStatic	m_cSeparator;
	CButton	m_cSetting;
	CButton	m_cRegist;
	CButton	m_cDelete;
	CButton	m_cChange;
	CMyButton m_cCommonRL;
	CMyButton m_cSeparateRL;
	CMyTab m_cMicCalTab;
	class CWaveInEx *m_pWaveIn;
	class CWaveOutEx *m_pWaveOut;
	CMicCal2 *m_pMicCalL;
	CMicCal2 *m_pMicCalR;
	int m_nSortItem;
	int m_nSortDir;
	CSize m_WindowSize;
	BOOL m_bSetting;
	BOOL m_bSignalOut;
	double m_fAngle;

	void InitMicCalList();
	void SetMicCalList();
	int GetCurSel();
	void SetCurSel(DWORD lParam);
	static int CALLBACK CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	void SaveCheck();
	void SetTabCtrl();
	void DeleteTabCtrl();
	int OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify);
	void WaveOutData(LPWAVENOTIFY pWaveNotify);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnItemchangedMiccalList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRegist();
	afx_msg void OnChange();
	afx_msg void OnDelete();
	afx_msg void OnSetting();
	afx_msg void OnColumnclickMiccalList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRead();
	afx_msg void OnDblclkMiccalList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnReset();
	afx_msg void OnBnClickedCommonRl();
	afx_msg void OnBnClickedSeparateRl();
	afx_msg void OnSignalOut();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
