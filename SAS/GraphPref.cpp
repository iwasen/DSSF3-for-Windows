// GraphPref.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "GraphPref.h"
#include "Calc.h"
#include "Help\ContextHelp.h"

#define MAX_TE	300
#define COLOR_DATA			RGB(255, 0, 0)

/////////////////////////////////////////////////////////////////////////////
// CGraphPref ダイアログ

CGraphPref::CGraphPref(CWnd* pParent /*=NULL*/)
	: CDialogAF(CGraphPref::IDD, pParent)
{
	m_pData = NULL;
	m_bReDraw = FALSE;
}

void CGraphPref::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BEST_TAUE, m_cBestTauE);
	DDX_Control(pDX, IDC_S4, m_cS4);
	DDX_Control(pDX, IDC_S3, m_cS3);
	DDX_Control(pDX, IDC_S2, m_cS2);
	DDX_Control(pDX, IDC_S1, m_cS1);
	DDX_Control(pDX, IDC_S, m_cS);
	DDX_Control(pDX, IDC_TAUE, m_cTauE);
	DDX_Control(pDX, IDC_BESTSPL, m_cBestSPL);
	DDX_Control(pDX, IDC_GRAPH_PREF, m_cGraphPref);
}

BEGIN_MESSAGE_MAP(CGraphPref, CDialogAF)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CALC, OnCalc)
	ON_WM_SHOWWINDOW()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphPref メッセージ ハンドラ

BOOL CGraphPref::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	m_cGraphPref.Initialize(90);

	m_cBestSPL.Format("%.2lf", LogMean(m_pDbAcParamRec->dbAcParamData[0].fSplL, m_pDbAcParamRec->dbAcParamData[0].fSplR) + m_fAdjustSPL);

	m_pData = new double[MAX_TE];

	GetParameter();
	DispPrefWindow();
	m_cTauE = m_cBestTauE;
	DispPreference();

	return TRUE;
}

void CGraphPref::OnDestroy()
{
	CDialogAF::OnDestroy();

	if (m_pData != NULL) {
		delete [] m_pData;
		m_pData = NULL;
	}
}

void CGraphPref::DispPrefWindow()
{
	double s[5];
	int maxTauE  = 0;

	double maxS = -1000;
	for (int i = 0; i < MAX_TE; i++) {
		CalcPreference(m_fSPL, m_fDT1, m_fTsub, m_fIACC, m_fA, i + 1, (double)m_cBestSPL, s);
		m_pData[i] = s[0];
		if (m_pData[i] > maxS) {
			maxS = m_pData[i];
			maxTauE = i + 1;
		}
	}
	m_cBestTauE = maxTauE;

	m_cGraphPref.DispGraph(m_pData, MAX_TE, (double)maxTauE, maxS);
}

void CGraphPref::OnCalc()
{
	GetParameter();
	DispPrefWindow();
	DispPreference();
}

void CGraphPref::DispPreference()
{
	double s[5];

	if ((double)m_cTauE <= 0) {
		CString msg;
		msg.LoadString(IDS_MSG_TAUE);
		MessageBox(msg, NULL, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	CalcPreference(m_fSPL, m_fDT1, m_fTsub, m_fIACC, m_fA, (double)m_cTauE, (double)m_cBestSPL, s);
	m_cS.Format("%.2lf", s[0]);
	m_cS1.Format("%.2lf", s[1]);
	m_cS2.Format("%.2lf", s[2]);
	m_cS3.Format("%.2lf", s[3]);
	m_cS4.Format("%.2lf", s[4]);
}

void CGraphPref::GetParameter()
{
	DbAcParamData *pAcParam = &m_pDbAcParamRec->dbAcParamData[0];

	m_fSPL = MeanSPL(pAcParam->fSplL, pAcParam->fSplR) + m_fAdjustSPL;
	m_fDT1 = MeanDT1(m_pDbAcParamRec->dbAcParamResult.fDeltaT1L - m_pDbAcParamRec->dbAcParamResult.fDeltaT0L, m_pDbAcParamRec->dbAcParamResult.fDeltaT1R - m_pDbAcParamRec->dbAcParamResult.fDeltaT0R);
	m_fTsub = MeanTsub(pAcParam->tSubL, pAcParam->tSubR);
	m_fIACC = pAcParam->fIACC;
	m_fA = MeanA(pAcParam->fAL, pAcParam->fAR);
}

void CGraphPref::ReDraw()
{
	if (m_hWnd != NULL) {
		if (IsWindowVisible()) {
			GetParameter();
			DispPrefWindow();
			DispPreference();
		} else
			m_bReDraw = TRUE;
	}
}

void CGraphPref::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogAF::OnShowWindow(bShow, nStatus);

	if (bShow && m_bReDraw) {
		GetParameter();
		DispPrefWindow();
		DispPreference();
		m_bReDraw = FALSE;
	}
}

BOOL CGraphPref::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_GRAPH_PREF, IDH_GRAPH_PREF,
		IDC_BESTSPL, IDH_GRAPH_PREF_BESTSPL,
		IDC_TAUE, IDH_GRAPH_PREF_TAUE,
		IDC_BEST_TAUE, IDH_GRAPH_PREF_BEST_TAUE,
		IDC_S1, IDH_GRAPH_PREF_S1,
		IDC_S2, IDH_GRAPH_PREF_S2,
		IDC_S3, IDH_GRAPH_PREF_S3,
		IDC_S4, IDH_GRAPH_PREF_S4,
		IDC_S, IDH_GRAPH_PREF_S,
		IDC_CALC, IDH_GRAPH_PREF_CALC,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
