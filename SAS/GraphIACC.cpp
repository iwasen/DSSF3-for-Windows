// GraphIACC.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "GraphIACC.h"
#include "ImpFactor.h"
#include "Calc.h"
#include "Help\ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CGraphIACC ダイアログ

CGraphIACC::CGraphIACC(CWnd* pParent /*=NULL*/)
	: CDialogAF(CGraphIACC::IDD, pParent)
{
	m_pIACCData = NULL;
	m_bReDraw = FALSE;
}

void CGraphIACC::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHANGE_WIACC, m_cChangeWIACC);
	DDX_Control(pDX, IDC_GRAPH_IACC, m_cGraphIACC);
}

BEGIN_MESSAGE_MAP(CGraphIACC, CDialogAF)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CHANGE_WIACC, OnChangeWiacc)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphIACC メッセージ ハンドラ

BOOL CGraphIACC::OnInitDialog()
{
	CWaitCursor waitCursor;
	CString str;

	CDialogAF::OnInitDialog();

	str.LoadString(IDS_CCF);
	m_cGraphIACC.Initialize(90, str, 4);

	m_nData = 1 << m_pDbImpulseRec->nMeasTime;
	m_nIACCData = (int)(m_fRate / 500);
	m_nIACCData += m_nIACCData - 1;

	m_pIACCData = new double[m_nIACCData];

	DispIACCWindow(TRUE);

	if (m_pDbAcParamRec == NULL)
		m_cChangeWIACC.EnableWindow(FALSE);

	return TRUE;
}

void CGraphIACC::OnDestroy()
{
	CDialogAF::OnDestroy();

	if (m_pIACCData != NULL) {
		delete [] m_pIACCData;
		m_pIACCData = NULL;
	}
}

void CGraphIACC::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogAF::OnShowWindow(bShow, nStatus);

	if (bShow && m_bReDraw) {
		DispIACCWindow(TRUE);
		m_bReDraw = FALSE;
	}
}

void CGraphIACC::ReDraw()
{
	if (m_hWnd != NULL) {
		if (IsWindowVisible())
			DispIACCWindow(TRUE);
		else
			m_bReDraw = TRUE;
	}
}

void CGraphIACC::DispIACCWindow(BOOL bCalc)
{
	if (bCalc) {
		double fWIACCLevel;

		CalcIACCData(m_pIRLeft, m_pIRRight, m_nData, m_pIACCData, m_nIACCData);

		if (m_pDbAcParamRec != NULL)
			fWIACCLevel = m_pDbAcParamRec->dbAcParamCond.fWIACCLevel;
		else
			fWIACCLevel = setData.CalcParam.fWIACCLevel;
		CalcParamIACC(m_pIACCData, m_nIACCData, m_fRate, &m_fIACC, &m_TIACC, &m_WIACC, fWIACCLevel, &m_WIACC1, &m_WIACC2);
	}

	m_cGraphIACC.DispGraph(m_pIACCData, m_nIACCData, m_fRate, m_fIACC, m_TIACC, m_WIACC1, m_WIACC2);
}

void CGraphIACC::OnChangeWiacc()
{
	CChangeWIACCDlg dlg(this);

	dlg.m_dWIACC = m_pDbAcParamRec->dbAcParamCond.fWIACCLevel;
	if (dlg.DoModal() == IDOK) {
		CWaitCursor waitCursor;
		m_pDbAcParamRec->dbAcParamCond.fWIACCLevel = dlg.m_dWIACC;
		CalcIACC(m_pDbImpulseRec, m_hWaveData, m_pDbAcParamRec, m_fRate);
		m_pParent->ChangeData();
	}
}

BOOL CGraphIACC::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_GRAPH_IACC, IDH_GRAPH_IACC,
		IDC_CHANGE_WIACC, IDH_GRAPH_IACC_CHANGE_WIACC,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CChangeWIACCDlg ダイアログ

CChangeWIACCDlg::CChangeWIACCDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CChangeWIACCDlg::IDD, pParent)
{
	m_dWIACC = 0.0;
}

void CChangeWIACCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_IACC_WLEVEL, m_dWIACC);
}

BEGIN_MESSAGE_MAP(CChangeWIACCDlg, CDialogAF)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeWIACCDlg メッセージ ハンドラ

BOOL CChangeWIACCDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDOK, IDH_CHANGE_WIACC_OK,
		IDCANCEL, IDH_CHANGE_WIACC_CANCEL,
		IDC_IACC_WLEVEL, IDH_CHANGE_WIACC_WLEVEL,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
