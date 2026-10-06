// FilterDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "RTA.h"
#include "FilterDlg.h"
#include "DigitalFilter.h"

// CFilterDlg ダイアログ

CFilterDlg::CFilterDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CFilterDlg::IDD, pParent)
{
	m_bInit = FALSE;

	Create(IDD, g_oSetData.Flt.nPosLeft, g_oSetData.Flt.nPosTop, 0, pParent);
}

void CFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FILTER_BAND, m_cComboFilterBand);
	DDX_Control(pDX, IDC_COMBO_FILTER_ORDER, m_ComboFilterOrder);
	DDX_Control(pDX, IDC_COMBO_FILTER_SHAPE, m_cComboFilterShape);
	DDX_Control(pDX, IDC_EDIT_FILTER_RIPPLE1, m_cEditFilterRipple1);
	DDX_Control(pDX, IDC_EDIT_FILTER_CUTOFF1, m_cEditFilterCutoff1);
	DDX_Control(pDX, IDC_EDIT_FILTER_CUTOFF2, m_cEditFilterCutoff2);
	DDX_Control(pDX, IDC_RADIO_FILTER_INPUT, m_cRadioFilterInput);
	DDX_Control(pDX, IDC_RADIO_FILTER_OUTPUT, m_cRadioFilterOutput);
	DDX_Control(pDX, IDC_STATIC_FILTER_CURVE, m_cStaticFilterCurve);
}

BEGIN_MESSAGE_MAP(CFilterDlg, CBaseDlg)
	ON_WM_MOVE()
	ON_CBN_SELCHANGE(IDC_COMBO_FILTER_BAND, &CFilterDlg::OnCbnSelchangeComboFilterBand)
	ON_CBN_SELCHANGE(IDC_COMBO_FILTER_SHAPE, &CFilterDlg::OnCbnSelchangeComboFilterShape)
	ON_CBN_SELCHANGE(IDC_COMBO_FILTER_ORDER, &CFilterDlg::OnCbnSelchangeComboFilterOrder)
	ON_EN_CHANGE(IDC_EDIT_FILTER_CUTOFF1, &CFilterDlg::OnEnChangeEditFilterCutoff1)
	ON_EN_CHANGE(IDC_EDIT_FILTER_CUTOFF2, &CFilterDlg::OnEnChangeEditFilterCutoff2)
	ON_EN_CHANGE(IDC_EDIT_FILTER_RIPPLE1, &CFilterDlg::OnEnChangeEditFilterRipple1)
	ON_BN_CLICKED(IDC_RADIO_FILTER_INPUT, &CFilterDlg::OnBnClickedRadioFilterInput)
	ON_BN_CLICKED(IDC_RADIO_FILTER_OUTPUT, &CFilterDlg::OnBnClickedRadioFilterOutput)
END_MESSAGE_MAP()

// CFilterDlg メッセージ ハンドラ

BOOL CFilterDlg::OnInitDialog()
{
	CBaseDlg::OnInitDialog();

	m_fSamplingFreq = 48000;

	m_cStaticFilterCurve.Initialize(90, NULL, 0, RGB(255, 0, 0), 0);

	m_cComboFilterBand.SetItemData(m_cComboFilterBand.AddString("Low Pass"), EDF_LPF);
	m_cComboFilterBand.SetItemData(m_cComboFilterBand.AddString("High Pass"), EDF_HPF);
	m_cComboFilterBand.SetItemData(m_cComboFilterBand.AddString("Band Pass"), EDF_BPF);
	m_cComboFilterBand.SetItemData(m_cComboFilterBand.AddString("Band Stop"), EDF_BSF);
	SelectListBox(m_cComboFilterBand, g_oSetData.Flt.nBand);

	m_cComboFilterShape.SetItemData(m_cComboFilterShape.AddString("Butterworth"), EDF_Butterworth);
	m_cComboFilterShape.SetItemData(m_cComboFilterShape.AddString("Chebyshev"), EDF_Chebyshev);
	m_cComboFilterShape.SetItemData(m_cComboFilterShape.AddString("Bessel"), EDF_Bessel);
	SelectListBox(m_cComboFilterShape, g_oSetData.Flt.nShape);

	CString sText;
	for (int nOrder = 1; nOrder <= 10; nOrder++) {
		sText.Format("%d", nOrder);
		m_ComboFilterOrder.SetItemData(m_ComboFilterOrder.AddString(sText), nOrder);
	}
	SelectListBox(m_ComboFilterOrder, g_oSetData.Flt.nOrder);

	m_cEditFilterRipple1 = g_oSetData.Flt.fRipple1;
	m_cEditFilterCutoff1 = g_oSetData.Flt.fCutoff1;
	m_cEditFilterCutoff2 = g_oSetData.Flt.fCutoff2;
	m_cRadioFilterInput = (g_oSetData.Flt.nInputOutput == 0);
	m_cRadioFilterOutput = (g_oSetData.Flt.nInputOutput == 1);

	m_cEditFilterRipple1.SetValidChar(VC_NUM | VC_POINT);
	m_cEditFilterCutoff1.SetValidChar(VC_NUM | VC_POINT);
	m_cEditFilterCutoff2.SetValidChar(VC_NUM | VC_POINT);

	SetFilter();

	m_bInit = TRUE;

	return TRUE;
}

void CFilterDlg::OnMove(int x, int y)
{
	CBaseDlg::OnMove(x, y);

	if (m_bInit) {
		CRect rect;
		GetWindowRect(rect);
		g_oSetData.Flt.nPosLeft = rect.left;
		g_oSetData.Flt.nPosTop = rect.top;
	}
}

void CFilterDlg::OnCbnSelchangeComboFilterBand()
{
	if (m_bInit) {
		const int nIndex = m_cComboFilterBand.GetCurSel();
		g_oSetData.Flt.nBand = (int)m_cComboFilterBand.GetItemData(nIndex);
		SetFilter();
	}
}

void CFilterDlg::OnCbnSelchangeComboFilterShape()
{
	if (m_bInit) {
		const int nIndex = m_cComboFilterShape.GetCurSel();
		g_oSetData.Flt.nShape = (int)m_cComboFilterShape.GetItemData(nIndex);
		SetFilter();
	}
}

void CFilterDlg::OnCbnSelchangeComboFilterOrder()
{
	if (m_bInit) {
		const int nIndex = m_ComboFilterOrder.GetCurSel();
		g_oSetData.Flt.nOrder = (int)m_ComboFilterOrder.GetItemData(nIndex);
		SetFilter();
	}
}

void CFilterDlg::OnEnChangeEditFilterCutoff1()
{
	if (m_bInit) {
		const double fCutoff1 = m_cEditFilterCutoff1;
		if (fCutoff1 > 0 && fCutoff1 < m_fSamplingFreq / 2) {
			g_oSetData.Flt.fCutoff1 = fCutoff1;
			SetFilter();
		}
	}
}

void CFilterDlg::OnEnChangeEditFilterCutoff2()
{
	if (m_bInit) {
		const double fCutoff2 = m_cEditFilterCutoff2;
		if (fCutoff2 > 0 && fCutoff2 < m_fSamplingFreq / 2) {
			g_oSetData.Flt.fCutoff2 = fCutoff2;
			SetFilter();
		}
	}
}

void CFilterDlg::OnEnChangeEditFilterRipple1()
{
	if (m_bInit) {
		const double fRipple1 = m_cEditFilterRipple1;
		if (fRipple1 > 0) {
			g_oSetData.Flt.fRipple1 = fRipple1;
			SetFilter();
		}
	}
}

void CFilterDlg::SetFilter()
{
	m_cDigitalFilter.InitIIR((EDFShape)g_oSetData.Flt.nShape, (EDFBand)g_oSetData.Flt.nBand, g_oSetData.Flt.nOrder, m_fSamplingFreq, g_oSetData.Flt.fCutoff1, g_oSetData.Flt.fCutoff2, g_oSetData.Flt.fRipple1);

	m_cStaticFilterCurve.DispGraph(m_cDigitalFilter, 1, (int)(m_fSamplingFreq / 2), -100, 10);

	m_cEditFilterCutoff2.EnableWindow(g_oSetData.Flt.nBand == EDF_BPF || g_oSetData.Flt.nBand == EDF_BSF);
	m_cEditFilterRipple1.EnableWindow(g_oSetData.Flt.nShape == EDF_Chebyshev);

	g_nFilterUpdateCounter++;
}

void CFilterDlg::OnBnClickedRadioFilterInput()
{
	if (m_bInit) {
		g_oSetData.Flt.nInputOutput = 0;
		SetFilter();
	}
}

void CFilterDlg::OnBnClickedRadioFilterOutput()
{
	if (m_bInit) {
		g_oSetData.Flt.nInputOutput = 1;
		SetFilter();
	}
}
