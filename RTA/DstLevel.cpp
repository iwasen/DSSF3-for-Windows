// DstFreq.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "DstLevel.h"
#include "Help\ContextHelp.h"

#define MIN_FREQ_VAL	10
#define MAX_FREQ_VAL	10000

#define MIN_FREQ_POS 0
#define MAX_FREQ_POS 100

// CDstLevel ダイアログ

CDstLevel::CDstLevel(CWnd* pParent /*=NULL*/)
	: CDialogExt(CDstLevel::IDD, pParent)
{
	memset(m_pLeftDst, 0, sizeof(m_pLeftDst));
	memset(m_pRightDst, 0, sizeof(m_pRightDst));
	m_pLevel = NULL;
	m_bValidData = FALSE;
	m_bInitialized = FALSE;
}

CDstLevel::~CDstLevel()
{
	DelDataHold(FALSE);
	FreeBuffers();
}

void CDstLevel::DoDataExchange(CDataExchange* pDX)
{
	CDialogExt::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FREQ_SLIDER, m_cFreqSlider);
	DDX_Control(pDX, IDC_FREQ_EDIT, m_cFreqEdit);
	DDX_Control(pDX, IDC_LEFT_THD, m_cLeftTHD);
	DDX_Control(pDX, IDC_RIGHT_THD, m_cRightTHD);
	DDX_Control(pDX, IDC_HARM_GRAPH, m_cGraph);
	DDX_Control(pDX, IDC_LEFT_UNIT, m_cLeftUnit);
	DDX_Control(pDX, IDC_RIGHT_UNIT, m_cRightUnit);
	DDX_Control(pDX, IDC_LEVEL_START, m_cLevelStart);
	DDX_Control(pDX, IDC_LEVEL_END, m_cLevelEnd);
	DDX_Control(pDX, IDC_LEVEL_POINT, m_cLevelPoint);
	DDX_Control(pDX, IDC_LEVEL_CURRENT, m_cLevelCurrent);
	DDX_Control(pDX, IDC_LEVEL_SPLINE, m_cLevelSpline);
	DDX_Control(pDX, IDC_CHK_MARKER, m_cChkMarker);
	DDX_Control(pDX, IDC_COMBO_HD, m_cComboHD);
}

BEGIN_MESSAGE_MAP(CDstLevel, CDialogExt)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_FREQ_EDIT, OnEnChangeFreqEdit)
	ON_EN_CHANGE(IDC_LEVEL_START, OnEnChangeLevelStart)
	ON_EN_CHANGE(IDC_LEVEL_END, OnEnChangeLevelEnd)
	ON_EN_CHANGE(IDC_LEVEL_POINT, OnEnChangeLevelPoint)
	ON_BN_CLICKED(IDC_LEVEL_SPLINE, OnBnClickedLevelSpline)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_CHK_MARKER, &CDstLevel::OnBnClickedChkMarker)
	ON_CBN_SELCHANGE(IDC_COMBO_HD, &CDstLevel::OnCbnSelchangeComboHd)
	ON_MESSAGE(WM_TAB_INIT_DIALOG, OnTabInitDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CDstLevel メッセージ ハンドラ

BOOL CDstLevel::OnInitDialog()
{
	CDialogExt::OnInitDialog();

	m_cGraph.Initialize();

	InitFreqSlider();

	m_cLevelStart = -g_oSetData.Dst.nLevelStart;
	m_cLevelEnd = -g_oSetData.Dst.nLevelEnd;
	m_cLevelPoint = g_oSetData.Dst.nLevelPoint;
	m_cFreqEdit = g_oSetData.Dst.nLevelFreq;

	m_cLevelSpline = g_oSetData.Dst.bLevelSpline;
	m_cChkMarker = g_oSetData.Dst.bLevelMarker;

	m_cComboHD.AddString("THD");
	m_cComboHD.AddString("2ndHD");
	m_cComboHD.AddString("3rdHD");
	m_cComboHD.SetCurSel(g_oSetData.Dst.nLevelHD);

	m_cLevelStart.SetValidChar(VC_NUM);
	m_cLevelEnd.SetValidChar(VC_NUM);
	m_cLevelPoint.SetValidChar(VC_NUM);
	m_cFreqEdit.SetValidChar(VC_NUM);

	DispUnit();

	return TRUE;
}

LRESULT CDstLevel::OnTabInitDialog(WPARAM /*wParam*/, LPARAM /*lParam*/)
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

void CDstLevel::InitFreqSlider()
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

void CDstLevel::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (pScrollBar->GetDlgCtrlID()) {
	case IDC_FREQ_SLIDER:
		nPos = ((CSliderCtrl *)pScrollBar)->GetPos();
		SetFreq(GetFreqFromPos(nPos));
		break;
	}

	CDialogExt::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDstLevel::SetFreq(int nFreq)
{
//	if (nFreq < MIN_FREQ_VAL)
//		nFreq = MIN_FREQ_VAL;
//	else if (nFreq > MAX_FREQ_VAL)
//		nFreq = MAX_FREQ_VAL;

	m_cFreqEdit = nFreq;
	m_cFreqSlider.SetPos(GetPosFromFreq(nFreq));
	g_oSetData.Dst.nLevelFreq = nFreq;
}

int CDstLevel::GetPosFromFreq(int nFreq)
{
	if (nFreq < MIN_FREQ_VAL)
		nFreq = MIN_FREQ_VAL;
	else if (nFreq > MAX_FREQ_VAL)
		nFreq = MAX_FREQ_VAL;

	return (int)((log((double)nFreq) - log((double)MIN_FREQ_VAL)) * MAX_FREQ_POS / (log((double)MAX_FREQ_VAL) - log((double)MIN_FREQ_VAL)) + 0.5);
}

int CDstLevel::GetFreqFromPos(int nPos)
{
	if (nPos < MIN_FREQ_POS)
		nPos = MIN_FREQ_POS;
	else if (nPos > MAX_FREQ_POS)
		nPos = MAX_FREQ_POS;

	return (int)(exp(log((double)MIN_FREQ_VAL) + nPos * (log((double)MAX_FREQ_VAL) - log((double)MIN_FREQ_VAL)) / MAX_FREQ_POS) + 0.5);
}

void CDstLevel::OnEnChangeFreqEdit()
{
	SetFreq(m_cFreqEdit);
}

void CDstLevel::Initialize()
{
	FreeBuffers();

	m_cLevelCurrent.Blank();
	Redraw();

	m_bValidData = TRUE;
	m_fLevelCurrent = g_oSetData.Dst.nLevelStart;
	m_fLevelStep = (double)(g_oSetData.Dst.nLevelEnd - g_oSetData.Dst.nLevelStart) / (g_oSetData.Dst.nLevelPoint - 1);
	m_nLevelPoint = g_oSetData.Dst.nLevelPoint;
	m_nLevelCount = 0;

	for (int i = 0; i < 3; i++) {
		m_pLeftDst[i] = new double[m_nLevelPoint];
		if (g_oSetData.Dst.nChannel == 1)
			m_pRightDst[i] = new double[m_nLevelPoint];
	}

	m_pLevel = new double[m_nLevelPoint];
}

BOOL CDstLevel::WaveOutData(int &nFreq, double &fLevel)
{
	nFreq = g_oSetData.Dst.nLevelFreq;
	fLevel = m_fLevelCurrent;

	m_cLevelCurrent.Format("%.1f", m_fLevelCurrent);

	return TRUE;
}

BOOL CDstLevel::NotifyTHD(const double *pLeftDst, const double *pRightDst)
{
	for (int i = 0; i < 3; i++) {
		m_pLeftDst[i][m_nLevelCount] = pLeftDst[i];
		if (g_oSetData.Dst.nChannel == 1)
			m_pRightDst[i][m_nLevelCount] = pRightDst[i];
	}

	m_pLevel[m_nLevelCount] = m_fLevelCurrent;
	++m_nLevelCount;

	DispTHD(pLeftDst[g_oSetData.Dst.nLevelHD], pRightDst[g_oSetData.Dst.nLevelHD]);
	Redraw();

	if (m_nLevelCount < m_nLevelPoint) {
		m_fLevelCurrent += m_fLevelStep;
		return TRUE;
	} else
		return FALSE;
}

void CDstLevel::Redraw()
{
	if (!m_bValidData) {
		m_nLevelStart = g_oSetData.Dst.nLevelStart;
		m_nLevelEnd = g_oSetData.Dst.nLevelEnd;
		m_nLevelCount = 0;
		m_nLevelPoint = 0;
	}

	DispUnit();

	m_cGraph.SetBitmap(m_nLevelStart, m_nLevelEnd);

	CDstHoldData oHoldData;
	POSITION pos;
	for (pos = m_oHoldDataList.GetHeadPosition(); pos != NULL; m_oHoldDataList.GetNext(pos)) {
		oHoldData = m_oHoldDataList.GetAt(pos);
		m_cGraph.DispGraph(oHoldData.m_pLeftData[g_oSetData.Dst.nFreqHD], oHoldData.m_pRightData[g_oSetData.Dst.nFreqHD], oHoldData.m_pValue, oHoldData.m_nCount, m_nLevelStart, m_nLevelEnd, oHoldData.m_nPoint, FALSE, oHoldData.m_colorLeft, oHoldData.m_colorRight);
	}

	if (m_bValidData)
		m_cGraph.DispGraph(m_pLeftDst[g_oSetData.Dst.nLevelHD], m_pRightDst[g_oSetData.Dst.nLevelHD], m_pLevel, m_nLevelCount, m_nLevelStart, m_nLevelEnd, m_nLevelPoint, FALSE);
}

void CDstLevel::DispTHD(double fLeftTHD, double fRightTHD)
{
	if (g_oSetData.Dst.nScaleMode == 0) {
		m_cLeftTHD.Format("%.4f", sqrt(fLeftTHD) * 100);
		if (g_oSetData.Dst.nChannel == 1)
			m_cRightTHD.Format("%.4f", sqrt(fRightTHD) * 100);
	} else {
		m_cLeftTHD.Format("%.1f", dB10(fLeftTHD));
		if (g_oSetData.Dst.nChannel == 1)
			m_cRightTHD.Format("%.1f", dB10(fRightTHD));
	}
}

void CDstLevel::DispUnit()
{
	if (g_oSetData.Dst.nScaleMode == 0) {
		m_cLeftUnit.SetWindowText("%");
		m_cRightUnit.SetWindowText("%");
	} else {
		m_cLeftUnit.SetWindowText("dB");
		m_cRightUnit.SetWindowText("dB");
	}
}

void CDstLevel::FreeBuffers()
{
	for (int i = 0; i < 3; i++) {
		if (m_pLeftDst[i] != NULL) {
			delete [] m_pLeftDst[i];
			m_pLeftDst[i] = NULL;
		}

		if (m_pRightDst[i] != NULL) {
			delete [] m_pRightDst[i];
			m_pRightDst[i] = NULL;
		}
	}

	if (m_pLevel != NULL) {
		delete [] m_pLevel;
		m_pLevel = NULL;
	}

	m_bValidData = FALSE;
}

void CDstLevel::OnEnChangeLevelStart()
{
	int nLevelStart = -(int)m_cLevelStart;

	if (nLevelStart != g_oSetData.Dst.nLevelStart) {
		g_oSetData.Dst.nLevelStart = nLevelStart;
		Redraw();
	}
}

void CDstLevel::OnEnChangeLevelEnd()
{
	int nLevelEnd = -(int)m_cLevelEnd;

	if (nLevelEnd != g_oSetData.Dst.nLevelEnd) {
		g_oSetData.Dst.nLevelEnd = nLevelEnd;
		Redraw();
	}
}

void CDstLevel::OnEnChangeLevelPoint()
{
	int nLevelPoint = m_cLevelPoint;

	if (nLevelPoint <= 1)
		nLevelPoint = 1;

	g_oSetData.Dst.nLevelPoint = nLevelPoint;
}

void CDstLevel::OnBnClickedLevelSpline()
{
	if (g_oSetData.Dst.bLevelSpline != (int)m_cLevelSpline) {
		g_oSetData.Dst.bLevelSpline = m_cLevelSpline;
		Redraw();
	}
}

void CDstLevel::OnOK()
{
}

void CDstLevel::OnCancel()
{
}

void CDstLevel::CsvOutput(LPCTSTR pFileName)
{
	CCsvFile oCsvFile;

	if (oCsvFile.Open(pFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
		m_cGraph.CsvOutput(oCsvFile, m_pLeftDst[g_oSetData.Dst.nLevelHD], m_pRightDst[g_oSetData.Dst.nLevelHD], m_pLevel, m_nLevelCount);
}

BOOL CDstLevel::CheckDataExist()
{
	return m_bValidData && m_nLevelCount != 0;
}

HBITMAP CDstLevel::GetBitmap()
{
	return m_cGraph.GetBitmap();
}

BOOL CDstLevel::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_HARM_GRAPH, IDH_DST_LEVEL_HARM_GRAPH,
		IDC_FREQ_SLIDER, IDH_DST_LEVEL_FREQ_SLIDER,
		IDC_FREQ_EDIT, IDH_DST_LEVEL_FREQ_EDIT,
		IDC_LEVEL_START, IDH_DST_LEVEL_START,
		IDC_LEVEL_END, IDH_DST_LEVEL_END,
		IDC_LEVEL_POINT, IDH_DST_LEVEL_POINT,
		IDC_LEVEL_CURRENT, IDH_DST_LEVEL_CURRENT,
		IDC_LEVEL_SPLINE, IDH_DST_LEVEL_SPLINE,
		IDC_CHK_MARKER, IDH_DST_LEVEL_MARKER,
		IDC_LEFT_THD, IDH_DST_LEVEL_LEFT_THD,
		IDC_RIGHT_THD, IDH_DST_LEVEL_RIGHT_THD,
		IDC_COMBO_HD, IDH_DST_LEVEL_HD,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

void CDstLevel::OnBnClickedChkMarker()
{
	g_oSetData.Dst.bLevelMarker = m_cChkMarker;
	Redraw();
}

void CDstLevel::OnCbnSelchangeComboHd()
{
	g_oSetData.Dst.nLevelHD = m_cComboHD.GetCurSel();
	Redraw();
}

void CDstLevel::OnSize(UINT nType, int cx, int cy)
{
	CDialogExt::OnSize(nType, cx, cy);

	if (m_bInitialized) {
		m_cGraph.Resize();
		Redraw();
	}
}

void CDstLevel::AddDataHold(COLORREF colorLeft, COLORREF colorRight)
{
	CDstHoldData oHoldData;
	oHoldData.Set(colorLeft, colorRight, m_nLevelPoint, m_nLevelCount, m_pLeftDst, m_pRightDst, m_pLevel);

	m_oHoldDataList.AddTail(oHoldData);

	m_bValidData = FALSE;
	Redraw();
}

void CDstLevel::DelDataHold(BOOL bRedraw)
{
	while (!m_oHoldDataList.IsEmpty()) {
		CDstHoldData oHoldData = m_oHoldDataList.RemoveTail();

		oHoldData.FreeBuffers();
	}

	if (bRedraw)
		Redraw();
}

BOOL CDstLevel::CheckDataHold()
{
	return !m_oHoldDataList.IsEmpty();
}

void CDstLevel::SaveHoldData(CFile &oFile)
{
	INT32 count = (INT32)m_oHoldDataList.GetCount();

	oFile.Write(&count, sizeof(count));

	while (!m_oHoldDataList.IsEmpty()) {
		CDstHoldData oHoldData = m_oHoldDataList.RemoveHead();

		oHoldData.Save(oFile);
		oHoldData.FreeBuffers();
	}
}

void CDstLevel::LoadHoldData(CFile &oFile)
{
	INT32 count;
	oFile.Read(&count, sizeof(count));

	for (int i = 0; i < count; i++) {
		CDstHoldData oHoldData;
		oHoldData.Load(oFile);
		m_oHoldDataList.AddTail(oHoldData);
	}

	m_bValidData = FALSE;
	Redraw();
}
