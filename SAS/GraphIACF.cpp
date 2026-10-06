// GraphIACF.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Sas.h"
#include "GraphIACF.h"
#include "CalcAcf.h"
#include "MakeFilter.h"
#include "Help\ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CGraphIACF ダイアログ

CGraphIACF::CGraphIACF(CWnd* pParent /*=NULL*/)
	: CDialogAF(CGraphIACF::IDD, pParent)
{
	m_pIAcfData = NULL;
	m_bReDraw = FALSE;
}

void CGraphIACF::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRAPH_ACF, m_cGraphIACF);
}

BEGIN_MESSAGE_MAP(CGraphIACF, CDialogAF)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphIACF メッセージ ハンドラ

BOOL CGraphIACF::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	m_cGraphIACF.Initialize(90, "CCF", 4);

	m_nData = (int)(m_fRate * 0.001);
	m_nData = m_nData * 2 + 1;
	m_pIAcfData = new double[m_nData];

	CalcGraphWindow();
	DispGraphWindow();

	return TRUE;
}

void CGraphIACF::OnDestroy()
{
	CDialogAF::OnDestroy();

	if (m_pIAcfData != NULL) {
		delete [] m_pIAcfData;
		m_pIAcfData = NULL;
	}
}

void CGraphIACF::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogAF::OnShowWindow(bShow, nStatus);

	if (bShow && m_bReDraw) {
		CalcGraphWindow();
		DispGraphWindow();
		m_bReDraw = FALSE;
	}
}

void CGraphIACF::ReDraw(int nStep)
{
	if (nStep != -1)
		m_nStep = nStep;

	if (m_hWnd != NULL) {
		if (IsWindowVisible()) {
			CalcGraphWindow();
			DispGraphWindow();
		} else
			m_bReDraw = TRUE;
	}
}

void CGraphIACF::CalcGraphWindow()
{
	CWaitCursor waitCursor;
	double *pWaveBuf1;
	double *pWaveBuf2;
	int nIntegrationSize;
	int nFftSize, nFftSize2;
	double *pFilterTbl;
	double *pIAcfData;
	int i;
	double fSPL;

	// 積分データ数取得
	nIntegrationSize = (int)(m_pAcfCondition->fIntegrationTime * m_fRate);

	// Wave データ取得
	pWaveBuf1 = new double[nIntegrationSize];
	pWaveBuf2 = new double[nIntegrationSize];
	ReadWaveData(m_hWaveData, pWaveBuf1, pWaveBuf2, nIntegrationSize, (int)(m_nStep * m_pAcfCondition->fRunningStep * m_fRate));

	// FFT バッファサイズ取得
	nFftSize2 = (int)((m_pAcfCondition->fIntegrationTime + m_pAcfCondition->fMaxDelayTime) * m_fRate);
	for (nFftSize = 1; nFftSize < nFftSize2; nFftSize *= 2)
		;

	// フィルタ作成
	pFilterTbl = new double[nFftSize];
	if (m_pNmsMicCal != NULL) {
		double *pCalibration = new double[nFftSize];
		MakeFilterTbl3(pCalibration, nFftSize, m_fRate, (FilterData *)m_pNmsMicCal->freq, m_pNmsMicCal->nFreqData, 20);
		pCalibration[0] = 1;
		const double sens = pow(10.0, -m_pNmsMicCal->fInputSens / 20);
		MakeFilterTbl2(pFilterTbl, nFftSize, m_fRate, m_pAcfCondition->nFreqWeighting, 20);
		for (i = 0; i < nFftSize; i++)
			pFilterTbl[i] *= sens / pCalibration[i];
		delete [] pCalibration;
	} else
		MakeFilterTbl2(pFilterTbl, nFftSize, m_fRate, m_pAcfCondition->nFreqWeighting, 20);

	// IACF計算
	pIAcfData = CalcIAcf(pWaveBuf1, pWaveBuf2, nIntegrationSize, nFftSize, pFilterTbl);
	memcpy(m_pIAcfData, pIAcfData + (nFftSize - m_nData / 2), (m_nData / 2) * sizeof(double));
	memcpy(m_pIAcfData + m_nData / 2, pIAcfData, (m_nData / 2 + 1) * sizeof(double));
	delete [] pIAcfData;

	// SPLを計算
	fSPL = CalcSPL(pWaveBuf1, pWaveBuf2, nIntegrationSize, nFftSize, pFilterTbl);

	delete [] pFilterTbl;
	delete [] pWaveBuf1;
	delete [] pWaveBuf2;

	for (i = 0; i < m_nData; i++)
		m_pIAcfData[i] /= fSPL;
}

void CGraphIACF::DispGraphWindow()
{
	IAcfFactor *pIAcfFactor = &m_pIAcfFactor[m_nStep];
	double fWIACC1;
	double fWIACC2;

	if (pIAcfFactor->fWIACC1 != -1) {
		fWIACC1 = pIAcfFactor->fWIACC1;
		fWIACC2 = pIAcfFactor->fWIACC - pIAcfFactor->fWIACC1;
	} else {
		fWIACC1 = pIAcfFactor->fWIACC / 2;
		fWIACC2 = pIAcfFactor->fWIACC / 2;
	}

	m_cGraphIACF.DispGraph(m_pIAcfData, m_nData, m_fRate, pIAcfFactor->fIACC,
			pIAcfFactor->fTauIACC * 1000, fWIACC1 * 1000, fWIACC2 * 1000);
}

BOOL CGraphIACF::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDC_GRAPH_ACF, IDH_GRAPH_ACF,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
