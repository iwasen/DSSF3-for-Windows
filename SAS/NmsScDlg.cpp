// NmsScDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "NmsScDlg.h"
#include "SetData.h"
#include "Help\ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CNmsScDlg ダイアログ

CNmsScDlg::CNmsScDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CNmsScDlg::IDD, pParent)
{
	m_bNoWave = FALSE;
}

void CNmsScDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MMLIB, m_cMMLIB);
	DDX_Control(pDX, IDC_SC_SPL, m_cScSPL);
	DDX_Control(pDX, IDC_SC_WINDOW, m_cScWindow);
	DDX_Control(pDX, IDC_SC_GRAPH, m_cScGraph);
	DDX_Control(pDX, IDC_SC_WIACC, m_cScWIACC);
	DDX_Control(pDX, IDC_SC_TAUIACC, m_cScTauIACC);
	DDX_Control(pDX, IDC_SC_TAUE, m_cScTauE);
	DDX_Control(pDX, IDC_SC_TAU1, m_cScTau1);
	DDX_Control(pDX, IDC_SC_ROWSPACE, m_cScRowSpace);
	DDX_Control(pDX, IDC_SC_PHI1, m_cScPhi1);
	DDX_Control(pDX, IDC_SC_IACF, m_cScIACF);
	DDX_Control(pDX, IDC_SC_IACC, m_cScIACC);
	DDX_Control(pDX, IDC_SC_PHI0, m_cScPhi0);
	DDX_Control(pDX, IDC_SC_MULTI, m_cScMulti);
	DDX_Control(pDX, IDC_SC_FRAME, m_cScFrame);
	DDX_Control(pDX, IDC_SC_COLUMNS, m_cScColumns);
	DDX_Control(pDX, IDC_SC_COLSPACE, m_cScColSpace);
	DDX_Control(pDX, IDC_SC_ACF, m_cScACF);
}

BEGIN_MESSAGE_MAP(CNmsScDlg, CDialogAF)
	ON_BN_CLICKED(IDC_SC_WINDOW, OnScWindow)
	ON_BN_CLICKED(IDC_SC_GRAPH, OnScGraph)
	ON_BN_CLICKED(IDC_SC_MULTI, OnScMulti)
	ON_BN_CLICKED(IDC_MMLIB, OnMmlib)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNmsScDlg メッセージ ハンドラ

BOOL CNmsScDlg::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	m_cScWindow = (setData.NmsSC.nCopyMode == 0);
	m_cScGraph = (setData.NmsSC.nCopyMode == 1);
	m_cScFrame = setData.NmsSC.bFrame;
	m_cScMulti = setData.NmsSC.bMulti;
	m_cScPhi0 = setData.NmsSC.bPhi0;
	m_cScTauE = setData.NmsSC.bTaue;
	m_cScTau1 = setData.NmsSC.bTau1;
	m_cScPhi1 = setData.NmsSC.bPhi1;
	m_cScSPL = setData.NmsSC.bSPL;
	m_cScIACC = setData.NmsSC.bIACC;
	m_cScTauIACC = setData.NmsSC.bTauIACC;
	m_cScWIACC = setData.NmsSC.bWIACC;
	m_cScACF = setData.NmsSC.bACF;
	m_cScIACF = setData.NmsSC.bIACF;
	m_cScColumns = setData.NmsSC.nColumns;
	m_cScColSpace = setData.NmsSC.nColSpace;
	m_cScRowSpace = setData.NmsSC.nRowSpace;

	if (!CheckMMLib())
		m_cMMLIB.EnableWindow(FALSE);

	EnableControl();

	return TRUE;
}

void CNmsScDlg::OnOK()
{
	SaveSetData();

	EndDialog(IDOK);
}

void CNmsScDlg::OnScWindow()
{
	EnableControl();
}

void CNmsScDlg::OnScGraph()
{
	EnableControl();
}

void CNmsScDlg::OnScMulti()
{
	EnableControl();
}

void CNmsScDlg::EnableControl()
{
	BOOL bGraph = m_cScGraph;
	BOOL bMulti = bGraph && (BOOL)m_cScMulti;

	m_cScFrame.EnableWindow(bGraph);
	m_cScMulti.EnableWindow(bGraph);
	m_cScPhi0.EnableWindow(bMulti);
	m_cScTauE.EnableWindow(bMulti);
	m_cScTau1.EnableWindow(bMulti);
	m_cScPhi1.EnableWindow(bMulti);
	m_cScSPL.EnableWindow(bMulti);
	m_cScIACC.EnableWindow(bMulti);
	m_cScTauIACC.EnableWindow(bMulti);
	m_cScWIACC.EnableWindow(bMulti);
	m_cScACF.EnableWindow(bMulti && !m_bNoWave);
	m_cScIACF.EnableWindow(bMulti && !m_bNoWave);
	m_cScColumns.EnableWindow(bMulti);
	m_cScColSpace.EnableWindow(bMulti);
	m_cScRowSpace.EnableWindow(bMulti);
}

void CNmsScDlg::SaveSetData()
{
	setData.NmsSC.nCopyMode = m_cScGraph ? 1 : 0;
	setData.NmsSC.bFrame = m_cScFrame;
	setData.NmsSC.bMulti = m_cScMulti;
	setData.NmsSC.bPhi0 = m_cScPhi0;
	setData.NmsSC.bTaue = m_cScTauE;
	setData.NmsSC.bTau1 = m_cScTau1;
	setData.NmsSC.bPhi1 = m_cScPhi1;
	setData.NmsSC.bSPL = m_cScSPL;
	setData.NmsSC.bIACC = m_cScIACC;
	setData.NmsSC.bTauIACC = m_cScTauIACC;
	setData.NmsSC.bWIACC = m_cScWIACC;
	setData.NmsSC.bACF = m_cScACF;
	setData.NmsSC.bIACF = m_cScIACF;
	setData.NmsSC.nColumns = m_cScColumns;
	setData.NmsSC.nColSpace = m_cScColSpace;
	setData.NmsSC.nRowSpace = m_cScRowSpace;
}

void CNmsScDlg::OnMmlib()
{
	SaveSetData();

	EndDialog(IDC_MMLIB);
}

BOOL CNmsScDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDOK, IDH_NMS_SC_OK,
		IDC_MMLIB, IDH_NMS_SC_MMLIB,
		IDCANCEL, IDH_NMS_SC_CANCEL,
		IDC_SC_WINDOW, IDH_NMS_SC_WINDOW,
		IDC_SC_GRAPH, IDH_NMS_SC_GRAPH,
		IDC_SC_FRAME, IDH_NMS_SC_FRAME,
		IDC_SC_MULTI, IDH_NMS_SC_MULTI,
		IDC_SC_PHI0, IDH_NMS_SC_PHI0,
		IDC_SC_TAUE, IDH_NMS_SC_TAUE,
		IDC_SC_TAU1, IDH_NMS_SC_TAU1,
		IDC_SC_PHI1, IDH_NMS_SC_PHI1,
		IDC_SC_SPL, IDH_NMS_SC_SPL,
		IDC_SC_IACC, IDH_NMS_SC_IACC,
		IDC_SC_TAUIACC, IDH_NMS_SC_TAUIACC,
		IDC_SC_WIACC, IDH_NMS_SC_WIACC,
		IDC_SC_ACF, IDH_NMS_SC_ACF,
		IDC_SC_IACF, IDH_NMS_SC_IACF,
		IDC_SC_COLUMNS, IDH_NMS_SC_COLUMNS,
		IDC_SC_COLSPACE, IDH_NMS_SC_COLSPACE,
		IDC_SC_ROWSPACE, IDH_NMS_SC_ROWSPACE,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
