// AcfRealtimeDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "RTA.h"
#include "AcfRealtimeDlg.h"
#include "MakeFilter.h"
#include "CalcAcf.h"
#include "AcfRealtimeCalcDlg.h"
#include "Help\ContextHelp.h"

#define WAVEBUF_NUM		4

#define FACTOR_PHI0		0
#define FACTOR_TAUE		1
#define FACTOR_TAU1		2
#define FACTOR_PHI1		3

#define CLM_TIME	0
#define CLM_PHI0	1
#define CLM_TAUE	2
#define CLM_TAU1	3
#define CLM_PHI1	4
#define CLM_IACC	5
#define CLM_TIACC	6
#define CLM_WIACC	7

// CAcfRealtimeDlg ダイアログ

CAcfRealtimeDlg::CAcfRealtimeDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CAcfRealtimeDlg::IDD, pParent)
{
	m_hAcfDlg = pParent->m_hWnd;
	m_pCalcBufferL = NULL;
	m_pCalcBufferR = NULL;
	m_bExitFlag = FALSE;
	m_pFilterTbl = NULL;
	m_pFilterTbl2 = NULL;
	m_pFactorData = NULL;
	m_pSpectrim = NULL;
	m_pWindowFunc = NULL;
	m_bInitialized = FALSE;
	for (int i = 0; i < 4; i++)
		m_aFactorGraph[i].pFactorData = NULL;
	m_aFactorGraph[FACTOR_PHI0].pFactorWnd = &m_cGraphPhi0;
	m_aFactorGraph[FACTOR_TAUE].pFactorWnd = &m_cGraphTauE;
	m_aFactorGraph[FACTOR_TAU1].pFactorWnd = &m_cGraphTau1;
	m_aFactorGraph[FACTOR_PHI1].pFactorWnd = &m_cGraphPhi1;
	m_pWaveIn = CWaveInEx::GetInstance();

	Create(IDD, g_oSetData.Acf.nRealtimePosLeft, g_oSetData.Acf.nRealtimePosTop, 0, pParent);
}

CAcfRealtimeDlg::~CAcfRealtimeDlg()
{
	m_pWaveIn->ReleaseInstance();
}

void CAcfRealtimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRAPH_PHI0, m_cGraphPhi0);
	DDX_Control(pDX, IDC_GRAPH_PHI1, m_cGraphPhi1);
	DDX_Control(pDX, IDC_GRAPH_TAU_E, m_cGraphTauE);
	DDX_Control(pDX, IDC_GRAPH_TAU_1, m_cGraphTau1);
	DDX_Control(pDX, IDC_GRAPH_SPECTROGRAM, m_cGraphSpg);
	DDX_Control(pDX, IDC_FACTOR_LIST, m_cFactorList);
}

BEGIN_MESSAGE_MAP(CAcfRealtimeDlg, CBaseDlg)
	ON_WM_DESTROY()
	ON_WM_MOVE()
	ON_WM_HELPINFO()
	ON_COMMAND(IDM_START, &CAcfRealtimeDlg::OnStart)
	ON_COMMAND(IDM_STOP, &CAcfRealtimeDlg::OnStop)
	ON_COMMAND(IDM_CALC_CONDITIONS, &CAcfRealtimeDlg::OnCalcConditions)
	ON_MESSAGE(WM_DISP_ACF_REALTIME, &CAcfRealtimeDlg::OnDispAcfRealtime)
END_MESSAGE_MAP()

// CAcfRealtimeDlg メッセージ ハンドラ

BOOL CAcfRealtimeDlg::OnInitDialog()
{
	CBaseDlg::OnInitDialog();

	CreateStatusBar();
	SetStatusBar(IDS_STATUS_STOP);

	for (int i = 0; i < 4; i++)
		m_aFactorGraph[i].pFactorWnd->Initialize(90, NULL, NULL, (LPARAM)this);

	m_aFactorGraph[FACTOR_PHI0].sAxis.LoadString(IDS_PHI0);
	m_aFactorGraph[FACTOR_PHI0].sAxis += " [dB]";
	m_aFactorGraph[FACTOR_TAUE].sAxis.LoadString(IDS_TAUE);
	m_aFactorGraph[FACTOR_TAUE].sAxis += " [ms]";
	m_aFactorGraph[FACTOR_TAU1].sAxis.LoadString(IDS_TAU1);
	m_aFactorGraph[FACTOR_TAU1].sAxis += " [ms]";
	m_aFactorGraph[FACTOR_PHI1].sAxis.LoadString(IDS_PHI1);

	AfxBeginThread(CalcThreadEntry, this, THREAD_PRIORITY_LOWEST);

	SetCalcParams();

	m_cGraphSpg.Initialize();
	m_cGraphSpg.DispGraph(NULL, 0, 0, 0, m_nSamplingRate, 0, m_oAcfCondition.fRunningStep * m_nTimeDataNum, 0, 20, m_nSamplingRate / 2, m_bSpgScale, m_nSpgMinLevel, m_nSpgMaxLevel);

	m_bInitialized = TRUE;

	return TRUE;
}

void CAcfRealtimeDlg::OnDestroy()
{
	CBaseDlg::OnDestroy();

	m_pWaveIn->Close();

	m_bExitFlag = TRUE;
	m_oCalcEvent.SetEvent();
	m_oExitEvent.Lock();

	FreeBuffers();

	::SendMessage(m_hAcfDlg, WM_CLOSE_ACF_REALTIME_DIALOG, 0, 0);
}

void CAcfRealtimeDlg::SetCalcParams()
{
	m_nSamplingRate = g_oSetData.Acf.nSamplingRate;
	m_nChannel = g_oSetData.Acf.nChannel + 1;
	m_nMeasureTime = g_oSetData.Acf.nMeasureTime;
	m_nTimeDataNum = max((int)((m_nMeasureTime - g_oSetData.Acf.fRealtimeIntegrationTime) / g_oSetData.Acf.fRealtimeRunningStep) + 1, 0);
	m_nWaveBufSize = (int)(m_nSamplingRate * g_oSetData.Acf.fRealtimeRunningStep);
	m_nFactorCount = 0;
	m_fStartTime = 0;
	m_fElapseTime = 0;
	m_nSpgMaxLevel = g_oSetData.Acf.nRealtimeMaxLevel;
	m_nSpgMinLevel = g_oSetData.Acf.nRealtimeMinLevel;
	m_bSpgScale = !g_oSetData.Acf.nRealtimeScale;
	m_bTaueFixedScale = g_oSetData.Acf.bRealtimeTaueFixedScale;
	m_fTaueMinTime = g_oSetData.Acf.fRealtimeTaueMinTime;
	m_fTaueMaxTime = g_oSetData.Acf.fRealtimeTaueMaxTime;
	m_bTaueLogScale = !g_oSetData.Acf.nRealtimeTaueScale;

	// ACF計算パラメータ
	m_oAcfCondition.fIntegrationTime = g_oSetData.Acf.fRealtimeIntegrationTime;
	m_oAcfCondition.fRunningStep = g_oSetData.Acf.fRealtimeRunningStep;
	m_oAcfCondition.nFreqWeighting = g_oSetData.Acf.nRealtimeFreqWeighting;
	m_oAcfCondition.fMaxDelayTime = g_oSetData.Acf.fRealtimeMaxDelayTime;
	m_oAcfCondition.nPhi0RefData = -1;
	m_oAcfCondition.fPhi0RefLevel = 0;
	m_oAcfCondition.fTaueInterval = g_oSetData.Acf.fRealtimeTaueInterval;
	m_oAcfCondition.fTaueEndLevel = g_oSetData.Acf.fRealtimeTaueEndLevel;
	m_oAcfCondition.fTaueEndTime = g_oSetData.Acf.fRealtimeTaueEndTime;
	m_oAcfCondition.bTaueOrigin = (short)g_oSetData.Acf.bRealtimeTaueOrigin;
	m_oAcfCondition.nTau1Peak = (short)g_oSetData.Acf.nRealtimeTau1Peak;

	// IACF計算パラメータ
	m_oIAcfCondition.fIACCWLevel = 0.1;

	// 積分データサイズ取得
	m_nMaxDelaySize = (int)(m_oAcfCondition.fMaxDelayTime * m_nSamplingRate);
	m_nIntegrationSize1 = (int)(m_oAcfCondition.fIntegrationTime * m_nSamplingRate);
	m_nIntegrationSize2 = m_nIntegrationSize1 + m_nMaxDelaySize;

	for (int i = 0; i < 4; i++) {
		GetFactor(i, m_aFactorGraph[i]);
		m_aFactorGraph[i].pFactorWnd->DispGraph(m_oAcfCondition.fRunningStep, 0, m_oAcfCondition.fRunningStep * m_nTimeDataNum, m_aFactorGraph[i].pFactorData, m_nFactorCount, m_aFactorGraph[i].fMinValue + m_aFactorGraph[i].fTotalValue, m_aFactorGraph[i].fMinValue, m_aFactorGraph[i].sAxis, -1, FALSE, FALSE, NULL, 0, m_fStartTime, i == FACTOR_TAUE ? m_bTaueLogScale : FALSE);
	}

	InitAcfFactorList();
}

void CAcfRealtimeDlg::Start()
{
	// FFT バッファサイズ取得
	for (m_nFftSize = 1; m_nFftSize < m_nIntegrationSize2; m_nFftSize *= 2)
		;
	m_nFftSize2 = m_nFftSize / 2;

	AllocBuffers();

	// フィルタ作成
	MakeFilterTbl2(m_pFilterTbl, m_nFftSize, m_nSamplingRate, m_oAcfCondition.nFreqWeighting, 20);
	MakeFilterTbl2(m_pFilterTbl2, m_nFftSize2, m_nSamplingRate, m_oAcfCondition.nFreqWeighting, 20);

	MakeWindowFunc(WF_HANNING, m_nFftSize2, m_pWindowFunc, 2);

	m_nTimeCount = 0;

	m_cGraphSpg.DispGraph(NULL, 0, 0, 0, m_nSamplingRate, 0, m_oAcfCondition.fRunningStep * m_nTimeDataNum, 0, 20, m_nSamplingRate / 2, m_bSpgScale, m_nSpgMinLevel, m_nSpgMaxLevel);

	if (!m_pWaveIn->Open(g_oSetData.Pc.nInputDevice, this, m_nChannel, m_nSamplingRate, m_nWaveBufSize, WAVEBUF_NUM, TRUE)) {
		FreeBuffers();
		return;
	}

	m_pWaveIn->Start();
}

int CAcfRealtimeDlg::OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify)
{
	CMenu *pMenu;

	switch (nCode) {
	case WAVEIN_OPEN:
		m_bStart = TRUE;
		pMenu = GetMenu();
		pMenu->EnableMenuItem(IDM_START, MF_BYCOMMAND | MF_GRAYED);
		pMenu->EnableMenuItem(IDM_STOP, MF_BYCOMMAND | MF_ENABLED);
		DrawMenuBar();
		SetStatusBar(GetRunningStatus(IDS_STATUS_RUNNING, m_pWaveIn->GetBitsPerSample(), 0, 0));
		break;
	case WAVEIN_CLOSE:
		m_bStart = FALSE;
		pMenu = GetMenu();
		pMenu->EnableMenuItem(IDM_START, MF_BYCOMMAND | MF_ENABLED);
		pMenu->EnableMenuItem(IDM_STOP, MF_BYCOMMAND | MF_GRAYED);
		DrawMenuBar();
		SetStatusBar(IDS_STATUS_STOP);
		break;
	case WAVEIN_DATA:
		return WaveInData(pWaveNotify);
	}

	return 0;
}

BOOL CAcfRealtimeDlg::WaveInData(LPWAVENOTIFY pWaveNotify)
{
	m_oCriticalSection.Lock();

	double *pBuffer = new double[m_nWaveBufSize * m_nChannel];
	memcpy(pBuffer, pWaveNotify->pSamplesData, m_nWaveBufSize * m_nChannel * sizeof(double));
	m_aWaveBufferList.AddTail(pBuffer);

	m_oCriticalSection.Unlock();

	m_oCalcEvent.SetEvent();

	return TRUE;
}

void CAcfRealtimeDlg::AllocBuffers()
{
	FreeBuffers();

	m_pCalcBufferL = new double[m_nIntegrationSize2];
	if (m_nChannel == 2)
		m_pCalcBufferR = new double[m_nIntegrationSize2];

	m_pFilterTbl = new double[m_nFftSize];
	m_pFilterTbl2 = new double[m_nFftSize2];

	m_pFactorData = new AllFactorData[m_nTimeDataNum];

	m_pSpectrim = new double[m_nFftSize2 / 2];

	m_pWindowFunc = new double[m_nFftSize2];

	for (int i = 0; i < 4; i++)
		m_aFactorGraph[i].pFactorData = new double[m_nTimeDataNum];
}

void CAcfRealtimeDlg::FreeBuffers()
{
	if (m_pCalcBufferL != NULL) {
		delete [] m_pCalcBufferL;
		m_pCalcBufferL = NULL;
	}

	if (m_pCalcBufferR != NULL) {
		delete [] m_pCalcBufferR;
		m_pCalcBufferR = NULL;
	}

	if (m_pFilterTbl != NULL) {
		delete [] m_pFilterTbl;
		m_pFilterTbl = NULL;
	}

	if (m_pFilterTbl2 != NULL) {
		delete [] m_pFilterTbl2;
		m_pFilterTbl2 = NULL;
	}

	if (m_pFactorData != NULL) {
		delete [] m_pFactorData;
		m_pFactorData = NULL;
	}

	if (m_pSpectrim != NULL) {
		delete [] m_pSpectrim;
		m_pSpectrim = NULL;
	}

	if (m_pWindowFunc != NULL) {
		delete [] m_pWindowFunc;
		m_pWindowFunc = NULL;
	}

	while (m_aWaveBufferList.GetCount() != 0)
		delete [] (double *)m_aWaveBufferList.RemoveHead();

	for (int i = 0; i < 4; i++) {
		if (m_aFactorGraph[i].pFactorData != NULL) {
			delete [] m_aFactorGraph[i].pFactorData;
			m_aFactorGraph[i].pFactorData = NULL;
		}
	}
}

UINT CAcfRealtimeDlg::CalcThreadEntry(LPVOID pParam)
{
	((CAcfRealtimeDlg *)pParam)->CalcThread();
	return 0;
}

void CAcfRealtimeDlg::CalcThread()
{
	for (;;) {
		m_oCalcEvent.Lock();

		if (m_bExitFlag)
			break;

		if (m_aWaveBufferList.GetSize() * m_nWaveBufSize >= m_nIntegrationSize2) {
			m_oCriticalSection.Lock();

			int nCount = 0;
			double *pCalcBufferL = m_pCalcBufferL;
			double *pCalcBufferR = m_pCalcBufferR;
			BOOL bLoop = TRUE;
			for (POSITION pos = m_aWaveBufferList.GetHeadPosition(); pos != NULL && bLoop; ) {
				double *pWaveBuffer = (double *)m_aWaveBufferList.GetNext(pos);

				for (int i = 0; i < m_nWaveBufSize; i++) {
					*pCalcBufferL++ = *pWaveBuffer++;
					if (m_nChannel == 2)
						*pCalcBufferR++ = *pWaveBuffer++;

					if (++nCount >= m_nIntegrationSize2) {
						bLoop = FALSE;
						break;
					}
				}
			}

			delete [] (double *)m_aWaveBufferList.RemoveHead();

			m_oCriticalSection.Unlock();

			if (m_nFactorCount < m_nTimeDataNum)
				m_nFactorCount++;
			else {
				memmove(m_pFactorData, m_pFactorData + 1, sizeof(AllFactorData) * (m_nTimeDataNum - 1));
				m_fStartTime += m_oAcfCondition.fRunningStep;
			}

			CalcAcfFactor(&m_pFactorData[m_nFactorCount - 1]);

			CalcSpectrum();

			if (!m_bExitFlag)
				SendMessage(WM_DISP_ACF_REALTIME);
		}
	}

	m_oExitEvent.SetEvent();
}

void CAcfRealtimeDlg::CalcAcfFactor(AllFactorData *pFactorData)
{
	double *pIAcfData;
	double *pFft;
	int nIAcfData;
	double fSPL;

	// 計算結果格納エリアクリア
	memset(pFactorData, 0, sizeof(AllFactorData));

	// ACF計算
	pFft = CalcAcf1(m_pCalcBufferL, m_nIntegrationSize1, m_nIntegrationSize2, m_nFftSize, m_pFilterTbl);

	// ACFファクタ計算
	TaunPhinData oTaunPhinData;
	GetAcfFactor(&m_oAcfCondition, &pFactorData->Acf, &oTaunPhinData, m_nSamplingRate, pFft, m_nMaxDelaySize);
	delete [] pFft;

	if (m_nChannel == 2) {
		// SPL計算
		fSPL = CalcSPL(m_pCalcBufferL, m_pCalcBufferR, m_nIntegrationSize1, m_nFftSize, m_pFilterTbl);

		// IACF テンポラリバッファ確保
		nIAcfData = (int)(m_nSamplingRate * 0.002);		// ±1ms
		pIAcfData = new double[nIAcfData];

		// IACF計算
		pFft = CalcIAcf(m_pCalcBufferL, m_pCalcBufferR, m_nIntegrationSize1, m_nFftSize, m_pFilterTbl);
		memcpy(pIAcfData, pFft + (m_nFftSize - nIAcfData / 2), (nIAcfData / 2) * sizeof(double));
		memcpy(pIAcfData + nIAcfData / 2, pFft, (nIAcfData / 2) * sizeof(double));
		delete [] pFft;

		// IACFファクタ計算
		GetIAcfFactor(&m_oIAcfCondition, &pFactorData->IAcf, m_nSamplingRate, pIAcfData, nIAcfData, fSPL);
		delete [] pIAcfData;
	}
}

void CAcfRealtimeDlg::GetFactor(int nFactorKind, FACTOR_GRAPH &oFactorGraph)
{
	int i;
	double fMaxValue = 0, fMinValue = 0;

	switch (nFactorKind) {
	case FACTOR_PHI0:
		for (i = 0; i < m_nFactorCount; i++) {
			oFactorGraph.pFactorData[i] = m_pFactorData[i].Acf.fPhi0;
			if (!_isnan(oFactorGraph.pFactorData[i]) && _finite(oFactorGraph.pFactorData[i])) {
				if (i == 0 || fMaxValue < oFactorGraph.pFactorData[i])
					fMaxValue = oFactorGraph.pFactorData[i];
				if (i == 0 || fMinValue > oFactorGraph.pFactorData[i])
					fMinValue = oFactorGraph.pFactorData[i];
			}
		}
		if (fMaxValue == 0 && fMinValue == 0)
			fMaxValue = 1;
		else if (fMaxValue >= 0 && fMinValue >= 0) {
			fMinValue = 0;
			fMaxValue = GetCeilValue(fMaxValue);
		} else if (fMaxValue <= 0 && fMinValue <= 0){
			fMaxValue = 0;
			fMinValue = GetCeilValue(fMinValue);
		} else {
			fMaxValue = GetCeilValue(fMaxValue);
			fMinValue = -fMaxValue;
		}
		break;
	case FACTOR_TAUE:
		for (i = 0; i < m_nFactorCount; i++) {
			oFactorGraph.pFactorData[i] = m_pFactorData[i].Acf.fTauE * 1000;
			if (!_isnan(oFactorGraph.pFactorData[i]) && _finite(oFactorGraph.pFactorData[i])) {
				if (i == 0 || fMaxValue < oFactorGraph.pFactorData[i])
					fMaxValue = oFactorGraph.pFactorData[i];
			}
		}
		if (m_bTaueFixedScale) {
			fMaxValue = m_fTaueMaxTime;
			fMinValue = m_fTaueMinTime;
		} else {
			fMaxValue = GetCeilValue(fMaxValue);
			fMinValue = 0;
		}
		if (m_bTaueLogScale) {
			if (fMinValue < 1)
				fMinValue = 1;
			if (fMaxValue < 10)
				fMaxValue = 10;
		}
		break;
	case FACTOR_TAU1:
		for (i = 0; i < m_nFactorCount; i++) {
			oFactorGraph.pFactorData[i] = m_pFactorData[i].Acf.fTau1 * 1000;
			if (!_isnan(oFactorGraph.pFactorData[i]) && _finite(oFactorGraph.pFactorData[i])) {
				if (i == 0 || fMaxValue < oFactorGraph.pFactorData[i])
					fMaxValue = oFactorGraph.pFactorData[i];
			}
		}
		fMaxValue = GetCeilValue(fMaxValue);
		fMinValue = 0;
		break;
	case FACTOR_PHI1:
		for (i = 0; i < m_nFactorCount; i++)
			oFactorGraph.pFactorData[i] = m_pFactorData[i].Acf.fPhi1;
		fMaxValue = 1;
		fMinValue = 0;
		break;
	}

	if (fMaxValue <= fMinValue)
		fMaxValue = fMinValue + 1;

	oFactorGraph.fMinValue = fMinValue;
	oFactorGraph.fTotalValue = fMaxValue - fMinValue;
}

void CAcfRealtimeDlg::CalcSpectrum()
{
	int i, j;
	double xt, yt;

	double *pFftBuf = new double[m_nFftSize2];
	for (i = 0; i < m_nFftSize2; i++)
		pFftBuf[i] = m_pCalcBufferL[i] * m_pWindowFunc[i];

	m_oRFFT.fft(m_nFftSize2, pFftBuf);
	for (i = 1, j = 2; i < m_nFftSize2 / 2; i++) {
		xt = pFftBuf[j++] * m_pFilterTbl2[i];
		yt = pFftBuf[j++] * m_pFilterTbl2[i];

		m_pSpectrim[i] = xt * xt + yt * yt;
	}

	delete [] pFftBuf;
}

void CAcfRealtimeDlg::InitAcfFactorList()
{
	int i;
	struct LISTCOLUMN {
		UINT nID;
		char *text;
		int width;
	};
	static const LISTCOLUMN tColumnHeader1[] = {
		{0, "Time", 68},
		{IDS_PHI0, NULL, 68},
		{IDS_TAUE, NULL, 68},
		{IDS_TAU1, NULL, 68},
		{IDS_PHI1, NULL, 68}
	}, tColumnHeader2[] = {
		{0, "Time", 66},
		{IDS_PHI0, NULL, 55},
		{IDS_TAUE, NULL, 55},
		{IDS_TAU1, NULL, 55},
		{IDS_PHI1, NULL, 55},
		{0, "IACC", 55},
		{IDS_TIACC, NULL, 55},
		{0, "W_IACC", 55}
	};
	const LISTCOLUMN *pColumnHeader;
	static const char *tUnit1[] = {" [s]", " [dB]", " [ms]", " [ms]", ""};
	const char **pUnit = NULL;
	CString str;
	int nColumn;

	m_cFactorList.DeleteAllItems();

	while (m_cFactorList.DeleteColumn(0))
		;

	if (m_nChannel == 1) {
		pColumnHeader = tColumnHeader1;
		nColumn = sizeof(tColumnHeader1) / sizeof(LISTCOLUMN);
		pUnit = tUnit1;
	} else {
		pColumnHeader = tColumnHeader2;
		nColumn = sizeof(tColumnHeader2) / sizeof(LISTCOLUMN);
	}

	for (i = 0; i < nColumn; i++, pColumnHeader++) {
		if (pColumnHeader->nID == 0)
			str = pColumnHeader->text;
		else
			str.LoadString(pColumnHeader->nID);

		if (pUnit != NULL)
			str += pUnit[i];

		m_cFactorList.InsertColumn(i, str, LVCFMT_RIGHT, pColumnHeader->width);
	}
}

void CAcfRealtimeDlg::DispAcfFactorList(const AllFactorData *pFactorData)
{
	CString str;

	str.Format("%g", m_fElapseTime);
	m_cFactorList.InsertItem(0, str);

	str.Format("%.2lf", pFactorData->Acf.fPhi0);
	m_cFactorList.SetItemText(0, CLM_PHI0, str);

	str.Format("%.2lf", pFactorData->Acf.fTauE * 1000);
	m_cFactorList.SetItemText(0, CLM_TAUE, str);

	str.Format("%.2lf", pFactorData->Acf.fTau1 * 1000);
	m_cFactorList.SetItemText(0, CLM_TAU1, str);

	str.Format("%.2lf", pFactorData->Acf.fPhi1);
	m_cFactorList.SetItemText(0, CLM_PHI1, str);

	if (m_nChannel == 2) {
		str.Format("%.2lf", pFactorData->IAcf.fIACC);
		m_cFactorList.SetItemText(0, CLM_IACC, str);

		str.Format("%.2lf", pFactorData->IAcf.fTauIACC * 1000);
		m_cFactorList.SetItemText(0, CLM_TIACC, str);

		str.Format("%.2lf", pFactorData->IAcf.fWIACC * 1000);
		m_cFactorList.SetItemText(0, CLM_WIACC, str);
	}
}

void CAcfRealtimeDlg::OnMove(int x, int y)
{
	CBaseDlg::OnMove(x, y);

	if (m_bInitialized) {
		CRect rect;
		GetWindowRect(rect);
		g_oSetData.Acf.nRealtimePosLeft = rect.left;
		g_oSetData.Acf.nRealtimePosTop = rect.top;
	}
}

BOOL CAcfRealtimeDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_GRAPH_PHI0, IDH_ACF_REALTIME_GRAPH_PHI0,
		IDC_GRAPH_PHI1, IDH_ACF_REALTIME_GRAPH_PHI1,
		IDC_GRAPH_TAU_E, IDH_ACF_REALTIME_GRAPH_TAU_E,
		IDC_GRAPH_TAU_1, IDH_ACF_REALTIME_GRAPH_TAU_1,
		IDC_GRAPH_SPECTROGRAM, IDH_ACF_REALTIME_GRAPH_SPECTROGRAM,
		IDC_FACTOR_LIST, IDH_ACF_REALTIME_FACTOR_LIST,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

void CAcfRealtimeDlg::OnStart()
{
	SetCalcParams();
	Start();
}

void CAcfRealtimeDlg::OnStop()
{
	m_pWaveIn->Close();
}

void CAcfRealtimeDlg::OnCalcConditions()
{
	CAcfRealtimeCalcDlg dlg;

	if (dlg.DoModal() == IDOK) {
		if (m_bStart) {
			OnStop();
			OnStart();
		} else
			SetCalcParams();
	}
}

LRESULT CAcfRealtimeDlg::OnDispAcfRealtime(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	for (int i = 0; i < 4; i++) {
		GetFactor(i, m_aFactorGraph[i]);
		m_aFactorGraph[i].pFactorWnd->DispGraph(m_oAcfCondition.fRunningStep, 0, m_oAcfCondition.fRunningStep * m_nTimeDataNum, m_aFactorGraph[i].pFactorData, m_nFactorCount, m_aFactorGraph[i].fMinValue + m_aFactorGraph[i].fTotalValue, m_aFactorGraph[i].fMinValue, m_aFactorGraph[i].sAxis, -1, FALSE, FALSE, NULL, 0, m_fStartTime, i == FACTOR_TAUE ? m_bTaueLogScale : FALSE);
	}

	m_cGraphSpg.DispGraph(m_pSpectrim, m_nFftSize2 / 2, m_nTimeDataNum, m_nTimeCount++, m_nSamplingRate, 0, m_oAcfCondition.fRunningStep * m_nTimeDataNum, m_fStartTime, 20, m_nSamplingRate / 2, m_bSpgScale, m_nSpgMinLevel, m_nSpgMaxLevel);

	DispAcfFactorList(&m_pFactorData[m_nFactorCount - 1]);
	m_fElapseTime += m_oAcfCondition.fRunningStep;

	return 0;
}
