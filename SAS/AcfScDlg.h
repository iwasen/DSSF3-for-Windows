#pragma once

#include "MyCtrl.h"

class CAcfScDlg : public CDialogAF
{
public:
	CAcfScDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

	BOOL m_bNoWave;
	int m_nChannel;

protected:
	enum { IDD = IDD_ACF_SCREEN_COPY };

	CButton	m_cMMLIB;
	CMyButton	m_cScTauIACC;
	CMyButton	m_cScWIACC;
	CMyButton	m_cScIACF;
	CMyButton	m_cScWindow;
	CMyButton	m_cScGraph;
	CMyButton	m_cScTauE;
	CMyButton	m_cScTau1;
	CMyEdit	m_cScRowSpace;
	CMyButton	m_cScPhi1;
	CMyButton	m_cScPhi0;
	CMyButton	m_cScMulti;
	CMyButton	m_cScIACC;
	CMyButton	m_cScFrame;
	CMyEdit	m_cScColumns;
	CMyEdit	m_cScColSpace;
	CMyButton	m_cScACF;

	void EnableControl();
	void SaveSetData();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnScWindow();
	afx_msg void OnScGraph();
	afx_msg void OnScMulti();
	afx_msg void OnMmlib();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
