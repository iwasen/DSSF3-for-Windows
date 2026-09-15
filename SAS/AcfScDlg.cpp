// AcfScDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "AcfScDlg.h"
#include "SetData.h"
#include "Help\ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CAcfScDlg ダイアログ

CAcfScDlg::CAcfScDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CAcfScDlg::IDD, pParent)
{
}

void CAcfScDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MMLIB, m_cMMLIB);
	DDX_Control(pDX, IDC_SC_TAUIACC, m_cScTauIACC);
	DDX_Control(pDX, IDC_SC_WIACC, m_cScWIACC);
	DDX_Control(pDX, IDC_SC_IACF, m_cScIACF);
	DDX_Control(pDX, IDC_SC_WINDOW, m_cScWindow);
	DDX_Control(pDX, IDC_SC_GRAPH, m_cScGraph);
	DDX_Control(pDX, IDC_SC_TAUE, m_cScTauE);
	DDX_Control(pDX, IDC_SC_TAU1, m_cScTau1);
	DDX_Control(pDX, IDC_SC_ROWSPACE, m_cScRowSpace);
	DDX_Control(pDX, IDC_SC_PHI1, m_cScPhi1);
	DDX_Control(pDX, IDC_SC_PHI0, m_cScPhi0);
	DDX_Control(pDX, IDC_SC_MULTI, m_cScMulti);
	DDX_Control(pDX, IDC_SC_IACC, m_cScIACC);
	DDX_Control(pDX, IDC_SC_FRAME, m_cScFrame);
	DDX_Control(pDX, IDC_SC_COLUMNS, m_cScColumns);
	DDX_Control(pDX, IDC_SC_COLSPACE, m_cScColSpace);
	DDX_Control(pDX, IDC_SC_ACF, m_cScACF);
}

BEGIN_MESSAGE_MAP(CAcfScDlg, CDialogAF)
	ON_BN_CLICKED(IDC_SC_WINDOW, OnScWindow)
	ON_BN_CLICKED(IDC_SC_GRAPH, OnScGraph)
	ON_BN_CLICKED(IDC_SC_MULTI, OnScMulti)
	ON_BN_CLICKED(IDC_MMLIB, OnMmlib)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAcfScDlg メッセージ ハンドラ

BOOL CAcfScDlg::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	m_cScWindow = (setData.AcfSC.nCopyMode == 0);
	m_cScGraph = (setData.AcfSC.nCopyMode == 1);
	m_cScFrame = setData.AcfSC.bFrame;
	m_cScMulti = setData.AcfSC.bMulti;
	m_cScPhi0 = setData.AcfSC.bPhi0;
	m_cScTauE = setData.AcfSC.bTaue;
	m_cScTau1 = setData.AcfSC.bTau1;
	m_cScPhi1 = setData.AcfSC.bPhi1;
	m_cScIACC = setData.AcfSC.bIACC;
	m_cScTauIACC = setData.AcfSC.bTauIACC;
	m_cScWIACC = setData.AcfSC.bWIACC;
	m_cScACF = setData.AcfSC.bACF;
	m_cScIACF = setData.AcfSC.bIACF;
	m_cScColumns = setData.AcfSC.nColumns;
	m_cScColSpace = setData.AcfSC.nColSpace;
	m_cScRowSpace = setData.AcfSC.nRowSpace;

	if (!CheckMMLib())
		m_cMMLIB.EnableWindow(FALSE);

	EnableControl();

	return TRUE;
}

void CAcfScDlg::OnOK()
{
	SaveSetData();

	EndDialog(IDOK);
}

void CAcfScDlg::OnScWindow()
{
	EnableControl();
}

void CAcfScDlg::OnScGraph()
{
	EnableControl();
}

void CAcfScDlg::OnScMulti()
{
	EnableControl();
}

void CAcfScDlg::EnableControl()
{
	BOOL bGraph = m_cScGraph;
	BOOL bMulti = bGraph && (BOOL)m_cScMulti;
	BOOL bStereo = (m_nChannel == 2);

	m_cScFrame.EnableWindow(bGraph);
	m_cScMulti.EnableWindow(bGraph);
	m_cScPhi0.EnableWindow(bMulti);
	m_cScTauE.EnableWindow(bMulti);
	m_cScTau1.EnableWindow(bMulti);
	m_cScPhi1.EnableWindow(bMulti);
	m_cScIACC.EnableWindow(bMulti && bStereo);
	m_cScTauIACC.EnableWindow(bMulti && bStereo);
	m_cScWIACC.EnableWindow(bMulti && bStereo);
	m_cScACF.EnableWindow(bMulti && !m_bNoWave);
	m_cScIACF.EnableWindow(bMulti && !m_bNoWave && bStereo);
	m_cScColumns.EnableWindow(bMulti);
	m_cScColSpace.EnableWindow(bMulti);
}

void CAcfScDlg::SaveSetData()
{
	setData.AcfSC.nCopyMode = m_cScGraph ? 1 : 0;
	setData.AcfSC.bFrame = m_cScFrame;
	setData.AcfSC.bMulti = m_cScMulti;
	setData.AcfSC.bPhi0 = m_cScPhi0;
	setData.AcfSC.bTaue = m_cScTauE;
	setData.AcfSC.bTau1 = m_cScTau1;
	setData.AcfSC.bPhi1 = m_cScPhi1;
	setData.AcfSC.bIACC = m_cScIACC;
	setData.AcfSC.bTauIACC = m_cScTauIACC;
	setData.AcfSC.bWIACC = m_cScWIACC;
	setData.AcfSC.bACF = m_cScACF;
	setData.AcfSC.bIACF = m_cScIACF;
	setData.AcfSC.nColumns = m_cScColumns;
	setData.AcfSC.nColSpace = m_cScColSpace;
	setData.AcfSC.nRowSpace = m_cScRowSpace;
}

void CAcfScDlg::OnMmlib()
{
	SaveSetData();

	EndDialog(IDC_MMLIB);
}

BOOL CAcfScDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDOK, IDH_ACF_SC_OK,
		IDC_MMLIB, IDH_ACF_SC_MMLIB,
		IDCANCEL, IDH_ACF_SC_CANCEL,
		IDC_SC_WINDOW, IDH_ACF_SC_WINDOW,
		IDC_SC_GRAPH, IDH_ACF_SC_GRAPH,
		IDC_SC_FRAME, IDH_ACF_SC_FRAME,
		IDC_SC_MULTI, IDH_ACF_SC_MULTI,
		IDC_SC_PHI0, IDH_ACF_SC_PHI0,
		IDC_SC_TAUE, IDH_ACF_SC_TAUE,
		IDC_SC_TAU1, IDH_ACF_SC_TAU1,
		IDC_SC_PHI1, IDH_ACF_SC_PHI1,
		IDC_SC_IACC, IDH_ACF_SC_IACC,
		IDC_SC_TAUIACC, IDH_ACF_SC_TAUIACC,
		IDC_SC_WIACC, IDH_ACF_SC_WIACC,
		IDC_SC_ACF, IDH_ACF_SC_ACF,
		IDC_SC_IACF, IDH_ACF_SC_IACF,
		IDC_SC_COLUMNS, IDH_ACF_SC_COLUMNS,
		IDC_SC_COLSPACE, IDH_ACF_SC_COLSPACE,
		IDC_SC_ROWSPACE, IDH_ACF_SC_ROWSPACE,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
