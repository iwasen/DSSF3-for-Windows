// ImpScDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "ImpScDlg.h"
#include "SetData.h"
#include "Help\ContextHelp.h"

// CImpScDlg ダイアログ

CImpScDlg::CImpScDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CImpScDlg::IDD, pParent)
{
}

void CImpScDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MMLIB, m_cMMLIB);
	DDX_Control(pDX, IDC_SC_WINDOW, m_cScWindow);
	DDX_Control(pDX, IDC_SC_GRAPH, m_cScGraph);
	DDX_Control(pDX, IDC_SC_MULTI, m_cScMulti);
	DDX_Control(pDX, IDC_SC_FRAME, m_cScFrame);
	DDX_Control(pDX, IDC_SC_IMP, m_cScImp);
	DDX_Control(pDX, IDC_SC_ECHO, m_cScEcho);
	DDX_Control(pDX, IDC_SC_IACC, m_cScIACC);
	DDX_Control(pDX, IDC_SC_FREQ, m_cScFreq);
	DDX_Control(pDX, IDC_SC_PREF, m_cScPref);
	DDX_Control(pDX, IDC_SC_PARAM, m_cScParam);
	DDX_Control(pDX, IDC_SC_MTF, m_cScMTF);
	DDX_Control(pDX, IDC_SC_COLUMNS, m_cScColumns);
	DDX_Control(pDX, IDC_SC_COLSPACE, m_cScColSpace);
	DDX_Control(pDX, IDC_SC_ROWSPACE, m_cScRowSpace);
}

BEGIN_MESSAGE_MAP(CImpScDlg, CDialogAF)
	ON_BN_CLICKED(IDC_SC_MULTI, &CImpScDlg::OnBnClickedScMulti)
	ON_BN_CLICKED(IDC_SC_GRAPH, &CImpScDlg::OnScGraph)
	ON_BN_CLICKED(IDC_SC_MULTI, &CImpScDlg::OnScMulti)
	ON_BN_CLICKED(IDC_MMLIB, &CImpScDlg::OnMmlib)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

// CImpScDlg メッセージ ハンドラ

BOOL CImpScDlg::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	m_cScWindow = (setData.ImpSC.nCopyMode == 0);
	m_cScGraph = (setData.ImpSC.nCopyMode == 1);
	m_cScMulti = setData.ImpSC.bMulti;
	m_cScFrame = setData.ImpSC.bFrame;
	m_cScImp = setData.ImpSC.bImp;
	m_cScEcho = setData.ImpSC.bEcho;
	m_cScIACC = setData.ImpSC.bIACC;
	m_cScFreq = setData.ImpSC.bFreq;
	m_cScPref = setData.ImpSC.bPref;
	m_cScParam = setData.ImpSC.bParam;
	m_cScMTF = setData.ImpSC.bMTF;
	m_cScColumns = setData.ImpSC.nColumns;
	m_cScColSpace = setData.ImpSC.nColSpace;
	m_cScRowSpace = setData.ImpSC.nRowSpace;

	if (!CheckMMLib())
		m_cMMLIB.EnableWindow(FALSE);

	EnableControl();

	return TRUE;
}

void CImpScDlg::OnOK()
{
	SaveSetData();

	EndDialog(IDOK);
}

void CImpScDlg::OnScWindow()
{
	EnableControl();
}

void CImpScDlg::OnScGraph()
{
	EnableControl();
}

void CImpScDlg::OnBnClickedScMulti()
{
	EnableControl();
}

void CImpScDlg::OnScMulti()
{
	EnableControl();
}

void CImpScDlg::EnableControl()
{
	BOOL bGraph = m_cScGraph;
	BOOL bMulti = bGraph && (BOOL)m_cScMulti;
	BOOL bStereo = (m_nChannel == 2);

	m_cScFrame.EnableWindow(bGraph);
	m_cScMulti.EnableWindow(bGraph);
	m_cScImp.EnableWindow(bMulti);
	m_cScEcho.EnableWindow(bMulti);
	m_cScIACC.EnableWindow(bMulti && bStereo);
	m_cScFreq.EnableWindow(bMulti);
	m_cScPref.EnableWindow(bMulti && bStereo);
	m_cScParam.EnableWindow(bMulti);
	m_cScMTF.EnableWindow(bMulti);
	m_cScColumns.EnableWindow(bMulti);
	m_cScColSpace.EnableWindow(bMulti);
}

void CImpScDlg::SaveSetData()
{
	setData.ImpSC.nCopyMode = m_cScGraph ? 1 : 0;
	setData.ImpSC.bFrame = m_cScFrame;
	setData.ImpSC.bMulti = m_cScMulti;
	setData.ImpSC.bImp = m_cScImp;
	setData.ImpSC.bEcho = m_cScEcho;
	setData.ImpSC.bIACC = m_cScIACC;
	setData.ImpSC.bFreq = m_cScFreq;
	setData.ImpSC.bPref = m_cScPref;
	setData.ImpSC.bParam = m_cScParam;
	setData.ImpSC.bMTF = m_cScMTF;
	setData.ImpSC.nColumns = m_cScColumns;
	setData.ImpSC.nColSpace = m_cScColSpace;
	setData.ImpSC.nRowSpace = m_cScRowSpace;
}

void CImpScDlg::OnMmlib()
{
	SaveSetData();

	EndDialog(IDC_MMLIB);
}

BOOL CImpScDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDOK, IDH_IMP_SC_OK,
		IDC_MMLIB, IDH_IMP_SC_MMLIB,
		IDCANCEL, IDH_IMP_SC_CANCEL,
		IDC_SC_WINDOW, IDH_IMP_SC_WINDOW,
		IDC_SC_GRAPH, IDH_IMP_SC_GRAPH,
		IDC_SC_FRAME, IDH_IMP_SC_FRAME,
		IDC_SC_MULTI, IDH_IMP_SC_MULTI,
		IDC_SC_IMP, IDH_IMP_SC_IMP,
		IDC_SC_ECHO, IDH_IMP_SC_ECHO,
		IDC_SC_IACC, IDH_IMP_SC_IACC,
		IDC_SC_FREQ, IDH_IMP_SC_FREQ,
		IDC_SC_PREF, IDH_IMP_SC_PREF,
		IDC_SC_PARAM, IDH_IMP_SC_PARAM,
		IDC_SC_MTF, IDH_IMP_SC_MTF,
		IDC_SC_COLUMNS, IDH_IMP_SC_COLUMNS,
		IDC_SC_COLSPACE, IDH_IMP_SC_COLSPACE,
		IDC_SC_ROWSPACE, IDH_IMP_SC_ROWSPACE,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
