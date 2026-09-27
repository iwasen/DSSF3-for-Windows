// IRAssistDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "ImpulseDlg.h"
#include "IRAssistDlg.h"
#include "RtaDlg.h"
#include "WaveInEx.h"
#include "WaveOutEx.h"
#include "Help\ContextHelp.h"

#define N_SAMPLING_RATE		48000
#define N_STAGE_LEVEL		13
#define N_STAGE_POSITION	12

#define SWITCH_NONE		-1
#define SWITCH_LEVEL	0
#define SWITCH_POSITION	1
#define SWITCH_TIME		2
#define N_SWITCH		3

#define RANGE_LEVEL		20
#define RANGE_POSITION	6

#define IDT_NEXTTIME	5000

/////////////////////////////////////////////////////////////////////////////
// CIRAssistDlg ダイアログ

CIRAssistDlg::CIRAssistDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CIRAssistDlg::IDD, pParent)
{
	m_pCcfBuf1 = NULL;
	m_pCcfBuf2 = NULL;
	m_bStart = FALSE;
	m_nSwitchNo = SWITCH_NONE;
	m_pImpulseDlg = (CImpulseDlg *)pParent;
	m_pWaveIn = CWaveInEx::GetInstance();
	m_pWaveOut = CWaveOutEx::GetInstance();
}

CIRAssistDlg::~CIRAssistDlg()
{
	m_pWaveIn->ReleaseInstance();
	m_pWaveOut->ReleaseInstance();

	FreeBuffers();
}

void CIRAssistDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEVIATION, m_cDeviation);
	DDX_Control(pDX, IDC_RESULT3, m_cResult3);
	DDX_Control(pDX, IDC_RESULT2, m_cResult2);
	DDX_Control(pDX, IDC_RESULT1, m_cResult1);
	DDX_Control(pDX, IDC_POSITION_RIGHT, m_cPositionRight);
	DDX_Control(pDX, IDC_POSITION_LEFT, m_cPositionLeft);
	DDX_Control(pDX, IDC_SN_RATIO, m_cSNRatio);
	DDX_Control(pDX, IDC_MEASURING_TIME, m_cMeasuringTime);
	DDX_Control(pDX, IDC_LEVEL_WND, m_cLevelWnd);
	DDX_Control(pDX, IDC_SWITCH1, m_cSwitch1);
	DDX_Control(pDX, IDC_SWITCH2, m_cSwitch2);
	DDX_Control(pDX, IDC_SWITCH3, m_cSwitch3);
	DDX_Control(pDX, IDC_DISTANCE, m_cDistance);
}

BEGIN_MESSAGE_MAP(CIRAssistDlg, CDialogAF)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_SWITCH1, OnSwitch1)
	ON_BN_CLICKED(IDC_SWITCH2, OnSwitch2)
	ON_BN_CLICKED(IDC_SWITCH3, OnSwitch3)
	ON_WM_TIMER()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIRAssistDlg メッセージ ハンドラ

BOOL CIRAssistDlg::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	m_nTimeSave = g_oSetData.Imp.nTime;

	CString str;
	LOGFONT font{};
#ifdef _LANG_JPN
	font.lfHeight = 200;
#else
	font.lfHeight = 160;
#endif
	font.lfCharSet = DEFAULT_CHARSET;
	str.LoadString(IDS_DEFAULTFONT);
	strcpy_s(font.lfFaceName, str);
	m_Font.CreatePointFontIndirect(&font);

	m_cResult1.SetFont(&m_Font);
	m_cResult2.SetFont(&m_Font);
	m_cResult3.SetFont(&m_Font);

	static const RANGE_COLOR rangeColor[3] = {
		{-0.2, COLOR_LEFT},
		{0.2, COLOR_GREEN},
		{1.0, COLOR_RIGHT}
	};
	m_cLevelWnd.SetRangeColor(rangeColor, 3);

	return TRUE;
}

void CIRAssistDlg::OnOK()
{
}

void CIRAssistDlg::OnDestroy()
{
	CDialogAF::OnDestroy();

	StopAdjust();
}

void CIRAssistDlg::StartAdjust(int nSwitchNo)
{
	BOOL bOK = FALSE;

	StopAdjust();

	SetSwitch(nSwitchNo);

	switch (m_nSwitchNo) {
	case SWITCH_LEVEL:
		bOK = StartAdjustLevel();
		break;
	case SWITCH_POSITION:
		bOK = StartAdjustPosition();
		break;
	case SWITCH_TIME:
		bOK = StartAdjustTime();
		break;
	}

	if (!bOK)
		StopAdjust();
}

BOOL CIRAssistDlg::StartAdjustLevel()
{
	m_MSeq.InitMethod(N_STAGE_LEVEL);

	m_nData = 1 << N_STAGE_LEVEL;

	if (!m_pWaveOut->Open(g_nWaveOutDevice, this, 1, N_SAMPLING_RATE, m_nData, 4))
		return FALSE;

	if (!m_pWaveIn->Open(g_nWaveInDevice, this, 2, N_SAMPLING_RATE, m_nData, 4, FALSE)) {
		::AfxMessageBox(IDS_MSG_SOUNDINOUT);
		return FALSE;
	}

	m_cLevelWnd.EnableWindow(TRUE);

	m_fLevel = 0;

	StartWaveInOut(m_pWaveIn, m_pWaveOut, TRUE);

	return TRUE;
}

BOOL CIRAssistDlg::StartAdjustPosition()
{
	m_MSeq.InitMethod(N_STAGE_POSITION);

	m_nData = 1 << N_STAGE_POSITION;

	if (!m_pWaveOut->Open(g_nWaveOutDevice, this, 1, N_SAMPLING_RATE, m_nData, 4))
		return FALSE;

	if (!m_pWaveIn->Open(g_nWaveInDevice, this, 2, N_SAMPLING_RATE, m_nData, 4, FALSE)) {
		::AfxMessageBox(IDS_MSG_SOUNDINOUT);
		return FALSE;
	}

	FreeBuffers();
	m_pCcfBuf1 = new double[m_nData];
	m_pCcfBuf2 = new double[m_nData];

	m_cPositionLeft.EnableWindow(TRUE);
	m_cPositionRight.EnableWindow(TRUE);

	m_nPosition = 0;
	m_fPosition = 0;

	StartWaveInOut(m_pWaveIn, m_pWaveOut, TRUE);

	return TRUE;
}

BOOL CIRAssistDlg::StartAdjustTime()
{
	g_oSetData.Imp.nTime = 0;

	StartMeasuringIR();

	return TRUE;
}

void CIRAssistDlg::StartMeasuringIR()
{
	m_pImpulseDlg->StartMeasuring();
	m_cMeasuringTime.Format("%.3f", (double)(1 << (g_oSetData.Imp.nTime + STAGE_OFFSET)) / g_oSetData.Imp.nSamplingRate);
}

void CIRAssistDlg::StopAdjust()
{
	switch (m_nSwitchNo) {
	case SWITCH_LEVEL:
	case SWITCH_POSITION:
		m_pWaveOut->Close();
		m_pWaveIn->Close();
		break;
	case SWITCH_TIME:
		m_pImpulseDlg->CancelMeasuring();
		break;
	}

	SetSwitch(SWITCH_NONE);
}

int CIRAssistDlg::OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify)
{
	switch (nCode) {
	case WAVEOUT_DATA:
		m_MSeq.GenerateSequence(pWaveNotify->pSamplesData, 2);
		return m_nData;
	case WAVEIN_DATA:
		switch (m_nSwitchNo) {
		case SWITCH_LEVEL:
			WaveAdjustLevel(pWaveNotify);
			break;
		case SWITCH_POSITION:
			WaveAdjustPosition(pWaveNotify);
			break;
		case SWITCH_TIME:
			break;
		}
		return TRUE;
	}

	return 0;
}

void CIRAssistDlg::WaveAdjustLevel(LPWAVENOTIFY pWaveNotify)
{
	double fAbsValue;
	double fMaxValue = 0;
	CRtaApp *pWinApp = (CRtaApp *)::AfxGetApp();

	double *pData = pWaveNotify->pSamplesData;
	for (int i = 0; i < m_nData * 2; i++, pData++) {
		fAbsValue = fabs(*pData);
		if (fAbsValue > fMaxValue)
			fMaxValue = fAbsValue;
	}

	double fLevel = dB20(fMaxValue) + 6;
	if (m_fLevel == 0)
		m_fLevel = fLevel;
	else
		m_fLevel = fLevel * 0.2 + m_fLevel * 0.8;

	double fMarkerPos = m_fLevel / RANGE_LEVEL;

	if (fMarkerPos < -0.2) {
		m_cResult1.SetTextColor(COLOR_BLUE);
		m_cResult1 = "NG";
	} else if (fMarkerPos > 0.2) {
		m_cResult1.SetTextColor(COLOR_RED);
		m_cResult1 = "NG";
	} else {
		m_cResult1.SetTextColor(COLOR_GREEN);
		m_cResult1 = "OK";
	}

	m_cLevelWnd.SetMarker(fMarkerPos);

	m_cDeviation.Format("%.1f", m_fLevel);

	pWinApp->m_pWndRta->UpdownInputVolume((int)(-fMarkerPos * 3000));
}

#define OVERSAMPLING	8

void CIRAssistDlg::WaveAdjustPosition(LPWAVENOTIFY pWaveNotify)
{
	double *pData;
	int i, j;
	double xt1, yt1, xt2, yt2;
	double fMaxValue;
	int nMaxPos;

	memset(m_pCcfBuf1, 0, m_nData * sizeof(double));
	memset(m_pCcfBuf2, 0, m_nData * sizeof(double));

	pData = pWaveNotify->pSamplesData;
	for (i = 0; i < m_nData / OVERSAMPLING; i++) {
		m_pCcfBuf1[i * OVERSAMPLING] = *pData++;
		m_pCcfBuf2[i * OVERSAMPLING] = *pData++;
	}

	m_oRFFT.fft(m_nData, m_pCcfBuf1);
	m_oRFFT.fft(m_nData, m_pCcfBuf2);

	memset(&m_pCcfBuf1[m_nData / (OVERSAMPLING * 2)], 0, m_nData * (OVERSAMPLING - 1) / OVERSAMPLING * sizeof(double));
	memset(&m_pCcfBuf2[m_nData / (OVERSAMPLING * 2)], 0, m_nData * (OVERSAMPLING - 1) / OVERSAMPLING * sizeof(double));

	m_pCcfBuf1[0] = 0;
	m_pCcfBuf2[0] = 0;

	for (i = 1; i < m_nData / 2; i++) {
		j = i * 2;

		xt1 = m_pCcfBuf1[j];
		yt1 = m_pCcfBuf1[j + 1];

		xt2 = m_pCcfBuf2[j];
		yt2 = m_pCcfBuf2[j + 1];

		m_pCcfBuf1[j] = xt1 * xt2 + yt1 * yt2;
		m_pCcfBuf1[j + 1] = xt2 * yt1 - xt1 * yt2;
	}

	m_oRFFT.ifft(m_nData, m_pCcfBuf1);

	fMaxValue = m_pCcfBuf1[0];
	nMaxPos = 0;
	for (i = 1; i < m_nData; i++) {
		if (m_pCcfBuf1[i] > fMaxValue) {
			fMaxValue = m_pCcfBuf1[i];
			nMaxPos = i;
		}
	}

	if (nMaxPos > m_nData / 2)
		nMaxPos -= m_nData;

	double fMarkerPos = (double)nMaxPos * 340 * 20 / (N_SAMPLING_RATE * OVERSAMPLING);

	if (fabs(m_fPosition - fMarkerPos) > 1.0) {
		if (++m_nPosition < 3)
			return;
	}
	m_nPosition = 0;
	m_fPosition = fMarkerPos;

	if (fMarkerPos < -0.2) {
		m_cResult2 = "NG";
		m_cResult2.SetTextColor(COLOR_BLUE);
	} else if (fMarkerPos > 0.2) {
		m_cResult2 = "NG";
		m_cResult2.SetTextColor(COLOR_RED);
	} else {
		m_cResult2 = "OK";
		m_cResult2.SetTextColor(COLOR_GREEN);
	}

	m_cPositionLeft.SetMarker(-fMarkerPos / 2, COLOR_LEFT, 0);
	m_cPositionRight.SetMarker(fMarkerPos / 2, COLOR_RIGHT, 1);

	m_cDistance.Format("%.1f", fMarkerPos * RANGE_POSITION);
}

#define N_DATA	6

void CIRAssistDlg::EndIR()
{
	double *pData;
	double fData[N_DATA];
	double fPower;
	int i;

	double fMax = 0;
	memset(fData, 0, sizeof(fData));

	pData = m_pImpulseDlg->m_pLeftImpulseData;
	for (i = 0; i < m_pImpulseDlg->m_nData; i++) {
		fPower = *pData * *pData;
		if (fPower > fMax)
			fMax = fPower;
		fData[i * N_DATA / m_pImpulseDlg->m_nData] += fPower;
		pData++;
	}

	for (i = 0; i < N_DATA; i++)
		fData[i] = dB10(fData[i] * N_DATA / m_pImpulseDlg->m_nData);

	double fSNRatio = dB10(fMax) - fData[N_DATA - 1];
	if (fSNRatio < 0)
		fSNRatio = 0;

	m_cSNRatio.Format("%.1f", fSNRatio);

	if (fSNRatio > 40 && fData[0] - fData[1] > 15 && fabs(fData[1] - fData[2]) < 4 && fabs(fData[2] - fData[3]) < 2) {
		StopAdjust();
		m_cResult3 = "OK";
		m_cResult3.SetTextColor(COLOR_GREEN);
		return;
	}

	if (++g_oSetData.Imp.nTime == STAGE_NUM - 1) {
		StopAdjust();
		m_cResult3 = "NG";
		m_cResult3.SetTextColor(COLOR_BLUE);
		return;
	}

	SetTimer(IDT_NEXTTIME, 100, NULL);
}

void CIRAssistDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent) {
	case IDT_NEXTTIME:
		KillTimer(nIDEvent);
		StartMeasuringIR();
		break;
	}

	CDialogAF::OnTimer(nIDEvent);
}

void CIRAssistDlg::FreeBuffers()
{
	if (m_pCcfBuf1 != NULL) {
		delete [] m_pCcfBuf1;
		m_pCcfBuf1 = NULL;
	}

	if (m_pCcfBuf2 != NULL) {
		delete [] m_pCcfBuf2;
		m_pCcfBuf2 = NULL;
	}
}

void CIRAssistDlg::OnPaint()
{
	CPaintDC dc(this); // 描画用のデバイス コンテキスト
	CString sText;
	CSize size;
	CRect rect;
	int y;
	int i;
	CPen pen1(PS_SOLID, 0, RGB(128, 128, 128));
	WINDOWPLACEMENT wp, wp2;

	CFont *pFont = dc.SelectObject(GetFont());
	CPen *pPen = dc.SelectObject(&pen1);
	dc.SetBkMode(TRANSPARENT);

	m_cLevelWnd.GetWindowPlacement(&wp);
	m_cLevelWnd.GetClientRect(rect);

	for (i = -RANGE_LEVEL; i <= RANGE_LEVEL; i += 5) {
		y = wp.rcNormalPosition.bottom - rect.Height() * (i + RANGE_LEVEL) / (RANGE_LEVEL * 2);
		dc.MoveTo(wp.rcNormalPosition.right, y);
		dc.LineTo(wp.rcNormalPosition.right + 6, y);
//		if (i % 2 == 0) {
			sText.Format("%d", i);
			size = dc.GetOutputTextExtent(sText);
			dc.TextOut(wp.rcNormalPosition.right + 10, y - size.cy / 2, sText);
//		}
	}

	m_cPositionLeft.GetWindowPlacement(&wp);
	m_cPositionRight.GetWindowPlacement(&wp2);
	m_cPositionLeft.GetClientRect(rect);

	for (i = -RANGE_POSITION * 2; i <= RANGE_POSITION * 2; i++) {
		y = wp.rcNormalPosition.bottom - rect.Height() * (i + (RANGE_POSITION * 2)) / (RANGE_POSITION * 4);

		dc.MoveTo(wp.rcNormalPosition.right, y);
		dc.LineTo(wp.rcNormalPosition.right + 6, y);

		dc.MoveTo(wp2.rcNormalPosition.left, y);
		dc.LineTo(wp2.rcNormalPosition.left - 6, y);

		if (i % 2 == 0) {
			sText.Format("%d", i / 2);
			size = dc.GetOutputTextExtent(sText);
			dc.TextOut(wp.rcNormalPosition.right + (wp2.rcNormalPosition.left - wp.rcNormalPosition.right - size.cx) / 2, y - size.cy / 2, sText);
		}
	}

	dc.SelectObject(pFont);
	dc.SelectObject(pPen);
}

void CIRAssistDlg::SetSwitch(int nSwitchNo)
{
	m_nSwitchNo = nSwitchNo;

	CButton* pSwitch[N_SWITCH]{
		&m_cSwitch1,
		&m_cSwitch2,
		&m_cSwitch3
	};

	for (int i = 0; i < N_SWITCH; i++) {
		if (i == m_nSwitchNo) {
			pSwitch[i]->SetWindowText("&Stop");
			pSwitch[i]->SetCheck(1);
		} else {
			pSwitch[i]->SetWindowText("&Start");
			pSwitch[i]->SetCheck(0);
		}
	}
}

void CIRAssistDlg::OnSwitch1()
{
	if (m_nSwitchNo == SWITCH_LEVEL)
		StopAdjust();
	else
		StartAdjust(SWITCH_LEVEL);
}

void CIRAssistDlg::OnSwitch2()
{
	if (m_nSwitchNo == SWITCH_POSITION)
		StopAdjust();
	else
		StartAdjust(SWITCH_POSITION);
}

void CIRAssistDlg::OnSwitch3()
{
	if (m_nSwitchNo == SWITCH_TIME)
		StopAdjust();
	else
		StartAdjust(SWITCH_TIME);
}

BOOL CIRAssistDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_SWITCH1, IDH_IMP_ASSIST_SWITCH1,
		IDC_RESULT1, IDH_IMP_ASSIST_RESULT1,
		IDC_LEVEL_WND, IDH_IMP_ASSIST_LEVEL_WND,
		IDC_DEVIATION, IDH_IMP_ASSIST_DEVIATION,
		IDC_SWITCH2, IDH_IMP_ASSIST_SWITCH2,
		IDC_RESULT2, IDH_IMP_ASSIST_RESULT2,
		IDC_POSITION_LEFT, IDH_IMP_ASSIST_POSITION_LEFT,
		IDC_POSITION_RIGHT, IDH_IMP_ASSIST_POSITION_RIGHT,
		IDC_DISTANCE, IDH_IMP_ASSIST_DISTANCE,
		IDC_SWITCH3, IDH_IMP_ASSIST_SWITCH3,
		IDC_RESULT3, IDH_IMP_ASSIST_RESULT3,
		IDC_MEASURING_TIME, IDH_IMP_ASSIST_MEASURING_TIME,
		IDC_SN_RATIO, IDH_IMP_ASSIST_SN_RATIO,
		IDCANCEL, IDH_IMP_ASSIST_CLOSE,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
