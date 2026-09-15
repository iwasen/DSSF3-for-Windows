#pragma once

#include "MyCtrl.h"

class CImpScDlg : public CDialogAF
{
public:
	CImpScDlg(CWnd* pParent = NULL);   // 標準コンストラクタ

	int m_nChannel;

protected:
	enum { IDD = IDD_IMP_SCREEN_COPY };

	CMyButton m_cMMLIB;
	CMyButton m_cScWindow;
	CMyButton m_cScGraph;
	CMyButton m_cScMulti;
	CMyButton m_cScFrame;
	CMyButton m_cScImp;
	CMyButton m_cScEcho;
	CMyButton m_cScIACC;
	CMyButton m_cScFreq;
	CMyButton m_cScPref;
	CMyButton m_cScParam;
	CMyButton m_cScMTF;
	CMyEdit m_cScColumns;
	CMyEdit m_cScColSpace;
	CMyEdit m_cScRowSpace;

	void EnableControl();
	void SaveSetData();

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnScWindow();
	afx_msg void OnBnClickedScMulti();
	afx_msg void OnScGraph();
	afx_msg void OnScMulti();
	afx_msg void OnMmlib();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
