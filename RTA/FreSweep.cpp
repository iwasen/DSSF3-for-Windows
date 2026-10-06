// FreSweep.cpp : 実装ファイル
//

#include "stdafx.h"
#include "RTA.h"
#include "FreSweep.h"
#include "FreDlg.h"
#include "spline.h"
#include "MakeFilter.h"
#include "Help\ContextHelp.h"

#define MIN_LEVEL_VAL	-60
#define MAX_LEVEL_VAL	0

#define MIN_LEVEL_POS 0
#define MAX_LEVEL_POS (-(MIN_LEVEL_VAL))

#define RESOLUTION	10

// CFreSweep ダイアログ

CFreSweep::CFreSweep(CWnd* pParent /*=NULL*/)
	: CDialogExt(CFreSweep::IDD, pParent)
{
	m_pWaveLeft = NULL;
	m_pWaveRight = NULL;
	m_pLeftData = NULL;
	m_pRightData = NULL;
	m_pFreq = NULL;
	m_bValidData = FALSE;
	m_bInitialized = FALSE;
}

CFreSweep::~CFreSweep()
{
	DelDataHold(FALSE);
	FreeBuffers();
}

void CFreSweep::DoDataExchange(CDataExchange* pDX)
{
	CDialogExt::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRAPH, m_cGraph);
	DDX_Control(pDX, IDC_FREQ_START, m_cFreqStart);
	DDX_Control(pDX, IDC_FREQ_END, m_cFreqEnd);
	DDX_Control(pDX, IDC_SWEEP_TIME, m_cSweepTime);
	DDX_Control(pDX, IDC_FREQ_CURRENT, m_cFreqCurrent);
	DDX_Control(pDX, IDC_LEVEL_SLIDER, m_cLevelSlider);
	DDX_Control(pDX, IDC_LEVEL_EDIT, m_cLevelEdit);
	DDX_Control(pDX, IDC_SWEEP_TIME_SPIN, m_cSweepTimeSpin);
}

BEGIN_MESSAGE_MAP(CFreSweep, CDialogExt)
	ON_EN_CHANGE(IDC_FREQ_START, &CFreSweep::OnEnChangeFreqStart)
	ON_EN_CHANGE(IDC_FREQ_END, &CFreSweep::OnEnChangeFreqEnd)
	ON_EN_CHANGE(IDC_SWEEP_TIME, &CFreSweep::OnEnChangeSweepTime)
	ON_EN_CHANGE(IDC_LEVEL_EDIT, &CFreSweep::OnEnChangeLevelEdit)
	ON_WM_HSCROLL()
	ON_WM_HELPINFO()
	ON_MESSAGE(WM_TAB_INIT_DIALOG, OnTabInitDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CFreSweep メッセージ ハンドラ

BOOL CFreSweep::OnInitDialog()
{
	CDialogExt::OnInitDialog();

	m_cGraph.Initialize(0, g_oSetData.Fre.nSweepFreqStart, g_oSetData.Fre.nSweepFreqEnd);

	InitLevelSlider();

	m_cFreqStart = g_oSetData.Fre.nSweepFreqStart;
	m_cFreqEnd = g_oSetData.Fre.nSweepFreqEnd;
	m_cSweepTime = g_oSetData.Fre.nSweepTime;
	m_cLevelEdit = -g_oSetData.Fre.nSweepLevel;

	m_cFreqStart.SetValidChar(VC_NUM);
	m_cFreqEnd.SetValidChar(VC_NUM);
	m_cSweepTime.SetValidChar(VC_NUM);
	m_cLevelEdit.SetValidChar(VC_NUM);

	m_cSweepTimeSpin.SetRange(1, 999);

	return TRUE;
}

LRESULT CFreSweep::OnTabInitDialog(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	SaveWindowSize();

	CWnd *pWnd = GetTopWindow();
	while (pWnd != NULL) {
		switch (pWnd->GetDlgCtrlID()) {
		case IDC_GRAPH:
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

void CFreSweep::OnEnChangeFreqStart()
{
	int nFreqStart = m_cFreqStart;

	if (nFreqStart <= 1)
		nFreqStart = 1;

	if (nFreqStart != g_oSetData.Fre.nSweepFreqStart) {
		g_oSetData.Fre.nSweepFreqStart = nFreqStart;
		Redraw();
	}
}

void CFreSweep::OnEnChangeFreqEnd()
{
	int nFreqEnd = m_cFreqEnd;

	if (nFreqEnd <= 1)
		nFreqEnd = 1;

	if (nFreqEnd != g_oSetData.Fre.nSweepFreqEnd) {
		g_oSetData.Fre.nSweepFreqEnd = nFreqEnd;
		Redraw();
	}
}

void CFreSweep::OnEnChangeSweepTime()
{
	if (m_cSweepTime.m_hWnd) {
		int nSweepTime = m_cSweepTime;

		if (nSweepTime <= 1)
			nSweepTime = 1;

		g_oSetData.Fre.nSweepTime = nSweepTime;
	}
}

void CFreSweep::OnEnChangeLevelEdit()
{
	SetLevel(-(int)m_cLevelEdit);
}

void CFreSweep::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (pScrollBar->GetDlgCtrlID()) {
	case IDC_LEVEL_SLIDER:
		nPos = ((CSliderCtrl *)pScrollBar)->GetPos();
		SetLevel((int)nPos - MAX_LEVEL_POS);
		break;
	}

	CDialogExt::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CFreSweep::InitLevelSlider()
{
	m_cLevelSlider.SetRange(MIN_LEVEL_POS, MAX_LEVEL_POS);

	for (int nLevel = MIN_LEVEL_POS; nLevel <= MAX_LEVEL_POS; nLevel += 10)
		m_cLevelSlider.SetTic(nLevel);
}

void CFreSweep::SetLevel(int nLevel)
{
	if (nLevel < MIN_LEVEL_VAL)
		nLevel = MIN_LEVEL_VAL;
	else if (nLevel > MAX_LEVEL_VAL)
		nLevel = MAX_LEVEL_VAL;

	m_cLevelEdit = -nLevel;
	m_cLevelSlider.SetPos(nLevel + MAX_LEVEL_POS);
	g_oSetData.Fre.nSweepLevel = nLevel;
}

int CFreSweep::Initialize()
{
	FreeBuffers();

	m_cFreqCurrent.Blank();
	Redraw();

	m_bValidData = TRUE;
	m_nSamplingRate = g_oSetData.Fre.nSamplingRate;
	m_nChannel = g_oSetData.Fre.nChannel + 1;
	m_fFreqCurrent = g_oSetData.Fre.nSweepFreqStart;
	m_fFreqStep = pow((double)g_oSetData.Fre.nSweepFreqEnd / g_oSetData.Fre.nSweepFreqStart, 1.0 / (g_oSetData.Fre.nSweepTime * RESOLUTION));
	m_nFreqPoint = g_oSetData.Fre.nSweepTime * RESOLUTION + 1;
	m_nFreqCount = 0;
	m_nWaveBufSize = g_oSetData.Fre.nSamplingRate / RESOLUTION;
	m_fSweepAngle = 0;
	m_fSweepStep = pow((double)g_oSetData.Fre.nSweepFreqEnd / g_oSetData.Fre.nSweepFreqStart, 1.0 / (g_oSetData.Fre.nSamplingRate * g_oSetData.Fre.nSweepTime));
	m_fSweepFreq = g_oSetData.Fre.nSweepFreqStart / sqrt(m_fFreqStep);
	m_fSweepEnd = g_oSetData.Fre.nSweepFreqEnd * sqrt(m_fFreqStep);
	m_fLevel = pow(10.0, g_oSetData.Fre.nSweepLevel / 20.0);

	m_pWaveLeft = new double[m_nWaveBufSize];
	m_pLeftData = new double[m_nFreqPoint];
	if (g_oSetData.Fre.nChannel == 1) {
		m_pWaveRight = new double[m_nWaveBufSize];
		m_pRightData = new double[m_nFreqPoint];
	}

	m_pFreq = new double[m_nFreqPoint];

	m_nInStartDelay = 6;
	m_nOutStartDelay = 6;

	return m_nWaveBufSize;
}

void CFreSweep::Redraw()
{
	if (!m_bValidData) {
		m_nFreqStart = g_oSetData.Fre.nSweepFreqStart;
		m_nFreqEnd = g_oSetData.Fre.nSweepFreqEnd;
		m_nFreqCount = 0;
	}

	m_cGraph.SetBitmap(m_nFreqStart, m_nFreqEnd);

	CFreHoldData oHoldData;
	for (POSITION pos = m_oHoldDataList.GetHeadPosition(); pos != NULL; m_oHoldDataList.GetNext(pos)) {
		oHoldData = m_oHoldDataList.GetAt(pos);
		m_cGraph.DispGraph(oHoldData.m_pLeftData, oHoldData.m_pRightData, oHoldData.m_pFreq, oHoldData.m_nFreqCount, m_nFreqStart, m_nFreqEnd, 0, FALSE, oHoldData.m_colorLeft, oHoldData.m_colorRight);
	}

	m_cGraph.DispGraph(m_pLeftData, m_pRightData, m_pFreq, m_nFreqCount, m_nFreqStart, m_nFreqEnd, 0, FALSE);
}

void CFreSweep::FreeBuffers()
{
	if (m_pWaveLeft != NULL) {
		delete [] m_pWaveLeft;
		m_pWaveLeft = NULL;
	}

	if (m_pWaveRight != NULL) {
		delete [] m_pWaveRight;
		m_pWaveRight = NULL;
	}

	if (m_pLeftData != NULL) {
		delete [] m_pLeftData;
		m_pLeftData = NULL;
	}

	if (m_pRightData != NULL) {
		delete [] m_pRightData;
		m_pRightData = NULL;
	}

	if (m_pFreq != NULL) {
		delete [] m_pFreq;
		m_pFreq = NULL;
	}

	m_bValidData = FALSE;
}

BOOL CFreSweep::WaveOutData(double *pData, int nBitsPerSample)
{
	if (m_nOutStartDelay > 0) {
		m_nOutStartDelay--;
		memset(pData, 0, sizeof(double) * m_nWaveBufSize);
		return FALSE;
	}

	for (int i = 0; i < m_nWaveBufSize; i++) {
		if (m_fSweepFreq <= m_fSweepEnd) {
			*pData++ = sin(2 * M_PI * m_fSweepAngle) * m_fLevel + GetDither(nBitsPerSample);

			const double fAngleStep = m_fSweepFreq / m_nSamplingRate;
			m_fSweepAngle += fAngleStep;
			while (m_fSweepAngle >= 1)
				m_fSweepAngle -= 1;

			m_fSweepFreq *= m_fSweepStep;
		} else
			*pData++ = 0;
	}

	m_cFreqCurrent.Format("%.0f", m_fFreqCurrent);

	return FALSE;
}

BOOL CFreSweep::WaveInData(const double *pData)
{
	if (m_nInStartDelay > 0) {
		m_nInStartDelay--;
		return TRUE;
	}

	CFreDlg *pDlg = (CFreDlg *)GetParent();

	for (int i = 0; i < m_nWaveBufSize; i++) {
		m_pWaveLeft[i] = *pData++;
		if (m_nChannel == 2)
			m_pWaveRight[i] = *pData++;
	}

	if (m_nFreqCount < m_nFreqPoint) {
		m_pLeftData[m_nFreqCount] = CalcFreqResponse(m_pWaveLeft, m_fFreqCurrent, pDlg->m_oMicCalDataL);
		if (m_nChannel == 2)
			m_pRightData[m_nFreqCount] = CalcFreqResponse(m_pWaveRight, m_fFreqCurrent, pDlg->m_oMicCalDataR);

		m_pFreq[m_nFreqCount++] = m_fFreqCurrent;

		Redraw();

		m_fFreqCurrent *= m_fFreqStep;
	}

	return m_nFreqCount < m_nFreqPoint;
}

double CFreSweep::CalcFreqResponse(const double *pData, double fFreq, const DbMicCalRec &oMicCalData) const
{
	int i;

	double fFilter;
	const int nFilterData = oMicCalData.nFreqData;
	if (nFilterData < 3)
		fFilter = 1;
	else {
		CSpline spline;
		FilterData *pFilterData = (FilterData *)oMicCalData.aFreq;
		double *pFreq = new double[nFilterData];
		double *pLevel = new double[nFilterData];

		for (i = 0; i < nFilterData; i++) {
			pFreq[i] = log(pFilterData->fFreq);
			pLevel[i] = pFilterData->fLevel;
			pFilterData++;
		}

		spline.MakeTable(pFreq, pLevel, nFilterData);

		fFilter = pow(10.0, spline.Spline(log(fFreq)) / 20);

		delete [] pFreq;
		delete [] pLevel;
	}

	fFilter = pow(10.0, -oMicCalData.fInputSens / 20) / fFilter;

	double fOffset = 0;
	for (i = 0; i < m_nWaveBufSize; i++)
		fOffset += pData[i];
	fOffset /= m_nWaveBufSize;

	double fSum = 0;
	for (i = 0; i < m_nWaveBufSize; i++) {
		const double fData = (*pData++ - fOffset) * fFilter;
		fSum += fData * fData;
	}

	return fSum / m_nWaveBufSize;
}

BOOL CFreSweep::CheckDataExist() const
{
	return m_bValidData && m_nFreqCount != 0;
}

void CFreSweep::CsvOutput(LPCSTR pFileName)
{
	CCsvFile oCsvFile;

	if (oCsvFile.Open(pFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
		m_cGraph.CsvOutput(oCsvFile, m_pLeftData, m_pRightData, m_pFreq, m_nFreqCount, "Sweep");
}

void CFreSweep::ReadCsv(CCsvFile &oCsvFile)
{
	if (!oCsvFile.Input() || oCsvFile.GetString(0) != "Frequency[Hz]") {
		::AfxMessageBox(IDS_ERR_CSV_FORMAT, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	CString sItem = oCsvFile.GetString(1);
	if (sItem == "Level[dB]") {
		m_nChannel = 1;
	} else if (sItem == "Level-L[dB]") {
		m_nChannel = 2;
	} else {
		::AfxMessageBox(IDS_ERR_CSV_FORMAT, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	struct SCsvData {
		double fFreq;
		double fLeftData;
		double fRightData;
	};
	CList<SCsvData, SCsvData&> oCsvDataList;

	while (oCsvFile.Input()) {
		SCsvData oCsvData{
			.fFreq = oCsvFile.GetDouble(0),
			.fLeftData = oCsvFile.GetDouble(1),
			.fRightData = oCsvFile.GetDouble(2)
		};
		oCsvDataList.AddTail(oCsvData);
	}

	m_nFreqCount = (int)oCsvDataList.GetCount();
	m_nFreqStart = g_oSetData.Fre.nNoiseFreqStart;
	m_nFreqEnd = g_oSetData.Fre.nNoiseFreqEnd;

	double *pFreq, *pLeftData, *pRightData = NULL;

	FreeBuffers();

	m_pLeftData = new double[m_nFreqCount];
	pLeftData = m_pLeftData;

	if (m_nChannel == 2) {
		m_pRightData = new double[m_nFreqCount];
		pRightData = m_pRightData;
	}

	m_pFreq = new double[m_nFreqCount];
	pFreq = m_pFreq;

	while (oCsvDataList.GetCount() != 0) {
		SCsvData oCsvData = oCsvDataList.RemoveHead();
		*pFreq++ = oCsvData.fFreq;
		*pLeftData++ = pow(10.0, oCsvData.fLeftData / 10);
		if (m_nChannel == 2) {
			*pRightData++ = pow(10.0, oCsvData.fRightData / 10);
		}
	}

	m_bValidData = TRUE;
	Redraw();
}

HBITMAP CFreSweep::GetBitmap()
{
	return m_cGraph.GetBitmap();
}

BOOL CFreSweep::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDC_GRAPH, IDH_FRE_SWEEP_GRAPH,
		IDC_FREQ_START, IDH_FRE_SWEEP_FREQ_START,
		IDC_FREQ_END, IDH_FRE_SWEEP_FREQ_END,
		IDC_SWEEP_TIME, IDH_FRE_SWEEP_TIME,
		IDC_FREQ_CURRENT, IDH_FRE_SWEEP_FREQ_CURRENT,
		IDC_LEVEL_SLIDER, IDH_FRE_SWEEP_LEVEL_SLIDER,
		IDC_LEVEL_EDIT, IDH_FRE_SWEEP_LEVEL_EDIT,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

void CFreSweep::OnSize(UINT nType, int cx, int cy)
{
	CDialogExt::OnSize(nType, cx, cy);

	if (m_bInitialized) {
		m_cGraph.Resize();
		Redraw();
	}
}

void CFreSweep::AddDataHold(COLORREF colorLeft, COLORREF colorRight)
{
	CFreHoldData oHoldData;
	oHoldData.Set(colorLeft, colorRight, m_nFreqPoint, m_nFreqCount,
			m_nFreqPoint, m_nFreqPoint, m_nFreqPoint,
			m_pLeftData, m_pRightData, m_pFreq);

	m_oHoldDataList.AddTail(oHoldData);

	m_bValidData = FALSE;
	Redraw();
}

void CFreSweep::DelDataHold(BOOL bRedraw)
{
	while (!m_oHoldDataList.IsEmpty()) {
		CFreHoldData oHoldData = m_oHoldDataList.RemoveTail();

		oHoldData.FreeBuffers();
	}

	if (bRedraw)
		Redraw();
}

BOOL CFreSweep::CheckDataHold()
{
	return !m_oHoldDataList.IsEmpty();
}

void CFreSweep::SaveHoldData(CFile &oFile)
{
	const INT32 count = (INT32)m_oHoldDataList.GetCount();

	oFile.Write(&count, sizeof(count));

	while (!m_oHoldDataList.IsEmpty()) {
		CFreHoldData oHoldData = m_oHoldDataList.RemoveHead();

		oHoldData.Save(oFile);
		oHoldData.FreeBuffers();
	}
}

void CFreSweep::LoadHoldData(CFile &oFile)
{
	INT32 count;
	oFile.Read(&count, sizeof(count));

	for (int i = 0; i < count; i++) {
		CFreHoldData oHoldData;
		oHoldData.Load(oFile);
		m_oHoldDataList.AddTail(oHoldData);
	}

	m_bValidData = FALSE;
	Redraw();
}
