// DstFreq.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "DstFreq.h"
#include "Help\ContextHelp.h"

#define MIN_LEVEL_VAL	-60
#define MAX_LEVEL_VAL	0

#define MIN_LEVEL_POS 0
#define MAX_LEVEL_POS (-(MIN_LEVEL_VAL))

// CDstFreq ダイアログ

CDstFreq::CDstFreq(CWnd* pParent /*=NULL*/)
	: CDialogExt(CDstFreq::IDD, pParent)
{
	memset(m_pLeftDst, 0, sizeof(m_pLeftDst));
	memset(m_pRightDst, 0, sizeof(m_pRightDst));
	m_pFreq = NULL;
	m_bValidData = FALSE;
	m_bInitialized = FALSE;
}

CDstFreq::~CDstFreq()
{
	DelDataHold(FALSE);
	FreeBuffers();
}

void CDstFreq::DoDataExchange(CDataExchange* pDX)
{
	CDialogExt::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LEVEL_SLIDER, m_cLevelSlider);
	DDX_Control(pDX, IDC_LEVEL_EDIT, m_cLevelEdit);
	DDX_Control(pDX, IDC_LEFT_THD, m_cLeftTHD);
	DDX_Control(pDX, IDC_RIGHT_THD, m_cRightTHD);
	DDX_Control(pDX, IDC_HARM_GRAPH, m_cGraph);
	DDX_Control(pDX, IDC_LEFT_UNIT, m_cLeftUnit);
	DDX_Control(pDX, IDC_RIGHT_UNIT, m_cRightUnit);
	DDX_Control(pDX, IDC_FREQ_START, m_cFreqStart);
	DDX_Control(pDX, IDC_FREQ_END, m_cFreqEnd);
	DDX_Control(pDX, IDC_FREQ_POINT, m_cFreqPoint);
	DDX_Control(pDX, IDC_FREQ_CURRENT, m_cFreqCurrent);
	DDX_Control(pDX, IDC_FREQ_SPLINE, m_cFreqSpline);
	DDX_Control(pDX, IDC_CHK_MARKER, m_cChkMarker);
	DDX_Control(pDX, IDC_COMBO_HD, m_cComboHD);
}

BEGIN_MESSAGE_MAP(CDstFreq, CDialogExt)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_LEVEL_EDIT, OnEnChangeLevelEdit)
	ON_EN_CHANGE(IDC_FREQ_START, OnEnChangeFreqStart)
	ON_EN_CHANGE(IDC_FREQ_END, OnEnChangeFreqEnd)
	ON_EN_CHANGE(IDC_FREQ_POINT, OnEnChangeFreqPoint)
	ON_BN_CLICKED(IDC_FREQ_SPLINE, OnBnClickedFreqSpline)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_CHK_MARKER, &CDstFreq::OnBnClickedChkMarker)
	ON_CBN_SELCHANGE(IDC_COMBO_HD, &CDstFreq::OnCbnSelchangeComboHd)
	ON_MESSAGE(WM_TAB_INIT_DIALOG, OnTabInitDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CDstFreq メッセージ ハンドラ

BOOL CDstFreq::OnInitDialog()
{
	CDialogExt::OnInitDialog();

	m_cGraph.Initialize();

	InitLevelSlider();

	m_cFreqStart = g_oSetData.Dst.nFreqStart;
	m_cFreqEnd = g_oSetData.Dst.nFreqEnd;
	m_cFreqPoint = g_oSetData.Dst.nFreqPoint;
	m_cLevelEdit = -g_oSetData.Dst.nFreqLevel;

	m_cFreqSpline = g_oSetData.Dst.bFreqSpline;
	m_cChkMarker = g_oSetData.Dst.bFreqMarker;

	m_cComboHD.AddString("THD");
	m_cComboHD.AddString("2ndHD");
	m_cComboHD.AddString("3rdHD");
	m_cComboHD.SetCurSel(g_oSetData.Dst.nFreqHD);

	m_cFreqStart.SetValidChar(VC_NUM);
	m_cFreqEnd.SetValidChar(VC_NUM);
	m_cFreqPoint.SetValidChar(VC_NUM);
	m_cLevelEdit.SetValidChar(VC_NUM);

	DispUnit();

	return TRUE;
}

LRESULT CDstFreq::OnTabInitDialog(WPARAM /*wParam*/, LPARAM /*lParam*/)
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

void CDstFreq::InitLevelSlider()
{
	int nLevel;

	m_cLevelSlider.SetRange(MIN_LEVEL_POS, MAX_LEVEL_POS);

	for (nLevel = MIN_LEVEL_POS; nLevel <= MAX_LEVEL_POS; nLevel += 10)
		m_cLevelSlider.SetTic(nLevel);
}

void CDstFreq::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (pScrollBar->GetDlgCtrlID()) {
	case IDC_LEVEL_SLIDER:
		nPos = ((CSliderCtrl *)pScrollBar)->GetPos();
		SetLevel((int)nPos - MAX_LEVEL_POS);
		break;
	}

	CDialogExt::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDstFreq::SetLevel(int nLevel)
{
	if (nLevel < MIN_LEVEL_VAL)
		nLevel = MIN_LEVEL_VAL;
	else if (nLevel > MAX_LEVEL_VAL)
		nLevel = MAX_LEVEL_VAL;

	m_cLevelEdit = -nLevel;
	m_cLevelSlider.SetPos(nLevel + MAX_LEVEL_POS);
	g_oSetData.Dst.nFreqLevel = nLevel;
}

void CDstFreq::OnEnChangeLevelEdit()
{
	SetLevel(-(int)m_cLevelEdit);
}

void CDstFreq::Initialize()
{
	FreeBuffers();

	m_cFreqCurrent.Blank();
	m_cLeftTHD.Blank();
	m_cRightTHD.Blank();
	Redraw();

	m_bValidData = TRUE;
	m_fFreqCurrent = g_oSetData.Dst.nFreqStart;
	m_fFreqStep = pow((double)g_oSetData.Dst.nFreqEnd / g_oSetData.Dst.nFreqStart, 1.0 / (g_oSetData.Dst.nFreqPoint - 1));
	m_nFreqPoint = g_oSetData.Dst.nFreqPoint;
	m_nFreqCount = 0;

	for (int i = 0; i < 3; i++) {
		m_pLeftDst[i] = new double[m_nFreqPoint];
		if (g_oSetData.Dst.nChannel == 1)
			m_pRightDst[i] = new double[m_nFreqPoint];
	}

	m_pFreq = new double[m_nFreqPoint];
}

BOOL CDstFreq::WaveOutData(int &nFreq, double &fLevel)
{
	nFreq = (int)(m_fFreqCurrent + 0.5);
	fLevel = g_oSetData.Dst.nFreqLevel;

	m_cFreqCurrent = nFreq;

	return TRUE;
}

BOOL CDstFreq::NotifyTHD(const double *pLeftDst, const double *pRightDst)
{
	for (int i = 0; i < 3; i++) {
		m_pLeftDst[i][m_nFreqCount] = pLeftDst[i];
		if (g_oSetData.Dst.nChannel == 1)
			m_pRightDst[i][m_nFreqCount] = pRightDst[i];
	}

	m_pFreq[m_nFreqCount] = m_fFreqCurrent;
	++m_nFreqCount;

	DispTHD(pLeftDst[g_oSetData.Dst.nFreqHD], pRightDst[g_oSetData.Dst.nFreqHD]);
	Redraw();

	if (m_nFreqCount < m_nFreqPoint) {
		m_fFreqCurrent *= m_fFreqStep;
		return TRUE;
	} else
		return FALSE;
}

void CDstFreq::Redraw()
{
	if (!m_bValidData) {
		m_nFreqStart = g_oSetData.Dst.nFreqStart;
		m_nFreqEnd = g_oSetData.Dst.nFreqEnd;
		m_nFreqCount = 0;
		m_nFreqPoint = 0;
	}

	DispUnit();

	m_cGraph.SetBitmap(m_nFreqStart, m_nFreqEnd);

	CDstHoldData oHoldData;
	POSITION pos;
	for (pos = m_oHoldDataList.GetHeadPosition(); pos != NULL; m_oHoldDataList.GetNext(pos)) {
		oHoldData = m_oHoldDataList.GetAt(pos);
		m_cGraph.DispGraph(oHoldData.m_pLeftData[g_oSetData.Dst.nFreqHD], oHoldData.m_pRightData[g_oSetData.Dst.nFreqHD], oHoldData.m_pValue, oHoldData.m_nCount, m_nFreqStart, m_nFreqEnd, oHoldData.m_nPoint, FALSE, oHoldData.m_colorLeft, oHoldData.m_colorRight);
	}

	if (m_bValidData)
		m_cGraph.DispGraph(m_pLeftDst[g_oSetData.Dst.nFreqHD], m_pRightDst[g_oSetData.Dst.nFreqHD], m_pFreq, m_nFreqCount, m_nFreqStart, m_nFreqEnd, m_nFreqPoint, FALSE);
	else
		m_cGraph.Invalidate();
}

void CDstFreq::DispTHD(double fLeftTHD, double fRightTHD)
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

void CDstFreq::DispUnit()
{
	if (g_oSetData.Dst.nScaleMode == 0) {
		m_cLeftUnit.SetWindowText("%");
		m_cRightUnit.SetWindowText("%");
	} else {
		m_cLeftUnit.SetWindowText("dB");
		m_cRightUnit.SetWindowText("dB");
	}
}

void CDstFreq::FreeBuffers()
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

	if (m_pFreq != NULL) {
		delete [] m_pFreq;
		m_pFreq = NULL;
	}

	m_bValidData = FALSE;
}

void CDstFreq::OnEnChangeFreqStart()
{
	int nFreqStart = m_cFreqStart;

	if (nFreqStart <= 1)
		nFreqStart = 1;

	if (nFreqStart != g_oSetData.Dst.nFreqStart) {
		g_oSetData.Dst.nFreqStart = nFreqStart;
		Redraw();
	}
}

void CDstFreq::OnEnChangeFreqEnd()
{
	int nFreqEnd = m_cFreqEnd;

	if (nFreqEnd <= 1)
		nFreqEnd = 1;

	if (nFreqEnd != g_oSetData.Dst.nFreqEnd) {
		g_oSetData.Dst.nFreqEnd = nFreqEnd;
		Redraw();
	}
}

void CDstFreq::OnEnChangeFreqPoint()
{
	int nFreqPoint = m_cFreqPoint;

	if (nFreqPoint <= 1)
		nFreqPoint = 1;

	g_oSetData.Dst.nFreqPoint = nFreqPoint;
}

void CDstFreq::OnBnClickedFreqSpline()
{
	if (g_oSetData.Dst.bFreqSpline != (int)m_cFreqSpline) {
		g_oSetData.Dst.bFreqSpline = m_cFreqSpline;
		Redraw();
	}
}

void CDstFreq::OnOK()
{
}

void CDstFreq::OnCancel()
{
}

void CDstFreq::CsvOutput(LPCTSTR pFileName)
{
	CCsvFile oCsvFile;

	if (oCsvFile.Open(pFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
		m_cGraph.CsvOutput(oCsvFile, m_pLeftDst[g_oSetData.Dst.nFreqHD], m_pRightDst[g_oSetData.Dst.nFreqHD], m_pFreq, m_nFreqCount);
}

BOOL CDstFreq::CheckDataExist()
{
	return m_bValidData && m_nFreqCount != 0;
}

HBITMAP CDstFreq::GetBitmap()
{
	return m_cGraph.GetBitmap();
}

BOOL CDstFreq::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_HARM_GRAPH, IDH_DST_FREQ_HARM_GRAPH,
		IDC_FREQ_START, IDH_DST_FREQ_START,
		IDC_FREQ_END, IDH_DST_FREQ_END,
		IDC_FREQ_POINT, IDH_DST_FREQ_POINT,
		IDC_FREQ_CURRENT, IDH_DST_FREQ_CURRENT,
		IDC_LEVEL_SLIDER, IDH_DST_FREQ_LEVEL_SLIDER,
		IDC_LEVEL_EDIT, IDH_DST_FREQ_LEVEL_EDIT,
		IDC_FREQ_SPLINE, IDH_DST_FREQ_SPLINE,
		IDC_CHK_MARKER, IDH_DST_FREQ_MARKER,
		IDC_LEFT_THD, IDH_DST_FREQ_LEFT_THD,
		IDC_RIGHT_THD, IDH_DST_FREQ_RIGHT_THD,
		IDC_COMBO_HD, IDH_DST_FREQ_HD,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

void CDstFreq::OnBnClickedChkMarker()
{
	g_oSetData.Dst.bFreqMarker = m_cChkMarker;
	Redraw();
}

void CDstFreq::OnCbnSelchangeComboHd()
{
	g_oSetData.Dst.nFreqHD = m_cComboHD.GetCurSel();
	Redraw();
}

void CDstFreq::OnSize(UINT nType, int cx, int cy)
{
	CDialogExt::OnSize(nType, cx, cy);

	if (m_bInitialized) {
		m_cGraph.Resize();
		Redraw();
	}
}

void CDstFreq::AddDataHold(COLORREF colorLeft, COLORREF colorRight)
{
	CDstHoldData oHoldData;
	oHoldData.Set(colorLeft, colorRight, m_nFreqPoint, m_nFreqCount, m_pLeftDst, m_pRightDst, m_pFreq);

	m_oHoldDataList.AddTail(oHoldData);

	m_bValidData = FALSE;
	Redraw();
}

void CDstFreq::DelDataHold(BOOL bRedraw)
{
	while (!m_oHoldDataList.IsEmpty()) {
		CDstHoldData oHoldData = m_oHoldDataList.RemoveTail();

		oHoldData.FreeBuffers();
	}

	if (bRedraw)
		Redraw();
}

BOOL CDstFreq::CheckDataHold()
{
	return !m_oHoldDataList.IsEmpty();
}

void CDstFreq::SaveHoldData(CFile &oFile)
{
	INT32 count = (INT32)m_oHoldDataList.GetCount();

	oFile.Write(&count, sizeof(count));

	while (!m_oHoldDataList.IsEmpty()) {
		CDstHoldData oHoldData = m_oHoldDataList.RemoveHead();

		oHoldData.Save(oFile);
		oHoldData.FreeBuffers();
	}
}

void CDstFreq::LoadHoldData(CFile &oFile)
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
