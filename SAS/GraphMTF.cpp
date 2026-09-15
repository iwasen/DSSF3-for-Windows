// GraphMTF.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "GraphMTF.h"
#include "Calc.h"
#include "Help\ContextHelp.h"

#define COLOR_LEFT			RGB(0, 128, 255)
#define COLOR_RIGHT			RGB(255, 64, 192)

// CGraphMTF ダイアログ

CGraphMTF::CGraphMTF(CWnd* pParent /*=NULL*/)
	: CDialogAF(CGraphMTF::IDD, pParent)
{
	m_pMtfLeft = NULL;
	m_pMtfRight = NULL;
	m_bReDraw = FALSE;
}

void CGraphMTF::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MTF_LEFT, m_cMtfLeft);
	DDX_Control(pDX, IDC_MTF_RIGHT, m_cMtfRight);
	DDX_Control(pDX, IDC_STI_L, m_cSTIL);
	DDX_Control(pDX, IDC_STI_R, m_cSTIR);
	DDX_Control(pDX, IDC_STI_M_L, m_cSTIML);
	DDX_Control(pDX, IDC_STI_M_R, m_cSTIMR);
	DDX_Control(pDX, IDC_STI_F_L, m_cSTIFL);
	DDX_Control(pDX, IDC_STI_F_R, m_cSTIFR);
	DDX_Control(pDX, IDC_RASTI_L, m_cRASTIL);
	DDX_Control(pDX, IDC_RASTI_R, m_cRASTIR);
}

BEGIN_MESSAGE_MAP(CGraphMTF, CDialogAF)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

// CGraphMTF メッセージ ハンドラ

void CGraphMTF::ReDraw()
{
	if (m_hWnd != NULL) {
		if (IsWindowVisible())
			DispGraphWindow();
		else
			m_bReDraw = TRUE;
	}
}

BOOL CGraphMTF::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	CString str;
	if (m_pDbImpulseRec->nChannel == 1) {
		m_cMtfLeft.Initialize(90, NULL, 1, COLOR_LEFT);
	} else {
		str.LoadString(IDS_LEFTCHANNEL);
		m_cMtfLeft.Initialize(90, str, 1, COLOR_LEFT);
		str.LoadString(IDS_RIGHTCHANNEL);
		m_cMtfRight.Initialize(90, str, 1, COLOR_RIGHT);
	}

	m_pMtfLeft = new double[N_MTF_FREQ];
	m_pMtfRight = new double[N_MTF_FREQ];

	m_cMtfLeft.EnableWindow(TRUE);
	if (m_pDbImpulseRec->nChannel == 2)
		m_cMtfRight.EnableWindow(TRUE);

	DispGraphWindow();

	return TRUE;
}

void CGraphMTF::DispGraphWindow()
{
	CalcMTF(m_pIRLeft, m_nData, m_fRate, g_tMtfFreq, N_MTF_FREQ, m_pMtfLeft, FALSE, 0);
	m_cMtfLeft.DispGraph(m_pMtfLeft, g_tMtfFreq);

	m_cSTIL.Format("%.3f", m_pDbAcParamResult->fSTI_L);
	m_cSTIML.Format("%.3f", m_pDbAcParamResult->fSTIM_L);
	m_cSTIFL.Format("%.3f", m_pDbAcParamResult->fSTIF_L);
	m_cRASTIL.Format("%.3f", m_pDbAcParamResult->fRASTI_L);

	if (m_pDbImpulseRec->nChannel == 2) {
		CalcMTF(m_pIRRight, m_nData, m_fRate, g_tMtfFreq, N_MTF_FREQ, m_pMtfRight, FALSE, 0);
		m_cMtfRight.DispGraph(m_pMtfRight, g_tMtfFreq);

		m_cSTIR.Format("%.3f", m_pDbAcParamResult->fSTI_R);
		m_cSTIMR.Format("%.3f", m_pDbAcParamResult->fSTIM_R);
		m_cSTIFR.Format("%.3f", m_pDbAcParamResult->fSTIF_R);
		m_cRASTIR.Format("%.3f", m_pDbAcParamResult->fRASTI_R);
	}
}

void CGraphMTF::OnDestroy()
{
	CDialogAF::OnDestroy();

	if (m_pMtfLeft != NULL)
		delete [] m_pMtfLeft;

	if (m_pMtfRight != NULL)
		delete [] m_pMtfRight;
}

void CGraphMTF::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogAF::OnShowWindow(bShow, nStatus);

	if (bShow && m_bReDraw) {
		DispGraphWindow();
		m_bReDraw = FALSE;
	}
}

BOOL CGraphMTF::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_MTF_LEFT, IDH_GRAPH_MTF_LEFT,
		IDC_MTF_RIGHT, IDH_GRAPH_MTF_RIGHT,
		IDC_STI_L, IDH_GRAPH_MTF_STI_L,
		IDC_STI_R, IDH_GRAPH_MTF_STI_R,
		IDC_STI_M_L, IDH_GRAPH_MTF_STI_M_L,
		IDC_STI_M_R, IDH_GRAPH_MTF_STI_M_R,
		IDC_STI_F_L, IDH_GRAPH_MTF_STI_F_L,
		IDC_STI_F_R, IDH_GRAPH_MTF_STI_F_R,
		IDC_RASTI_L, IDH_GRAPH_MTF_RASTI_L,
		IDC_RASTI_R, IDH_GRAPH_MTF_RASTI_R,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
