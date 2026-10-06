// GraphParam.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "GraphParam.h"
#include "Calc.h"
#include "ImpFactor.h"
#include "Help\ContextHelp.h"

#define ITEM_SPL	0
#define ITEM_A		1
#define ITEM_TSUB	2
#define ITEM_IACC	3
#define ITEM_TIACC	4
#define ITEM_WIACC	5

#define ITEM_G		6
#define ITEM_T20	7
#define ITEM_T30	8
#define ITEM_T_CUSTOM	9
#define ITEM_EDT	10
#define ITEM_C50	11
#define ITEM_C80	12
#define ITEM_C_CUSTOM	13
#define ITEM_D50	14
#define ITEM_TS		15
#define ITEM_IACCE	16
#define ITEM_IACCL	17

/////////////////////////////////////////////////////////////////////////////
// CGraphParam ダイアログ

CGraphParam::CGraphParam(CWnd* pParent /*=NULL*/)
	: CDialogAF(CGraphParam::IDD, pParent)
{
	m_iParamItem = -1;

	m_pDataFreq = NULL;
	m_pDataAll = NULL;
	m_pDataLeft = NULL;
	m_pDataRight = NULL;
	m_bReDraw = FALSE;
}

void CGraphParam::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRAPH_PARAM, m_cGraphParam);
	DDX_Control(pDX, IDC_PARAM_IACC, m_cParamIACC);
	DDX_Control(pDX, IDC_PARAM_TIACC, m_cParamTIACC);
	DDX_Control(pDX, IDC_PARAM_WIACC, m_cParamWIACC);
	DDX_Radio(pDX, IDC_PARAM_SPL, m_iParamItem);
}

BEGIN_MESSAGE_MAP(CGraphParam, CDialogAF)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_PARAM_SPL, OnParamItem)
	ON_BN_CLICKED(IDC_PARAM_A, OnParamItem)
	ON_BN_CLICKED(IDC_PARAM_TSUB, OnParamItem)
	ON_BN_CLICKED(IDC_PARAM_IACC, OnParamItem)
	ON_BN_CLICKED(IDC_PARAM_TIACC, OnParamItem)
	ON_BN_CLICKED(IDC_PARAM_WIACC, OnParamItem)

	ON_BN_CLICKED(IDC_PARAM_G, OnParamItem)
	ON_BN_CLICKED(IDC_PARAM_T20, OnParamItem)
	ON_BN_CLICKED(IDC_PARAM_T30, OnParamItem)
	ON_BN_CLICKED(IDC_PARAM_T_CUSTOM, OnParamItem)
	ON_BN_CLICKED(IDC_PARAM_EDT, OnParamItem)
	ON_BN_CLICKED(IDC_PARAM_C50, OnParamItem)
	ON_BN_CLICKED(IDC_PARAM_C80, OnParamItem)
	ON_BN_CLICKED(IDC_PARAM_C_CUSTOM, OnParamItem)
	ON_BN_CLICKED(IDC_PARAM_D50, OnParamItem)
	ON_BN_CLICKED(IDC_PARAM_TS, OnParamItem)
	ON_BN_CLICKED(IDC_PARAM_IACCE, OnParamItem)
	ON_BN_CLICKED(IDC_PARAM_IACCL, OnParamItem)
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphParam メッセージ ハンドラ

BOOL CGraphParam::OnInitDialog()
{
	m_iParamItem = m_pParent->m_nParamItem;

	CDialogAF::OnInitDialog();

	m_cGraphParam.Initialize(90);

	const int nData = m_pDbAcParamRec->nDataNum;

	m_pDataFreq = new int[nData];
	for (int i = 0; i < nData; i++)
		m_pDataFreq[i] = m_pDbAcParamRec->dbAcParamData[i].nFreq;

	m_pDataAll = new double[nData];
	if (m_pDbImpulseRec->nChannel == 2) {
		m_pDataLeft = new double[nData];
		m_pDataRight = new double[nData];
	}

	if (m_pDbImpulseRec->nChannel == 1) {
		m_cParamIACC.EnableWindow(FALSE);
		m_cParamTIACC.EnableWindow(FALSE);
		m_cParamWIACC.EnableWindow(FALSE);
	}

	DispParamWindow();

	return TRUE;
}

void CGraphParam::OnDestroy()
{
	CDialogAF::OnDestroy();

	if (m_pDataFreq != NULL) {
		delete [] m_pDataFreq;
		m_pDataFreq = NULL;
	}

	if (m_pDataAll != NULL) {
		delete [] m_pDataAll;
		m_pDataAll = NULL;
	}

	if (m_pDataLeft != NULL) {
		delete [] m_pDataLeft;
		m_pDataLeft = NULL;
	}

	if (m_pDataRight != NULL) {
		delete [] m_pDataRight;
		m_pDataRight = NULL;
	}
}

void CGraphParam::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogAF::OnShowWindow(bShow, nStatus);

	if (bShow && m_bReDraw) {
		DispParamWindow();
		m_bReDraw = FALSE;
	}
}

void CGraphParam::ReDraw()
{
	if (m_hWnd != NULL) {
		if (IsWindowVisible())
			DispParamWindow();
		else
			m_bReDraw = TRUE;
	}
}

void CGraphParam::OnParamItem()
{
	UpdateData(TRUE);
	DispParamWindow();

	m_pParent->m_nParamItem = m_iParamItem;
}

void CGraphParam::DispParamWindow()
{
	DbAcParamData *pFreqRec;
	double *pDataAll = m_pDataAll;
	double *pDataLeft = m_pDataLeft;
	double *pDataRight = m_pDataRight;
	const int nData = m_pDbAcParamRec->nDataNum;
	double fScaleMin, fScaleMax, fScaleStep;
	LPCSTR pTitle;
	LPCSTR vAxis;
	int nRemark;
	int i;
	CString str, str2;

	switch (m_iParamItem) {
	case ITEM_SPL:
		for (i = 0, pFreqRec = m_pDbAcParamRec->dbAcParamData; i < nData; i++, pFreqRec++) {
			if (m_pDbImpulseRec->nChannel == 1) {
				pDataAll[i] = pFreqRec->fSplL + m_fAdjustSPL;
			} else {
				pDataAll[i] = MeanSPL(pFreqRec->fSplL, pFreqRec->fSplR) + m_fAdjustSPL;
				pDataLeft[i] = pFreqRec->fSplL + m_fAdjustSPL;
				pDataRight[i] = pFreqRec->fSplR + m_fAdjustSPL;
			}
		}
		fScaleMin = -80;
		fScaleMax = 80;
		fScaleStep = 10;
		pTitle = "SPL";
		nRemark = (m_pDbImpulseRec->nChannel) == 1 ? 1 : 4;
		vAxis = "SPL [dB]";
		break;
	case ITEM_A:
		for (i = 0, pFreqRec = m_pDbAcParamRec->dbAcParamData; i < nData; i++, pFreqRec++) {
			if (m_pDbImpulseRec->nChannel == 1)
				pDataAll[i] = pFreqRec->fAL;
			else {
				pDataAll[i] = MeanA(pFreqRec->fAL, pFreqRec->fAR);
				pDataLeft[i] = pFreqRec->fAL;
				pDataRight[i] = pFreqRec->fAR;
			}
		}
		fScaleMin = 0;
		fScaleMax = 20;
		fScaleStep = 1;
		pTitle = "A-value";
		nRemark = (m_pDbImpulseRec->nChannel) == 1 ? 1 : 4;
		vAxis = "A-value";
		break;
	case ITEM_TSUB:
		for (i = 0, pFreqRec = m_pDbAcParamRec->dbAcParamData; i < nData; i++, pFreqRec++) {
			if (m_pDbImpulseRec->nChannel == 1)
				pDataAll[i] = pFreqRec->tSubL;
			else {
				pDataAll[i] = MeanTsub(pFreqRec->tSubL, pFreqRec->tSubR);
				pDataLeft[i] = pFreqRec->tSubL;
				pDataRight[i] = pFreqRec->tSubR;
			}
		}
		fScaleMin = 0;
		fScaleMax = 5;
		fScaleStep = 0.5;
		pTitle = "Tsub";
		nRemark = (m_pDbImpulseRec->nChannel) == 1 ? 1 : 4;
		vAxis = "Tsub [s]";
		break;
	case ITEM_IACC:
		if (m_pDbImpulseRec->nChannel == 2) {
			for (i = 0, pFreqRec = m_pDbAcParamRec->dbAcParamData; i < nData; i++, pFreqRec++)
				pDataAll[i] = pFreqRec->fIACC;
		}
		pDataLeft = pDataRight = NULL;
		fScaleMin = 0;
		fScaleMax = 1;
		fScaleStep = 0.1;
		pTitle = "IACC";
		nRemark = 1;
		vAxis = "IACC";
		break;
	case ITEM_TIACC:
		if (m_pDbImpulseRec->nChannel == 2) {
			for (i = 0, pFreqRec = m_pDbAcParamRec->dbAcParamData; i < nData; i++, pFreqRec++)
				pDataAll[i] = pFreqRec->tIACC;
		}
		pDataLeft = pDataRight = NULL;
		fScaleMin = -2;
		fScaleMax = 2;
		fScaleStep = 0.2;
		str.LoadString(IDS_TIACC);
		pTitle = str;
		nRemark = 1;
		str2 = str + " [ms]";
		vAxis = (LPSTR)(LPCSTR)str2;
		break;
	case ITEM_WIACC:
		if (m_pDbImpulseRec->nChannel == 2) {
			for (i = 0, pFreqRec = m_pDbAcParamRec->dbAcParamData; i < nData; i++, pFreqRec++)
				pDataAll[i] = pFreqRec->wIACC;
		}
		pDataLeft = pDataRight = NULL;
		fScaleMin = 0;
		fScaleMax = 10;
		fScaleStep = 1;
		pTitle = "W_IACC";
		nRemark = 1;
		vAxis = "W_IACC [ms]";
		break;
	case ITEM_G:
		for (i = 0, pFreqRec = m_pDbAcParamRec->dbAcParamData; i < nData; i++, pFreqRec++) {
			if (m_pDbImpulseRec->nChannel == 1) {
				pDataAll[i] = pFreqRec->fG_L + m_fAdjustSPL;
			} else {
				pDataAll[i] = MeanSPL(pFreqRec->fG_L, pFreqRec->fG_R) + m_fAdjustSPL;
				pDataLeft[i] = pFreqRec->fG_L + m_fAdjustSPL;
				pDataRight[i] = pFreqRec->fG_R + m_fAdjustSPL;
			}
		}
		fScaleMin = -80;
		fScaleMax = 80;
		fScaleStep = 10;
		pTitle = "G";
		nRemark = (m_pDbImpulseRec->nChannel) == 1 ? 1 : 4;
		vAxis = "G [dB]";
		break;
	case ITEM_T20:
		for (i = 0, pFreqRec = m_pDbAcParamRec->dbAcParamData; i < nData; i++, pFreqRec++) {
			if (m_pDbImpulseRec->nChannel == 1)
				pDataAll[i] = pFreqRec->fT20_L;
			else {
				pDataAll[i] = MeanTsub(pFreqRec->fT20_L, pFreqRec->fT20_R);
				pDataLeft[i] = pFreqRec->fT20_L;
				pDataRight[i] = pFreqRec->fT20_R;
			}
		}
		fScaleMin = 0;
		fScaleMax = 5;
		fScaleStep = 0.5;
		pTitle = "T20";
		nRemark = (m_pDbImpulseRec->nChannel) == 1 ? 1 : 4;
		vAxis = "T20 [s]";
		break;
	case ITEM_T30:
		for (i = 0, pFreqRec = m_pDbAcParamRec->dbAcParamData; i < nData; i++, pFreqRec++) {
			if (m_pDbImpulseRec->nChannel == 1)
				pDataAll[i] = pFreqRec->fT30_L;
			else {
				pDataAll[i] = MeanTsub(pFreqRec->fT30_L, pFreqRec->fT30_R);
				pDataLeft[i] = pFreqRec->fT30_L;
				pDataRight[i] = pFreqRec->fT30_R;
			}
		}
		fScaleMin = 0;
		fScaleMax = 5;
		fScaleStep = 0.5;
		pTitle = "T30";
		nRemark = (m_pDbImpulseRec->nChannel) == 1 ? 1 : 4;
		vAxis = "T30 [s]";
		break;
	case ITEM_T_CUSTOM:
		for (i = 0, pFreqRec = m_pDbAcParamRec->dbAcParamData; i < nData; i++, pFreqRec++) {
			if (m_pDbImpulseRec->nChannel == 1)
				pDataAll[i] = pFreqRec->fTCustom_L;
			else {
				pDataAll[i] = MeanTsub(pFreqRec->fTCustom_L, pFreqRec->fTCustom_R);
				pDataLeft[i] = pFreqRec->fTCustom_L;
				pDataRight[i] = pFreqRec->fTCustom_R;
			}
		}
		fScaleMin = 0;
		fScaleMax = 5;
		fScaleStep = 0.5;
		pTitle = "T_custom";
		nRemark = (m_pDbImpulseRec->nChannel) == 1 ? 1 : 4;
		vAxis = "T_custom [s]";
		break;
	case ITEM_EDT:
		for (i = 0, pFreqRec = m_pDbAcParamRec->dbAcParamData; i < nData; i++, pFreqRec++) {
			if (m_pDbImpulseRec->nChannel == 1)
				pDataAll[i] = pFreqRec->fEDT_L;
			else {
				pDataAll[i] = MeanTsub(pFreqRec->fEDT_L, pFreqRec->fEDT_R);
				pDataLeft[i] = pFreqRec->fEDT_L;
				pDataRight[i] = pFreqRec->fEDT_R;
			}
		}
		fScaleMin = 0;
		fScaleMax = 5;
		fScaleStep = 0.5;
		pTitle = "EDT";
		nRemark = (m_pDbImpulseRec->nChannel) == 1 ? 1 : 4;
		vAxis = "EDT [s]";
		break;
	case ITEM_C50:
		for (i = 0, pFreqRec = m_pDbAcParamRec->dbAcParamData; i < nData; i++, pFreqRec++) {
			if (m_pDbImpulseRec->nChannel == 1)
				pDataAll[i] = pFreqRec->fC50_L;
			else {
				pDataAll[i] = MeanTsub(pFreqRec->fC50_L, pFreqRec->fC50_R);
				pDataLeft[i] = pFreqRec->fC50_L;
				pDataRight[i] = pFreqRec->fC50_R;
			}
		}
		fScaleMin = -40;
		fScaleMax = 40;
		fScaleStep = 5;
		pTitle = "C50";
		nRemark = (m_pDbImpulseRec->nChannel) == 1 ? 1 : 4;
		vAxis = "C50 [dB]";
		break;
	case ITEM_C80:
		for (i = 0, pFreqRec = m_pDbAcParamRec->dbAcParamData; i < nData; i++, pFreqRec++) {
			if (m_pDbImpulseRec->nChannel == 1)
				pDataAll[i] = pFreqRec->fC80_L;
			else {
				pDataAll[i] = MeanTsub(pFreqRec->fC80_L, pFreqRec->fC80_R);
				pDataLeft[i] = pFreqRec->fC80_L;
				pDataRight[i] = pFreqRec->fC80_R;
			}
		}
		fScaleMin = -40;
		fScaleMax = 40;
		fScaleStep = 5;
		pTitle = "C80";
		nRemark = (m_pDbImpulseRec->nChannel) == 1 ? 1 : 4;
		vAxis = "C80 [dB]";
		break;
	case ITEM_C_CUSTOM:
		for (i = 0, pFreqRec = m_pDbAcParamRec->dbAcParamData; i < nData; i++, pFreqRec++) {
			if (m_pDbImpulseRec->nChannel == 1)
				pDataAll[i] = pFreqRec->fCCustom_L;
			else {
				pDataAll[i] = MeanTsub(pFreqRec->fCCustom_L, pFreqRec->fCCustom_R);
				pDataLeft[i] = pFreqRec->fCCustom_L;
				pDataRight[i] = pFreqRec->fCCustom_R;
			}
		}
		fScaleMin = -40;
		fScaleMax = 40;
		fScaleStep = 5;
		pTitle = "C_custom";
		nRemark = (m_pDbImpulseRec->nChannel) == 1 ? 1 : 4;
		vAxis = "C_custom [dB]";
		break;
	case ITEM_D50:
		for (i = 0, pFreqRec = m_pDbAcParamRec->dbAcParamData; i < nData; i++, pFreqRec++) {
			if (m_pDbImpulseRec->nChannel == 1)
				pDataAll[i] = pFreqRec->fD50_L;
			else {
				pDataAll[i] = MeanTsub(pFreqRec->fD50_L, pFreqRec->fD50_R);
				pDataLeft[i] = pFreqRec->fD50_L;
				pDataRight[i] = pFreqRec->fD50_R;
			}
		}
		fScaleMin = 0;
		fScaleMax = 100;
		fScaleStep = 10;
		pTitle = "D50";
		nRemark = (m_pDbImpulseRec->nChannel) == 1 ? 1 : 4;
		vAxis = "D50 [%]";
		break;
	case ITEM_TS:
		for (i = 0, pFreqRec = m_pDbAcParamRec->dbAcParamData; i < nData; i++, pFreqRec++) {
			if (m_pDbImpulseRec->nChannel == 1)
				pDataAll[i] = pFreqRec->fTs_L;
			else {
				pDataAll[i] = MeanTsub(pFreqRec->fTs_L, pFreqRec->fTs_R);
				pDataLeft[i] = pFreqRec->fTs_L;
				pDataRight[i] = pFreqRec->fTs_R;
			}
		}
		fScaleMin = 0;
		fScaleMax = 2;
		fScaleStep = 0.2;
		pTitle = "Ts";
		nRemark = (m_pDbImpulseRec->nChannel) == 1 ? 1 : 4;
		vAxis = "Ts [s]";
		break;
	case ITEM_IACCE:
		if (m_pDbImpulseRec->nChannel == 2) {
			for (i = 0, pFreqRec = m_pDbAcParamRec->dbAcParamData; i < nData; i++, pFreqRec++)
				pDataAll[i] = pFreqRec->fIACCE;
		}
		pDataLeft = pDataRight = NULL;
		fScaleMin = 0;
		fScaleMax = 1;
		fScaleStep = 0.1;
		pTitle = "IACCE";
		nRemark = 1;
		vAxis = "IACCE";
		break;
	case ITEM_IACCL:
		if (m_pDbImpulseRec->nChannel == 2) {
			for (i = 0, pFreqRec = m_pDbAcParamRec->dbAcParamData; i < nData; i++, pFreqRec++)
				pDataAll[i] = pFreqRec->fIACCL;
		}
		pDataLeft = pDataRight = NULL;
		fScaleMin = 0;
		fScaleMax = 1;
		fScaleStep = 0.1;
		pTitle = "IACCL";
		nRemark = 1;
		vAxis = "IACCL";
		break;
	default:
		return;
	}

	m_cGraphParam.DispGraph(m_pDataFreq, m_pDbAcParamRec->dbAcParamCond.nFreqBand, pDataAll, pDataLeft, pDataRight, nData, fScaleMin, fScaleMax, fScaleStep, vAxis);
	m_cGraphParam.DispRemark(pTitle, nRemark);
}

BOOL CGraphParam::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDC_GRAPH_PARAM, IDH_GRAPH_PARAM,
		IDC_PARAM_SPL, IDH_GRAPH_PARAM_SPL,
		IDC_PARAM_A, IDH_GRAPH_PARAM_A,
		IDC_PARAM_TSUB, IDH_GRAPH_PARAM_TSUB,
		IDC_PARAM_IACC, IDH_GRAPH_PARAM_IACC,
		IDC_PARAM_TIACC, IDH_GRAPH_PARAM_TIACC,
		IDC_PARAM_WIACC, IDH_GRAPH_PARAM_WIACC,
		IDC_PARAM_G, IDH_GRAPH_PARAM_G,
		IDC_PARAM_T20, IDH_GRAPH_PARAM_T20,
		IDC_PARAM_T30, IDH_GRAPH_PARAM_T30,
		IDC_PARAM_T_CUSTOM, IDH_GRAPH_PARAM_T_CUSTOM,
		IDC_PARAM_EDT, IDH_GRAPH_PARAM_EDT,
		IDC_PARAM_C50, IDH_GRAPH_PARAM_C50,
		IDC_PARAM_C80, IDH_GRAPH_PARAM_C80,
		IDC_PARAM_C_CUSTOM, IDH_GRAPH_PARAM_C_CUSTOM,
		IDC_PARAM_D50, IDH_GRAPH_PARAM_D50,
		IDC_PARAM_TS, IDH_GRAPH_PARAM_TS,
		IDC_PARAM_IACCE, IDH_GRAPH_PARAM_IACCE,
		IDC_PARAM_IACCL, IDH_GRAPH_PARAM_IACCL,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
