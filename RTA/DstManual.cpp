// DstManual.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "DstManual.h"
#include "Help\ContextHelp.h"

#define MIN_FREQ_VAL	10
#define MAX_FREQ_VAL	10000
#define MIN_LEVEL_VAL	-60
#define MAX_LEVEL_VAL	0

#define MIN_FREQ_POS 0
#define MAX_FREQ_POS 100
#define MIN_LEVEL_POS 0
#define MAX_LEVEL_POS (-(MIN_LEVEL_VAL))

#define MAX_HARMONICS	30

// CDstManual ダイアログ

CDstManual::CDstManual(CWnd* pParent /*=NULL*/)
	: CDialogExt(CDstManual::IDD, pParent)
{
	m_pLeftDst = NULL;
	m_pRightDst = NULL;
	m_bValidData = FALSE;
	m_bInitialized = FALSE;
}

CDstManual::~CDstManual()
{
	FreeBuffers();
}

void CDstManual::DoDataExchange(CDataExchange* pDX)
{
	CDialogExt::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FREQ_SLIDER, m_cFreqSlider);
	DDX_Control(pDX, IDC_FREQ_EDIT, m_cFreqEdit);
	DDX_Control(pDX, IDC_LEVEL_SLIDER, m_cLevelSlider);
	DDX_Control(pDX, IDC_LEVEL_EDIT, m_cLevelEdit);
	DDX_Control(pDX, IDC_LEFT_THD, m_cLeftTHD);
	DDX_Control(pDX, IDC_RIGHT_THD, m_cRightTHD);
	DDX_Control(pDX, IDC_HARM_GRAPH, m_cGraph);
	DDX_Control(pDX, IDC_LEFT_UNIT, m_cLeftUnit);
	DDX_Control(pDX, IDC_RIGHT_UNIT, m_cRightUnit);
	DDX_Control(pDX, IDC_AVERAGE, m_cAverage);
}

BEGIN_MESSAGE_MAP(CDstManual, CDialogExt)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_FREQ_EDIT, OnEnChangeFreqEdit)
	ON_EN_CHANGE(IDC_LEVEL_EDIT, OnEnChangeLevelEdit)
	ON_BN_CLICKED(IDC_AVERAGE, OnBnClickedAverage)
	ON_WM_HELPINFO()
	ON_MESSAGE(WM_TAB_INIT_DIALOG, OnTabInitDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CDstManual メッセージ ハンドラ

BOOL CDstManual::OnInitDialog()
{
	CDialogExt::OnInitDialog();

	m_cGraph.Initialize();

	InitFreqSlider();
	InitLevelSlider();

	m_cFreqEdit = g_oSetData.Dst.nManualFreq;
	m_cLevelEdit = -g_oSetData.Dst.nManualLevel;
	m_cAverage = g_oSetData.Dst.bManualAverage;

	m_cFreqEdit.SetValidChar(VC_NUM);
	m_cLevelEdit.SetValidChar(VC_NUM);

	DispTHD();

	return TRUE;
}

LRESULT CDstManual::OnTabInitDialog(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	SaveWindowSize();

	CWnd *pWnd = GetTopWindow();
	while (pWnd != NULL) {
		switch (pWnd->GetDlgCtrlID()) {
		case IDC_HARM_GRAPH:
			SetCtlPosition(pWnd, 0, 0, 1, 1);
			break;
		default:
			SetCtlPosition(pWnd, 0, 1, 0, 1);
			break;
		}
		pWnd = pWnd->GetNextWindow();
	}

	m_bInitialized = TRUE;

	return 0;
}

void CDstManual::InitFreqSlider()
{
	int nFreq;
	int nPos;

	m_cFreqSlider.SetRange(MIN_FREQ_POS, MAX_FREQ_POS);

	for (nFreq = MIN_FREQ_VAL; nFreq <= MAX_FREQ_VAL; ) {
		nPos = GetPosFromFreq(nFreq);
		m_cFreqSlider.SetTic(nPos);
		if (nFreq < 100)
			nFreq += 10;
		else if (nFreq < 1000)
			nFreq += 100;
		else if (nFreq < 10000)
			nFreq += 1000;
		else
			nFreq += 10000;
	}
}

void CDstManual::InitLevelSlider()
{
	int nLevel;

	m_cLevelSlider.SetRange(MIN_LEVEL_POS, MAX_LEVEL_POS);

	for (nLevel = MIN_LEVEL_POS; nLevel <= MAX_LEVEL_POS; nLevel += 10)
		m_cLevelSlider.SetTic(nLevel);
}

void CDstManual::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (pScrollBar->GetDlgCtrlID()) {
	case IDC_FREQ_SLIDER:
		nPos = ((CSliderCtrl *)pScrollBar)->GetPos();
		SetFreq(GetFreqFromPos(nPos));
		break;
	case IDC_LEVEL_SLIDER:
		nPos = ((CSliderCtrl *)pScrollBar)->GetPos();
		SetLevel((int)nPos - MAX_LEVEL_POS);
		break;
	}

	CDialogExt::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDstManual::SetFreq(int nFreq)
{
//	if (nFreq < MIN_FREQ_VAL)
//		nFreq = MIN_FREQ_VAL;
//	else if (nFreq > MAX_FREQ_VAL)
//		nFreq = MAX_FREQ_VAL;

	if (nFreq <= 0)
		nFreq = 1;

	m_cFreqEdit = nFreq;
	m_cFreqSlider.SetPos(GetPosFromFreq(nFreq));
	g_oSetData.Dst.nManualFreq = nFreq;
}

void CDstManual::SetLevel(int nLevel)
{
	if (nLevel < MIN_LEVEL_VAL)
		nLevel = MIN_LEVEL_VAL;
	else if (nLevel > MAX_LEVEL_VAL)
		nLevel = MAX_LEVEL_VAL;

	m_cLevelEdit = -nLevel;
	m_cLevelSlider.SetPos(nLevel + MAX_LEVEL_POS);
	g_oSetData.Dst.nManualLevel = nLevel;
}

int CDstManual::GetPosFromFreq(int nFreq)
{
	if (nFreq < MIN_FREQ_VAL)
		nFreq = MIN_FREQ_VAL;
	else if (nFreq > MAX_FREQ_VAL)
		nFreq = MAX_FREQ_VAL;

	return (int)((log((double)nFreq) - log((double)MIN_FREQ_VAL)) * MAX_FREQ_POS / (log((double)MAX_FREQ_VAL) - log((double)MIN_FREQ_VAL)) + 0.5);
}

int CDstManual::GetFreqFromPos(int nPos)
{
	if (nPos < MIN_FREQ_POS)
		nPos = MIN_FREQ_POS;
	else if (nPos > MAX_FREQ_POS)
		nPos = MAX_FREQ_POS;

	return (int)(exp(log((double)MIN_FREQ_VAL) + nPos * (log((double)MAX_FREQ_VAL) - log((double)MIN_FREQ_VAL)) / MAX_FREQ_POS) + 0.5);
}

void CDstManual::OnEnChangeFreqEdit()
{
	SetFreq(m_cFreqEdit);
}

void CDstManual::OnEnChangeLevelEdit()
{
	SetLevel(-(int)m_cLevelEdit);
}

void CDstManual::Initialize()
{
	FreeBuffers();
	Redraw();

	m_pLeftDst = new double[MAX_HARMONICS];
	memset(m_pLeftDst, 0, sizeof(double) * MAX_HARMONICS);
	if (g_oSetData.Dst.nChannel == 1) {
		m_pRightDst = new double[MAX_HARMONICS];
		memset(m_pRightDst, 0, sizeof(double) * MAX_HARMONICS);
	}

	m_nAverageCnt = 0;
}

BOOL CDstManual::WaveOutData(int &nFreq, double &fLevel)
{
	nFreq = g_oSetData.Dst.nManualFreq;
	fLevel = g_oSetData.Dst.nManualLevel;

	return TRUE;
}

BOOL CDstManual::NotifyTHD(const double *pLeftDst, const double *pRightDst)
{
	m_bValidData = TRUE;
	m_nMaxHarmonics = g_oSetData.Dst.nMaxHarmonics;

	if (g_oSetData.Dst.bManualAverage && m_nAverageCnt != 0) {
		for (int i = 0; i < m_nMaxHarmonics; i++) {
			m_pLeftDst[i] = (m_pLeftDst[i] * m_nAverageCnt + pLeftDst[i]) / (m_nAverageCnt + 1);
			if (m_pRightDst != NULL)
				m_pRightDst[i] = (m_pRightDst[i] * m_nAverageCnt + pRightDst[i]) / (m_nAverageCnt + 1);
		}

		m_nAverageCnt++;
	} else {
		memcpy(m_pLeftDst, pLeftDst, sizeof(double) * m_nMaxHarmonics);
		if (m_pRightDst != NULL)
			memcpy(m_pRightDst, pRightDst, sizeof(double) * m_nMaxHarmonics);

		m_nAverageCnt = 1;
	}

	Redraw();

	return TRUE;
}

void CDstManual::Redraw()
{
	if (!m_bValidData) {
		m_nChannel = g_oSetData.Dst.nChannel;
		m_nMaxHarmonics = g_oSetData.Dst.nMaxHarmonics;
	}

	DispTHD();
	m_cGraph.DispGraph(m_pLeftDst, m_pRightDst, m_nChannel, m_nMaxHarmonics);
}

void CDstManual::DispTHD()
{
	if (g_oSetData.Dst.nScaleMode == 0) {
		if (m_pLeftDst != NULL)
			m_cLeftTHD.Format("%.4f", sqrt(m_pLeftDst[0]) * 100);
		if (m_pRightDst != NULL)
			m_cRightTHD.Format("%.4f", sqrt(m_pRightDst[0]) * 100);

		m_cLeftUnit.SetWindowText("%");
		m_cRightUnit.SetWindowText("%");
	} else {
		if (m_pLeftDst != NULL)
			m_cLeftTHD.Format("%.1f", dB10(m_pLeftDst[0]));
		if (m_pRightDst != NULL)
			m_cRightTHD.Format("%.1f", dB10(m_pRightDst[0]));

		m_cLeftUnit.SetWindowText("dB");
		m_cRightUnit.SetWindowText("dB");
	}
}

void CDstManual::FreeBuffers()
{
	if (m_pLeftDst != NULL) {
		delete [] m_pLeftDst;
		m_pLeftDst = NULL;
	}

	if (m_pRightDst != NULL) {
		delete [] m_pRightDst;
		m_pRightDst = NULL;
	}

	m_bValidData = FALSE;
}

void CDstManual::OnOK()
{
}

void CDstManual::OnCancel()
{
}

void CDstManual::OnBnClickedAverage()
{
	g_oSetData.Dst.bManualAverage = m_cAverage;
}

void CDstManual::CsvOutput(LPCTSTR pFileName)
{
	CCsvFile oCsvFile;

	if (oCsvFile.Open(pFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
		m_cGraph.CsvOutput(oCsvFile, m_pLeftDst, m_pRightDst, m_nMaxHarmonics);
}

BOOL CDstManual::CheckDataExist()
{
	return m_bValidData;
}

HBITMAP CDstManual::GetBitmap()
{
	return m_cGraph.GetBitmap();
}

BOOL CDstManual::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_HARM_GRAPH, IDH_DST_MANUAL_HARM_GRAPH,
		IDC_FREQ_SLIDER, IDH_DST_MANUAL_FREQ_SLIDER,
		IDC_FREQ_EDIT, IDH_DST_MANUAL_FREQ_EDIT,
		IDC_LEVEL_SLIDER, IDH_DST_MANUAL_LEVEL_SLIDER,
		IDC_LEVEL_EDIT, IDH_DST_MANUAL_LEVEL_EDIT,
		IDC_AVERAGE, IDH_DST_MANUAL_AVERAGE,
		IDC_LEFT_THD, IDH_DST_MANUAL_LEFT_THD,
		IDC_RIGHT_THD, IDH_DST_MANUAL_RIGHT_THD,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

void CDstManual::OnSize(UINT nType, int cx, int cy)
{
	CDialogExt::OnSize(nType, cx, cy);

	if (m_bInitialized) {
		m_cGraph.Resize();
		Redraw();
	}
}
