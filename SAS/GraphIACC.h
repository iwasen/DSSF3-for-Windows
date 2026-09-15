#pragma once

#include "IACCWnd.h"
#include "FileIO.h"
#include "mmdb.h"

class CGraphIACC : public CDialogAF
{
public:
	CGraphIACC(CWnd* pParent = NULL);   // 標準のコンストラクタ

	class CImpFactor *m_pParent;
	DbImpulseRec *m_pDbImpulseRec;
	DbAcParamRec *m_pDbAcParamRec;
	double *m_pIRLeft;
	double *m_pIRRight;
	double m_fRate;
	HWAVEDATA m_hWaveData;

	void ReDraw();

protected:
	enum { IDD = IDD_GRAPH_IACC };

	CButton	m_cChangeWIACC;
	CIACCWnd	m_cGraphIACC;
	int m_nData;
	double *m_pIACCData;
	int m_nIACCData;
	BOOL m_bReDraw;
	double m_fIACC, m_TIACC, m_WIACC, m_WIACC1, m_WIACC2;

	void DispIACCWindow(BOOL bCalc);

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	virtual void OnCancel() {};
	virtual void OnOK() {};
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnChangeWiacc();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};

/////////////////////////////////////////////////////////////////////////////
// CChangeWIACCDlg ダイアログ

class CChangeWIACCDlg : public CDialogAF
{
public:
	CChangeWIACCDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

	double	m_dWIACC;

protected:
	enum { IDD = IDD_CHANGE_WIACC };

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
