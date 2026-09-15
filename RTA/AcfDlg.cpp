// AcfDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "AcfDlg.h"
#include "MakeFilter.h"
#include "FileIO.h"
#include "FileDlg.h"
#include "WaveInEx.h"
#include "WaveOutEx.h"
#include "WaveFile.h"
#include "Regist.h"
#include "Help\ContextHelp.h"

#define WAVEBUF_NUM		4
#define DECIMATION		5
#define WAVEOUT_SAMPLESPERBUFFER	1024
#define WAVEOUT_BUFNUM	5

#define SCROLL_RANGE	16384
#define MIN_DISP_TIME	0.001
#define MAX_ZOOM_TIME	4096

/////////////////////////////////////////////////////////////////////////////
// CAcfDlg ダイアログ

CAcfDlg::CAcfDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CAcfDlg::IDD, pParent)
{
	m_iTimeDir = -1;
	m_iScaleType = -1;
	m_iChannel = -1;
	m_iDispChannel = -1;

	m_bInitialized = FALSE;
	m_pRingBuf = NULL;
	m_pWaveBuf = NULL;
	m_pAcfBuf1 = NULL;
	m_pFilterTbl = NULL;
	m_nScrollSize = -1;
	m_nScrollPos = -1;
	m_fSelectStart = -1;
	m_fSelectEnd = -1;
	m_bDispSelectArea = FALSE;
	m_nChannel = 1;
	m_bOpenWaveOut = FALSE;
	m_pWaveIn = CWaveInEx::GetInstance();
	m_pWaveOut = CWaveOutEx::GetInstance();
	m_pAcfRealtimeDlg = NULL;
	m_nWindowKind = WINDOW_ACF;

	Create(IDD, g_oSetData.Acf.nPosLeft, g_oSetData.Acf.nPosTop, g_oSetData2.nFontSizeAcf, pParent);
}

CAcfDlg::~CAcfDlg()
{
	m_pWaveIn->ReleaseInstance();
	m_pWaveOut->ReleaseInstance();
}

void CAcfDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ZOOM_H, m_cZoomH);
	DDX_Control(pDX, IDC_SCROLL_H, m_cGraphScroll);
	DDX_Control(pDX, IDC_CHANNEL_LCH, m_cChannelLch);
	DDX_Control(pDX, IDC_CHANNEL_RCH, m_cChannelRch);
	DDX_Control(pDX, IDC_SCALE_LINER, m_cScale_Liner);
	DDX_Control(pDX, IDC_SCALE_LOG, m_cScale_Log);
	DDX_Control(pDX, IDC_TIME_FWD, m_cTimeFwd);
	DDX_Control(pDX, IDC_TIME_BWD, m_cTimeBwd);
	DDX_Control(pDX, IDC_CHANNEL_MONO, m_cChannelMono);
	DDX_Control(pDX, IDC_CHANNEL_STEREO, m_cChannelStereo);
	DDX_Control(pDX, IDC_SELECT_TIME, m_cSelectTime);
	DDX_Control(pDX, IDC_SELECT_ALL, m_cSelectAll);
	DDX_Control(pDX, IDC_SELECT_START, m_cSelectStart);
	DDX_Control(pDX, IDC_SELECT_END, m_cSelectEnd);
	DDX_Control(pDX, IDC_REPLAY, m_cReplay);
	DDX_Control(pDX, IDC_LAUNCH_SA, m_cLaunchSA);
	DDX_Control(pDX, IDC_CUT, m_cCut);
	DDX_Control(pDX, IDC_FILE_SAVE, m_cFileSave);
	DDX_Control(pDX, IDC_REDRAW, m_cRedraw);
	DDX_Control(pDX, IDC_TIME_DATA_NUM_SPIN, m_cTimeDataNumSpin);
	DDX_Control(pDX, IDC_MEASURE_TIME_SPIN, m_cMeasureTimeSpin);
	DDX_Control(pDX, IDC_MR_TIME, m_cMrTime);
	DDX_Control(pDX, IDC_MR_SAMPLING, m_cMrSampling);
	DDX_Control(pDX, IDC_MR_MTIME, m_cMrMtime);
	DDX_Control(pDX, IDC_SCALE_LEVEL, m_cScaleLevel);
	DDX_Control(pDX, IDC_SCALE_ABS, m_cScaleAbs);
	DDX_Control(pDX, IDC_FREQ_FILTER, m_cFreqFilter);
	DDX_Control(pDX, IDC_MEASURE_TIME, m_cMeasureTime);
	DDX_Control(pDX, IDC_GRAPH_WAVE, m_cGraphWave);
	DDX_Control(pDX, IDC_GRAPH_NORM, m_cGraphNorm);
	DDX_Control(pDX, IDC_GRAPH_ACF, m_cGraphAcf);
	DDX_Control(pDX, IDC_TIME_DATA_NUM, m_cTimeDataNum);
	DDX_Control(pDX, IDC_MAX_TAU, m_cMaxTau);
	DDX_Control(pDX, IDC_CALC_CYCLE, m_cCalcCycle);
	DDX_Control(pDX, IDC_BTN_STOP, m_cBtnStop);
	DDX_Control(pDX, IDC_BTN_START, m_cBtnStart);
	DDX_Control(pDX, IDC_INTEGRATION_TIME, m_cIntegrationTime);
	DDX_Control(pDX, IDC_SAMPLING_RATE, m_cSamplingRate);
	DDX_Control(pDX, IDC_FILE_LOAD, m_cFileLoad);
	DDX_Control(pDX, IDCANCEL, m_cExit);
	DDX_Radio(pDX, IDC_TIME_FWD, m_iTimeDir);
	DDX_Radio(pDX, IDC_SCALE_LINER, m_iScaleType);
	DDX_Radio(pDX, IDC_CHANNEL_MONO, m_iChannel);
	DDX_Radio(pDX, IDC_CHANNEL_LCH, m_iDispChannel);
	DDX_Control(pDX, IDC_ACF_REALTIME_GRAPH, m_cAcfRealtimeGraph);
}

BEGIN_MESSAGE_MAP(CAcfDlg, CBaseDlg)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_BTN_START, OnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, OnStop)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_CALC_CYCLE, OnChangeCalcCycle)
	ON_EN_CHANGE(IDC_TIME_DATA_NUM, OnChangeTimeDataNum)
	ON_BN_CLICKED(IDC_CHANNEL_MONO, OnChannelMono)
	ON_BN_CLICKED(IDC_CHANNEL_STEREO, OnChannelStereo)
	ON_CBN_SELCHANGE(IDC_SAMPLING_RATE, OnSelchangeSamplingRate)
	ON_EN_CHANGE(IDC_INTEGRATION_TIME, OnChangeIntegrationTime)
	ON_EN_CHANGE(IDC_MAX_TAU, OnChangeMaxTau)
	ON_BN_CLICKED(IDC_TIME_BWD, OnTimeBwd)
	ON_BN_CLICKED(IDC_TIME_FWD, OnTimeFwd)
	ON_EN_CHANGE(IDC_MEASURE_TIME, OnChangeMeasureTime)
	ON_WM_HSCROLL()
	ON_COMMAND(IDM_ZOOM_IN_H, OnZoomInH)
	ON_COMMAND(IDM_ZOOM_OUT_H, OnZoomOutH)
	ON_COMMAND(IDM_ZOOM_FULL_H, OnZoomFullH)
	ON_BN_CLICKED(IDC_GRAPH_NORM, OnGraphNorm)
	ON_BN_CLICKED(IDC_SCALE_ABS, OnScaleAbs)
	ON_EN_CHANGE(IDC_SCALE_LEVEL, OnChangeScaleLevel)
	ON_BN_CLICKED(IDC_SCALE_LINER, OnScaleLiner)
	ON_BN_CLICKED(IDC_SCALE_LOG, OnScaleLog)
	ON_CBN_SELCHANGE(IDC_FREQ_FILTER, OnSelchangeFreqFilter)
	ON_BN_CLICKED(IDC_FILE_LOAD, OnFileLoad)
	ON_BN_CLICKED(IDC_REDRAW, OnRedraw)
	ON_BN_CLICKED(IDC_FILE_SAVE, OnFileSave)
	ON_BN_CLICKED(IDC_REPLAY, OnReplay)
	ON_BN_CLICKED(IDC_CUT, OnCut)
	ON_BN_CLICKED(IDC_SELECT_ALL, OnSelectAll)
	ON_EN_CHANGE(IDC_SELECT_START, OnChangeSelectStart)
	ON_EN_CHANGE(IDC_SELECT_END, OnChangeSelectEnd)
	ON_BN_CLICKED(IDC_CHANNEL_LCH, OnChannelLch)
	ON_BN_CLICKED(IDC_CHANNEL_RCH, OnChannelRch)
	ON_WM_HELPINFO()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_LAUNCH_SA, &CAcfDlg::OnBnClickedLaunchSa)
	ON_MESSAGE(WM_CLOSE_ACF_REALTIME_DIALOG, &CAcfDlg::OnCloseAcfRealtimeDialog)
	ON_BN_CLICKED(IDC_ACF_REALTIME_GRAPH, &CAcfDlg::OnBnClickedAcfRealtimeGraph)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAcfDlg メッセージ ハンドラ

BOOL CAcfDlg::OnInitDialog()
{
	CBaseDlg::OnInitDialog();

	CreateStatusBar();
	SetStatusBar(IDS_STATUS_STOP);

	if (m_tbZoomH.Create(this, WS_CHILD | WS_VISIBLE | CBRS_SIZE_FIXED | CBRS_ALIGN_LEFT | CBRS_TOOLTIPS, ID_ZOOM_H) &&
			m_tbZoomH.LoadToolBar(IDR_ZOOM_H)) {
		WINDOWPLACEMENT wp;
		m_cZoomH.GetWindowPlacement(&wp);

		CSize sizeToolBar;
		sizeToolBar = m_tbZoomH.CalcFixedLayout(FALSE, FALSE);
		m_tbZoomH.MoveWindow(10, wp.rcNormalPosition.top - 8, sizeToolBar.cx, sizeToolBar.cy);
	}

	m_cGraphAcf.Initialize(90);
	m_cGraphWave.Initialize(90, GraphCallBack, (LPARAM)this, TRUE);

	CRect rectGraphAcf;
	CRect rectControl;
	m_cGraphAcf.GetWindowRect(rectGraphAcf);
	CWnd *pWnd = GetTopWindow();
	while (pWnd != NULL) {
		switch (pWnd->GetDlgCtrlID()) {
		case IDC_GRAPH_ACF:
			SetCtlPosition(pWnd, 0, 0, 1, 0.6);
			break;
		case IDC_GRAPH_WAVE:
			SetCtlPosition(pWnd, 0, 0.6, 1, 1);
			break;
		case IDC_SCROLL_H:
		case IDC_SEPARATOR_LINE:
			SetCtlPosition(pWnd, 0, 1, 1, 1);
			break;
		case IDC_GRAPH_NORM:
			SetCtlPosition(pWnd, 1, 1, 1, 1);
			break;
		default:
			pWnd->GetWindowRect(rectControl);
			if (rectControl.left > rectGraphAcf.right)
				SetCtlPosition(pWnd, 1, 0, 1, 0);
			else
				SetCtlPosition(pWnd, 0, 1, 0, 1);
			break;
		}
		pWnd = pWnd->GetNextWindow();
	}
	SetCtlPosition(&m_cGraphWave.m_wndRemark, 1, 0, 1, 0);

	SetSamplingRateList(m_cSamplingRate, WAVE_IN, g_oSetData.Pc.nInputDevice, 0, g_oSetData.Acf.nSamplingRate);

	m_cIntegrationTime = g_oSetData.Acf.fIntegrationTime;
	m_cCalcCycle = g_oSetData.Acf.fCalcCycle;
	m_iChannel = g_oSetData.Acf.nChannel;
	m_cTimeDataNum = g_oSetData.Acf.nTimeDataNum;
	m_iTimeDir = g_oSetData.Acf.nTimeDir;
	m_cMaxTau = g_oSetData.Acf.fMaxTau;
	m_cMeasureTime = g_oSetData.Acf.nMeasureTime;
	m_iScaleType = g_oSetData.Acf.nScaleType;
	m_cScaleAbs = g_oSetData.Acf.bScaleAbs;
	m_cScaleLevel = g_oSetData.Acf.nScaleLevel;
	m_iDispChannel = g_oSetData.Acf.nDispChannel;

	m_cMeasureTimeSpin.SetRange(1, 30);
	m_cTimeDataNumSpin.SetRange(1, 100);

	SetFilterNameList(m_cFreqFilter);
	m_cFreqFilter.SetCurSel(g_oSetData.Acf.nFilter);

	EnableScaleCtrl();
	EnableDispRch();

	UpdateData(FALSE);

	InitDispTime();
	DispWaveGraph();

	m_cMeasureTime.SetValidChar(VC_NUM | VC_POINT);
	m_cIntegrationTime.SetValidChar(VC_NUM | VC_POINT);
	m_cCalcCycle.SetValidChar(VC_NUM | VC_POINT);
	m_cMaxTau.SetValidChar(VC_NUM | VC_POINT);
	m_cTimeDataNum.SetValidChar(VC_NUM);
	m_cScaleLevel.SetValidChar(VC_NUM | VC_MINUS);
	m_cSelectStart.SetValidChar(VC_NUM | VC_POINT);
	m_cSelectEnd.SetValidChar(VC_NUM | VC_POINT);

	m_bInitialized = TRUE;

	return TRUE;
}

void CAcfDlg::OnDestroy()
{
	CBaseDlg::OnDestroy();

	m_pWaveIn->Close();

	if (m_bOpenWaveOut)
		m_pWaveOut->Close();

	FreeBuffers();
	if (m_pWaveBuf != NULL)
		delete [] m_pWaveBuf;

	if (m_pAcfRealtimeDlg != NULL)
		m_pAcfRealtimeDlg->DestroyWindow();

	CWnd *pWnd = CWnd::FindWindowEx(HWND_DESKTOP, NULL, NULL, SA_ACF_TITLE);
	while (pWnd != NULL) {
		pWnd->PostMessage(WM_CLOSE);
		pWnd = CWnd::FindWindowEx(HWND_DESKTOP, pWnd->m_hWnd, NULL, SA_ACF_TITLE);
	}
}

void CAcfDlg::OnOK()
{
	if (m_cRedraw.IsWindowEnabled())
		OnRedraw();
}

void CAcfDlg::OnMove(int x, int y)
{
	CBaseDlg::OnMove(x, y);

	if (m_bInitialized) {
		CRect rect;
		GetWindowRect(rect);
		g_oSetData.Acf.nPosLeft = rect.left;
		g_oSetData.Acf.nPosTop = rect.top;
	}
}

void CAcfDlg::OnStart()
{
	int nSamplingRate;

	if ((nSamplingRate = m_pWaveIn->GetFixedSamplingRate()) != 0)
		ChangeSamplingRate(nSamplingRate);

	FreeBuffers();

	if (m_pWaveBuf != NULL) {
		delete [] m_pWaveBuf;
		m_pWaveBuf = NULL;
	}

	SetTimeDataNum(g_oSetData.Acf.nMeasureTime);

	m_nSamplingRate = g_oSetData.Acf.nSamplingRate;
	m_nChannel = g_oSetData.Acf.nChannel + 1;
	m_nSample = g_oSetData.Acf.nMeasureTime * m_nSamplingRate;

	m_nWaveBufCount = 0;
	m_nRingBufPtr = 0;
	m_bRingBufFull = FALSE;
	m_fDataTime = 0;
	m_fMaxData = 0;
	m_nDispChannel = (m_nChannel == 1) ? 0 : g_oSetData.Acf.nDispChannel;

	m_cGraphNorm = FALSE;

	InitDispTime();
	DispWaveGraph();
/*
	if (!m_cGraphAcf.ClearGraph()) {
		::AfxMessageBox(IDS_ERR_RUNNINGSTEP);
		m_cCalcCycle.SetFocus();
		m_cCalcCycle.SetSel(0, -1);
		return;
	}
*/
	m_cGraphAcf.ClearGraph();

	if (!AllocBuffers()) {
		::AfxMessageBox(IDS_ERR_MEMORYALLOC);
		return;
	}

	SetFilter();

	m_nWaveBufSize = m_nSample * m_nChannel;
	m_pWaveBuf = new double[m_nWaveBufSize];
	MEMCLEAR(m_pWaveBuf, m_nWaveBufSize);

	SYSTEMTIME st;
	GetLocalTime(&st);
	m_sTime.Format("%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	m_pWaveOut->Close();
	if (!m_pWaveIn->Open(g_oSetData.Pc.nInputDevice, this, m_nChannel, m_nSamplingRate, (int)(m_nSamplingRate * g_oSetData.Acf.fCalcCycle), WAVEBUF_NUM, TRUE)) {
		FreeBuffers();
		return;
	}

	m_nBitsPerSample = m_pWaveIn->GetBitsPerSample();

	m_pWaveIn->Start();
}

void CAcfDlg::OnStop()
{
	m_pWaveIn->Close();
}

void CAcfDlg::SetFilter()
{
	MakeFilterTbl2(m_pFilterTbl, m_nAcfBufSize, (double)m_nSamplingRate / DECIMATION, g_oSetData.Acf.nFilter, 20);
}

BOOL CAcfDlg::AllocBuffers()
{
	int n;

	m_nAcfSize = (int)(m_nSamplingRate * g_oSetData.Acf.fIntegrationTime / DECIMATION);
	m_nRingBufSize = m_nAcfSize;
	for (m_nAcfBufSize = 1, n = m_nAcfSize - 1; n != 0; n >>= 1)
		m_nAcfBufSize <<= 1;

	if ((m_pRingBuf = new double[m_nRingBufSize]) == NULL)
		return FALSE;

	if ((m_pAcfBuf1 = new double[m_nAcfBufSize]) == NULL)
		return FALSE;

	if ((m_pFilterTbl = new double[m_nAcfBufSize]) == NULL)
		return FALSE;

	return TRUE;
}

void CAcfDlg::FreeBuffers()
{
	if (m_pRingBuf != NULL) {
		delete [] m_pRingBuf;
		m_pRingBuf = NULL;
	}

	if (m_pAcfBuf1 != NULL) {
		delete [] m_pAcfBuf1;
		m_pAcfBuf1 = NULL;
	}

	if (m_pFilterTbl != NULL) {
		delete [] m_pFilterTbl;
		m_pFilterTbl = NULL;
	}
}

int CAcfDlg::OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify)
{
	CString str;

	switch (nCode) {
	case WAVEIN_OPEN:
		m_bStart = TRUE;
		m_cBtnStart.EnableWindow(FALSE);
		m_cBtnStop.EnableWindow(TRUE);
		SetStatusBar(GetRunningStatus(IDS_STATUS_RUNNING, m_pWaveIn->GetBitsPerSample(), 0, 0));
		if (GetActiveWindow()->GetSafeHwnd() == GetSafeHwnd())
			m_cBtnStop.SetFocus();
		EnableButton(FALSE);
		EnableControl(FALSE);
		pWaveNotify->nWindow = WINDOW_ACF;
		break;
	case WAVEIN_CLOSE:
		m_cBtnStart.EnableWindow(TRUE);
		m_cBtnStop.EnableWindow(FALSE);
		SetStatusBar(IDS_STATUS_STOP);
		if (GetActiveWindow()->GetSafeHwnd() == GetSafeHwnd())
			m_cBtnStart.SetFocus();
		m_fMaxData = GetMaxData(m_pWaveBuf, m_nWaveBufSize);
		DispWaveGraph();
		EnableButton();
		EnableControl(TRUE);
		m_cGraphWave.SetSelectArea(0, m_fDataTime);
		SetMeasureParam();
		m_bStart = FALSE;
		break;
	case WAVEIN_DATA:
		return WaveInData(pWaveNotify);
	case WAVEOUT_OPEN:
		m_cReplay.SetCheck(1);
		str.LoadString(IDS_STOP2);
		m_cReplay.SetWindowText(str);
		m_bOpenWaveOut = TRUE;
		pWaveNotify->nWindow = WINDOW_ACF;
		break;
	case WAVEOUT_DATA:
	case WAVEOUT_CLOSE_WAIT:
		return WaveOutData(pWaveNotify);
	case WAVEOUT_CLOSE:
		m_cGraphWave.SetReplayPosition(-1);
		m_cReplay.SetCheck(0);
		str.LoadString(IDS_REPLAY);
		m_cReplay.SetWindowText(str);
		m_bOpenWaveOut = FALSE;
		break;
	}

	return 0;
}

BOOL CAcfDlg::WaveInData(LPWAVENOTIFY pWaveNotify)
{
	int len;

	if (m_pWaveBuf == NULL)
		return FALSE;

	len = min(pWaveNotify->nSamplesRecorded * m_nChannel, m_nWaveBufSize - m_nWaveBufCount);
	memcpy(&m_pWaveBuf[m_nWaveBufCount], pWaveNotify->pSamplesData, len * sizeof(double));
	m_cGraphWave.DispGraph2(m_pWaveBuf, m_nWaveBufSize, m_nChannel, m_nDispChannel, m_nWaveBufCount, len);
	m_nWaveBufCount += len;

	m_fDataTime += (double)pWaveNotify->nSamplesRecorded / m_nSamplingRate;

	SetRingBuf(pWaveNotify->pSamplesData, pWaveNotify->nSamplesRecorded / DECIMATION);

	return m_nWaveBufCount < m_nWaveBufSize;
}

int CAcfDlg::WaveOutData(LPWAVENOTIFY pWaveNotify)
{
	int len;

	if (m_pWaveBuf == NULL)
		return 0;

	if (pWaveNotify->nFlags & WHDR_DONE) {
		m_nReplayPos += pWaveNotify->nSamplesNum * m_nChannel;
		double fReplayTime = (double)m_nReplayPos / (m_nSamplingRate * m_nChannel);
		m_cGraphWave.SetReplayPosition(fReplayTime);
	}

	len = min(WAVEOUT_SAMPLESPERBUFFER, m_nWaveOutEndCount - m_nWaveOutCount);
	memcpy(pWaveNotify->pSamplesData, &m_pWaveBuf[m_nWaveOutCount], len * sizeof(double));
	m_nWaveOutCount += len;

	return len / m_nChannel;
}

void CAcfDlg::SetRingBuf(const double *pData, int nData)
{
	int i;
	int len;

	while (nData > 0) {
		len = min(nData, m_nRingBufSize - m_nRingBufPtr);
		for (i = 0; i < len; i++) {
			m_pRingBuf[m_nRingBufPtr++] = pData[m_nDispChannel];
			nData--;
			pData += DECIMATION * m_nChannel;
		}

		if (m_nRingBufPtr == m_nRingBufSize) {
			m_nRingBufPtr = 0;
			m_bRingBufFull = TRUE;
		}
	}

	if (m_bRingBufFull)
		CalcAcf();
}

void CAcfDlg::CalcAcf()
{
	int i, j;
	double xt, yt;

	j = m_nRingBufPtr;

	for (i = 0; i < m_nAcfSize; i++) {
		m_pAcfBuf1[i] = m_pRingBuf[j];
		if (++j >= m_nRingBufSize)
			j = 0;
	}
	memset(&m_pAcfBuf1[i], 0, (m_nAcfBufSize - m_nAcfSize) * sizeof(double));

	m_oRFFT.fft(m_nAcfBufSize, m_pAcfBuf1);

	m_pAcfBuf1[0] = 0;
	m_pAcfBuf1[1] = 0;

	for (i = 1; i < m_nAcfBufSize / 2; i++) {
		j = i * 2;

		xt = m_pAcfBuf1[j] * m_pFilterTbl[i];
		yt = m_pAcfBuf1[j + 1] * m_pFilterTbl[i];

		m_pAcfBuf1[j] = xt * xt + yt * yt;
		m_pAcfBuf1[j + 1] = 0;
	}

	m_oRFFT.ifft(m_nAcfBufSize, m_pAcfBuf1);

	double d1 = m_pAcfBuf1[0];
	if (d1 != 0) {
		for (i = 0; i < m_nAcfBufSize; i++)
			m_pAcfBuf1[i] /= d1;
	} else {
		MEMCLEAR(m_pAcfBuf1, m_nAcfBufSize);
	}

	DispAcfGraph();
}

void CAcfDlg::DispAcfGraph()
{
	if (m_nDispChannel == 0)
		m_cGraphAcf.DispGraph(m_pAcfBuf1, NULL, m_nAcfBufSize, (double)m_nSamplingRate / DECIMATION);
	else
		m_cGraphAcf.DispGraph(NULL, m_pAcfBuf1, m_nAcfBufSize, (double)m_nSamplingRate / DECIMATION);
}

void CAcfDlg::RedrawAcfGraph()
{
	m_cGraphAcf.ClearGraph();

	if (m_pWaveBuf == NULL)
		return;

	CWaitCursor waitCursor;

	FreeBuffers();
	AllocBuffers();

	int nWaveInBufSize = (int)(m_nSamplingRate * g_oSetData.Acf.fCalcCycle) * m_nChannel;
	m_nRingBufPtr = 0;
	m_bRingBufFull = FALSE;

	m_nDispChannel = (m_nChannel == 1) ? 0 : g_oSetData.Acf.nDispChannel;

	SetFilter();

	for (int nWaveBufPtr = 0; nWaveBufPtr + nWaveInBufSize <= m_nWaveBufSize; nWaveBufPtr += nWaveInBufSize) {
		SetRingBuf(&m_pWaveBuf[nWaveBufPtr], nWaveInBufSize / (m_nChannel * DECIMATION));
	}
}

void CAcfDlg::OnChangeCalcCycle()
{
	g_oSetData.Acf.fCalcCycle = m_cCalcCycle;

	SetTimeDataNum(m_pWaveBuf == NULL ? g_oSetData.Acf.nMeasureTime : m_fDataTime);

	EnableRedraw();
}

void CAcfDlg::OnChangeTimeDataNum()
{
	if (m_cTimeDataNum.m_hWnd != NULL)
		g_oSetData.Acf.nTimeDataNum = max((int)m_cTimeDataNum, 1);
	EnableRedraw();
}

void CAcfDlg::OnChannelMono()
{
	g_oSetData.Acf.nChannel = CH_MONO;
	EnableDispRch();
}

void CAcfDlg::OnChannelStereo()
{
	g_oSetData.Acf.nChannel = CH_STEREO;
	EnableDispRch();
}

void CAcfDlg::OnSelchangeSamplingRate()
{
	int nIndex;

	if ((nIndex = m_cSamplingRate.GetCurSel()) != CB_ERR)
		g_oSetData.Acf.nSamplingRate = (int)m_cSamplingRate.GetItemData(nIndex);
}

void CAcfDlg::OnChangeIntegrationTime()
{
	g_oSetData.Acf.fIntegrationTime = m_cIntegrationTime;

	SetTimeDataNum(m_pWaveBuf == NULL ? g_oSetData.Acf.nMeasureTime : m_fDataTime);

	EnableRedraw();
}

void CAcfDlg::OnChangeMaxTau()
{
	g_oSetData.Acf.fMaxTau = m_cMaxTau;
	EnableRedraw();
}

void CAcfDlg::OnTimeFwd()
{
	if (g_oSetData.Acf.nTimeDir != 0) {
		g_oSetData.Acf.nTimeDir = 0;
		m_cGraphAcf.Invalidate(FALSE);
	}
}

void CAcfDlg::OnTimeBwd()
{
	if (g_oSetData.Acf.nTimeDir != 1) {
		g_oSetData.Acf.nTimeDir = 1;
		m_cGraphAcf.Invalidate(FALSE);
	}
}

void CAcfDlg::OnChangeMeasureTime()
{
	if (m_cMeasureTime.m_hWnd != NULL) {
		g_oSetData.Acf.nMeasureTime = m_cMeasureTime;
		SetTimeDataNum(m_pWaveBuf == NULL ? g_oSetData.Acf.nMeasureTime : m_fDataTime);
	}
}

void CALLBACK CAcfDlg::GraphCallBack(WPARAM code, LPARAM data, double param1, double param2)
{
	CAcfDlg *pWnd = (CAcfDlg *)data;

	switch (code) {
	case CC_SCROLL:
		pWnd->m_fStartTime -= param1 * pWnd->m_fDispTime;
		pWnd->DispWaveGraph();
		break;
	case CC_SELECT_SET:
		pWnd->DispSelectArea(param1, param2);
		break;
	}
}

void CAcfDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
{
	switch (nSBCode) {
	case SB_LEFT:
		m_fStartTime = 0;
		break;
	case SB_LINELEFT:
		m_fStartTime -= m_fDispTime / 32;
		break;
	case SB_LINERIGHT:
		m_fStartTime += m_fDispTime / 32;
		break;
	case SB_PAGELEFT:
		m_fStartTime -= m_fDispTime;
		break;
	case SB_PAGERIGHT:
		m_fStartTime += m_fDispTime;
		break;
	case SB_RIGHT:
		m_fStartTime = m_fTotalTime - m_fDispTime;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_fStartTime = m_fTotalTime * nPos / SCROLL_RANGE;
		break;
	}

	DispWaveGraph();
}

void CAcfDlg::OnZoomFullH()
{
	m_fDispTime = m_fTotalTime;
	DispWaveGraph();
}

void CAcfDlg::OnZoomInH()
{
	m_fDispTime /= 2;
	DispWaveGraph();
}

void CAcfDlg::OnZoomOutH()
{
	m_fDispTime *= 2;
	DispWaveGraph();
}

void CAcfDlg::OnGraphNorm()
{
	DispWaveGraph();
}

void CAcfDlg::DispWaveGraph()
{
	SCROLLINFO si;
	int nScrollSize;
	int nScrollPos;

	if (m_fDispTime == 0)
		m_fDispTime = m_fTotalTime;
	else if (m_fDispTime < MIN_DISP_TIME)
		m_fDispTime = MIN_DISP_TIME;
	else if (m_fDispTime > m_fTotalTime)
		m_fDispTime = m_fTotalTime;

	if (m_fStartTime < 0)
		m_fStartTime = 0;
	else if (m_fStartTime > m_fTotalTime - m_fDispTime)
		m_fStartTime = m_fTotalTime - m_fDispTime;

	nScrollSize = (int)(m_fDispTime / m_fTotalTime * SCROLL_RANGE);
	nScrollPos = (int)(m_fStartTime / m_fTotalTime * SCROLL_RANGE);

	if (nScrollSize != m_nScrollSize || nScrollPos != m_nScrollPos) {
		m_nScrollSize = nScrollSize;
		m_nScrollPos = nScrollPos;

		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		si.nMin = 0;
		si.nMax = SCROLL_RANGE;
		si.nPage = nScrollSize;
		si.nPos = nScrollPos;
		m_cGraphScroll.SetScrollInfo(&si, TRUE);
	}

	CToolBarCtrl &tc = m_tbZoomH.GetToolBarCtrl();
	if (m_fDispTime == m_fTotalTime / MAX_ZOOM_TIME) {
		tc.EnableButton(IDM_ZOOM_FULL_H, TRUE);
		tc.EnableButton(IDM_ZOOM_IN_H, FALSE);
		tc.EnableButton(IDM_ZOOM_OUT_H, TRUE);
	} else if (m_fDispTime == m_fTotalTime) {
		tc.EnableButton(IDM_ZOOM_FULL_H, FALSE);
		tc.EnableButton(IDM_ZOOM_IN_H, TRUE);
		tc.EnableButton(IDM_ZOOM_OUT_H, FALSE);
	} else {
		tc.EnableButton(IDM_ZOOM_FULL_H, TRUE);
		tc.EnableButton(IDM_ZOOM_IN_H, TRUE);
		tc.EnableButton(IDM_ZOOM_OUT_H, TRUE);
	}

	m_cGraphWave.DispGraph(m_fTotalTime, m_fStartTime, m_fDispTime, m_pWaveBuf, m_nWaveBufSize, m_nChannel, m_nDispChannel, (BOOL)m_cGraphNorm ? m_fMaxData : 1.0);

	m_cZoomH = int(m_fTotalTime / m_fDispTime);
}

void CAcfDlg::InitDispTime()
{
	m_fTotalTime = g_oSetData.Acf.nMeasureTime;
	m_fDispTime = m_fTotalTime;
	m_fStartTime = 0;
}

void CAcfDlg::OnScaleAbs()
{
	g_oSetData.Acf.bScaleAbs = m_cScaleAbs;
	RedrawAcfGraph();
}

void CAcfDlg::OnChangeScaleLevel()
{
	if ((int)m_cScaleLevel > 0)
			m_cScaleLevel = -(int)m_cScaleLevel;

	if (m_cScaleLevel != 0) {
		g_oSetData.Acf.nScaleLevel = m_cScaleLevel;
	}

	EnableRedraw();
}

void CAcfDlg::OnScaleLiner()
{
	UpdateData(TRUE);
	g_oSetData.Acf.nScaleType = m_iScaleType;
	EnableScaleCtrl();
	RedrawAcfGraph();
}

void CAcfDlg::OnScaleLog()
{
	UpdateData(TRUE);
	g_oSetData.Acf.nScaleType = m_iScaleType;
	EnableScaleCtrl();
	RedrawAcfGraph();
}

void CAcfDlg::OnSelchangeFreqFilter()
{
	g_oSetData.Acf.nFilter = m_cFreqFilter.GetCurSel();
	RedrawAcfGraph();
}

void CAcfDlg::EnableScaleCtrl()
{
	m_cScaleAbs.EnableWindow(g_oSetData.Acf.nScaleType == 0);
	m_cScaleLevel.EnableWindow(g_oSetData.Acf.nScaleType != 0);
}

void CAcfDlg::SetMeasureParam()
{
	m_cMrTime = ::FormatTime(m_sTime);
	m_cMrSampling = m_nSamplingRate;
	m_cMrMtime = m_fDataTime;
}

void CAcfDlg::OnFileSave()
{
	if (!CHECK_LICENSE1(APP_RA))
		return;

	HWAVEDATA hWaveData = MakeWaveDataHandle(m_nSamplingRate, m_nBitsPerSample, m_nChannel, m_nWaveBufSize, m_pWaveBuf, NULL);

	if (hWaveData == NULL)
		return;

	m_pWaveOut->Close();

	CSaveACF dlg(this);

	dlg.m_iSampling = m_nSamplingRate;
	dlg.m_nChannel = m_nChannel;
	dlg.m_nSample = m_nSample;
	dlg.m_nBitsPerSample = m_nBitsPerSample;
	dlg.m_fDataTime = m_fDataTime;
	dlg.m_Time = m_sTime;
	dlg.m_hWaveData = hWaveData;

	dlg.DoModal();

	::GlobalFree(hWaveData);
}

void CAcfDlg::OnFileLoad()
{
	if (!CHECK_LICENSE1(APP_RA))
		return;

	CLoadACF dlg(this);

	m_pWaveOut->Close();

	switch (dlg.DoModal()) {
	case IDOK:
		LoadACFData(dlg.m_nAcfID);
		break;
	case IDC_WAVE_FILE:
		LoadWaveFile(dlg.m_sWaveFile);
		break;
	}
}

void CAcfDlg::LoadACFData(long nAcfID)
{
	CDbAcf dbAcf;
	DbAcfRec dbAcfRec;
	HWAVEDATA hWaveData;
	PWAVEDATA pWaveData;

	if (!dbAcf.Open())
		return;

	if (!dbAcf.ReadRecID(nAcfID, &dbAcfRec))
		return;

	if (MMWaveReadDB(&hWaveData, dbAcf.GetFCB(), "WAVEDATA") != 0) {
		::AfxMessageBox(IDS_ERR_READDATA, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	m_nSamplingRate = dbAcfRec.nSampling;
	m_nChannel = dbAcfRec.nChannel & 0x03;
	m_nSample = dbAcfRec.nSampleNum;
	m_fDataTime = dbAcfRec.fDataTime;
	SetTimeDataNum(m_fDataTime);

	m_fTotalTime = m_fDataTime;
	m_fDispTime = m_fTotalTime;
	m_fStartTime = 0;

	m_sTime = dbAcfRec.sTime;
	m_nWaveBufSize = m_nSample * m_nChannel;

	if (m_pWaveBuf != NULL)
		delete [] m_pWaveBuf;

	pWaveData = (PWAVEDATA)::GlobalLock(hWaveData);
	if (pWaveData == NULL)
		return;
	m_nBitsPerSample = pWaveData->nBitsPerSample;
	m_pWaveBuf = new double[m_nWaveBufSize];
	CopyWaveToDouble((BYTE *)pWaveData->waveData, m_pWaveBuf, NULL, m_nWaveBufSize, m_nBitsPerSample);
	::GlobalFree(hWaveData);

	EnableDispRch();

	m_fMaxData = GetMaxData(m_pWaveBuf, m_nWaveBufSize);
	RedrawAcfGraph();
	DispWaveGraph();
	m_cGraphWave.SetSelectArea(0, m_fDataTime);
	SetMeasureParam();
	EnableButton();
}

void CAcfDlg::LoadWaveFile(LPCTSTR pFileName)
{
	CFileStatus st;
	if (!CFile::GetStatus(pFileName, st))
		return;

	CString fileTime;
	fileTime.Format("%04d%02d%02d%02d%02d%02d", st.m_mtime.GetYear(), st.m_mtime.GetMonth(), st.m_mtime.GetDay(),
					st.m_mtime.GetHour(), st.m_mtime.GetMinute(), st.m_mtime.GetSecond());

	CWaveFile waveFile;
	PCMWAVEFORMAT waveFormat;
	DWORD dwDataSize;

	if (!waveFile.Open(pFileName, &waveFormat, &dwDataSize)) {
		::AfxMessageBox(IDS_ERR_READWAVEFILE, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	m_nSamplingRate = waveFormat.wf.nSamplesPerSec;
	m_nBitsPerSample = waveFormat.wBitsPerSample;
	m_nChannel = waveFormat.wf.nChannels;
	m_nWaveBufSize = dwDataSize / (waveFormat.wBitsPerSample / 8);
	m_nSample = m_nWaveBufSize / m_nChannel;
	m_fDataTime = double(m_nSample) / m_nSamplingRate;
	SetTimeDataNum(m_fDataTime);

	m_fTotalTime = m_fDataTime;
	m_fDispTime = m_fTotalTime;
	m_fStartTime = 0;

	m_sTime = fileTime;

	if (m_pWaveBuf != NULL)
		delete [] m_pWaveBuf;

	m_pWaveBuf = new double[m_nWaveBufSize];
	waveFile.Read(m_pWaveBuf, m_nWaveBufSize);

	EnableDispRch();

	m_fMaxData = GetMaxData(m_pWaveBuf, m_nWaveBufSize);
	RedrawAcfGraph();
	DispWaveGraph();
	m_cGraphWave.SetSelectArea(0, m_fDataTime);
	SetMeasureParam();
	EnableButton();
}

void CAcfDlg::OnRedraw()
{
	m_cRedraw.EnableWindow(FALSE);
	RedrawAcfGraph();
}

void CAcfDlg::EnableRedraw()
{
	if (m_pWaveBuf != NULL)
		m_cRedraw.EnableWindow(TRUE);
}

void CAcfDlg::EnableButton(BOOL bEnable)
{
	BOOL bValid = bEnable && m_pWaveBuf != NULL;

	m_cFileSave.EnableWindow(bValid);
	m_cReplay.EnableWindow(bValid);
	m_cCut.EnableWindow(bValid);
	m_cSelectAll.EnableWindow(bValid);
	m_cLaunchSA.EnableWindow(bValid);
}

void CAcfDlg::OnReplay()
{
	if (m_cReplay.GetCheck() == 0) {
		m_pWaveIn->Close();
		m_nWaveOutCount = min((int)(m_fSelectStart * m_nSamplingRate * m_nChannel), m_nWaveBufSize);
		m_nWaveOutEndCount = min((int)(m_fSelectEnd * m_nSamplingRate * m_nChannel), m_nWaveBufSize);
		m_nReplayPos = m_nWaveOutCount;
		if (m_pWaveOut->Open(g_nWaveOutDevice, this, m_nChannel, m_nSamplingRate, WAVEOUT_SAMPLESPERBUFFER, WAVEOUT_BUFNUM, TRUE, FALSE))
			m_pWaveOut->Start();
	} else
		m_pWaveOut->Close();
}

void CAcfDlg::OnCut()
{
	int nSelectStart;
	int nSelectEnd;
	int nSample;
	double *pWaveBuf;

	if (m_pWaveBuf == NULL)
		return;

	nSelectStart = min((int)(m_fSelectStart * m_nSamplingRate * m_nChannel), m_nWaveBufSize);
	nSelectEnd = min((int)(m_fSelectEnd * m_nSamplingRate * m_nChannel), m_nWaveBufSize);
	nSample = nSelectEnd - nSelectStart;

	if (nSample <= 0)
		return;

	pWaveBuf = new double[nSample];
	memcpy(pWaveBuf, m_pWaveBuf + nSelectStart, nSample * sizeof(double));
	delete [] m_pWaveBuf;

	m_pWaveBuf = pWaveBuf;
	m_nWaveBufSize = nSample;

	m_nSample = nSample / m_nChannel;
	m_fDataTime = (double)m_nSample / m_nSamplingRate;
	SetTimeDataNum(m_fDataTime);
	m_fTotalTime = m_fDataTime;

	m_fMaxData = GetMaxData(m_pWaveBuf, m_nWaveBufSize);
	RedrawAcfGraph();
	DispWaveGraph();
	m_cGraphWave.SetSelectArea(0, m_fDataTime);
	SetMeasureParam();
}

void CAcfDlg::OnSelectAll()
{
	m_cGraphWave.SetSelectArea(0, m_fDataTime);
}

void CAcfDlg::DispSelectArea(double fSelectStart, double fSelectEnd)
{
	m_bDispSelectArea = TRUE;

	if (m_fSelectStart != fSelectStart) {
		m_fSelectStart = fSelectStart;
		m_cSelectStart.Format("%.2f", fSelectStart);
	}

	if (m_fSelectEnd != fSelectEnd) {
		m_fSelectEnd = fSelectEnd;
		m_cSelectEnd.Format("%.2f", fSelectEnd);
	}

	m_cSelectTime.Format("%.2f", fSelectEnd - fSelectStart);

	m_bDispSelectArea = FALSE;
}

void CAcfDlg::SetTimeDataNum(double fTime)
{
	m_cTimeDataNum = max((int)((fTime - g_oSetData.Acf.fIntegrationTime) / g_oSetData.Acf.fCalcCycle) + 1, 0);
}

void CAcfDlg::OnChangeSelectStart()
{
	if (m_bDispSelectArea)
		return;

	m_fSelectStart = m_cSelectStart;

	if (m_fSelectStart < 0)
		m_fSelectStart = 0;

	if (m_fSelectStart > m_fSelectEnd - MIN_DISP_TIME)
		m_fSelectStart = m_fSelectEnd - MIN_DISP_TIME;

	m_cGraphWave.SetSelectArea(m_fSelectStart, m_fSelectEnd);
}

void CAcfDlg::OnChangeSelectEnd()
{
	if (m_bDispSelectArea)
		return;

	m_fSelectEnd = m_cSelectEnd;

	if (m_fSelectEnd > m_fDataTime)
		m_fSelectEnd = m_fDataTime;

	if (m_fSelectEnd < m_fSelectStart + MIN_DISP_TIME)
		m_fSelectEnd = m_fSelectStart + MIN_DISP_TIME;

	m_cGraphWave.SetSelectArea(m_fSelectStart, m_fSelectEnd);
}

void CAcfDlg::OnChannelLch()
{
	g_oSetData.Acf.nDispChannel = 0;
	RedrawAcfGraph();
	DispWaveGraph();
}

void CAcfDlg::OnChannelRch()
{
	g_oSetData.Acf.nDispChannel = 1;
	RedrawAcfGraph();
	DispWaveGraph();
}

void CAcfDlg::EnableDispRch()
{
	BOOL bEnable;

	if (m_pWaveBuf == NULL)
		bEnable = (g_oSetData.Acf.nChannel == CH_STEREO);
	else
		bEnable = (m_nChannel == 2);

	if (!bEnable && g_oSetData.Acf.nDispChannel != 0) {
		g_oSetData.Acf.nDispChannel = 0;
		m_iDispChannel = 0;
		UpdateData(FALSE);
	}

	m_cChannelRch.EnableWindow(bEnable);
}

void CAcfDlg::EnableControl(BOOL bEnable)
{
	m_cFileLoad.EnableWindow(bEnable);
	m_cSamplingRate.EnableWindow(bEnable);
	m_cMeasureTime.EnableWindow(bEnable);
	m_cChannelMono.EnableWindow(bEnable);
	m_cChannelStereo.EnableWindow(bEnable);
	m_cIntegrationTime.EnableWindow(bEnable);
	m_cCalcCycle.EnableWindow(bEnable);
	m_cMaxTau.EnableWindow(bEnable);
	m_cFreqFilter.EnableWindow(bEnable);
	m_cTimeDataNum.EnableWindow(bEnable);
	m_cTimeFwd.EnableWindow(bEnable);
	m_cTimeBwd.EnableWindow(bEnable);
	m_cScale_Liner.EnableWindow(bEnable);
	m_cScale_Log.EnableWindow(bEnable);
	m_cChannelLch.EnableWindow(bEnable);
	m_cGraphNorm.EnableWindow(bEnable);
	m_cGraphScroll.EnableWindow(bEnable);
	m_cExit.EnableWindow(bEnable);
	m_cAcfRealtimeGraph.EnableWindow(bEnable);

	if (bEnable) {
		EnableDispRch();
		EnableScaleCtrl();
	} else {
		m_cChannelRch.EnableWindow(FALSE);
		m_cScaleAbs.EnableWindow(FALSE);
		m_cScaleLevel.EnableWindow(FALSE);
	}
}

void CAcfDlg::ChangeWaveDevice(int nInOut)
{
	if (nInOut & WAVE_IN) {
		OnStop();

		SetSamplingRateList(m_cSamplingRate, WAVE_IN, g_oSetData.Pc.nInputDevice, 0, g_oSetData.Acf.nSamplingRate);
	}
}

void CAcfDlg::ChangeSamplingRate(int nSamplingRate)
{
	if (g_oSetData.Acf.nSamplingRate != nSamplingRate) {
		int n = m_cSamplingRate.GetCount();
		for (int i = 0; i < n; i++) {
			if ((int)m_cSamplingRate.GetItemData(i) == nSamplingRate) {
				m_cSamplingRate.SetCurSel(i);
				OnSelchangeSamplingRate();
				break;
			}
		}
	}
}

BOOL CAcfDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_GRAPH_ACF, IDH_ACF_GRAPH,
		IDC_GRAPH_WAVE, IDH_ACF_GRAPH_WAVE,
		IDC_ZOOM_H, IDH_ACF_ZOOM_H,
		IDC_SCROLL_H, IDH_ACF_SCROLL_H,
		IDC_GRAPH_NORM, IDH_ACF_GRAPH_NORM,
		IDC_MR_TIME, IDH_ACF_MR_TIME,
		IDC_MR_SAMPLING, IDH_ACF_MR_SAMPLING,
		IDC_MR_MTIME, IDH_ACF_MR_MTIME,
		IDC_SELECT_START, IDH_ACF_SELECT_START,
		IDC_SELECT_END, IDH_ACF_SELECT_END,
		IDC_SELECT_TIME, IDH_ACF_SELECT_TIME,
		IDC_CUT, IDH_ACF_CUT,
		IDC_SELECT_ALL, IDH_ACF_SELECT_ALL,
		IDC_REPLAY, IDH_ACF_REPLAY,
		IDC_BTN_START, IDH_ACF_START,
		IDC_BTN_STOP, IDH_ACF_STOP,
		IDC_FILE_SAVE, IDH_ACF_FILE_SAVE,
		IDC_FILE_LOAD, IDH_ACF_FILE_LOAD,
		IDC_SAMPLING_RATE, IDH_ACF_SAMPLING_RATE,
		IDC_MEASURE_TIME, IDH_ACF_MEASURE_TIME,
		IDC_CHANNEL_MONO, IDH_ACF_CHANNEL_MONO,
		IDC_CHANNEL_STEREO, IDH_ACF_CHANNEL_STEREO,
		IDC_INTEGRATION_TIME, IDH_ACF_INTEGRATION_TIME,
		IDC_CALC_CYCLE, IDH_ACF_CALC_CYCLE,
		IDC_MAX_TAU, IDH_ACF_MAX_TAU,
		IDC_FREQ_FILTER, IDH_ACF_FREQ_FILTER,
		IDC_TIME_DATA_NUM, IDH_ACF_TIME_DATA_NUM,
		IDC_TIME_FWD, IDH_ACF_TIME_FWD,
		IDC_TIME_BWD, IDH_ACF_TIME_BWD,
		IDC_SCALE_LINER, IDH_ACF_SCALE_LINER,
		IDC_SCALE_ABS, IDH_ACF_SCALE_ABS,
		IDC_SCALE_LOG, IDH_ACF_SCALE_LOG,
		IDC_SCALE_LEVEL, IDH_ACF_SCALE_LEVEL,
		IDC_CHANNEL_LCH, IDH_ACF_CHANNEL_LCH,
		IDC_CHANNEL_RCH, IDH_ACF_CHANNEL_RCH,
		IDC_REDRAW, IDH_ACF_REDRAW,
		IDCANCEL, IDH_ACF_CLOSE,
		ID_ZOOM_H_FULL, IDH_ACF_ZOOM_H_FULL,
		ID_ZOOM_H_IN, IDH_ACF_ZOOM_H_IN,
		ID_ZOOM_H_OUT, IDH_ACF_ZOOM_H_OUT,
		IDC_LAUNCH_SA, IDH_ACF_LAUNCH_SA,
		IDC_ACF_REALTIME_GRAPH, IDH_ACF_REALTIME_GRAPH,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

void CAcfDlg::OnSize(UINT nType, int cx, int cy)
{
	if (m_cGraphWave.m_hWnd)
		m_cGraphWave.ModifyStyle(WS_CLIPCHILDREN, 0);

	CBaseDlg::OnSize(nType, cx, cy);

	if (m_cGraphWave.m_hWnd)
		m_cGraphWave.ModifyStyle(0, WS_CLIPCHILDREN);

	if (m_bInitialized) {
		m_cGraphAcf.Resize();
		m_cGraphWave.Resize();
		RedrawAcfGraph();
		DispWaveGraph();
	}
}

void CAcfDlg::OnBnClickedLaunchSa()
{
	HWAVEDATA hWaveData = MakeWaveDataHandle(m_nSamplingRate, m_nBitsPerSample, m_nChannel, m_nWaveBufSize, m_pWaveBuf, NULL);
	if (hWaveData != NULL) {
		if (SaveACF(hWaveData)) {
			STARTUPINFO si;
			PROCESS_INFORMATION pi;
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			::CreateProcess(NULL, "SAS /ACF", NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
			::CloseHandle(pi.hProcess);
			::CloseHandle(pi.hThread);
		}
	}

	::GlobalFree(hWaveData);
}

BOOL CAcfDlg::SaveACF(HWAVEDATA hWaveData)
{
	CDbAcf dbAcf;
	DbAcfRec dbAcfRec;

	if (!dbAcf.Open())
		return FALSE;

	CString sTitle;
	sTitle.LoadString(IDS_MEASUREMENT_DATA);

	dbAcfRec.nAcfID = -2;
	dbAcfRec.nFolderID = -2;
	dbAcfRec.nSampling = m_nSamplingRate;
	dbAcfRec.nChannel = m_nChannel | (m_nBitsPerSample >= 24 ? 0x04 : 0);
	dbAcfRec.nSampleNum = m_nSample;
	dbAcfRec.fDataTime = m_fDataTime;
	dbAcfRec.sTime = m_sTime;
	dbAcfRec.sTitle= sTitle;
	dbAcfRec.sComment.Empty();

	dbAcf.DBChgIdx(dbAcf.m_nIdxAcfID);
	dbAcf.DBDeleteKey("-2", 2, 1);

	if (!dbAcf.StoreRec(&dbAcfRec))
		return FALSE;

	if (MMWaveWriteDB(hWaveData, dbAcf.GetFCB(), "WAVEDATA", MMTYPE_WAV) != 0)
		return FALSE;

	return TRUE;
}

LRESULT CAcfDlg::OnCloseAcfRealtimeDialog(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	m_pAcfRealtimeDlg = NULL;
	m_cAcfRealtimeGraph = FALSE;

	return 0;
}

void CAcfDlg::OnBnClickedAcfRealtimeGraph()
{
	if (m_cAcfRealtimeGraph) {
		if (m_pAcfRealtimeDlg == NULL)
			m_pAcfRealtimeDlg = new CAcfRealtimeDlg(this);
	} else {
		if (m_pAcfRealtimeDlg != NULL)
			m_pAcfRealtimeDlg->DestroyWindow();
	}
}
