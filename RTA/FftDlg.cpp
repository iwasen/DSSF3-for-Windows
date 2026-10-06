// FftDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "RtaDlg.h"
#include "FftDlg.h"
#include "MicCalDlg.h"
#include "FileIO.h"
#include "WaveInEx.h"
#include "DataRecordDlg.h"
#include "Regist.h"
#include "Help\ContextHelp.h"

#define MSG_NULL		0
#define MSG_CPU_WARNING	1
#define LIMIT_FREQ	50000
#define ID_FFT_WINDOW	31000

/////////////////////////////////////////////////////////////////////////////
// CFftDlg ダイアログ

CFftDlg::CFftDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CFftDlg::IDD, pParent)
{
	m_iFftCorrelation = -1;
	m_iFftScale = -1;
	m_iFftTimeDir = -1;
	m_iFftPeakHoldMode = -1;

	m_bStart = FALSE;
	m_nCurrentMsgNo = MSG_NULL;
	m_bInitialized = FALSE;
	m_bMax = FALSE;
	m_bModeChange = FALSE;
	m_pWaveBuf = NULL;
	m_nBufSize = 0;
	m_nTimeCount = -1;
	m_pDataRecord = NULL;
	m_fTimeCount = 0;
	m_pWaveIn = CWaveInEx::GetInstance();
	m_bEnableCsvOutput = FALSE;
	m_pFftLevelDlg = NULL;
	m_pFftFreqDlg = NULL;
	m_nWindowKind = WINDOW_FFT;

	Create(IDD, g_oSetData.Fft.nPosLeft, g_oSetData.Fft.nPosTop, g_oSetData2.nFontSizeFft, pParent);
}

CFftDlg::~CFftDlg()
{
	if (m_pWaveBuf != NULL)
		delete [] m_pWaveBuf;

	m_pWaveIn->ReleaseInstance();
}

void CFftDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FFT_AVERAGE, m_cFftAverage);
	DDX_Control(pDX, IDC_FFT_TIME, m_cFftTime);
	DDX_Control(pDX, IDC_FFT_PEAK_RESET, m_cFftPeakReset);
	DDX_Control(pDX, IDC_FFT_TIME_RES, m_cFftTimeRes);
	DDX_Control(pDX, IDC_FFT_PEAK_TIME, m_cFftPeakHoldTime);
	DDX_Control(pDX, IDC_FFT_CF_ZOOM, m_cFftCfZoom);
	DDX_Control(pDX, IDC_RL_SPLIT, m_cRlSplit);
	DDX_Control(pDX, IDC_FFT_FILTER, m_cFftFilter);
	DDX_Control(pDX, IDC_FFT_PEAK_FREQ, m_cFftPeakFreq);
	DDX_Control(pDX, IDC_FFT_PEAK_LEVEL, m_cFftPeakLevel);
	DDX_Control(pDX, IDC_FFT_RES_TIME, m_cFftResTime);
	DDX_Control(pDX, IDC_FFT_RES_FREQ, m_cFftResFreq);
	DDX_Control(pDX, IDC_FFT_PEAK_LEVEL_RIGHT, m_cFftPeakLevelRight);
	DDX_Control(pDX, IDC_FFT_PEAK_LEVEL_LEFT, m_cFftPeakLevelLeft);
	DDX_Control(pDX, IDC_FFT_PEAK_FREQ_RIGHT, m_cFftPeakFreqRight);
	DDX_Control(pDX, IDC_FFT_PEAK_FREQ_LEFT, m_cFftPeakFreqLeft);
	DDX_Control(pDX, IDC_FFT_MESSAGE, m_cFftMessage);
	DDX_Control(pDX, IDC_FFT_PEAK_DISP, m_cFftPeakDisp);
	DDX_Control(pDX, IDC_FFT_PEAK_HOLD, m_cFftPeakHold);
	DDX_Control(pDX, IDC_FFT_MIN_LEVEL, m_cFftMinLevel);
	DDX_Control(pDX, IDC_FFT_MIN_FREQ, m_cFftMinFreq);
	DDX_Control(pDX, IDC_FFT_MAX_LEVEL, m_cFftMaxLevel);
	DDX_Control(pDX, IDC_FFT_MAX_FREQ, m_cFftMaxFreq);
	DDX_Control(pDX, IDC_FFT_AUTO_LEVEL, m_cFftAutoLevel);
	DDX_Control(pDX, IDC_FFT_AUTO_FREQ, m_cFftAutoFreq);
	DDX_Control(pDX, IDC_FFT_TIME_DATA_NUM, m_cFftTimeDataNum);
	DDX_Control(pDX, IDC_BTN_START, m_cFftStart);
	DDX_Control(pDX, IDC_FFT_WINFUNC, m_cFftWinFunc);
	DDX_Control(pDX, IDC_FFT_FFT_SIZE, m_cFftFftSize);
	DDX_Control(pDX, IDC_FFT_SMOOTHING, m_cFftSmoothing);
	DDX_Control(pDX, IDC_SAMPLING_RATE, m_cSamplingRate);
	DDX_Control(pDX, IDC_FFT_TAB, m_cFftTab);
	DDX_Control(pDX, IDC_FFT_OCT_BAND, m_cFftOctBand);
	DDX_Control(pDX, IDC_DATA_RECORD, m_cDataRecord);
	DDX_Control(pDX, IDC_FFT_TIME_RANGE, m_cFftTimeRange);
	DDX_Control(pDX, IDC_FFT_CHANNEL, m_cFftChannel);
	DDX_Radio(pDX, IDC_FFT_CF_AUTO, m_iFftCorrelation);
	DDX_Radio(pDX, IDC_FFT_LOG_SCALE, m_iFftScale);
	DDX_Radio(pDX, IDC_FFT_TIME_FWD, m_iFftTimeDir);
	DDX_Radio(pDX, IDC_FFT_PEAK_AUTO, m_iFftPeakHoldMode);
	DDX_Radio(pDX, IDC_FFT_COLOR, m_iFftColorScale);
	DDX_Control(pDX, IDC_FFT_AUTO_STOP, m_cFftAutoStop);
	DDX_Control(pDX, IDC_FFT_AUTO_STOP_SPIN, m_cFftAutoStopSpin);
}

BEGIN_MESSAGE_MAP(CFftDlg, CBaseDlg)
	ON_MESSAGE(WM_TAB_DRAG, OnTabDrag)
	ON_MESSAGE(WM_TAB_DBLCLICK, OnTabDblClick)
	ON_MESSAGE(WM_FREQ_LEVEL_LEFT, OnFreqLevelLeft)
	ON_MESSAGE(WM_FREQ_LEVEL_RIGHT, OnFreqLevelRight)
	ON_MESSAGE(WM_DATA_RECORD, OnDataRecord)
	ON_BN_CLICKED(IDC_BTN_START, OnStart)
	ON_CBN_SELCHANGE(IDC_SAMPLING_RATE, OnSelchangeSamplingRate)
	ON_NOTIFY(TCN_SELCHANGE, IDC_FFT_TAB, OnSelchangeFftTab)
	ON_BN_CLICKED(IDC_FFT_AUTO_FREQ, OnFftAutoFreq)
	ON_BN_CLICKED(IDC_FFT_AUTO_LEVEL, OnFftAutoLevel)
	ON_CBN_SELCHANGE(IDC_FFT_SMOOTHING, OnSelchangeFftSmoothing)
	ON_CBN_SELCHANGE(IDC_FFT_FFT_SIZE, OnSelchangeFftFftSize)
	ON_BN_CLICKED(IDC_FFT_LINEAR_SCALE, OnFftLinearScale)
	ON_BN_CLICKED(IDC_FFT_LOG_SCALE, OnFftLogScale)
	ON_EN_CHANGE(IDC_FFT_MAX_FREQ, OnChangeFftMaxFreq)
	ON_EN_CHANGE(IDC_FFT_MAX_LEVEL, OnChangeFftMaxLevel)
	ON_EN_CHANGE(IDC_FFT_MIN_FREQ, OnChangeFftMinFreq)
	ON_EN_CHANGE(IDC_FFT_MIN_LEVEL, OnChangeFftMinLevel)
	ON_CBN_SELCHANGE(IDC_FFT_WINFUNC, OnSelchangeFftWinfunc)
	ON_BN_CLICKED(IDC_FFT_PEAK_DISP, OnFftPeakDisp)
	ON_BN_CLICKED(IDC_FFT_PEAK_HOLD, OnFftPeakHold)
	ON_BN_CLICKED(IDC_FFT_TIME_FWD, OnFftTimeFwd)
	ON_BN_CLICKED(IDC_FFT_TIME_BWD, OnFftTimeBwd)
	ON_CBN_SELCHANGE(IDC_FFT_TIME_DATA_NUM, OnSelchangeFftTimeDataNum)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_FFT_CF_AUTO, OnFftCfAuto)
	ON_BN_CLICKED(IDC_FFT_CF_LATERAL, OnFftCfLateral)
	ON_BN_CLICKED(IDC_FFT_CALIBRATION, OnFftCalibration)
	ON_WM_MOVE()
	ON_CBN_SELCHANGE(IDC_FFT_FILTER, OnSelchangeFftFilter)
	ON_BN_CLICKED(IDC_RL_SPLIT, OnRlSplit)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_FFT_PEAK_AUTO, OnFftPeakAuto)
	ON_BN_CLICKED(IDC_FFT_PEAK_MANUAL, OnFftPeakManual)
	ON_EN_CHANGE(IDC_FFT_PEAK_TIME, OnChangeFftPeakTime)
	ON_BN_CLICKED(IDC_FFT_PEAK_RESET, OnFftPeakReset)
	ON_CBN_SELCHANGE(IDC_FFT_TIME_RES, OnSelchangeFftTimeRes)
	ON_CBN_SELCHANGE(IDC_FFT_AVERAGE, OnSelchangeFftAverage)
	ON_BN_CLICKED(IDC_DATA_RECORD, OnBnClickedDataRecord)
	ON_BN_CLICKED(IDC_SCREEN_SHOT, OnBnClickedScreenShot)
	ON_CBN_SELCHANGE(IDC_FFT_OCT_BAND, OnSelchangeFftOctBand)
	ON_CBN_SELCHANGE(IDC_FFT_TIME_RANGE, OnSelchangeFftTimeRange)
	ON_BN_CLICKED(IDC_FFT_COLOR, OnFftColor)
	ON_BN_CLICKED(IDC_FFT_GRAY, OnFftGray)
	ON_CBN_SELCHANGE(IDC_FFT_CHANNEL, OnSelchangeFftChannel)
	ON_EN_CHANGE(IDC_FFT_AUTO_STOP, OnEnChangeFftAutoStop)
	ON_BN_CLICKED(IDC_CSV_OUTPUT, OnBnClickedCsvOutput)
	ON_WM_HELPINFO()
	ON_WM_THEMECHANGED()
	ON_WM_SIZE()
	ON_MESSAGE(WM_MYCTRL_RBUTTONDOWN, OnMyCtrlRButtonDown)
	ON_BN_CLICKED(IDC_CSV_INPUT, &CFftDlg::OnBnClickedCsvInput)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFftDlg メッセージ ハンドラ

BOOL CFftDlg::OnInitDialog()
{
	CString str;
	int i;

	CBaseDlg::OnInitDialog();

	CreateStatusBar();
	SetStatusBar(IDS_STATUS_STOP);

	CRect rectSeparator;
	CRect rectControl;
	GetDlgItem(IDC_SEPARATOR_LINE)->GetWindowRect(rectSeparator);
	CWnd *pWnd = GetTopWindow();
	while (pWnd != NULL) {
		switch (pWnd->GetDlgCtrlID()) {
		case IDC_FFT_TAB:
			SetCtlPosition(pWnd, 0, 0, 1, 1);
			break;
		case IDC_SEPARATOR_LINE:
			SetCtlPosition(pWnd, 0, 1, 1, 1);
			break;
		default:
			pWnd->GetWindowRect(rectControl);
			if (rectControl.top < rectSeparator.top)
				SetCtlPosition(pWnd, 1, 0, 1, 0);
			else
				SetCtlPosition(pWnd, 0, 1, 0, 1);
			break;
		}
		pWnd = pWnd->GetNextWindow();
	}

	SetSamplingRateList(m_cSamplingRate, WAVE_IN, g_oSetData.Pc.nInputDevice, 0, g_oSetData.Fft.nSamplingRate);

	if (!g_dllUxTheme.IsThemeActive())
		m_cFftTab.ModifyStyle(0, TCS_OWNERDRAWFIXED);

	SetMicCalData();

	BOOL bRegistRA = (BOOL)CHECK_LICENSE2(APP_RA);
	for (i = 0; i < FFT_MODE_NUM; i++) {
		if (bRegistRA || i <= FFT_MODE_SPG) {
			m_cFftTab.AddPage(NULL, i, g_tFftModeId[i], FALSE, TRUE);
			if (g_oSetData.Fft.bSeparateWindow[i])
				m_cFftTab.SetGrayed(i, TRUE);
			else
				m_cFftTab.SetDrag(i, TRUE);
		}
	}

	m_wndFft.CreateEx(/*WS_EX_CLIENTEDGE*/0, ::AfxRegisterWndClass(CS_DBLCLKS),
				"FftWindow", WS_CHILD/*  | WS_VISIBLE| WS_BORDER*/,
				GetFftWindowRect(),
				&m_cFftTab, ID_FFT_WINDOW);

	m_cFftTab.SetPage(g_oSetData.Fft.nMode);

	m_wndFft.InitFFT();

	if (!g_oSetData.Fft.bSeparateWindow[g_oSetData.Fft.nMode])
		EnableFftWindow();

	m_wndFft.m_pFftDlg = this;

	m_cFftAutoFreq = g_oSetData.Fft.bFftAutoFreq;
	m_cFftMaxFreq = g_oSetData.Fft.nFftMaxFreq;
	m_cFftMinFreq = g_oSetData.Fft.nFftMinFreq;

	m_cFftAutoLevel = g_oSetData.Fft.bFftAutoLevel;
	m_cFftMaxLevel = g_oSetData.Fft.nFftMaxLevel;
	m_cFftMinLevel = g_oSetData.Fft.nFftMinLevel;

	m_iFftScale = g_oSetData.Fft.nFftScale;

	m_cRlSplit = g_oSetData.Fft.bRlSplit;

	// 平滑化
	static constexpr int tTimeConstant[] = {35, 125, 250, 500, 1000, 2000, 4000, 8000, 16000};
	// 平滑化の旧バージョンからのコンバージョン
	if (g_oSetData.Fft.nSmoothing != 0 && g_oSetData.Fft.nSmoothing <= 10)
		g_oSetData.Fft.nSmoothing = 125;
	str.LoadString(IDS_NONE);
	m_cFftSmoothing.AddString(str);
	for (i = 0; i < ARRAY_SIZE(tTimeConstant); i++) {
		const int nTimeConstant = tTimeConstant[i];
		if (nTimeConstant < 1000)
			str.Format("%dms", nTimeConstant);
		else
			str.Format("%ds", nTimeConstant / 1000);
		if (nTimeConstant == 125)
			str += " (fast)";
		else if (nTimeConstant == 1000)
			str += " (slow)";
		const int n = m_cFftSmoothing.AddString(str);
		m_cFftSmoothing.SetItemData(n, nTimeConstant);
	}
	SelectListBox(m_cFftSmoothing, g_oSetData.Fft.nSmoothing);

	SetWindowFuncList(m_cFftWinFunc);
	m_cFftWinFunc.SetCurSel(g_oSetData.Fft.nFftWindowFunc);

	SetFilterNameList(m_cFftFilter);
	m_cFftFilter.SetCurSel(g_oSetData.Fft.nFilter);

	for (i = 1024; i <= 65536; i *= 2) {
		str.Format("%d", i);
		m_cFftFftSize.AddString(str);
	}
	str.Format("%d", g_oSetData.Fft.nFftSize);
	m_cFftFftSize.SelectString(-1, str);

	m_cFftPeakDisp = g_oSetData.Fft.bPeakDisp;
	m_cFftPeakHold = g_oSetData.Fft.bPeakHold;
	m_iFftPeakHoldMode = g_oSetData.Fft.nPeakHoldMode;
	m_cFftPeakHoldTime = g_oSetData.Fft.nPeakHoldTime;

	for (i = 5; i <= MAX_3DD; i++) {
		str.Format("%d", i);
		m_cFftTimeDataNum.AddString(str);
	}
	str.Format("%d", g_oSetData.Fft.nTimeDataNum);
	m_cFftTimeDataNum.SelectString(-1, str);

	static constexpr int TimeResTbl[] = {1, 2, 4, 8};
	for (i = 0; i < ARRAY_SIZE(TimeResTbl); i++) {
		str.Format("%dx", TimeResTbl[i]);
		m_cFftTimeRes.InsertString(i, str);
		m_cFftTimeRes.SetItemData(i, TimeResTbl[i]);
	}
	SelectListBox(m_cFftTimeRes, g_oSetData.Fft.nTimeRes);

	for (i = 0; i <= 10; i++) {
		if (i == 0)
			str.LoadString(IDS_NONE);
		else
			str.Format("%ds", i);
		m_cFftAverage.InsertString(i, str);
		m_cFftAverage.SetItemData(i, i);
	}
	str.LoadString(IDS_INFINITE);
	m_cFftAverage.InsertString(i, str);
	m_cFftAverage.SetItemData(i, (DWORD_PTR)-1);
	SelectListBox(m_cFftAverage, g_oSetData.Fft.nAverage);

	m_iFftTimeDir = g_oSetData.Fft.nTimeDir;
	m_iFftCorrelation = g_oSetData.Fft.nCorrelation;

	DispResolution();

	for (i = 0; i < OCT_BAND_NUM; i++) {
		str.Format("1/%d", g_nOctBandTbl[i]);
		m_cFftOctBand.InsertString(i, str);
	}
	m_cFftOctBand.SetCurSel(g_oSetData.Fft.nOctaveBand);

	for (i = 1; i <= 20; i++) {
		str.Format("%d s", i);
		m_cFftTimeRange.SetItemData(m_cFftTimeRange.AddString(str), i);
	}
	SelectListBox(m_cFftTimeRange, g_oSetData.Fft.nTimeRange);

	static constexpr int tChannel[] = {IDS_MONO, IDS_STEREO, IDS_LEFT_ONLY, IDS_RIGHT_ONLY, IDS_DIFFERENCE};
	for (i = 0; i < ARRAY_SIZE(tChannel); i++) {
		str.LoadString(tChannel[i]);
		m_cFftChannel.AddString(str);
	}
	m_cFftChannel.SetCurSel(g_oSetData.Fft.nChannel);

	m_iFftColorScale = g_oSetData.Fft.nColorScale;

	UpdateData(FALSE);

	OnFftAutoFreq();
	OnFftAutoLevel();

	m_cFftMinFreq.SetValidChar(VC_NUM | VC_POINT);
	m_cFftMaxFreq.SetValidChar(VC_NUM | VC_POINT);
	m_cFftMinLevel.SetValidChar(VC_NUM | VC_MINUS | VC_POINT);
	m_cFftMaxLevel.SetValidChar(VC_NUM | VC_MINUS | VC_POINT);
	m_cFftPeakHoldTime.SetValidChar(VC_NUM);
	m_cFftAutoStop.SetValidChar(VC_NUM);

	m_cFftCfZoom.SetRange(1, 10);
	m_cFftCfZoom.SetTicFreq(1);
	m_cFftCfZoom.SetPageSize(1);
	m_cFftCfZoom.SetPos(g_oSetData.Fft.nCrfZoom);

	for (i = 0; i < FFT_MODE_NUM; i++) {
		if (g_oSetData.Fft.bSeparateWindow[i])
			m_wndFft.CreateFftWindow(i, g_oSetData.Fft.rectFftWindow[i]);
	}

	m_cFftAutoStopSpin.SetRange(0, 9999);
	m_cFftAutoStop = g_oSetData.Fft.nAutoStopTime;

	EnableControls();

	m_cFftMaxLevel.SetNotifyWnd(m_hWnd);
	m_cFftMinLevel.SetNotifyWnd(m_hWnd);
	m_cFftMaxFreq.SetNotifyWnd(m_hWnd);
	m_cFftMinFreq.SetNotifyWnd(m_hWnd);

	m_bInitialized = TRUE;

	return TRUE;
}

void CFftDlg::EnableFftWindow()
{
	m_wndFft.AddFftWindow(g_oSetData.Fft.nMode, &m_wndFft, TRUE);
	m_wndFft.SetBitmap(&m_wndFft);
	m_wndFft.ShowWindow(SW_SHOW);
}

void CFftDlg::DisableFftWindow()
{
	m_wndFft.RemoveFftWindow(&m_wndFft);
	m_wndFft.ShowWindow(SW_HIDE);
}

CRect CFftDlg::GetFftWindowRect()
{
	CRect rect;

	m_cFftTab.GetClientRect(rect);

	m_cFftTab.AdjustRect(FALSE, rect);
	rect.DeflateRect(1, 2);

	return rect;
}

void CFftDlg::OnOK()
{
	OnStart();
}

void CFftDlg::OnStart()
{
	if (m_bStart)
		Stop();
	else {
		int nSamplingRate;

		if ((nSamplingRate = m_pWaveIn->GetFixedSamplingRate()) != 0)
			ChangeSamplingRate(nSamplingRate);

		if (g_oSetData.Fft.nMicCalID != -1) {
			GetInputDevice(&m_nOrgInputSel, &m_nOrgInputVol);
			SetInputDevice(m_wndFft.m_oMicCalDataL.nInputSel, m_wndFft.m_oMicCalDataL.nInputVol);
		}

		m_fTimeCount = 0;

		m_bRegist = (BOOL)CHECK_LICENSE2(APP_RAL);
		m_bNoRegist = FALSE;

		if (g_oSetData.Dtr.bDeleteAtStart)
			DeleteRecordData();

		if (!Start())
			m_cFftStart.SetCheck(FALSE);
	}
}

int CFftDlg::OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify)
{
	switch (nCode) {
	case WAVEIN_OPEN:
		m_bStart = TRUE;
		m_cFftStart.SetCheck(m_bStart);
		m_cFftStart.SetWindowText(GetString(IDS_STOP));
		SetStatusBar(GetRunningStatus(IDS_STATUS_RUNNING, m_pWaveIn->GetBitsPerSample(), 0, 0));
		pWaveNotify->nWindow = WINDOW_FFT;
		break;
	case WAVEIN_CLOSE:
		m_bStart = FALSE;
		m_cFftStart.SetCheck(m_bStart);
		m_cFftStart.SetWindowText(GetString(IDS_START));
		SetStatusBar(IDS_STATUS_STOP);
		if (g_oSetData.Fft.nMicCalID != -1 && !m_bModeChange)
			SetInputDevice(m_nOrgInputSel, m_nOrgInputVol);

		// 未登録時のダイアログ
		if (m_bNoRegist)
			CHECK_LICENSE1(APP_RAL);
		break;
	case WAVEIN_DATA:
		m_fTimeCount += m_fBlockTime;
		const int nTimeCount = (int)m_fTimeCount;
		BOOL bRecordData = FALSE;
		if (m_nTimeCount != nTimeCount) {
			m_cFftTime.Format("%d:%02d:%02d", nTimeCount / 3600, nTimeCount / 60 % 60, nTimeCount % 60);
			m_nTimeCount = nTimeCount;

			// データ記録タイミング
			if (m_pDataRecord != NULL && g_oSetData.Dtr.nTimingMode == TIMING_CYCLE && nTimeCount != 0) {
				if (nTimeCount % g_oSetData.Dtr.nTimingInterval == 0)
					bRecordData = TRUE;
			}

			// ピークリセットタイミング
			if (g_oSetData.Fft.bPeakHold && g_oSetData.Fft.nPeakHoldMode == 0) {
				if (nTimeCount % g_oSetData.Fft.nPeakHoldTime == 0) {
					m_wndFft.m_bPeakReset = TRUE;
				}
			}
		}

		// 未登録時の動作制限
		if (nTimeCount >= NOREG_LIMIT_TIME && !m_bRegist) {
			m_bNoRegist = TRUE;
			return FALSE;
		}

		if (bRecordData && m_fTimeCount != 0)
			RecordData();

		const int nSize1 = m_nBufSize / g_oSetData.Fft.nTimeRes;
		const int nSize2 = m_nBufSize - nSize1;
		memmove(m_pWaveBuf, m_pWaveBuf + nSize1, nSize2 * sizeof(double));
		memcpy(m_pWaveBuf + nSize2, pWaveNotify->pSamplesData, nSize1 * sizeof(double));
		m_wndFft.GetWaveData(m_pWaveBuf);

		if (g_oSetData.Fft.nAutoStopTime != 0 && m_nTimeCount >= g_oSetData.Fft.nAutoStopTime) {
			return FALSE;
		}

		if (!m_bEnableCsvOutput) {
			m_bEnableCsvOutput = TRUE;
			EnableCsvOutput();
		}

		m_wndFft.m_bPeakReset = FALSE;

		return TRUE;
	}
	return 0;
}

LRESULT CFftDlg::OnTabDrag(WPARAM wParam, LPARAM lParam)
{
	CRect rect;

	rect.SetRect((int)wParam, (int)lParam, 0, 0);
	SeparatePage(g_oSetData.Fft.nMode, rect);

	return 0;
}

LRESULT CFftDlg::OnTabDblClick(WPARAM wParam, LPARAM /*lParam*/)
{
	const int nMode = (int)wParam;

	if (g_oSetData.Fft.bSeparateWindow[nMode]) {
		FFTWINDOW *pFftWindow = m_wndFft.GetFftWindow(nMode);
		if (pFftWindow != NULL)
			pFftWindow->m_pWnd->DestroyWindow();
	} else
		SeparatePage(g_oSetData.Fft.nMode, g_oSetData.Fft.rectFftWindow[nMode]);

	return 0;
}

void CFftDlg::SeparatePage(int nMode, RECT rectWindow)
{
	g_oSetData.Fft.bSeparateWindow[nMode] = TRUE;

	m_wndFft.CreateFftWindow(nMode, rectWindow);
	m_cFftTab.SetGrayed(nMode, TRUE);
	m_cFftTab.SetDrag(nMode, FALSE);

	SelchangeFftTab();
}

void CFftDlg::RestorePage(int nMode)
{
	g_oSetData.Fft.bSeparateWindow[nMode] = FALSE;

	m_cFftTab.SetGrayed(nMode, FALSE);
	m_cFftTab.SetDrag(nMode, TRUE);

	if (nMode == g_oSetData.Fft.nMode) {
		SelchangeFftTab();
		m_wndFft.Invalidate();
	} else
		EnableControls();
}

LRESULT CFftDlg::OnFreqLevelLeft(WPARAM wParam, LPARAM lParam)
{
	m_cFftPeakFreqLeft = (LPCSTR)wParam;
	m_cFftPeakLevelLeft = (LPCSTR)lParam;

	return 0;
}

LRESULT CFftDlg::OnFreqLevelRight(WPARAM wParam, LPARAM lParam)
{
	m_cFftPeakFreqRight = (LPCSTR)wParam;
	m_cFftPeakLevelRight = (LPCSTR)lParam;

	return 0;
}

void CFftDlg::OnSelchangeSamplingRate()
{
	int index;

	if ((index = m_cSamplingRate.GetCurSel()) != CB_ERR) {
		BOOL bStart = m_bStart;
		if (bStart)
			Stop(TRUE);

		g_oSetData.Fft.nSamplingRate = (int)m_cSamplingRate.GetItemData(index);
		SetFFT();

		ChangeDataRecord();

		if (bStart)
			Start();
	}
}

void CFftDlg::OnSelchangeFftTab(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	SelchangeFftTab();

	*pResult = 0;
}

void CFftDlg::SelchangeFftTab()
{
	const int n = m_cFftTab.GetCurSel();
	if (n != -1) {
		g_oSetData.Fft.nMode = n;

		if (g_oSetData.Fft.bSeparateWindow[n]) {
			DisableFftWindow();
			EnableControls();
		} else {
			if (n != FFT_MODE_OCT && m_cDataRecord == TRUE) {
				m_cDataRecord = FALSE;
				CloseDataRecord();
			}

			FFTWINDOW *pFftWindow = m_wndFft.GetFftWindow(&m_wndFft);
			if (pFftWindow == NULL) {
				EnableFftWindow();
				pFftWindow = m_wndFft.GetFftWindow(&m_wndFft);
			}
			pFftWindow->m_nMode = n;

			ChangeDispMode(pFftWindow);
			EnableControls();

			ClearPeak();

			m_wndFft.m_bPeakReset = TRUE;

			m_wndFft.ShowWindow(SW_SHOW);
		}
	}
}

void CFftDlg::OnSelchangeFftFilter()
{
	if (g_oSetData.Fft.nFilter != m_cFftFilter.GetCurSel()) {
		g_oSetData.Fft.nFilter = m_cFftFilter.GetCurSel();
		SetFFT();
	}
}

void CFftDlg::OnFftAutoFreq()
{
	if (g_oSetData.Fft.bFftAutoFreq != m_cFftAutoFreq) {
		g_oSetData.Fft.bFftAutoFreq = m_cFftAutoFreq;
		ChangeDispMode();
		EnableControls();
	}
}

void CFftDlg::OnFftAutoLevel()
{
	if (g_oSetData.Fft.bFftAutoLevel != m_cFftAutoLevel) {
		g_oSetData.Fft.bFftAutoLevel = m_cFftAutoLevel;
		ChangeDispMode();
		EnableControls();
	}
}

void CFftDlg::OnSelchangeFftSmoothing()
{
	g_oSetData.Fft.nSmoothing = (int)m_cFftSmoothing.GetItemData(m_cFftSmoothing.GetCurSel());
}

void CFftDlg::OnSelchangeFftFftSize()
{
	BOOL bStart = m_bStart;
	if (bStart)
		Stop(TRUE);

	CString str;
	m_cFftFftSize.GetWindowText(str);
	g_oSetData.Fft.nFftSize = atoi(str);

	SetFFT();

	ChangeDataRecord();

	if (bStart)
		Start();
}

void CFftDlg::OnFftLinearScale()
{
	if (g_oSetData.Fft.nFftScale != 1) {
		g_oSetData.Fft.nFftScale = 1;
		ChangeDispMode();
	}
}

void CFftDlg::OnFftLogScale()
{
	if (g_oSetData.Fft.nFftScale != 0) {
		g_oSetData.Fft.nFftScale = 0;
		ChangeDispMode();
	}
}

void CFftDlg::OnChangeFftMaxFreq()
{
	if (m_cFftMaxFreq != g_oSetData.Fft.nFftMaxFreq) {
		if ((int)m_cFftMaxFreq < 0)
			m_cFftMaxFreq = 0;
		else if ((int)m_cFftMaxFreq > LIMIT_FREQ)
			m_cFftMaxFreq = LIMIT_FREQ;

		g_oSetData.Fft.nFftMaxFreq = m_cFftMaxFreq;
		ChangeDispMode();
	}
}

void CFftDlg::OnChangeFftMaxLevel()
{
	if (m_cFftMaxLevel != g_oSetData.Fft.nFftMaxLevel) {
		g_oSetData.Fft.nFftMaxLevel = m_cFftMaxLevel;
		ChangeDispMode();
	}
}

void CFftDlg::OnChangeFftMinFreq()
{
	if (m_cFftMinFreq != g_oSetData.Fft.nFftMinFreq) {
		if ((int)m_cFftMinFreq < 0)
			m_cFftMinFreq = 0;
		else if ((int)m_cFftMinFreq > LIMIT_FREQ)
			m_cFftMinFreq = LIMIT_FREQ;

		g_oSetData.Fft.nFftMinFreq = m_cFftMinFreq;
		ChangeDispMode();
	}
}

void CFftDlg::OnChangeFftMinLevel()
{
	if (m_cFftMinLevel != g_oSetData.Fft.nFftMinLevel) {
		g_oSetData.Fft.nFftMinLevel = m_cFftMinLevel;
		ChangeDispMode();
	}
}

void CFftDlg::OnSelchangeFftChannel()
{
	const int nChannel = m_cFftChannel.GetCurSel();

	if (g_oSetData.Fft.nChannel != nChannel) {
		g_oSetData.Fft.nChannel = nChannel;

		EnableControls();

		ChangeDispMode();

		ChangeDataRecord();
	}
}

void CFftDlg::OnSelchangeFftWinfunc()
{
	if (g_oSetData.Fft.nFftWindowFunc != m_cFftWinFunc.GetCurSel()) {
		g_oSetData.Fft.nFftWindowFunc = m_cFftWinFunc.GetCurSel();
		SetFFT();
	}
}

void CFftDlg::EnableControls()
{
	FFTWINDOW *pFftWindow;
	BOOL bFftAutoFreq = FALSE;
	BOOL bFftAutoLevel = FALSE;
	BOOL bFftLogScale = FALSE;
	BOOL bFftLinearScale = FALSE;
	BOOL bFftSmoothing = FALSE;
	BOOL bFftPeakDisp = FALSE;
	BOOL bFftPeakHold = FALSE;
	BOOL bFftAverage = FALSE;
	BOOL bFftChannel = FALSE;
	BOOL bFftFilter = FALSE;
	BOOL bFftMaxFreq = FALSE;
	BOOL bFftMinFreq = FALSE;
	BOOL bFftMaxLevel = FALSE;
	BOOL bFftMinLevel = FALSE;
	BOOL bFftPeakAuto = FALSE;
	BOOL bFftPeakManual = FALSE;
	BOOL bFftPeakHoldTime = FALSE;
	BOOL bFftPeakReset = FALSE;
	BOOL bFftOctBand = FALSE;
	BOOL bDataRecord = FALSE;
	BOOL bFftTimeBwd = FALSE;
	BOOL bFftTimeFwd = FALSE;
	BOOL bFftTimeDataNum = FALSE;
	BOOL bFftCfAuto = FALSE;
	BOOL bFftCfLateral = FALSE;
	BOOL bFftCfZoom = FALSE;
	BOOL bFftTimeRange = FALSE;
	BOOL bFftColor = FALSE;
	BOOL bFftGray = FALSE;
	BOOL bRlSplit = FALSE;

	for (POSITION pos = m_wndFft.m_oFftWindow.GetHeadPosition(); pos != NULL; ) {
		pFftWindow = (FFTWINDOW *)m_wndFft.m_oFftWindow.GetNext(pos);

		switch (pFftWindow->m_nMode) {
		case FFT_MODE_PWS:
			bFftAutoFreq = TRUE;
			bFftAutoLevel = TRUE;
			bFftLogScale = TRUE;
			bFftLinearScale = TRUE;
			bFftSmoothing = TRUE;
			bFftPeakDisp = TRUE;
			bFftPeakHold = TRUE;
			bFftAverage = TRUE;
			bFftChannel = TRUE;
			bFftFilter = TRUE;
			if (!g_oSetData.Fft.bFftAutoFreq) {
				bFftMaxFreq = TRUE;
				bFftMinFreq = TRUE;
			}
			if (!g_oSetData.Fft.bFftAutoLevel) {
				bFftMaxLevel = TRUE;
				bFftMinLevel = TRUE;
			}
			if (g_oSetData.Fft.bPeakHold) {
				bFftPeakAuto = TRUE;
				bFftPeakManual = TRUE;
				if (g_oSetData.Fft.nPeakHoldMode == 0)
					bFftPeakHoldTime = TRUE;
				if (g_oSetData.Fft.nPeakHoldMode == 1)
					bFftPeakReset = TRUE;
			}
			break;
		case FFT_MODE_OCT:
			bFftAutoLevel = TRUE;
			bFftSmoothing = TRUE;
			bFftPeakDisp = TRUE;
			bFftPeakHold = TRUE;
			bFftAverage = TRUE;
			bFftOctBand = TRUE;
			bDataRecord = TRUE;
			bFftChannel = TRUE;
			bFftFilter = TRUE;
			if (!g_oSetData.Fft.bFftAutoLevel) {
				bFftMaxLevel = TRUE;
				bFftMinLevel = TRUE;
			}
			if (g_oSetData.Fft.bPeakHold) {
				bFftPeakAuto = TRUE;
				bFftPeakManual = TRUE;
				if (g_oSetData.Fft.nPeakHoldMode == 0)
					bFftPeakHoldTime = TRUE;
				if (g_oSetData.Fft.nPeakHoldMode == 1)
					bFftPeakReset = TRUE;
			}
			break;
		case FFT_MODE_3DD:
			bFftAutoFreq = TRUE;
			bFftAutoLevel = TRUE;
			bFftLogScale = TRUE;
			bFftLinearScale = TRUE;
			bFftSmoothing = TRUE;
			bFftTimeBwd = TRUE;
			bFftTimeFwd = TRUE;
			bFftTimeDataNum = TRUE;
			bFftAverage = TRUE;
			bFftChannel = TRUE;
			bFftFilter = TRUE;
			if (!g_oSetData.Fft.bFftAutoFreq) {
				bFftMaxFreq = TRUE;
				bFftMinFreq = TRUE;
			}
			if (!g_oSetData.Fft.bFftAutoLevel) {
				bFftMaxLevel = TRUE;
				bFftMinLevel = TRUE;
			}
			break;
		case FFT_MODE_CRF:
			bFftCfAuto = TRUE;
			bFftCfLateral = TRUE;
			bFftCfZoom = TRUE;
			if (g_oSetData.Fft.nCorrelation == 0)
				bFftChannel = TRUE;
			break;
		case FFT_MODE_PHS:
			bFftAutoFreq = TRUE;
			bFftLogScale = TRUE;
			bFftLinearScale = TRUE;
			bFftSmoothing = TRUE;
			if (!g_oSetData.Fft.bFftAutoFreq) {
				bFftMaxFreq = TRUE;
				bFftMinFreq = TRUE;
			}
			break;
		case FFT_MODE_SPG:
			bFftAutoFreq = TRUE;
			bFftAutoLevel = TRUE;
			bFftLogScale = TRUE;
			bFftLinearScale = TRUE;
			bFftSmoothing = TRUE;
			bFftTimeBwd = TRUE;
			bFftTimeFwd = TRUE;
			bFftAverage = TRUE;
			bFftTimeRange = TRUE;
			bFftChannel = TRUE;
			bFftFilter = TRUE;
			if (!g_oSetData.Fft.bFftAutoFreq) {
				bFftMaxFreq = TRUE;
				bFftMinFreq = TRUE;
			}
			if (!g_oSetData.Fft.bFftAutoLevel) {
				bFftMaxLevel = TRUE;
				bFftMinLevel = TRUE;
			}
			bFftColor = TRUE;
			bFftGray = TRUE;
			break;
		case FFT_MODE_CRS:
			bFftAutoFreq = TRUE;
			bFftAutoLevel = TRUE;
			bFftLogScale = TRUE;
			bFftLinearScale = TRUE;
			bFftSmoothing = TRUE;
			bFftPeakDisp = TRUE;
			bFftPeakHold = TRUE;
			bFftAverage = TRUE;
//			bFftChannel = TRUE;
			bFftFilter = TRUE;
			if (!g_oSetData.Fft.bFftAutoFreq) {
				bFftMaxFreq = TRUE;
				bFftMinFreq = TRUE;
			}
			if (!g_oSetData.Fft.bFftAutoLevel) {
				bFftMaxLevel = TRUE;
				bFftMinLevel = TRUE;
			}
			if (g_oSetData.Fft.bPeakHold) {
				bFftPeakAuto = TRUE;
				bFftPeakManual = TRUE;
				if (g_oSetData.Fft.nPeakHoldMode == 0)
					bFftPeakHoldTime = TRUE;
				if (g_oSetData.Fft.nPeakHoldMode == 1)
					bFftPeakReset = TRUE;
			}
			break;
		case FFT_MODE_COH:
			bFftAutoFreq = TRUE;
			bFftLogScale = TRUE;
			bFftLinearScale = TRUE;
			bFftFilter = TRUE;
			bFftSmoothing = TRUE;
			bFftAverage = TRUE;
			if (!g_oSetData.Fft.bFftAutoFreq) {
				bFftMaxFreq = TRUE;
				bFftMinFreq = TRUE;
			}
			break;
		case FFT_MODE_CEP:
			bFftAutoLevel = TRUE;
			bFftSmoothing = TRUE;
			bFftPeakDisp = TRUE;
			bFftPeakHold = TRUE;
			bFftAverage = TRUE;
			bFftChannel = TRUE;
			bFftFilter = TRUE;
			if (!g_oSetData.Fft.bFftAutoLevel) {
				bFftMaxLevel = TRUE;
				bFftMinLevel = TRUE;
			}
			if (g_oSetData.Fft.bPeakHold) {
				bFftPeakAuto = TRUE;
				bFftPeakManual = TRUE;
				if (g_oSetData.Fft.nPeakHoldMode == 0)
					bFftPeakHoldTime = TRUE;
				if (g_oSetData.Fft.nPeakHoldMode == 1)
					bFftPeakReset = TRUE;
			}
			bFftCfZoom = TRUE;
			break;
		}

		if (g_oSetData.Fft.nChannel == CH_STEREO && (pFftWindow->m_nMode != FFT_MODE_CRF || g_oSetData.Fft.nCorrelation != 1) && pFftWindow->m_nMode != FFT_MODE_PHS && pFftWindow->m_nMode != FFT_MODE_CRS && pFftWindow->m_nMode != FFT_MODE_COH)
			bRlSplit = TRUE;
	}

	LockWindowUpdate();

	GetDlgItem(IDC_FFT_AUTO_FREQ)->EnableWindow(bFftAutoFreq);
	GetDlgItem(IDC_FFT_AUTO_LEVEL)->EnableWindow(bFftAutoLevel);
	GetDlgItem(IDC_FFT_LOG_SCALE)->EnableWindow(bFftLogScale);
	GetDlgItem(IDC_FFT_LINEAR_SCALE)->EnableWindow(bFftLinearScale);
	GetDlgItem(IDC_FFT_SMOOTHING)->EnableWindow(bFftSmoothing);
	GetDlgItem(IDC_FFT_PEAK_DISP)->EnableWindow(bFftPeakDisp);
	GetDlgItem(IDC_FFT_PEAK_HOLD)->EnableWindow(bFftPeakHold);
	GetDlgItem(IDC_FFT_AVERAGE)->EnableWindow(bFftAverage);
	GetDlgItem(IDC_FFT_CHANNEL)->EnableWindow(bFftChannel);
	GetDlgItem(IDC_FFT_FILTER)->EnableWindow(bFftFilter);
	GetDlgItem(IDC_FFT_MAX_FREQ)->EnableWindow(bFftMaxFreq);
	GetDlgItem(IDC_FFT_MIN_FREQ)->EnableWindow(bFftMinFreq);
	GetDlgItem(IDC_FFT_MAX_LEVEL)->EnableWindow(bFftMaxLevel);
	GetDlgItem(IDC_FFT_MIN_LEVEL)->EnableWindow(bFftMinLevel);
	GetDlgItem(IDC_FFT_PEAK_AUTO)->EnableWindow(bFftPeakAuto);
	GetDlgItem(IDC_FFT_PEAK_MANUAL)->EnableWindow(bFftPeakManual);
	GetDlgItem(IDC_FFT_PEAK_TIME)->EnableWindow(bFftPeakHoldTime);
	GetDlgItem(IDC_FFT_PEAK_RESET)->EnableWindow(bFftPeakReset);
	GetDlgItem(IDC_FFT_OCT_BAND)->EnableWindow(bFftOctBand);
	GetDlgItem(IDC_DATA_RECORD)->EnableWindow(bDataRecord);
	GetDlgItem(IDC_FFT_TIME_BWD)->EnableWindow(bFftTimeBwd);
	GetDlgItem(IDC_FFT_TIME_FWD)->EnableWindow(bFftTimeFwd);
	GetDlgItem(IDC_FFT_TIME_DATA_NUM)->EnableWindow(bFftTimeDataNum);
	GetDlgItem(IDC_FFT_CF_AUTO)->EnableWindow(bFftCfAuto);
	GetDlgItem(IDC_FFT_CF_LATERAL)->EnableWindow(bFftCfLateral);
	GetDlgItem(IDC_FFT_CF_ZOOM)->EnableWindow(bFftCfZoom);
	GetDlgItem(IDC_FFT_ZOOM_SCALE)->EnableWindow(bFftCfZoom);
	GetDlgItem(IDC_FFT_TIME_RANGE)->EnableWindow(bFftTimeRange);
	GetDlgItem(IDC_FFT_COLOR)->EnableWindow(bFftColor);
	GetDlgItem(IDC_FFT_GRAY)->EnableWindow(bFftGray);
	GetDlgItem(IDC_RL_SPLIT)->EnableWindow(bRlSplit);

	GetDlgItem(IDC_SCREEN_SHOT)->EnableWindow(!g_oSetData.Fft.bSeparateWindow[g_oSetData.Fft.nMode]);

	CString sNameX, sNameY, sUnitX, sUnitY, sAxisX, sAxisY;
	GetAxisName(g_oSetData.Fft.nMode, sNameX, sNameY);
	GetAxisUnit(g_oSetData.Fft.nMode, sUnitX, sUnitY);
	if (!sUnitX.IsEmpty())
		sAxisX.Format("%s [%s]", (LPCSTR)sNameX, (LPCSTR)sUnitX);
	else
		sAxisX = sNameX;
	m_cFftPeakFreq.SetWindowText(sAxisX);
	if (!sUnitY.IsEmpty())
		sAxisY.Format("%s [%s]", (LPCSTR)sNameY, (LPCSTR)sUnitY);
	else
		sAxisY = sNameY;
	m_cFftPeakLevel.SetWindowText(sAxisY);

	EnableCsvOutput();

	UnlockWindowUpdate();
}

void CFftDlg::EnableCsvOutput()
{
	GetDlgItem(IDC_CSV_OUTPUT)->EnableWindow(m_bEnableCsvOutput && !g_oSetData.Fft.bSeparateWindow[g_oSetData.Fft.nMode] && g_oSetData.Fft.nMode != FFT_MODE_3DD && g_oSetData.Fft.nMode != FFT_MODE_SPG);
}

void CFftDlg::DispMessage(int msgNo)
{
	if (m_nCurrentMsgNo == msgNo)
		return;

	switch(msgNo) {
	case MSG_NULL:
		m_cFftMessage.Blank();
		break;
	case MSG_CPU_WARNING:
		{
			CString str;
			str.LoadString(IDS_CPUOVERLOAD);
			m_cFftMessage = str;
		}
		break;
	}

	m_nCurrentMsgNo = msgNo;
}

void CFftDlg::OnFftPeakDisp()
{
	if (g_oSetData.Fft.bPeakDisp != m_cFftPeakDisp) {
		g_oSetData.Fft.bPeakDisp = m_cFftPeakDisp;

		if (!g_oSetData.Fft.bPeakDisp)
			m_wndFft.ResetDispFreqAll();

		m_wndFft.Redraw();
	}
}

void CFftDlg::OnFftPeakHold()
{
	if (g_oSetData.Fft.bPeakHold != m_cFftPeakHold) {
		g_oSetData.Fft.bPeakHold = m_cFftPeakHold;
		m_wndFft.m_bPeakReset = TRUE;
		EnableControls();
		if (!m_bStart)
			m_wndFft.Redraw();

		ChangeDataRecord();
	}
}

void CFftDlg::SetFFT()
{
	m_wndFft.InitFFT();

	DispResolution();

	m_bEnableCsvOutput = FALSE;
	EnableCsvOutput();

	ChangeDispMode();
}

void CFftDlg::ChangeDispMode(FFTWINDOW *pFftWindow)
{
	ClearPeak();

	m_wndFft.ChangeDispMode(pFftWindow);

	m_bEnableCsvOutput = FALSE;
	EnableCsvOutput();
}

void CFftDlg::DispResolution()
{
	m_cFftResFreq.Format("%.2f", m_wndFft.m_fFreqStep);
	m_cFftResTime.Format("%.1f", m_wndFft.m_fTimeStep2 * 1000);
}

void CFftDlg::OnFftTimeFwd()
{
	if (g_oSetData.Fft.nTimeDir != 0) {
		g_oSetData.Fft.nTimeDir = 0;

		if (g_oSetData.Fft.nMode == FFT_MODE_SPG)
			ChangeDispMode();
		else
			m_wndFft.Redraw();
	}
}

void CFftDlg::OnFftTimeBwd()
{
	if (g_oSetData.Fft.nTimeDir != 1) {
		g_oSetData.Fft.nTimeDir = 1;

		if (g_oSetData.Fft.nMode == FFT_MODE_SPG)
			ChangeDispMode();
		else
			m_wndFft.Redraw();
	}
}

void CFftDlg::OnSelchangeFftTimeDataNum()
{
	CString str;

	m_cFftTimeDataNum.GetWindowText(str);
	g_oSetData.Fft.nTimeDataNum = atoi(str);
	ChangeDispMode();
}

BOOL CFftDlg::Start()
{
	const int nBufSize = g_oSetData.Fft.nFftSize * 2;

	m_fBlockTime = (double)g_oSetData.Fft.nFftSize / g_oSetData.Fft.nSamplingRate / g_oSetData.Fft.nTimeRes;

	m_wndFft.InitBuf();

	if (!m_pWaveIn->Open(g_oSetData.Pc.nInputDevice, this, 2, g_oSetData.Fft.nSamplingRate, g_oSetData.Fft.nFftSize / g_oSetData.Fft.nTimeRes, 4))
		return FALSE;

	if (m_nBufSize != nBufSize) {
		m_nBufSize = nBufSize;

		delete [] m_pWaveBuf;

		m_pWaveBuf = new double[nBufSize];
		memset(m_pWaveBuf, 0, nBufSize * sizeof(double));
	}

	m_pWaveIn->Start();

	return TRUE;
}

void CFftDlg::Stop(BOOL bModeChange)
{
	m_bModeChange = bModeChange;

	m_pWaveIn->Close();

	m_bModeChange = FALSE;
}

void CFftDlg::OnDestroy()
{
	CloseDataRecord();

	if (m_pFftLevelDlg != NULL)
		m_pFftLevelDlg->DestroyWindow();

	if (m_pFftFreqDlg != NULL)
		m_pFftFreqDlg->DestroyWindow();

	CBaseDlg::OnDestroy();

	if (m_bStart)
		Stop();
}

void CFftDlg::OnFftCfAuto()
{
	if (g_oSetData.Fft.nCorrelation != 0) {
		g_oSetData.Fft.nCorrelation = 0;
		EnableControls();
		ChangeDispMode();
	}
}

void CFftDlg::OnFftCfLateral()
{
	if (g_oSetData.Fft.nCorrelation != 1) {
		g_oSetData.Fft.nCorrelation = 1;
		EnableControls();
		ChangeDispMode();
	}
}

void CFftDlg::OnFftCalibration()
{
	int nInputDevice, nInputVolume;
	CMicCalDlg dlg(this);

	if (m_bStart)
		Stop();

	GetInputDevice(&nInputDevice, &nInputVolume);

	dlg.m_MicCalID = g_oSetData.Fft.nMicCalID;
	if (dlg.DoModal() == IDOK) {
		g_oSetData.Fft.nMicCalID = dlg.m_MicCalID;
		SetMicCalData();
		SetFFT();
	}

	SetInputDevice(nInputDevice, nInputVolume);
}

void CFftDlg::SetMicCalData()
{
	if (g_oSetData.Fft.nMicCalID != -1) {
		CDbMicCal dbMicCal;

		if (dbMicCal.Open()) {
			if (dbMicCal.ReadRecID(g_oSetData.Fft.nMicCalID, &m_wndFft.m_oMicCalDataL)) {
				if (dbMicCal.SearchRecID(-g_oSetData.Fft.nMicCalID - 1))
					dbMicCal.ReadRecID(-g_oSetData.Fft.nMicCalID - 1, &m_wndFft.m_oMicCalDataR);
				else
					m_wndFft.m_oMicCalDataR = m_wndFft.m_oMicCalDataL;

				m_cFftMessage = m_wndFft.m_oMicCalDataL.sMicInfo;

				return;
			}
		}
	}

	m_wndFft.m_oMicCalDataL.fInputSens = 0;
	m_wndFft.m_oMicCalDataL.nFreqData = 0;
	m_wndFft.m_oMicCalDataR.fInputSens = 0;
	m_wndFft.m_oMicCalDataR.nFreqData = 0;

	m_cFftMessage.Blank();
}

void CFftDlg::OnMove(int x, int y)
{
	CBaseDlg::OnMove(x, y);

	if (m_bInitialized && !IsIconic()) {
		CRect rect;
		GetWindowRect(rect);
		g_oSetData.Fft.nPosLeft = rect.left;
		g_oSetData.Fft.nPosTop = rect.top;
	}
}

void CFftDlg::ShowChildWindow(BOOL bShow)
{
	LockWindowUpdate();

	CWnd *pWnd = GetTopWindow();
	while (pWnd != NULL) {
		if (pWnd->m_hWnd != m_cFftTab.m_hWnd && pWnd->m_hWnd != m_wndStatusBar.m_hWnd)
			pWnd->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
		pWnd = pWnd->GetNextWindow();
	}

	if (bShow)
		EnableControls();

	UnlockWindowUpdate();
}

void CFftDlg::GetInputDevice(int *pInputDevice, int *pInputVolume)
{
	CRtaApp *pWinApp = (CRtaApp *)::AfxGetApp();
	pWinApp->m_pWndRta->GetInputDevice(pInputDevice, pInputVolume);
}

void CFftDlg::SetInputDevice(int nInputDevice, int nInputVolume)
{
	CRtaApp *pWinApp = (CRtaApp *)::AfxGetApp();
	pWinApp->m_pWndRta->SetInputDevice(nInputDevice, nInputVolume);
}

void CFftDlg::OnRlSplit()
{
	if (g_oSetData.Fft.bRlSplit != m_cRlSplit) {
		g_oSetData.Fft.bRlSplit = m_cRlSplit;
		ChangeDispMode();
	}
}

void CFftDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (pScrollBar->GetDlgCtrlID()) {
	case IDC_FFT_CF_ZOOM:
		g_oSetData.Fft.nCrfZoom = ((CSliderCtrl *)pScrollBar)->GetPos();
		ChangeDispMode();
		break;
	}

	CBaseDlg::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CFftDlg::OnFftPeakAuto()
{
	if (g_oSetData.Fft.nPeakHoldMode != 0) {
		g_oSetData.Fft.nPeakHoldMode = 0;
		EnableControls();
	}
}

void CFftDlg::OnFftPeakManual()
{
	if (g_oSetData.Fft.nPeakHoldMode != 1) {
		g_oSetData.Fft.nPeakHoldMode = 1;
		EnableControls();
	}
}

void CFftDlg::OnChangeFftPeakTime()
{
	if (g_oSetData.Fft.nPeakHoldTime != (int)m_cFftPeakHoldTime) {
		g_oSetData.Fft.nPeakHoldTime = m_cFftPeakHoldTime;
	}
}

void CFftDlg::OnFftPeakReset()
{
	m_wndFft.m_bPeakReset = TRUE;
}

void CFftDlg::OnSelchangeFftTimeRes()
{
	BOOL bStart = m_bStart;
	if (bStart)
		Stop(TRUE);

	g_oSetData.Fft.nTimeRes = (int)m_cFftTimeRes.GetItemData(m_cFftTimeRes.GetCurSel());

	SetFFT();

	if (bStart)
		Start();
}

void CFftDlg::OnSelchangeFftAverage()
{
	BOOL bStart = m_bStart;
	if (bStart)
		Stop(TRUE);

	g_oSetData.Fft.nAverage = (int)m_cFftAverage.GetItemData(m_cFftAverage.GetCurSel());

	SetFFT();

	if (bStart)
		Start();
}

void CFftDlg::OnSelchangeFftOctBand()
{
	const int nOctaveBand = m_cFftOctBand.GetCurSel();

	if (g_oSetData.Fft.nOctaveBand != nOctaveBand) {
		g_oSetData.Fft.nOctaveBand = nOctaveBand;
		ChangeDispMode();
		ChangeDataRecord();
	}
}

void CFftDlg::ChangeWaveDevice(int nInOut)
{
	if (nInOut & WAVE_IN) {
		if (m_bStart)
			Stop();

		SetSamplingRateList(m_cSamplingRate, WAVE_IN, g_oSetData.Pc.nInputDevice, 0, g_oSetData.Fft.nSamplingRate);
		OnSelchangeSamplingRate();
	}
}

void CFftDlg::ChangeSamplingRate(int nSamplingRate)
{
	if (g_oSetData.Fft.nSamplingRate != nSamplingRate) {
		const int n = m_cSamplingRate.GetCount();
		for (int i = 0; i < n; i++) {
			if ((int)m_cSamplingRate.GetItemData(i) == nSamplingRate) {
				m_cSamplingRate.SetCurSel(i);
				OnSelchangeSamplingRate();
				break;
			}
		}
	}
}

void CFftDlg::OnBnClickedDataRecord()
{
	if (m_cDataRecord == TRUE)
		OpenDataRecord();
	else
		CloseDataRecord();
}

LRESULT CFftDlg::OnDataRecord(WPARAM wParam, LPARAM /*lParam*/)
{
	switch (wParam) {
	case DR_CLOSE:
		m_cDataRecord.SetCheck(0);
		CloseDataRecord();
		break;
	case DR_RECORD:
		RecordData();
		break;
	}

	return 0;
}

void CFftDlg::OpenDataRecord()
{
	CloseDataRecord();
	m_pDataRecord = new CDataRecordDlg(this);
	ChangeDataRecord();
}

void CFftDlg::CloseDataRecord()
{
	if (m_pDataRecord != NULL) {
		m_pDataRecord->DestroyWindow();
		m_pDataRecord = NULL;
	}
}

void CFftDlg::ChangeDataRecord()
{
	if (m_pDataRecord != NULL)
		m_pDataRecord->InitDataList(m_wndFft.m_nOctBand, m_wndFft.m_aFreqScale);
}

void CFftDlg::RecordData()
{
	if (m_pDataRecord != NULL)
		m_pDataRecord->RecordData((int)m_fTimeCount, m_wndFft.m_aFftData);
}

void CFftDlg::DeleteRecordData()
{
	if (m_pDataRecord != NULL)
		m_pDataRecord->DeleteRecordData();
}

void CFftDlg::OnBnClickedScreenShot()
{
	CString sMode;
	sMode.LoadString(g_tFftModeId[g_oSetData.Fft.nMode]);

	CString sTitle;
	const int nTimeCount = (int)m_fTimeCount;
	sTitle.Format("%s - %d:%02d:%02d", (LPCSTR)sMode, nTimeCount / 3600, nTimeCount / 60 % 60, nTimeCount % 60);

	HBITMAP hBmp = m_wndFft.GetBitmap();
	m_oScreenShot.ShowScreenShot(sTitle, hBmp, FALSE);
}

void CFftDlg::OnSelchangeFftTimeRange()
{
	g_oSetData.Fft.nTimeRange = (int)m_cFftTimeRange.GetItemData(m_cFftTimeRange.GetCurSel());

	ChangeDispMode();
}

void CFftDlg::OnFftColor()
{
	if (g_oSetData.Fft.nColorScale != 0) {
		g_oSetData.Fft.nColorScale = 0;

		ChangeDispMode();
	}
}

void CFftDlg::OnFftGray()
{
	if (g_oSetData.Fft.nColorScale != 1) {
		g_oSetData.Fft.nColorScale = 1;

		ChangeDispMode();
	}
}

void CFftDlg::ClearPeak()
{
	m_cFftPeakLevelLeft.Blank();
	m_cFftPeakLevelRight.Blank();
	m_cFftPeakFreqLeft.Blank();
	m_cFftPeakFreqRight.Blank();

	for (POSITION pos = m_wndFft.m_oFftWindow.GetHeadPosition(); pos != NULL; ) {
		FFTWINDOW *pFftWindow = (FFTWINDOW *)m_wndFft.m_oFftWindow.GetNext(pos);

		pFftWindow->m_pWnd->SendMessage(WM_FREQ_LEVEL_LEFT, (WPARAM)"", (LPARAM)"");
		pFftWindow->m_pWnd->SendMessage(WM_FREQ_LEVEL_RIGHT, (WPARAM)"", (LPARAM)"");
	}
}

void CFftDlg::OnEnChangeFftAutoStop()
{
	if (m_cFftAutoStop.m_hWnd)
		g_oSetData.Fft.nAutoStopTime = m_cFftAutoStop;
}

void CFftDlg::OnBnClickedCsvOutput()
{
	CFileDialog fileDlg(FALSE, "csv", NULL,
			OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
			"CSV File (*.csv)|*.csv|Text File (*.txt)|*.txt|All Files (*.*)|*.*||", this);

	if (fileDlg.DoModal() == IDOK)
		m_wndFft.CsvOutput(fileDlg.GetPathName());
}

void CFftDlg::OnBnClickedCsvInput()
{
	CFileDialog fileDlg(TRUE, "csv", NULL,
			OFN_NOCHANGEDIR,
			"CSV File (*.csv)|*.csv|Text File (*.txt)|*.txt|All Files (*.*)|*.*||", this);

	if (fileDlg.DoModal() == IDOK)
		m_wndFft.CsvInput(fileDlg.GetPathName());
}

void CFftDlg::FullScreen()
{
	if (!m_bMax) {
		ShowChildWindow(FALSE);

		SetMaximunControl(m_cFftTab.m_hWnd);

		m_wndFft.MoveWindow(GetFftWindowRect());
		m_wndFft.SetBitmap(&m_wndFft);

		m_bMax = TRUE;
	} else {
		SetMaximunControl(0);

		m_wndFft.MoveWindow(GetFftWindowRect());
		m_wndFft.SetBitmap(&m_wndFft);

		m_cFftTab.UpdateWindow();
		ShowChildWindow(TRUE);

		m_bMax = FALSE;
	}
}

BOOL CFftDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDC_FFT_TAB, IDH_FFT_TAB,
		ID_FFT_WINDOW, IDH_FFT_TAB,
		IDC_BTN_START, IDH_FFT_START,
		IDC_FFT_TIME, IDH_FFT_TIME,
		IDC_FFT_MESSAGE, IDH_FFT_MESSAGE,
		IDC_FFT_CALIBRATION, IDH_FFT_CALIBRATION,
		IDC_FFT_PEAK_LEVEL_LEFT, IDH_FFT_PEAK_LEVEL_LEFT,
		IDC_FFT_PEAK_LEVEL_RIGHT, IDH_FFT_PEAK_LEVEL_RIGHT,
		IDC_FFT_PEAK_FREQ_LEFT, IDH_FFT_PEAK_FREQ_LEFT,
		IDC_FFT_PEAK_FREQ_RIGHT, IDH_FFT_PEAK_FREQ_RIGHT,
		IDC_FFT_RES_FREQ, IDH_FFT_RES_FREQ,
		IDC_FFT_RES_TIME, IDH_FFT_RES_TIME,
		IDC_SCREEN_SHOT, IDH_FFT_SCREEN_SHOT,
		IDC_RL_SPLIT, IDH_FFT_RL_SPLIT,
		IDC_DATA_RECORD, IDH_FFT_DATA_RECORD,
		IDC_CSV_OUTPUT, IDH_FFT_CSV_OUTPUT,
		IDC_CSV_INPUT, IDH_FFT_CSV_INPUT,
		IDCANCEL, IDH_FFT_CLOSE,
		IDC_FFT_AUTO_LEVEL, IDH_FFT_AUTO_LEVEL,
		IDC_FFT_MAX_LEVEL, IDH_FFT_MAX_LEVEL,
		IDC_FFT_MIN_LEVEL, IDH_FFT_MIN_LEVEL,
		IDC_FFT_AUTO_FREQ, IDH_FFT_AUTO_FREQ,
		IDC_FFT_MAX_FREQ, IDH_FFT_MAX_FREQ,
		IDC_FFT_MIN_FREQ, IDH_FFT_MIN_FREQ,
		IDC_SAMPLING_RATE, IDH_FFT_SAMPLING_RATE,
		IDC_FFT_FFT_SIZE, IDH_FFT_FFT_SIZE,
		IDC_FFT_WINFUNC, IDH_FFT_WINFUNC,
		IDC_FFT_CHANNEL, IDH_FFT_CHANNEL,
		IDC_FFT_FILTER, IDH_FFT_FILTER,
		IDC_FFT_SMOOTHING, IDH_FFT_SMOOTHING,
		IDC_FFT_AVERAGE, IDH_FFT_AVERAGE,
		IDC_FFT_LOG_SCALE, IDH_FFT_LOG_SCALE,
		IDC_FFT_LINEAR_SCALE, IDH_FFT_LINEAR_SCALE,
		IDC_FFT_TIME_RES, IDH_FFT_TIME_RES,
		IDC_FFT_PEAK_DISP, IDH_FFT_PEAK_DISP,
		IDC_FFT_PEAK_HOLD, IDH_FFT_PEAK_HOLD,
		IDC_FFT_PEAK_AUTO, IDH_FFT_PEAK_AUTO,
		IDC_FFT_PEAK_TIME, IDH_FFT_PEAK_TIME,
		IDC_FFT_PEAK_MANUAL, IDH_FFT_PEAK_MANUAL,
		IDC_FFT_PEAK_RESET, IDH_FFT_PEAK_RESET,
		IDC_FFT_TIME_RANGE, IDH_FFT_TIME_RANGE,
		IDC_FFT_TIME_DATA_NUM, IDH_FFT_TIME_DATA_NUM,
		IDC_FFT_TIME_FWD, IDH_FFT_TIME_FWD,
		IDC_FFT_TIME_BWD, IDH_FFT_TIME_BWD,
		IDC_FFT_CF_ZOOM, IDH_FFT_CF_ZOOM,
		IDC_FFT_OCT_BAND, IDH_FFT_OCT_BAND,
		IDC_FFT_COLOR, IDH_FFT_COLOR,
		IDC_FFT_GRAY, IDH_FFT_GRAY,
		IDC_FFT_CF_AUTO, IDH_FFT_CF_AUTO,
		IDC_FFT_CF_LATERAL, IDH_FFT_CF_LATERAL,
		IDC_FFT_AUTO_STOP, IDH_FFT_AUTO_STOP,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

LRESULT CFftDlg::OnThemeChanged()
{
	if (g_dllUxTheme.IsThemeActive())
		m_cFftTab.ModifyStyle(TCS_OWNERDRAWFIXED, 0);
	else
		m_cFftTab.ModifyStyle(0, TCS_OWNERDRAWFIXED);

	return 1;
}

void CFftDlg::OnSize(UINT nType, int cx, int cy)
{
	CBaseDlg::OnSize(nType, cx, cy);

	if (m_bInitialized) {
		m_wndFft.MoveWindow(GetFftWindowRect());
		m_wndFft.SetBitmap(&m_wndFft);
		m_wndFft.Invalidate();
	}
}

LRESULT CFftDlg::OnMyCtrlRButtonDown(WPARAM wParam, LPARAM lParam)
{
	switch (wParam) {
	case IDC_FFT_MAX_LEVEL:
	case IDC_FFT_MIN_LEVEL:
		if (m_pFftLevelDlg == NULL) {
			m_pFftLevelDlg = new CFftLevelDlg;

			m_pFftLevelDlg->m_pFftDlg = this;
			m_pFftLevelDlg->m_nMaxRange = ((int)(-m_wndFft.m_oMicCalDataL.fInputSens) + 9) / 10 * 10;
			m_pFftLevelDlg->m_nMinRange = m_pFftLevelDlg->m_nMaxRange - 140;
			m_pFftLevelDlg->m_nMaxLevel = g_oSetData.Fft.nFftMaxLevel;
			m_pFftLevelDlg->m_nMinLevel = g_oSetData.Fft.nFftMinLevel;
			m_pFftLevelDlg->Create(m_pFftLevelDlg->IDD, this);
			m_pFftLevelDlg->SetWindowPos(NULL, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
		} else
			m_pFftLevelDlg->SetFocus();
		break;
	case IDC_FFT_MAX_FREQ:
	case IDC_FFT_MIN_FREQ:
		if (m_pFftFreqDlg == NULL) {
			m_pFftFreqDlg = new CFftFreqDlg;

			m_pFftFreqDlg->m_pFftDlg = this;
			m_pFftFreqDlg->m_nMaxRange = g_oSetData.Fft.nSamplingRate / 2;
			m_pFftFreqDlg->m_nMinRange = 20;
			m_pFftFreqDlg->m_nMaxFreq = g_oSetData.Fft.nFftMaxFreq;
			m_pFftFreqDlg->m_nMinFreq = g_oSetData.Fft.nFftMinFreq;
			m_pFftFreqDlg->Create(m_pFftFreqDlg->IDD, this);
			m_pFftFreqDlg->SetWindowPos(NULL, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
		} else
			m_pFftFreqDlg->SetFocus();
		break;
	}

	return 0;
}

void CFftDlg::CloseLevelDlg()
{
	m_pFftLevelDlg = NULL;
}

void CFftDlg::SetFftMaxLevel(int nMaxLevel)
{
	if (nMaxLevel != g_oSetData.Fft.nFftMaxLevel) {
		g_oSetData.Fft.nFftMaxLevel = nMaxLevel;
		m_cFftMaxLevel = nMaxLevel;
		ChangeDispMode();
		RedrawSpectrogram();
	}
}

void CFftDlg::SetFftMinLevel(int nMinLevel)
{
	if (nMinLevel != g_oSetData.Fft.nFftMinLevel) {
		g_oSetData.Fft.nFftMinLevel = nMinLevel;
		m_cFftMinLevel = nMinLevel;
		ChangeDispMode();
		RedrawSpectrogram();
	}
}

void CFftDlg::CloseFreqDlg()
{
	m_pFftFreqDlg = NULL;
}

void CFftDlg::SetFftMaxFreq(int nMaxFreq)
{
	if (nMaxFreq != g_oSetData.Fft.nFftMaxFreq) {
		g_oSetData.Fft.nFftMaxFreq = nMaxFreq;
		m_cFftMaxFreq = nMaxFreq;
		ChangeDispMode();
		RedrawSpectrogram();
	}
}

void CFftDlg::SetFftMinFreq(int nMinFreq)
{
	if (nMinFreq != g_oSetData.Fft.nFftMinFreq) {
		g_oSetData.Fft.nFftMinFreq = nMinFreq;
		m_cFftMinFreq = nMinFreq;
		ChangeDispMode();
		RedrawSpectrogram();
	}
}

void CFftDlg::RedrawSpectrogram()
{
	for (POSITION pos = m_wndFft.m_oFftWindow.GetHeadPosition(); pos != NULL; ) {
		FFTWINDOW *pFftWindow = (FFTWINDOW *)m_wndFft.m_oFftWindow.GetNext(pos);
		if (pFftWindow->m_nMode == FFT_MODE_SPG) {
			m_wndFft.RedrawSpg(pFftWindow);
			break;
		}
	}
}

void CFftDlg::SetCsvConditions(int nMode, int nChannel, int nFftSizeIndex, int nSamplingRateIndex, int nOctBandIndex, int nCorrelation)
{
	m_cFftTab.SetPage(nMode);
	SelchangeFftTab();

	if (nChannel >= 0) {
		m_cFftChannel.SetCurSel(nChannel);
		OnSelchangeFftChannel();
	}

	if (nFftSizeIndex >= 0) {
		m_cFftFftSize.SetCurSel(nFftSizeIndex);
		OnSelchangeFftFftSize();
	}

	if (nSamplingRateIndex >= 0) {
		m_cSamplingRate.SetCurSel(nSamplingRateIndex);
		OnSelchangeSamplingRate();
	}

	if (nOctBandIndex >= 0) {
		m_cFftOctBand.SetCurSel(nOctBandIndex);
		OnSelchangeFftOctBand();
	}

	if (nCorrelation >= 0) {
		m_iFftCorrelation = nCorrelation;
		UpdateData(FALSE);

		switch (nCorrelation) {
		case 0:
			OnFftCfAuto();
			break;
		case 1:
			OnFftCfLateral();
			break;
		}
	}
}
