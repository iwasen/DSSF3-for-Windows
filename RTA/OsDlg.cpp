// OsDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "OsDlg.h"
#include "OsCalDlg.h"
#include "WaveInEx.h"
#include "Regist.h"
#include "Help\ContextHelp.h"

#define N_INTERPOLATION 15
#define MAX_LEVEL 50000

static const double tSweepTime[] = {
	1,
	0.5, 0.2, 0.1,
	0.05, 0.02, 0.01,
	0.005, 0.002, 0.001,
	0.0005, 0.0002, 0.0001,
	0.00005, 0.00002, 0.00001
};

static const int tInterpolation[] = {
	1,
	1, 1, 1,
	1, 1, 1,
	1, 1, 1,
	2, 2, 4,
	4, 8, 16
};

static const int tLevelRange[] = {
	1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000
};

/////////////////////////////////////////////////////////////////////////////
// COsDlg ダイアログ

COsDlg::COsDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(COsDlg::IDD, pParent)
{
	m_cOsTrigChannel = -1;
	m_cOsTrigSlope = -1;

	m_bStart = FALSE;
	m_bTrigSingleMode = FALSE;
	m_bInitialized = FALSE;
	m_bMax = FALSE;
	m_pInterpolationFilter = NULL;
	m_pInterpolationBufL = new double[N_INTERPOLATION];
	m_pInterpolationBufR = new double[N_INTERPOLATION];
	m_pLeftData = NULL;
	m_pRightData = NULL;
	m_fTimeCount = 0;
	m_nTimeCount = -1;
	m_pWaveIn = CWaveInEx::GetInstance();
	m_bLch = FALSE;
	m_bRch = FALSE;
	m_nWindowKind = WINDOW_OS;

	Create(IDD, g_oSetData.Os.nPosLeft, g_oSetData.Os.nPosTop, g_oSetData2.nFontSizeOs, pParent);
}

COsDlg::~COsDlg()
{
	m_pWaveIn->ReleaseInstance();
}

void COsDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OS_TRIG_HIGHCUT, m_cOsTrigHighCut);
	DDX_Control(pDX, IDC_OS_XY, m_cOsXY);
	DDX_Control(pDX, IDC_OS_TRIG_SINGLE, m_cOsTrigSingle);
	DDX_Control(pDX, IDC_OS_TRIG_POS, m_cOsTrigPos);
	DDX_Control(pDX, IDC_OS_TRIG_LOWCUT, m_cOsTrigLowCut);
	DDX_Control(pDX, IDC_OS_TRIG_LEVEL2, m_cOsTrigLevel2);
	DDX_Control(pDX, IDC_OS_TRIG_FREE, m_cOsTrigFree);
	DDX_Control(pDX, IDC_OS_TRIG_DISP, m_cOsTrigDisp);
	DDX_Control(pDX, IDC_OS_TRIG_AUTO, m_cOsTrigAuto);
	DDX_Control(pDX, IDC_OS_SWEEP2, m_cOsSweep2);
	DDX_Control(pDX, IDC_OS_REVERSE_R, m_cOsReverseR);
	DDX_Control(pDX, IDC_OS_REVERSE_L, m_cOsReverseL);
	DDX_Control(pDX, IDC_OS_POS_R2, m_cOsPosR2);
	DDX_Control(pDX, IDC_OS_POS_L2, m_cOsPosL2);
	DDX_Control(pDX, IDC_OS_LEVEL_R2, m_cOsLevelR2);
	DDX_Control(pDX, IDC_OS_LEVEL_L2, m_cOsLevelL2);
	DDX_Control(pDX, IDC_OS_LEVEL_AUTO, m_cOsLevelAuto);
	DDX_Control(pDX, IDC_OS_DELAY2, m_cOsDelay2);
	DDX_Control(pDX, IDC_OS_DELAY_RANGE, m_cOsDelayRange);
	DDX_Control(pDX, IDC_OS_TRIG_RESET, m_cOsTrigReset);
	DDX_Control(pDX, IDC_BTN_START, m_cBtnStart);
	DDX_Control(pDX, IDC_OS_TRIG_LEVEL, m_cOsTrigLevel);
	DDX_Control(pDX, IDC_OS_DELAY, m_cOsDelay);
	DDX_Control(pDX, IDC_OS_SWEEP, m_cOsSweep);
	DDX_Control(pDX, IDC_SAMPLING_RATE, m_cSamplingRate);
	DDX_Control(pDX, IDC_OS_SCREEN, m_cOsScreen);
	DDX_Control(pDX, IDC_OS_POS_R, m_cOsPosR);
	DDX_Control(pDX, IDC_OS_POS_L, m_cOsPosL);
	DDX_Control(pDX, IDC_OS_LEVEL_R, m_cOsLevelR);
	DDX_Control(pDX, IDC_OS_LEVEL_L, m_cOsLevelL);
	DDX_Control(pDX, IDC_OS_OVERLAY, m_cOverlay);
	DDX_Control(pDX, IDC_SCREEN_SHOT, m_cScreenShot);
	DDX_Control(pDX, IDC_RENDOU, m_cRendou);
	DDX_Control(pDX, IDC_OS_ZERO_LEVEL, m_cOsZeroLevel);
	DDX_Control(pDX, IDC_OS_TIME, m_cOsTime);
	DDX_Radio(pDX, IDC_OS_TRIG_CH1, m_cOsTrigChannel);
	DDX_Radio(pDX, IDC_OS_TRIG_PLUS, m_cOsTrigSlope);
	DDX_Control(pDX, IDC_OS_CHANNEL, m_cOsChannel);
}

BEGIN_MESSAGE_MAP(COsDlg, CBaseDlg)
	ON_BN_CLICKED(IDC_OS_REVERSE_L, OnOsReverseL)
	ON_BN_CLICKED(IDC_OS_REVERSE_R, OnOsReverseR)
	ON_CBN_SELCHANGE(IDC_SAMPLING_RATE, OnSelchangeSamplingRate)
	ON_BN_CLICKED(IDC_OS_TRIG_PLUS, OnOsTrigPlus)
	ON_BN_CLICKED(IDC_OS_TRIG_AUTO, OnOsTrigAuto)
	ON_BN_CLICKED(IDC_OS_TRIG_CH1, OnOsTrigCh1)
	ON_BN_CLICKED(IDC_OS_TRIG_CH2, OnOsTrigCh2)
	ON_BN_CLICKED(IDC_OS_TRIG_MINUS, OnOsTrigMinus)
	ON_BN_CLICKED(IDC_OS_TRIG_RESET, OnOsTrigReset)
	ON_BN_CLICKED(IDC_OS_TRIG_SINGLE, OnOsTrigSingle)
	ON_BN_CLICKED(IDC_BTN_START, OnStart)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_OS_LEVEL_AUTO, OnOsLevelAuto)
	ON_BN_CLICKED(IDC_OS_TRIG_FREE, OnOsTrigFree)
	ON_BN_CLICKED(IDC_OS_TRIG_HIGHCUT, OnOsTrigHighcut)
	ON_BN_CLICKED(IDC_OS_TRIG_LOWCUT, OnOsTrigLowcut)
	ON_BN_CLICKED(IDC_OS_TRIG_POS, OnOsTrigPos)
	ON_BN_CLICKED(IDC_OS_TRIG_DISP, OnOsTrigDisp)
	ON_BN_CLICKED(IDC_OS_POS_CENTER, OnOsPosCenter)
	ON_BN_CLICKED(IDC_OS_POS_ALT, OnOsPosAlt)
	ON_CBN_SELCHANGE(IDC_OS_DELAY_RANGE, OnSelchangeOsDelayRange)
	ON_BN_CLICKED(IDC_OS_XY, OnOsXy)
	ON_WM_DESTROY()
	ON_WM_MOVE()
	ON_CBN_SELCHANGE(IDC_OS_OVERLAY, OnCbnSelchangeOsOverlay)
	ON_BN_CLICKED(IDC_SCREEN_SHOT, OnBnClickedScreenShot)
	ON_BN_CLICKED(IDC_RENDOU, OnBnClickedRendou)
	ON_BN_CLICKED(IDC_OS_ZERO_LEVEL, OnBnClickedOsZeroLevel)
	ON_CBN_SELCHANGE(IDC_OS_CHANNEL, OnCbnSelchangeOsChannel)
	ON_STN_DBLCLK(IDC_OS_SCREEN, &COsDlg::OnStnDblclickOsScreen)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_OS_CALIBRATION, &COsDlg::OnBnClickedOsCalibration)
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COsDlg メッセージ ハンドラ

BOOL COsDlg::OnInitDialog()
{
	CString str;
	int i, n;

	CBaseDlg::OnInitDialog();

	CreateStatusBar();
	SetStatusBar(IDS_STATUS_STOP);

	CRect rectSeparator;
	CRect rectControl;
	GetDlgItem(IDC_SEPARATOR_LINE)->GetWindowRect(rectSeparator);
	CWnd *pWnd = GetTopWindow();
	while (pWnd != NULL) {
		switch (pWnd->GetDlgCtrlID()) {
		case IDC_OS_SCREEN:
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

	m_cOsScreen.Initialize();

	SetLevelRange();

	SetChannelMode();

	m_cOsLevelL.SetRange(0, 14);
	m_cOsLevelL.SetTicFreq(1);
	m_cOsLevelL.SetPageSize(1);
	SetLevelL();

	m_cOsLevelR.SetRange(0, 14);
	m_cOsLevelR.SetTicFreq(1);
	m_cOsLevelR.SetPageSize(1);
	SetLevelR();

	m_cOsLevelAuto = g_oSetData.Os.bLevelAuto;
	m_cRendou = g_oSetData.Os.bRendou;

	m_cOsPosL.SetRange(-(m_cOsScreen.m_nVDiv * 5), m_cOsScreen.m_nVDiv * 5);
	m_cOsPosL.SetTicFreq(10);
	m_cOsPosL.SetPageSize(10);
	SetPosL();

	m_cOsPosR.SetRange(-(m_cOsScreen.m_nVDiv * 5), m_cOsScreen.m_nVDiv * 5);
	m_cOsPosR.SetTicFreq(10);
	m_cOsPosR.SetPageSize(10);
	SetPosR();

	m_cOsSweep.SetRange(0, 15);
	m_cOsSweep.SetTicFreq(1);
	m_cOsSweep.SetPageSize(1);
	m_cOsSweep.SetPos(g_oSetData.Os.nSweep);
	SetSweepVal();

	m_cOsDelay.SetRange(0, 100);
	m_cOsDelay.SetTicFreq(10);
	m_cOsDelay.SetPageSize(10);

	for (i = 0; i < 4; i++) {
		str.Format("%d ms", (int)pow(10.0, i));
		m_cOsDelayRange.AddString(str);
	}
	m_cOsDelayRange.SetCurSel(g_oSetData.Os.nDelayRange);

	m_nDelayVal = 0;
	SetDelayVal();

	m_cOsTrigLevel.SetRange(-(m_cOsScreen.m_nVDiv * 5), m_cOsScreen.m_nVDiv * 5);
	m_cOsTrigLevel.SetTicFreq(10);
	m_cOsTrigLevel.SetPageSize(10);
	m_cOsTrigLevel.SetPos(-g_oSetData.Os.nTrigLevel);
	m_cOsTrigLevel2 = g_oSetData.Os.nTrigLevel;

	m_cOsTrigChannel = g_oSetData.Os.nTrigChannel;
	m_cOsTrigSlope = g_oSetData.Os.nTrigSlope;

	m_cOsTrigAuto = g_oSetData.Os.bTrigAuto;
	m_cOsTrigPos = g_oSetData.Os.bTrigPos;
	m_cOsTrigDisp = g_oSetData.Os.bTrigDisp;
	m_cOsTrigFree = g_oSetData.Os.bTrigFree;
	m_cOsTrigHighCut = g_oSetData.Os.bTrigHighCut;
	m_cOsTrigLowCut = g_oSetData.Os.bTrigLowCut;

	m_cOsReverseL = g_oSetData.Os.bReverseL;
	m_cOsReverseR = g_oSetData.Os.bReverseR;
	m_cOsXY = g_oSetData.Os.bXY;
	m_cOsZeroLevel = g_oSetData.Os.bZeroLevel;

	SetSamplingRateList(m_cSamplingRate, WAVE_IN, g_oSetData.Pc.nInputDevice, 0, g_oSetData.Os.nSamplingRate);

	static const int tChannel[] = {IDS_MONO, IDS_STEREO, IDS_LEFT_ONLY, IDS_RIGHT_ONLY, IDS_DIFFERENCE};
	for (i = 0; i < ARRAY_SIZE(tChannel); i++) {
		str.LoadString(tChannel[i]);
		m_cOsChannel.AddString(str);
	}
	m_cOsChannel.SetCurSel(g_oSetData.Os.nChannel);

	for (i = 1; i <= 10; i++) {
		if (i == 1)
			str.LoadString(IDS_NONE);
		else
			str.Format("%d", i);
		n = m_cOverlay.AddString(str);
		m_cOverlay.SetItemData(n, i);
	}
	str.LoadString(IDS_ALL);
	n = m_cOverlay.AddString(str);
	m_cOverlay.SetItemData(n, 0);
	SelectListBox(m_cOverlay, g_oSetData.Os.nOverlay);

	m_cOsTrigLevel.EnableWindow(!g_oSetData.Os.bTrigAuto);

	UpdateData(FALSE);

	InitScreen();

	EnableControls();
	EnableTrigReset();

	m_bInitialized = TRUE;

	return TRUE;
}

void COsDlg::OnOK()
{
	OnStart();
}

void COsDlg::OnOsXy()
{
	g_oSetData.Os.bXY = m_cOsXY;
	if (m_bStart) {
		m_cOsScreen.ClearScreen();
		DispInfo();
	}
	DispTriggerMark(FALSE);
}

void COsDlg::OnOsReverseL()
{
	g_oSetData.Os.bReverseL = m_cOsReverseL;
}

void COsDlg::OnOsReverseR()
{
	g_oSetData.Os.bReverseR = m_cOsReverseR;
}

void COsDlg::OnSelchangeSamplingRate()
{
	int nIndex;

	if ((nIndex = m_cSamplingRate.GetCurSel()) != CB_ERR) {
		g_oSetData.Os.nSamplingRate = (int)m_cSamplingRate.GetItemData(nIndex);
		SetSweepVal();
		Restart();
	}
}

void COsDlg::OnOsTrigPlus()
{
	g_oSetData.Os.nTrigSlope = 0;
}

void COsDlg::OnOsTrigAuto()
{
	g_oSetData.Os.bTrigAuto = m_cOsTrigAuto;
	m_cOsTrigLevel.EnableWindow(!g_oSetData.Os.bTrigAuto);
}

void COsDlg::OnOsTrigCh1()
{
	g_oSetData.Os.nTrigChannel = 0;
	DispTriggerMark(FALSE);
}

void COsDlg::OnOsTrigCh2()
{
	g_oSetData.Os.nTrigChannel = 1;
	DispTriggerMark(FALSE);
}

void COsDlg::OnOsTrigMinus()
{
	g_oSetData.Os.nTrigSlope = 1;
}

void COsDlg::OnOsTrigReset()
{
	m_bTrigSingle = FALSE;
	EnableTrigReset();
}

void COsDlg::OnOsTrigDisp()
{
	g_oSetData.Os.bTrigDisp = m_cOsTrigDisp;
	DispTriggerMark(FALSE);
}

void COsDlg::OnOsTrigSingle()
{
	m_bTrigSingleMode = m_cOsTrigSingle;
	EnableTrigReset();
}

void COsDlg::OnOsLevelAuto()
{
	g_oSetData.Os.bLevelAuto = m_cOsLevelAuto;
}

void COsDlg::OnOsTrigFree()
{
	g_oSetData.Os.bTrigFree = m_cOsTrigFree;
}

void COsDlg::OnOsTrigHighcut()
{
	g_oSetData.Os.bTrigHighCut = m_cOsTrigHighCut;
}

void COsDlg::OnOsTrigLowcut()
{
	g_oSetData.Os.bTrigLowCut = m_cOsTrigLowCut;
}

void COsDlg::OnOsTrigPos()
{
	g_oSetData.Os.bTrigPos = m_cOsTrigPos;
	DispTriggerMark(FALSE);
}

void COsDlg::OnOsPosCenter()
{
	g_oSetData.Os.nPosL = 0;
	SetPosL();

	g_oSetData.Os.nPosR = 0;
	SetPosR();

	DispTriggerMark(FALSE);
}

void COsDlg::OnOsPosAlt()
{
	g_oSetData.Os.nPosL = 20;
	SetPosL();

	g_oSetData.Os.nPosR = -20;
	SetPosR();

	DispTriggerMark(FALSE);
}

void COsDlg::OnSelchangeOsDelayRange()
{
	g_oSetData.Os.nDelayRange = m_cOsDelayRange.GetCurSel();
	SetDelayVal();
}

void COsDlg::OnStart()
{
	if (m_bStart)
		Stop();
	else {
		int nSamplingRate;

		if ((nSamplingRate = m_pWaveIn->GetFixedSamplingRate()) != 0)
			ChangeSamplingRate(nSamplingRate);

		m_fTimeCount = 0;

		m_bRegist = (BOOL)CHECK_LICENSE2(APP_RAL);
		m_bNoRegist = FALSE;

		if (!Start())
			m_cBtnStart.SetCheck(FALSE);
	}
}

void COsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (pScrollBar->GetDlgCtrlID()) {
	case IDC_OS_SWEEP:
		g_oSetData.Os.nSweep = ((CSliderCtrl *)pScrollBar)->GetPos();
		SetSweepVal();
		DispInfo();
		break;
	case IDC_OS_DELAY:
		m_nDelayVal = ((CSliderCtrl *)pScrollBar)->GetPos();
		SetDelayVal();
		DispInfo();
		break;
	}

	CBaseDlg::OnHScroll(nSBCode, nPos, pScrollBar);
}

void COsDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (pScrollBar->GetDlgCtrlID()) {
	case IDC_OS_LEVEL_L:
		g_oSetData.Os.nLevelL = 14 - ((CSliderCtrl *)pScrollBar)->GetPos();
		m_nLevelL = tLevelRange[g_oSetData.Os.nLevelL];
		m_fLevelL = m_nLevelL / PowDouble(16);
		m_cOsLevelL2 = AddNumberPrefix(m_nLevelL * m_fLevelRangeMul);
		if (g_oSetData.Os.bRendou && g_oSetData.Os.nChannel == CH_STEREO) {
			g_oSetData.Os.nLevelR = g_oSetData.Os.nLevelL;
			SetLevelR();
		}
		DispInfo();
		break;
	case IDC_OS_LEVEL_R:
		g_oSetData.Os.nLevelR = 14 - ((CSliderCtrl *)pScrollBar)->GetPos();
		m_nLevelR = tLevelRange[g_oSetData.Os.nLevelR];
		m_fLevelR = m_nLevelR / PowDouble(16);
		m_cOsLevelR2 = AddNumberPrefix(m_nLevelR * m_fLevelRangeMul);
		if (g_oSetData.Os.bRendou && g_oSetData.Os.nChannel == CH_STEREO) {
			g_oSetData.Os.nLevelL = g_oSetData.Os.nLevelR;
			SetLevelL();
		}
		DispInfo();
		break;
	case IDC_OS_POS_L:
		g_oSetData.Os.nPosL = -((CSliderCtrl *)pScrollBar)->GetPos();
		SetPosL();
		DispTriggerMark(FALSE);
		break;
	case IDC_OS_POS_R:
		g_oSetData.Os.nPosR = -((CSliderCtrl *)pScrollBar)->GetPos();
		SetPosR();
		DispTriggerMark(FALSE);
		break;
	case IDC_OS_TRIG_LEVEL:
		g_oSetData.Os.nTrigLevel = -((CSliderCtrl *)pScrollBar)->GetPos();
		m_cOsTrigLevel2 = g_oSetData.Os.nTrigLevel;
		GetTriggerLevel();
		m_fTrigWork = m_fTrigLevel;
		DispTriggerMark(nSBCode != SB_ENDSCROLL);
		break;
	}

	CBaseDlg::OnVScroll(nSBCode, nPos, pScrollBar);
}

void COsDlg::SetSweepVal()
{
	m_cOsSweep2 = m_fSweepDiv = tSweepTime[g_oSetData.Os.nSweep] * 1000;
	m_fSweepTime = g_oSetData.Os.nSamplingRate * tSweepTime[g_oSetData.Os.nSweep] * m_cOsScreen.m_nHDiv * tInterpolation[g_oSetData.Os.nSweep];
	m_bTrigger = FALSE;
	m_fTrigWork = 0;

	MakeInterpolationFilter(tInterpolation[g_oSetData.Os.nSweep]);

	if (m_bStart)
		AllocDataBuf();
}

void COsDlg::SetDelayVal()
{
	m_cOsDelay2 = m_fDelayTime = pow(10.0, m_cOsDelayRange.GetCurSel()) * m_nDelayVal / 100;
	m_nDelayTime = int(g_oSetData.Os.nSamplingRate * m_fDelayTime / 1000);
}

BOOL COsDlg::Start()
{
	m_nBlockSize = g_oSetData.Os.nSamplingRate / 20;
	m_bTrigger = FALSE;
	m_nTrigCounter = (int)m_fSweepTime;
	m_bTrigSingle = FALSE;
	m_fTrigWork = 0;
	m_fAvrVal = 0;
	m_nLvAdjCounterL = 0;
	m_nLvAdjCounterR = 0;
	memset(m_pInterpolationBufL, 0, sizeof(double) * N_INTERPOLATION);
	memset(m_pInterpolationBufR, 0, sizeof(double) * N_INTERPOLATION);
	m_nInterpolationPtr = 0;

	AllocDataBuf();

	EnableTrigReset();

	InitScreen();

	m_fBlockTime = (double)m_nBlockSize / g_oSetData.Os.nSamplingRate;

	if (!m_pWaveIn->Open(g_oSetData.Pc.nInputDevice, this, 2, g_oSetData.Os.nSamplingRate, m_nBlockSize, 4))
		return FALSE;

	m_pWaveIn->Start();

	return TRUE;
}

void COsDlg::Restart()
{
	if (m_bStart) {
		Stop();
		Start();
	}
}

void COsDlg::Stop()
{
	m_pWaveIn->Close();
}

int COsDlg::OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify)
{
	CString str;

	switch (nCode) {
	case WAVEIN_OPEN:
		m_bStart = TRUE;
		m_cBtnStart.SetCheck(m_bStart);
		str.LoadString(IDS_STOP);
		m_cBtnStart.SetWindowText(str);
		SetStatusBar(GetRunningStatus(IDS_STATUS_RUNNING, m_pWaveIn->GetBitsPerSample(), 0, 0));
		DispInfo();
		pWaveNotify->nWindow = WINDOW_OS;
		break;
	case WAVEIN_CLOSE:
		m_bStart = FALSE;
		m_cBtnStart.SetCheck(m_bStart);
		str.LoadString(IDS_START);
		m_cBtnStart.SetWindowText(str);
		SetStatusBar(IDS_STATUS_STOP);

		// 未登録時のダイアログ
		if (m_bNoRegist)
			CHECK_LICENSE1(APP_RAL);
		break;
	case WAVEIN_DATA:
		m_fTimeCount += m_fBlockTime;
		int nTimeCount = (int)m_fTimeCount;
		if (m_nTimeCount != nTimeCount) {
			m_cOsTime.Format("%d:%02d:%02d", nTimeCount / 3600, nTimeCount / 60 % 60, nTimeCount % 60);
			m_nTimeCount = nTimeCount;
		}

		GetWaveData(pWaveNotify->pSamplesData);

		// 未登録時の動作制限
		if (nTimeCount >= NOREG_LIMIT_TIME && !m_bRegist) {
			m_bNoRegist = TRUE;
			return FALSE;
		}

		return TRUE;
	}

	return 0;
}

void COsDlg::GetWaveData(double *pInData)
{
	int i;
	int nDataCounter = 0;
	int nSweepCounter = 0;
	double fOffset;
	int nBlockSize;
	int nOffset = -1;

	switch (g_oSetData.Os.nChannel) {
	case CH_MONO:
		for (i = 0; i < m_nBlockSize; i++) {
			pInData[i * 2] = (pInData[i * 2] + pInData[i * 2 + 1]) / 2;
		}
		break;
	case CH_DIFFERENCE:
		for (i = 0; i < m_nBlockSize; i++) {
			pInData[i * 2] = pInData[i * 2] - pInData[i * 2 + 1];
		}
		break;
	}

	if (g_oSetData.Os.bReverseL) {
		for (i = 0; i < m_nBlockSize; i++)
			pInData[i * 2] = -pInData[i * 2];
	}

	if (g_oSetData.Os.bReverseR) {
		for (i = 0; i < m_nBlockSize; i++)
			pInData[i * 2 + 1] = -pInData[i * 2 + 1];
	}

	if (g_oSetData.Os.bLevelAuto)
		AutoLevelAdjust(pInData);

	if (g_oSetData.Os.bXY) {
		m_cOsScreen.DispXY(pInData, m_nBlockSize, m_fLevelL * m_fLevelMul, m_fLevelR * m_fLevelMul, g_oSetData.Os.nPosL, g_oSetData.Os.nPosR);
		return;
	}

	nBlockSize = Interpolation(pInData);

	if (g_oSetData.Os.bTrigLowCut) {
		if (g_oSetData.Os.nTrigChannel == 0) {
			for (i = 0; i < nBlockSize; i++)
				m_fAvrVal = (m_fAvrVal * 9999 + m_pLeftData[i]) / 10000;
		} else {
			for (i = 0; i < nBlockSize; i++)
				m_fAvrVal = (m_fAvrVal * 9999 + m_pRightData[i]) / 10000;
		}
	}

	if (g_oSetData.Os.bTrigAuto)
		AutoTrigAdjust(pInData);

	GetTriggerLevel();

	BOOL bAvail = TRUE;
	for (i = 0; i < nBlockSize; i++) {
		if (CheckTrigger(i, &fOffset)) {
			if (bAvail && !m_bTrigger) {
				m_bTrigger = TRUE;
				m_nDelayCounter = 0;
				m_nSweepCounter = 0;
				m_fOffset = fOffset;
			}
		}

		if (m_bTrigger) {
			if (m_nDelayCounter >= m_nDelayTime * tInterpolation[g_oSetData.Os.nSweep]) {
				if (nOffset == -1) {
					nOffset = i;
					nDataCounter = 0;
					nSweepCounter = m_nSweepCounter;
				}

				nDataCounter++;

				if (m_nSweepCounter++ > m_fSweepTime) {
					m_bTrigger = FALSE;
					bAvail = FALSE;
				}
			} else
				m_nDelayCounter++;
		}
	}

	if (nOffset != -1) {
		if (nOffset != 0 && nSweepCounter == 0) {
			nOffset--;
			nSweepCounter--;
			nDataCounter++;
		}
		m_cOsScreen.DispScreen(&m_pLeftData[nOffset], &m_pRightData[nOffset], m_fSweepTime, nSweepCounter, nDataCounter,
				g_oSetData.Os.nChannel, m_fLevelL * m_fLevelMul, m_fLevelR * m_fLevelMul, g_oSetData.Os.nPosL, g_oSetData.Os.nPosR, m_fOffset);
	}
}

BOOL COsDlg::CheckTrigger(int nOffset, double *pOffset)
{
	double fTrigWork = m_fTrigWork;
	double fDataWork = (g_oSetData.Os.nTrigChannel == 0) ? m_pLeftData[nOffset] : m_pRightData[nOffset];

	if (g_oSetData.Os.bTrigHighCut)
		fDataWork = (m_fTrigWork * 3 + fDataWork) / 4;

	if (g_oSetData.Os.bTrigLowCut)
		fDataWork -= m_fAvrVal;

	m_fTrigWork = fDataWork;

	if (m_bTrigSingleMode && m_bTrigSingle)
		return FALSE;

	BOOL bTrigger = FALSE;
	if (g_oSetData.Os.nTrigSlope == 0) {
		if (fTrigWork < m_fTrigLevel && m_fTrigWork >= m_fTrigLevel)
			bTrigger = TRUE;
	} else {
		if (fTrigWork > m_fTrigLevel && m_fTrigWork <= m_fTrigLevel)
			bTrigger = TRUE;
	}

	if (bTrigger) {
		m_nTrigCounter = (int)(m_fSweepTime * 10);
		if (m_bTrigSingleMode && !m_bTrigSingle) {
			m_bTrigSingle = TRUE;
			EnableTrigReset();
		}
		*pOffset = (m_fTrigWork - m_fTrigLevel) / (m_fTrigWork - fTrigWork);
		return TRUE;
	}

	if (g_oSetData.Os.bTrigFree && !m_bTrigSingleMode && --m_nTrigCounter < 0) {
		m_nTrigCounter = (int)(m_fSweepTime / 4);
		*pOffset = 0;
		return TRUE;
	}

	return FALSE;
}

void COsDlg::GetTriggerLevel()
{
	m_fTrigLevel = g_oSetData.Os.nTrigLevel;

	if (!g_oSetData.Os.bTrigPos)
		m_fTrigLevel -= (g_oSetData.Os.nTrigChannel == 0) ? g_oSetData.Os.nPosL : g_oSetData.Os.nPosR;

	double fLevel = (g_oSetData.Os.nTrigChannel == 0) ? m_fLevelL : m_fLevelR;
	m_fTrigLevel = m_fTrigLevel * fLevel * m_fLevelMul / 10;
}

void COsDlg::DispTriggerMark(BOOL bDisp)
{
	int nTrigLevel = g_oSetData.Os.nTrigLevel;
	if (g_oSetData.Os.bTrigPos)
		nTrigLevel += (g_oSetData.Os.nTrigChannel == 0) ? g_oSetData.Os.nPosL : g_oSetData.Os.nPosR;

	m_cOsScreen.DispTriggerLevel(nTrigLevel, g_oSetData.Os.nTrigChannel, !g_oSetData.Os.bXY && (g_oSetData.Os.bTrigDisp || bDisp));
}

void COsDlg::InitScreen()
{
	m_cOsScreen.ClearScreen();
	DispTriggerMark(FALSE);
}

void COsDlg::EnableControls()
{
	m_cOsLevelL.EnableWindow(m_bLch);
	GetDlgItem(IDC_OS_LEVEL_L2)->EnableWindow(m_bLch);
	m_cOsPosL.EnableWindow(m_bLch);
	GetDlgItem(IDC_OS_POS_L2)->EnableWindow(m_bLch);
	GetDlgItem(IDC_OS_TRIG_CH1)->EnableWindow(m_bLch);
	GetDlgItem(IDC_OS_REVERSE_L)->EnableWindow(m_bLch);

	m_cOsLevelR.EnableWindow(m_bRch);
	GetDlgItem(IDC_OS_LEVEL_R2)->EnableWindow(m_bRch);
	m_cOsPosR.EnableWindow(m_bRch);
	GetDlgItem(IDC_OS_POS_R2)->EnableWindow(m_bRch);
	GetDlgItem(IDC_OS_TRIG_CH2)->EnableWindow(m_bRch);
	GetDlgItem(IDC_OS_REVERSE_R)->EnableWindow(m_bRch);

	GetDlgItem(IDC_OS_POS_ALT)->EnableWindow(g_oSetData.Os.nChannel == CH_STEREO);
	GetDlgItem(IDC_OS_XY)->EnableWindow(g_oSetData.Os.nChannel == CH_STEREO);
	m_cRendou.EnableWindow(g_oSetData.Os.nChannel == CH_STEREO);
}

void COsDlg::EnableTrigReset()
{
	m_cOsTrigReset.EnableWindow(m_bTrigSingleMode && m_bTrigSingle);
}

void COsDlg::AutoLevelAdjust(const double *pInData)
{
	double fPeakLeft = 0, fPeakRight = 0;
	double fData;
	BOOL bRendou = (g_oSetData.Os.nChannel == CH_STEREO && g_oSetData.Os.bRendou);

	for (int i = 0; i < m_nBlockSize; i++) {
		fData = fabs(*pInData++);
		if (fData > fPeakLeft)
			fPeakLeft = fData;

		fData = fabs(*pInData++);
		if (fData > fPeakRight)
			fPeakRight = fData;
	}

	if (bRendou)
		fPeakLeft = max(fPeakLeft, fPeakRight);

	if (m_bLch) {
		if (fPeakLeft > m_fLevelL * (m_cOsScreen.m_nVDiv - 2) / 2) {
			if (++m_nLvAdjCounterL >= 10) {
				if (g_oSetData.Os.nLevelL < 15) {
					g_oSetData.Os.nLevelL++;
					SetLevelL();
					if (bRendou) {
						g_oSetData.Os.nLevelR = g_oSetData.Os.nLevelL;
						SetLevelR();
					}
					DispInfo();
				}
			}
		} else if (fPeakLeft < m_fLevelL * 1.5) {
			if (++m_nLvAdjCounterL >= 10) {
				if (g_oSetData.Os.nLevelL > 0) {
					g_oSetData.Os.nLevelL--;
					SetLevelL();
					if (bRendou) {
						g_oSetData.Os.nLevelR = g_oSetData.Os.nLevelL;
						SetLevelR();
					}
					DispInfo();
				}
			}
		} else
			m_nLvAdjCounterL = 0;
	}

	if (m_bRch && !bRendou) {
		if (fPeakRight > m_fLevelR * (m_cOsScreen.m_nVDiv - 2) / 2) {
			if (++m_nLvAdjCounterR >= 10) {
				if (g_oSetData.Os.nLevelR < 15) {
					g_oSetData.Os.nLevelR++;
					SetLevelR();
					DispInfo();
				}
			}
		} else if (fPeakRight < m_fLevelR * 1.5) {
			if (++m_nLvAdjCounterR >= 10) {
				if (g_oSetData.Os.nLevelR > 0) {
					g_oSetData.Os.nLevelR--;
					SetLevelR();
					DispInfo();
				}
			}
		} else
			m_nLvAdjCounterR = 0;
	}
}

void COsDlg::SetLevelL()
{
	m_cOsLevelL.SetPos(14 - g_oSetData.Os.nLevelL);
	m_nLevelL = tLevelRange[g_oSetData.Os.nLevelL];
	m_fLevelL = m_nLevelL / PowDouble(16);
	m_cOsLevelL2 = AddNumberPrefix(m_nLevelL * m_fLevelRangeMul);
}

void COsDlg::SetLevelR()
{
	m_cOsLevelR.SetPos(14 - g_oSetData.Os.nLevelR);
	m_nLevelR = tLevelRange[g_oSetData.Os.nLevelR];
	m_fLevelR = m_nLevelR / PowDouble(16);
	m_cOsLevelR2 = AddNumberPrefix(m_nLevelR * m_fLevelRangeMul);
}

void COsDlg::SetPosL()
{
	m_cOsPosL.SetPos(-g_oSetData.Os.nPosL);
	m_cOsPosL2 = g_oSetData.Os.nPosL;

	if (!g_oSetData.Os.bXY && g_oSetData.Os.bZeroLevel)
		m_cOsScreen.Invalidate(FALSE);
}

void COsDlg::SetPosR()
{
	m_cOsPosR.SetPos(-g_oSetData.Os.nPosR);
	m_cOsPosR2 = g_oSetData.Os.nPosR;

	if (!g_oSetData.Os.bXY && g_oSetData.Os.bZeroLevel)
		m_cOsScreen.Invalidate(FALSE);
}

void COsDlg::AutoTrigAdjust(const double *pInData)
{
	double fLeftSum = 0, fRightSum = 0;

	for (int i = 0; i < m_nBlockSize; i++) {
		fLeftSum += *pInData++;
		fRightSum += *pInData++;
	}

	double fSum = (g_oSetData.Os.nTrigChannel == 0) ? fLeftSum : fRightSum;
	fSum = (fSum / m_nBlockSize) * 10 / ((g_oSetData.Os.nTrigChannel == 0 ? m_fLevelL : m_fLevelR) * m_fLevelMul);

	if (!g_oSetData.Os.bTrigPos)
		fSum += (g_oSetData.Os.nTrigChannel == 0) ? g_oSetData.Os.nPosL : g_oSetData.Os.nPosR;

	if (fabs(fSum - g_oSetData.Os.nTrigLevel) > 1) {
		g_oSetData.Os.nTrigLevel = (int)fSum;
		m_cOsTrigLevel.SetPos(-g_oSetData.Os.nTrigLevel / 2);
		m_cOsTrigLevel2 = g_oSetData.Os.nTrigLevel;
		DispTriggerMark(FALSE);
	}
}

void COsDlg::DispInfo()
{
	if (!m_bStart)
		return;

	char text[100];
	size_t nSize = sizeof(text);
	sprintf_s(text, "SamplingRate:%gkHz", (double)g_oSetData.Os.nSamplingRate / 1000);
	m_cOsScreen.DispText(text, 0);

	LPCSTR p = text;
	size_t nLen;

	if (m_bLch) {
		LPCSTR p2;
		switch (g_oSetData.Os.nChannel) {
		case CH_MONO:
			p2 = "Lch+Rch";
			break;
		case CH_DIFFERENCE:
			p2 = "Lch-Rch";
			break;
		default:
			p2 = "Lch";
			break;
		}

		sprintf_s((char *const)p, nSize, "%s:%s%s/div  ", p2, (LPCSTR)AddNumberPrefix(m_nLevelL * m_fLevelRangeMul), (LPCSTR)g_oSetData.Os.sCalUnit);
		nLen = strlen(p);
		p += nLen;
		nSize -= nLen;
	}

	if (m_bRch) {
		sprintf_s((char* const)p, nSize, "Rch:%s%s/div  ", (LPCSTR)AddNumberPrefix(m_nLevelR * m_fLevelRangeMul), (LPCSTR)g_oSetData.Os.sCalUnit);
		nLen = strlen(p);
		p += nLen;
		nSize -= nLen;
	}

	if (!g_oSetData.Os.bXY)
		sprintf_s((char* const)p, nSize, "Time:%gms/div  Delay:%gms", m_fSweepDiv, m_fDelayTime);

	m_cOsScreen.DispText(text, 1);
}

void COsDlg::OnDestroy()
{
	CBaseDlg::OnDestroy();

	if (m_bStart)
		Stop();

	if (m_pInterpolationFilter != NULL) {
		delete [] m_pInterpolationFilter;
		m_pInterpolationFilter = NULL;
	}

	delete [] m_pInterpolationBufL;
	delete [] m_pInterpolationBufR;

	FreeDataBuf();
}

void COsDlg::OnMove(int x, int y)
{
	CBaseDlg::OnMove(x, y);

	if (m_bInitialized && !IsIconic()) {
		CRect rect;
		GetWindowRect(rect);
		g_oSetData.Os.nPosLeft = rect.left;
		g_oSetData.Os.nPosTop = rect.top;
	}
}

void COsDlg::ShowChildWindow(BOOL bShow)
{
	LockWindowUpdate();

	CWnd *pWnd = GetTopWindow();
	while (pWnd != NULL) {
		if (pWnd->m_hWnd != m_cOsScreen.m_hWnd && pWnd->m_hWnd != m_wndStatusBar.m_hWnd)
			pWnd->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
		pWnd = pWnd->GetNextWindow();
	}

	UnlockWindowUpdate();
}

void COsDlg::MakeInterpolationFilter(int n)
{
	if (m_pInterpolationFilter != NULL) {
		delete [] m_pInterpolationFilter;
		m_pInterpolationFilter = NULL;
	}

	if (n == 1)
		return;

	int kn = (N_INTERPOLATION - 1) * n + 1;
	m_pInterpolationFilter = new double[kn];

	double h;
	for (int i = 0; i < kn; i++) {
		int k = i - kn / 2;
		double a = k * M_PI / n;
		if (a == 0)
			h = 1;
		else
			h = sin(a) / a;

		h *= (0.54 - 0.46 * cos(2 * M_PI * i / (kn - 1)));

		m_pInterpolationFilter[i] = h;
	}
}

int COsDlg::Interpolation(const double *pInData)
{
	int n = tInterpolation[g_oSetData.Os.nSweep];

	for (int i = 0; i < m_nBlockSize; i++) {
		if (n == 1) {
			m_pLeftData[i * n] = *pInData++;
			m_pRightData[i * n] = *pInData++;
		} else {
			Convolution(*pInData++, &m_pLeftData[i * n], m_pInterpolationBufL);
			Convolution(*pInData++, &m_pRightData[i * n], m_pInterpolationBufR);
		}

		if (++m_nInterpolationPtr >= N_INTERPOLATION)
			m_nInterpolationPtr = 0;
	}

	return m_nBlockSize * n;
}

void COsDlg::Convolution(double in, double *out, double *pInterpolationBuf)
{
	pInterpolationBuf[m_nInterpolationPtr] = in;

	int n = tInterpolation[g_oSetData.Os.nSweep];
	int kn = (N_INTERPOLATION - 1) * n + 1;
	for (int i = 0; i < n; i++) {
		double s = 0;
		int offset = m_nInterpolationPtr;
		for (int j = 0; j < N_INTERPOLATION; j++) {
			int k = j * n + i;
			if (k < kn)
				s += pInterpolationBuf[offset] * m_pInterpolationFilter[k];
			if (--offset < 0)
				offset = N_INTERPOLATION - 1;
		}
		*out++ = s;
	}
}

void COsDlg::AllocDataBuf()
{
	FreeDataBuf();

	m_pLeftData = new double[m_nBlockSize * tInterpolation[g_oSetData.Os.nSweep]];
	m_pRightData = new double[m_nBlockSize * tInterpolation[g_oSetData.Os.nSweep]];
}

void COsDlg::FreeDataBuf()
{
	if (m_pLeftData != NULL) {
		delete [] m_pLeftData;
		m_pLeftData = NULL;
	}

	if (m_pRightData != NULL) {
		delete [] m_pRightData;
		m_pRightData = NULL;
	}
}

void COsDlg::OnCbnSelchangeOsOverlay()
{
	g_oSetData.Os.nOverlay = (int)m_cOverlay.GetItemData(m_cOverlay.GetCurSel());
	m_bTrigger = FALSE;
	m_fTrigWork = 0;
}

void COsDlg::ChangeWaveDevice(int nInOut)
{
	if (nInOut & WAVE_IN) {
		if (m_bStart)
			Stop();

		SetSamplingRateList(m_cSamplingRate, WAVE_IN, g_oSetData.Pc.nInputDevice, 0, g_oSetData.Os.nSamplingRate);
		OnSelchangeSamplingRate();
	}
}

void COsDlg::ChangeSamplingRate(int nSamplingRate)
{
	if (g_oSetData.Os.nSamplingRate != nSamplingRate) {
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

void COsDlg::OnBnClickedScreenShot()
{
	HBITMAP hBmp = m_cOsScreen.GetBitmap();
	CString sText;
	GetWindowText(sText);
	CString title;
	int nTimeCount = (int)m_fTimeCount;
	title.Format("%s - %d:%02d:%02d", (LPCSTR)sText, nTimeCount / 3600, nTimeCount / 60 % 60, nTimeCount % 60);
	m_oScreenShot.ShowScreenShot(title, hBmp, FALSE);
}

void COsDlg::OnBnClickedRendou()
{
	g_oSetData.Os.bRendou = m_cRendou;
}

void COsDlg::OnBnClickedOsZeroLevel()
{
	g_oSetData.Os.bZeroLevel = m_cOsZeroLevel;
	m_cOsScreen.Invalidate(FALSE);
}

void COsDlg::OnCbnSelchangeOsChannel()
{
	int nChannel = m_cOsChannel.GetCurSel();

	if (g_oSetData.Os.nChannel != nChannel) {
		g_oSetData.Os.nChannel = nChannel;
		SetChannelMode();

		if (nChannel != CH_STEREO) {
			UpdateData(TRUE);

			g_oSetData.Os.nTrigChannel = (nChannel == CH_RIGHT_ONLY) ? 1 : 0;
			if (m_cOsTrigChannel != g_oSetData.Os.nTrigChannel) {
				m_cOsTrigChannel = g_oSetData.Os.nTrigChannel;
				UpdateData(FALSE);
			}

			g_oSetData.Os.bXY = FALSE;
			if (m_cOsXY != g_oSetData.Os.bXY) {
				m_cOsXY = g_oSetData.Os.bXY;
				UpdateData(FALSE);
				m_cOsScreen.ClearScreen();
			}
		}

		EnableControls();

		if (!g_oSetData.Os.bXY && g_oSetData.Os.bZeroLevel)
			m_cOsScreen.Invalidate(FALSE);

		Restart();
	}
}

void COsDlg::SetChannelMode()
{
	m_bLch = (g_oSetData.Os.nChannel != CH_RIGHT_ONLY);
	m_bRch = (g_oSetData.Os.nChannel == CH_STEREO || g_oSetData.Os.nChannel == CH_RIGHT_ONLY);
}

void COsDlg::OnStnDblclickOsScreen()
{
	if (!m_bMax) {
		ShowChildWindow(FALSE);

		SetMaximunControl(m_cOsScreen.m_hWnd);
		m_cOsScreen.Resize();
		m_cOsScreen.Invalidate(FALSE);
		DispInfo();
		DispTriggerMark(FALSE);

		m_bMax = TRUE;
	} else {
		SetMaximunControl(0);

		m_cOsScreen.Resize();
		m_cOsScreen.Invalidate(FALSE);
		DispInfo();
		DispTriggerMark(FALSE);

		m_cOsScreen.UpdateWindow();
		ShowChildWindow(TRUE);

		m_bMax = FALSE;
	}
}

BOOL COsDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_OS_SCREEN, IDH_OS_SCREEN,
		IDC_BTN_START, IDH_OS_START,
		IDC_OS_TIME, IDH_OS_TIME,
		IDCANCEL, IDH_OS_CLOSE,
		IDC_SCREEN_SHOT, IDH_OS_SCREEN_SHOT,
		IDC_OS_LEVEL_L, IDH_OS_LEVEL_L,
		IDC_OS_LEVEL_L2, IDH_OS_LEVEL_L2,
		IDC_OS_LEVEL_R, IDH_OS_LEVEL_R,
		IDC_OS_LEVEL_R2, IDH_OS_LEVEL_R2,
		IDC_OS_LEVEL_AUTO, IDH_OS_LEVEL_AUTO,
		IDC_RENDOU, IDH_OS_RENDOU,
		IDC_OS_POS_L, IDH_OS_POS_L,
		IDC_OS_POS_L2, IDH_OS_POS_L2,
		IDC_OS_POS_R, IDH_OS_POS_R,
		IDC_OS_POS_R2, IDH_OS_POS_R2,
		IDC_OS_POS_CENTER, IDH_OS_POS_CENTER,
		IDC_OS_POS_ALT, IDH_OS_POS_ALT,
		IDC_OS_TRIG_LEVEL, IDH_OS_TRIG_LEVEL,
		IDC_OS_TRIG_LEVEL2, IDH_OS_TRIG_LEVEL2,
		IDC_OS_TRIG_AUTO, IDH_OS_TRIG_AUTO,
		IDC_OS_TRIG_CH1, IDH_OS_TRIG_CH1,
		IDC_OS_TRIG_CH2, IDH_OS_TRIG_CH2,
		IDC_OS_TRIG_PLUS, IDH_OS_TRIG_PLUS,
		IDC_OS_TRIG_MINUS, IDH_OS_TRIG_MINUS,
		IDC_OS_TRIG_HIGHCUT, IDH_OS_TRIG_HIGHCUT,
		IDC_OS_TRIG_LOWCUT, IDH_OS_TRIG_LOWCUT,
		IDC_OS_TRIG_POS, IDH_OS_TRIG_POS,
		IDC_OS_TRIG_DISP, IDH_OS_TRIG_DISP,
		IDC_OS_TRIG_FREE, IDH_OS_TRIG_FREE,
		IDC_OS_TRIG_SINGLE, IDH_OS_TRIG_SINGLE,
		IDC_OS_TRIG_RESET, IDH_OS_TRIG_RESET,
		IDC_OS_OVERLAY, IDH_OS_OVERLAY,
		IDC_OS_SWEEP, IDH_OS_SWEEP,
		IDC_OS_SWEEP2, IDH_OS_SWEEP2,
		IDC_OS_DELAY_RANGE, IDH_OS_DELAY_RANGE,
		IDC_OS_DELAY, IDH_OS_DELAY,
		IDC_OS_DELAY2, IDH_OS_DELAY2,
		IDC_OS_REVERSE_L, IDH_OS_REVERSE_L,
		IDC_OS_REVERSE_R, IDH_OS_REVERSE_R,
		IDC_OS_XY, IDH_OS_XY,
		IDC_OS_ZERO_LEVEL, IDH_OS_ZERO_LEVEL,
		IDC_SAMPLING_RATE, IDH_OS_SAMPLING_RATE,
		IDC_OS_CHANNEL, IDH_OS_CHANNEL,
		IDC_OS_CALIBRATION, IDH_OS_CALIBRATION,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

void COsDlg::OnBnClickedOsCalibration()
{
	COsCalDlg dlg;

	if (dlg.DoModal() == IDOK) {
		SetLevelRange();
		SetLevelL();
		SetLevelR();
		if (m_bStart)
			DispInfo();
	}
}

void COsDlg::SetLevelRange()
{
	double fCalFactor = (g_oSetData.Os.fCalValue == 0) ? 1 : g_oSetData.Os.fCalValue / g_oSetData.Os.fCalLevel;

	m_fLevelRangeMul = pow(10, floor(log10(fCalFactor * 2)));
	m_fLevelMul = m_fLevelRangeMul / fCalFactor;
}

CString COsDlg::AddNumberPrefix(double fNumber)
{
	double fMul;
	CString sPrefix;
	CString sNumber;

	if (fNumber < 0.0001) {
		fMul = 1e6;
		sPrefix = "μ";
	} else if (fNumber < 0.1) {
		fMul = 1e3;
		sPrefix = "m";
	} else {
		fMul = 1;
		sPrefix.Empty();
	}

	sNumber.Format("%g%s", fNumber * fMul, (LPCSTR)sPrefix);
	return sNumber;
}

void COsDlg::OnSize(UINT nType, int cx, int cy)
{
	CBaseDlg::OnSize(nType, cx, cy);

	if (m_bInitialized) {
		m_cOsScreen.Resize();
		DispInfo();
		DispTriggerMark(FALSE);

		m_cOsScreen.Invalidate(FALSE);
	}
}
