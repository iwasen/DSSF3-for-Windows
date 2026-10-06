#include "stdafx.h"
#include "RTA.h"
#include "WaveEditManual.h"
#include "Help\ContextHelp.h"

#define SCROLL_RANGE	16384
#define MAX_ZOOM_X		512
#define MAX_ZOOM_Y		512

// CWaveEditManual ダイアログ

CWaveEditManual::CWaveEditManual(CWnd* pParent /*=NULL*/)
	: CDialogAF(CWaveEditManual::IDD, pParent)
	, m_iEditMode(0)
{
	m_nHScrollSize = -1;
	m_nHScrollPos = -1;
	m_nVScrollSize = -1;
	m_nVScrollPos = -1;
	m_fStartX = 0;
	m_fDispX = 0;
	m_fStartY = 0;
	m_fDispY = 0;
	m_nSelPoint = -1;
}

void CWaveEditManual::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WAVE_WND, m_cWaveEditWnd);
	DDX_Control(pDX, IDC_POS_X, m_cPosX);
	DDX_Control(pDX, IDC_POS_Y, m_cPosY);
	DDX_Control(pDX, IDC_ZOOM_H, m_cZoomH);
	DDX_Control(pDX, IDC_ZOOM_V, m_cZoomV);
	DDX_Control(pDX, IDC_SMOOTHING, m_cSmoothing);
	DDX_Control(pDX, IDC_SCROLL_V, m_cScrollV);
	DDX_Control(pDX, IDC_SCROLL_H, m_cScrollH);
	DDX_Radio(pDX, IDC_VERTEX_ADD, m_iEditMode);
	DDX_Control(pDX, IDC_BTN_TEST, m_cBtnTest);
}

BEGIN_MESSAGE_MAP(CWaveEditManual, CDialogAF)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_COMMAND(IDM_ZOOM_FULL_H, OnZoomFullH)
	ON_COMMAND(IDM_ZOOM_IN_H, OnZoomInH)
	ON_COMMAND(IDM_ZOOM_OUT_H, OnZoomOutH)
	ON_COMMAND(IDM_ZOOM_FULL_V, OnZoomFullV)
	ON_COMMAND(IDM_ZOOM_IN_V, OnZoomInV)
	ON_COMMAND(IDM_ZOOM_OUT_V, OnZoomOutV)
	ON_BN_CLICKED(IDC_VERTEX_DELETE, OnBnClickedVertexDelete)
	ON_BN_CLICKED(IDC_ALL_CLEAR, OnBnClickedAllClear)
	ON_EN_CHANGE(IDC_POS_X, OnEnChangePosX)
	ON_EN_CHANGE(IDC_POS_Y, OnEnChangePosY)
	ON_BN_CLICKED(IDC_SMOOTHING, OnBnClickedSmoothing)
	ON_BN_CLICKED(IDC_BTN_TEST, OnBnClickedTest)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

// CWaveEditManual メッセージ ハンドラ

BOOL CWaveEditManual::OnInitDialog()
{
	if (m_oWaveForm.nDataNum > MAX_WAVEVERTEX)
		InitData();

	if (m_oWaveForm.nDataNum > 2)
		m_iEditMode = 1;

	CDialogAF::OnInitDialog();

	if (m_tbZoomH.Create(this, WS_CHILD | WS_VISIBLE | CBRS_SIZE_FIXED | CBRS_ALIGN_LEFT | CBRS_TOOLTIPS, ID_ZOOM_H) && m_tbZoomH.LoadToolBar(IDR_ZOOM_H)) {
		WINDOWPLACEMENT wp;
		m_cZoomH.GetWindowPlacement(&wp);

		CSize sizeToolBar;
		sizeToolBar = m_tbZoomH.CalcFixedLayout(FALSE, FALSE);
		m_tbZoomH.MoveWindow(wp.rcNormalPosition.left - sizeToolBar.cx - 2, wp.rcNormalPosition.top - 8, sizeToolBar.cx, sizeToolBar.cy);
	}

	if (m_tbZoomV.Create(this, WS_CHILD | WS_VISIBLE | CBRS_SIZE_DYNAMIC | CBRS_ALIGN_RIGHT | CBRS_TOOLTIPS, ID_ZOOM_V) && m_tbZoomV.LoadToolBar(IDR_ZOOM_V)) {
		WINDOWPLACEMENT wp;
		m_cZoomV.GetWindowPlacement(&wp);

		CSize sizeToolBar;
		sizeToolBar = m_tbZoomV.CalcFixedLayout(FALSE, FALSE);
		m_tbZoomV.MoveWindow(wp.rcNormalPosition.left - 3, wp.rcNormalPosition.top - sizeToolBar.cy, sizeToolBar.cx, sizeToolBar.cy);
	}

	m_fTotalX = 1;
	m_fTotalY = 2;
	m_fMinY = -1;

	m_cSmoothing = m_oWaveForm.bSmoothing;

	m_cWaveEditWnd.Initialize(this);

	DispGraphWindow();

	m_cPosX.SetValidChar(VC_NUM | VC_POINT);
	m_cPosY.SetValidChar(VC_NUM | VC_POINT | VC_MINUS);

	return TRUE;
}

void CWaveEditManual::OnZoomFullH()
{
	m_fDispX = m_fTotalX;
	DispGraphWindow();
}

void CWaveEditManual::OnZoomInH()
{
	m_fDispX /= 2;
	DispGraphWindow();
}

void CWaveEditManual::OnZoomOutH()
{
	m_fDispX *= 2;
	DispGraphWindow();
}

void CWaveEditManual::OnZoomFullV()
{
	m_fDispY = m_fTotalY;
	DispGraphWindow();
}

void CWaveEditManual::OnZoomInV()
{
	m_fDispY /= 2;
	m_fStartY += m_fDispY / 2;

	DispGraphWindow();
}

void CWaveEditManual::OnZoomOutV()
{
	m_fStartY -= m_fDispY / 2;
	m_fDispY *= 2;
	DispGraphWindow();
}

void CWaveEditManual::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
{
	switch (nSBCode) {
	case SB_LEFT:
		m_fStartX = 0;
		break;
	case SB_RIGHT:
		m_fStartX = m_fTotalX - m_fDispX;
		break;
	case SB_LINELEFT:
		m_fStartX -= m_fDispX / 32;
		break;
	case SB_LINERIGHT:
		m_fStartX += m_fDispX / 32;
		break;
	case SB_PAGELEFT:
		m_fStartX -= m_fDispX;
		break;
	case SB_PAGERIGHT:
		m_fStartX += m_fDispX;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_fStartX = m_fTotalX * nPos / SCROLL_RANGE;
		break;
	}

	DispGraphWindow();
}

void CWaveEditManual::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
{
	switch (nSBCode) {
	case SB_LEFT:
		m_fStartY = m_fMinY + m_fTotalY - m_fDispY;
		break;
	case SB_RIGHT:
		m_fStartY = m_fMinY;
		break;
	case SB_LINELEFT:
		m_fStartY += m_fDispY / 32;
		break;
	case SB_LINERIGHT:
		m_fStartY -= m_fDispY / 32;
		break;
	case SB_PAGELEFT:
		m_fStartY += m_fDispY;
		break;
	case SB_PAGERIGHT:
		m_fStartY -= m_fDispY;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_fStartY = m_fMinY + m_fTotalY * int(SCROLL_RANGE - nPos - m_nVScrollSize) / SCROLL_RANGE;
		break;
	}

	DispGraphWindow();
}

void CWaveEditManual::InitData()
{
	m_oWaveForm.nDataNum = 2;
	m_oWaveForm.vertex[0].x = 0;
	m_oWaveForm.vertex[0].y = 0;
	m_oWaveForm.vertex[1].x = 1;
	m_oWaveForm.vertex[1].y = 0;
}

void CWaveEditManual::SetHScroll()
{
	const int nScrollSize = (int)(m_fDispX / m_fTotalX * SCROLL_RANGE);
	const int nScrollPos = (int)(m_fStartX / m_fTotalX * SCROLL_RANGE);

	if (nScrollSize != m_nHScrollSize || nScrollPos != m_nHScrollPos) {
		m_nHScrollSize = nScrollSize;
		m_nHScrollPos = nScrollPos;

		SCROLLINFO si{
			.cbSize = sizeof(SCROLLINFO),
			.fMask = SIF_ALL,
			.nMin = 0,
			.nMax = SCROLL_RANGE,
			.nPage = (UINT)nScrollSize,
			.nPos = nScrollPos
		};
		m_cScrollH.SetScrollInfo(&si, TRUE);
	}

	CToolBarCtrl &tc = m_tbZoomH.GetToolBarCtrl();
	if (m_fDispX == m_fTotalX / MAX_ZOOM_X) {
		tc.EnableButton(IDM_ZOOM_FULL_H, TRUE);
		tc.EnableButton(IDM_ZOOM_IN_H, FALSE);
		tc.EnableButton(IDM_ZOOM_OUT_H, TRUE);
	} else if (m_fDispX == m_fTotalX) {
		tc.EnableButton(IDM_ZOOM_FULL_H, FALSE);
		tc.EnableButton(IDM_ZOOM_IN_H, TRUE);
		tc.EnableButton(IDM_ZOOM_OUT_H, FALSE);
	} else {
		tc.EnableButton(IDM_ZOOM_FULL_H, TRUE);
		tc.EnableButton(IDM_ZOOM_IN_H, TRUE);
		tc.EnableButton(IDM_ZOOM_OUT_H, TRUE);
	}
}

void CWaveEditManual::SetVScroll()
{
	const int nScrollSize = (int)(m_fDispY / m_fTotalY * SCROLL_RANGE);
	const int nScrollPos = (int)((m_fStartY - m_fMinY) / m_fTotalY * SCROLL_RANGE);

	if (nScrollSize != m_nVScrollSize || nScrollPos != m_nVScrollPos) {
		m_nVScrollSize = nScrollSize;
		m_nVScrollPos = nScrollPos;

		SCROLLINFO si{
			.cbSize = sizeof(SCROLLINFO),
			.fMask = SIF_ALL,
			.nMin = 0,
			.nMax = SCROLL_RANGE,
			.nPage = (UINT)nScrollSize,
			.nPos = SCROLL_RANGE - nScrollPos - m_nVScrollSize
		};
		m_cScrollV.SetScrollInfo(&si, TRUE);
	}

	CToolBarCtrl &tc = m_tbZoomV.GetToolBarCtrl();
	if (m_fDispY == m_fTotalY / MAX_ZOOM_Y) {
		tc.EnableButton(IDM_ZOOM_FULL_V, TRUE);
		tc.EnableButton(IDM_ZOOM_IN_V, FALSE);
		tc.EnableButton(IDM_ZOOM_OUT_V, TRUE);
	} else if (m_fDispY == m_fTotalY) {
		tc.EnableButton(IDM_ZOOM_FULL_V, FALSE);
		tc.EnableButton(IDM_ZOOM_IN_V, TRUE);
		tc.EnableButton(IDM_ZOOM_OUT_V, FALSE);
	} else {
		tc.EnableButton(IDM_ZOOM_FULL_V, TRUE);
		tc.EnableButton(IDM_ZOOM_IN_V, TRUE);
		tc.EnableButton(IDM_ZOOM_OUT_V, TRUE);
	}
}

void CWaveEditManual::DispGraphWindow(BOOL bDispPosition)
{
	if (m_fDispX == 0)
		m_fDispX = m_fTotalX;
	else if (m_fDispX < m_fTotalX / MAX_ZOOM_X)
		m_fDispX = m_fTotalX / MAX_ZOOM_X;
	else if (m_fDispX > m_fTotalX)
		m_fDispX = m_fTotalX;

	if (m_fStartX < 0)
		m_fStartX = 0;
	else if (m_fStartX > m_fTotalX - m_fDispX)
		m_fStartX = m_fTotalX - m_fDispX;

	if (m_fDispY == 0)
		m_fDispY = m_fTotalY;
	else if (m_fDispY < m_fTotalY / MAX_ZOOM_Y)
		m_fDispY = m_fTotalY / MAX_ZOOM_Y;
	else if (m_fDispY > m_fTotalY)
		m_fDispY = m_fTotalY;

	if (m_fStartY < m_fMinY)
		m_fStartY = m_fMinY;
	else if (m_fStartY > m_fMinY + m_fTotalY - m_fDispY)
		m_fStartY = m_fMinY + m_fTotalY - m_fDispY;

	SetHScroll();
	SetVScroll();

	m_cWaveEditWnd.DispGraph(m_fStartX, m_fDispX, &m_oWaveForm, m_fStartY + m_fDispY, m_fStartY, m_nSelPoint);

	m_cZoomH = int(m_fTotalX / m_fDispX);
	m_cZoomV = int(m_fTotalY / m_fDispY);

	if (bDispPosition)
		DispPosition();
}

void CWaveEditManual::MouseDown(float x, float y)
{
	UpdateData(TRUE);

	switch (m_iEditMode) {
	case EDIT_MODE_ADD:
		if (x > 0 && x < 1 && y > -1 && y < 1) {
			AddVertex(x, y);
			DispGraphWindow();
		}
		break;
	case EDIT_MODE_SEL:
		SelectVertex(x, y);
		DispGraphWindow();
		break;
	}
}

void CWaveEditManual::MouseMove(float x, float y)
{
	UpdateData(TRUE);

	switch (m_iEditMode) {
	case EDIT_MODE_ADD:
	case EDIT_MODE_SEL:
		if (MoveVertex(x, y))
			DispGraphWindow();
		break;
	}
}

void CWaveEditManual::MouseAdd(float x, float y)
{
	AddVertex(x, y);
	DispGraphWindow();
}

void CWaveEditManual::MouseDelete()
{
	DeleteVertex(m_nSelPoint);
	DispGraphWindow();
}

int CWaveEditManual::GetSelPoint() const
{
	return m_nSelPoint;
}

int CWaveEditManual::GetEditMode()
{
	UpdateData(TRUE);

	return m_iEditMode;
}

void CWaveEditManual::AddVertex(float x, float y)
{
	int i;

	if (m_oWaveForm.nDataNum < MAX_WAVEVERTEX) {
		for (i = 0; i < m_oWaveForm.nDataNum; i++) {
			if (x == m_oWaveForm.vertex[i].x)
				break;

			if (x < m_oWaveForm.vertex[i].x) {
				for (int j = m_oWaveForm.nDataNum; j > i; j--)
					m_oWaveForm.vertex[j] = m_oWaveForm.vertex[j - 1];

				m_oWaveForm.nDataNum++;
				break;
			}
		}

		m_oWaveForm.vertex[i].x = x;
		m_oWaveForm.vertex[i].y = y;

		m_nSelPoint = i;
	}
}

void CWaveEditManual::DeleteVertex(int n)
{
	if (n > 0 && n < m_oWaveForm.nDataNum - 1) {
		for (int i = n; i < m_oWaveForm.nDataNum - 1; i++)
			m_oWaveForm.vertex[i] = m_oWaveForm.vertex[i + 1];

		m_oWaveForm.nDataNum--;

		m_nSelPoint = -1;
	}
}

void CWaveEditManual::SelectVertex(float x, float y)
{
	double d, dx, dy;
	int nSelPoint = 0;
	double fMinDistance = 10;

	for (int i = 1; i < m_oWaveForm.nDataNum - 1; i++) {
		dx = (x - m_oWaveForm.vertex[i].x) / m_fDispX * 2;
		dy = (y - m_oWaveForm.vertex[i].y) / m_fDispY;
		d = sqrt(dx * dx + dy * dy);

		if (d < fMinDistance) {
			fMinDistance = d;
			nSelPoint = i;
		}
	}

	if (nSelPoint != 0)
		m_nSelPoint = nSelPoint;
}

BOOL CWaveEditManual::MoveVertex(float x, float y)
{
	if (m_nSelPoint != -1) {
		if (x > m_oWaveForm.vertex[m_nSelPoint - 1].x && x < m_oWaveForm.vertex[m_nSelPoint + 1].x && y >= -1 && y <= 1) {
			m_oWaveForm.vertex[m_nSelPoint].x = x;
			m_oWaveForm.vertex[m_nSelPoint].y = y;

			return TRUE;
		}
	}

	return FALSE;
}

void CWaveEditManual::OnBnClickedVertexDelete()
{
	DeleteVertex(m_nSelPoint);
	DispGraphWindow();
}

void CWaveEditManual::OnBnClickedAllClear()
{
	InitData();
	DispGraphWindow();
}

void CWaveEditManual::OnOK()
{
}

void CWaveEditManual::DispPosition()
{
	if (m_nSelPoint != -1) {
		m_cPosX.EnableWindow(TRUE);
		m_cPosY.EnableWindow(TRUE);

		m_cPosX.Format("%.3f", m_oWaveForm.vertex[m_nSelPoint].x);
		m_cPosY.Format("%.3f", m_oWaveForm.vertex[m_nSelPoint].y);
	} else {
		m_cPosX.EnableWindow(FALSE);
		m_cPosY.EnableWindow(FALSE);

		m_cPosX.Blank();
		m_cPosY.Blank();
	}
}

void CWaveEditManual::OnEnChangePosX()
{
	float x = m_cPosX;

	if (m_nSelPoint != -1) {
		if (x < m_oWaveForm.vertex[m_nSelPoint - 1].x)
			x = m_oWaveForm.vertex[m_nSelPoint - 1].x;
		else if (x > m_oWaveForm.vertex[m_nSelPoint + 1].x)
			x = m_oWaveForm.vertex[m_nSelPoint + 1].x;

		m_oWaveForm.vertex[m_nSelPoint].x = x;

		DispGraphWindow(FALSE);
	}
}

void CWaveEditManual::OnEnChangePosY()
{
	float y = m_cPosY;

	if (m_nSelPoint != -1) {
		if (y < -1)
			y = -1;
		else if (y > 1)
			y = 1;

		m_oWaveForm.vertex[m_nSelPoint].y = y;

		DispGraphWindow(FALSE);
	}
}

void CWaveEditManual::OnBnClickedSmoothing()
{
	m_oWaveForm.bSmoothing = m_cSmoothing;
	DispGraphWindow(FALSE);
}

void CWaveEditManual::OnBnClickedTest()
{
//	m_cBtnStart.SetWindowText(GetString(m_cBtnStart ? IDS_STOP : IDS_START));

	g_oWaveForm.SetTestWaveForm((WaveForm *)&m_oWaveForm);

	m_pSgDlg->StartStop(m_cBtnTest);
}

BOOL CWaveEditManual::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDC_WAVE_WND, IDH_SG_EDIT_WAVE_WND,
		IDC_ZOOM_V, IDH_SG_EDIT_ZOOM_V,
		IDC_SCROLL_V, IDH_SG_EDIT_SCROLL_V,
		IDC_SCROLL_H, IDH_SG_EDIT_SCROLL_H,
		IDC_ZOOM_H, IDH_SG_EDIT_ZOOM_H,
		IDC_POS_X, IDH_SG_EDIT_POS_X,
		IDC_POS_Y, IDH_SG_EDIT_POS_Y,
		IDC_VERTEX_ADD, IDH_SG_EDIT_VERTEX_ADD,
		IDC_VERTEX_SEL, IDH_SG_EDIT_VERTEX_SEL,
		IDC_VERTEX_DELETE, IDH_SG_EDIT_VERTEX_DELETE,
		IDC_ALL_CLEAR, IDH_SG_EDIT_ALL_CLEAR,
		IDC_BTN_TEST, IDH_SG_EDIT_TEST,
		IDC_SMOOTHING, IDH_SG_EDIT_SMOOTHING,
		ID_ZOOM_H_FULL, IDH_SG_EDIT_ZOOM_H_FULL,
		ID_ZOOM_H_IN, IDH_SG_EDIT_ZOOM_H_IN,
		ID_ZOOM_H_OUT, IDH_SG_EDIT_ZOOM_H_OUT,
		ID_ZOOM_V_FULL, IDH_SG_EDIT_ZOOM_V_FULL,
		ID_ZOOM_V_IN, IDH_SG_EDIT_ZOOM_V_IN,
		ID_ZOOM_V_OUT, IDH_SG_EDIT_ZOOM_V_OUT,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}
