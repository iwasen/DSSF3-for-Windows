// SgDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "SgDlg.h"
#include "WaveInEx.h"
#include "WaveOutEx.h"
#include "Help\ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CSgDlg ダイアログ

CSgDlg::CSgDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CSgDlg::IDD, pParent)
{
	m_bMove = FALSE;
	m_nTimeCount = -1;
	m_pWaveOut = CWaveOutEx::GetInstance();
	m_nWindowKind = WINDOW_SG;

	Create(IDD, g_oSetData.Sg.nPosLeft, g_oSetData.Sg.nPosTop, g_oSetData2.nFontSizeSg, pParent);
}

CSgDlg::~CSgDlg()
{
	m_pWaveOut->ReleaseInstance();
}

void CSgDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SG_TIME, m_cSgTime);
	DDX_Control(pDX, IDC_SG_RLEVEL, m_cSgRLevel);
	DDX_Control(pDX, IDC_SG_LLEVEL, m_cSgLLevel);
	DDX_Control(pDX, IDC_RENDOU, m_cRendou);
	DDX_Control(pDX, IDC_SG_DLEVEL, m_cSgDLevel);
	DDX_Control(pDX, IDC_SG_DSLIDER, m_cSgDSlider);
	DDX_Control(pDX, IDC_BTN_START, m_cSgStart);
	DDX_Control(pDX, IDC_SAMPLING_RATE, m_cSamplingRate);
	DDX_Control(pDX, IDC_SG_LSLIDER, m_cSgLSlider);
	DDX_Control(pDX, IDC_SG_RSLIDER, m_cSgRSlider);
	DDX_Control(pDX, IDC_SG_TAB, m_cSgTab);
}

BEGIN_MESSAGE_MAP(CSgDlg, CBaseDlg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_SG_TAB, OnSelchangeSgTab)
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_SG_LLEVEL, OnChangeSgLlevel)
	ON_EN_CHANGE(IDC_SG_RLEVEL, OnChangeSgRlevel)
	ON_CBN_SELCHANGE(IDC_SAMPLING_RATE, OnSelchangeSamplingRate)
	ON_MESSAGE(WM_SG_CHANGE, OnSgChange)
	ON_BN_CLICKED(IDC_RENDOU, OnRendou)
	ON_BN_CLICKED(IDC_BTN_START, OnStart)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_SG_DLEVEL, OnChangeSgDlevel)
	ON_WM_HSCROLL()
	ON_WM_MOVE()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSgDlg メッセージ ハンドラ

BOOL CSgDlg::OnInitDialog()
{
	CBaseDlg::OnInitDialog();

	CreateStatusBar();
	SetStatusBar(IDS_STATUS_STOP);

	m_bStart = FALSE;
	m_bMove = TRUE;

	EnableLevel();

	SetSamplingRateList(m_cSamplingRate, WAVE_OUT, 0, g_nWaveOutDevice, g_oSetData.Sg.nSamplingRate);

	m_cSgLSlider.SetRange(0, 100);
	m_cSgLSlider.SetTicFreq(10);
	m_cSgLSlider.SetPageSize(10);
	SetLLevel(g_oSetData.Sg.nLLevel);

	m_cSgRSlider.SetRange(0, 100);
	m_cSgRSlider.SetTicFreq(10);
	m_cSgRSlider.SetPageSize(10);
	SetRLevel(g_oSetData.Sg.nRLevel);

	m_cSgDSlider.SetRange(0, 100);
	m_cSgDSlider.SetTicFreq(10);
	m_cSgDSlider.SetPageSize(10);
	SetDLevel(g_oSetData.Sg.nDLevel);

	m_cRendou = g_oSetData.Sg.bRendou;

	m_cSgTab.SetFontSize(g_oSetData2.nFontSizeSg);
	m_cSgTab.AddPage(&m_SgTone, IDD_SG_TONE, IDS_TONE);
	m_cSgTab.AddPage(&m_SgNoise, IDD_SG_NOISE, IDS_NOISE);
	m_cSgTab.AddPage(&m_SgSweep, IDD_SG_SWEEP, IDS_SWEEP);
	m_cSgTab.AddPage(&m_SgPulse, IDD_SG_PULSE, IDS_PULSE);
	m_cSgTab.AddPage(&m_SgSynth, IDD_SG_SYNTH, IDS_SYNTH);
	m_cSgTab.AddPage(&m_SgScale, IDD_SG_SCALE, IDS_SCALE);
	m_cSgTab.AddPage(&m_SgWave, IDD_SG_WAVE, IDS_WAVE);
	m_cSgTab.SetPage(g_oSetData.Sg.nMode);

	m_cSgLLevel.SetValidChar(VC_NUM);
	m_cSgRLevel.SetValidChar(VC_NUM);
	m_cSgDLevel.SetValidChar(VC_NUM);

	return TRUE;
}

void CSgDlg::SetLLevel(int nLevel)
{
	SetLLevel2(nLevel);
	if (g_oSetData.Sg.bRendou)
		SetRLevel2(nLevel);
}

void CSgDlg::SetLLevel2(int nLevel)
{
	if (nLevel < 0)
		nLevel = 0;
	else if (nLevel > 100)
		nLevel = 100;

	m_cSgLLevel = nLevel;
	m_cSgLSlider.SetPos(100 - nLevel);

	g_oSetData.Sg.nLLevel = nLevel;

	SetVolume();
}

void CSgDlg::SetRLevel(int nLevel)
{
	SetRLevel2(nLevel);
	if (g_oSetData.Sg.bRendou)
		SetLLevel2(nLevel);
}

void CSgDlg::SetRLevel2(int nLevel)
{
	if (nLevel < 0)
		nLevel = 0;
	else if (nLevel > 100)
		nLevel = 100;

	m_cSgRLevel = nLevel;
	m_cSgRSlider.SetPos(100 - nLevel);

	g_oSetData.Sg.nRLevel = nLevel;

	SetVolume();
}

void CSgDlg::SetDLevel(int nLevel)
{
	if (nLevel < 0)
		nLevel = 0;
	else if (nLevel > 100)
		nLevel = 100;

	m_cSgDLevel = nLevel;
	m_cSgDSlider.SetPos(nLevel);

	g_oSetData.Sg.nDLevel = nLevel;
}

void CSgDlg::OnSelchangeSgTab(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	int	n;

	if ((n = m_cSgTab.GetCurSel()) != -1) {
		g_oSetData.Sg.nMode = n;
		Stop();
	}

	*pResult = 0;
}

void CSgDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (pScrollBar->GetDlgCtrlID()) {
	case IDC_SG_LSLIDER:
		SetLLevel(100 - ((CSliderCtrl *)pScrollBar)->GetPos());
		break;
	case IDC_SG_RSLIDER:
		SetRLevel(100 - ((CSliderCtrl *)pScrollBar)->GetPos());
		break;
	}

	CBaseDlg::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CSgDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (pScrollBar->GetDlgCtrlID()) {
	case IDC_SG_DSLIDER:
//		m_pWaveOut->Reset();
		SetDLevel(((CSliderCtrl *)pScrollBar)->GetPos());
		break;
	}

	CBaseDlg::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSgDlg::OnChangeSgLlevel()
{
	SetLLevel(m_cSgLLevel);
}

void CSgDlg::OnChangeSgRlevel()
{
	SetRLevel(m_cSgRLevel);
}

void CSgDlg::OnChangeSgDlevel()
{
	SetDLevel(m_cSgDLevel);
}

void CSgDlg::OnStart()
{
	if (m_bStart)
		Stop();
	else {
		m_fTimeCount = 0;

		if (!Start())
			m_cSgStart.SetCheck(FALSE);
	}
}

void CSgDlg::OnSelchangeSamplingRate()
{
	int index;

	if ((index = m_cSamplingRate.GetCurSel()) != CB_ERR) {
		g_oSetData.Sg.nSamplingRate = (int)m_cSamplingRate.GetItemData(index);
		m_cSgTab.NotifyChildDlg(TN_CHANGE_SAMPLING_RATE);
		if (m_SgTone.m_hWnd != NULL)
			m_SgTone.SetFreqRange(TRUE);
		if (m_bStart)
			Start();
	}
}

int CSgDlg::OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify)
{
	switch (nCode) {
	case WAVEOUT_OPEN:
		m_bStart = TRUE;
		m_cSgStart.SetCheck(m_bStart);
		m_cSgStart.SetWindowText(GetString(IDS_STOP));
		SetStatusBar(GetRunningStatus(IDS_STATUS_RUNNING, 0, m_pWaveOut->GetBitsPerSample(), 0));
		pWaveNotify->nWindow = WINDOW_SG;
		break;
	case WAVEOUT_CLOSE:
		m_bStart = FALSE;
		m_cSgStart.SetCheck(m_bStart);
		m_cSgStart.SetWindowText(GetString(IDS_START));
		SetStatusBar(IDS_STATUS_STOP);
		break;
	case WAVEOUT_DATA:
		m_fTimeCount += m_fBlockTime;
		int nTimeCount = (int)m_fTimeCount;
		if (m_nTimeCount != nTimeCount) {
			m_cSgTime.Format("%d:%02d:%02d", nTimeCount / 3600, nTimeCount / 60 % 60, nTimeCount % 60);
			m_nTimeCount = nTimeCount;
		}

		return m_oSG.SetWaveData(pWaveNotify);
	}
	return 0;
}

LRESULT CSgDlg::OnSgChange(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 2) {
		if (lParam) {
			if (!m_bStart)
				OnStart();
		} else {
			if (m_bStart)
				OnStart();
		}
	} else {
		if (m_bStart) {
			if (m_nChannel == GetChannel()) {
				if (wParam != 0)
					m_pWaveOut->Reset();
			} else
				Start();

			if (lParam != 0)
				m_oSG.ResetToneFreq();
		}
	}

	return 0;
}

void CSgDlg::OnRendou()
{
	g_oSetData.Sg.bRendou = m_cRendou;
}

void CSgDlg::OnOK()
{
	OnStart();
}

BOOL CSgDlg::Start()
{
	m_pWaveOut->Close();

	SG_SPB = 2048;
	if (g_oSetData.Sg.nSamplingRate > 48000)
		SG_SPB *= g_oSetData.Sg.nSamplingRate / 48000;
	if (g_nWaveOutDevice <= -100)
		SG_SPB *= 4;

	m_nChannel = GetChannel();

	m_fBlockTime = (double)SG_SPB / g_oSetData.Sg.nSamplingRate;

	m_oSG.Init();

	if (m_pWaveOut->Open(g_nWaveOutDevice, this, m_nChannel, g_oSetData.Sg.nSamplingRate, SG_SPB, 4, FALSE)) {
		SetVolume();
		m_pWaveOut->Start();
	} else {
		CWaveInEx::CloseAll();
		if (m_pWaveOut->Open(g_nWaveOutDevice, this, m_nChannel, g_oSetData.Sg.nSamplingRate, SG_SPB, 4)) {
			SetVolume();
			m_pWaveOut->Start();
		} else
			return FALSE;
	}

	m_oSG.m_nBitsPerSample = m_pWaveOut->GetBitsPerSample();

	return TRUE;
}

void CSgDlg::Stop()
{
	m_pWaveOut->Close();
}

void CSgDlg::StartStop(BOOL bStart)
{
	if (m_bStart != bStart)
		OnStart();
}

void CSgDlg::SetVolume()
{
	m_pWaveOut->SetVolume(g_oSetData.Sg.nLLevel * 0xffff / 100, g_oSetData.Sg.nRLevel * 0xffff / 100);
}

int CSgDlg::GetChannel()
{
	int nChannel = 1;

	switch (g_oSetData.Sg.nMode) {
	case SG_MODE_TONE:
	case SG_MODE_NOISE:
		nChannel = 2;
		break;
	}

	return nChannel;
}

void CSgDlg::OnDestroy()
{
	CBaseDlg::OnDestroy();

	if (m_bStart)
		Stop();
}

void CSgDlg::OnMove(int x, int y)
{
	CBaseDlg::OnMove(x, y);

	if (m_bMove) {
		CRect rect;
		GetWindowRect(rect);
		g_oSetData.Sg.nPosLeft = rect.left;
		g_oSetData.Sg.nPosTop = rect.top;
	}
}

void CSgDlg::ChangeWaveDevice(int nInOut)
{
	if (nInOut & WAVE_OUT) {
		EnableLevel();

		if (m_bStart)
			Stop();

		SetSamplingRateList(m_cSamplingRate, WAVE_OUT, 0, g_nWaveOutDevice, g_oSetData.Sg.nSamplingRate);
		OnSelchangeSamplingRate();
	}
}

BOOL CSgDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_BTN_START, IDH_SG_START,
		IDC_SG_TIME, IDH_SG_TIME,
		IDC_SAMPLING_RATE, IDH_SG_SAMPLING_RATE,
		IDC_SG_DSLIDER, IDH_SG_DSLIDER,
		IDC_SG_DLEVEL, IDH_SG_DLEVEL,
		IDCANCEL, IDH_SG_CLOSE,
		IDC_SG_LSLIDER, IDH_SG_LSLIDER,
		IDC_SG_RSLIDER, IDH_SG_RSLIDER,
		IDC_SG_LLEVEL, IDH_SG_LLEVEL,
		IDC_SG_RLEVEL, IDH_SG_RLEVEL,
		IDC_RENDOU, IDH_SG_RENDOU,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

void CSgDlg::EnableLevel()
{
	BOOL bEnable = g_oSetData.Pc.nOutputDevice >= 0;
	m_cSgLSlider.EnableWindow(bEnable);
	m_cSgRSlider.EnableWindow(bEnable);
	m_cSgLLevel.EnableWindow(bEnable);
	m_cSgRLevel.EnableWindow(bEnable);
	m_cRendou.EnableWindow(bEnable);
}
