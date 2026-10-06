// DstDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "DstDlg.h"
#include "WaveInEx.h"
#include "WaveOutEx.h"
#include "Regist.h"
#include "Directory.h"
#include "Help\ContextHelp.h"

#define MODE_MANUAL			0
#define MODE_FREQ_SWEEP		1
#define MODE_LEVEL_SWEEP	2

#define FILENAME_HOLDDATA	"DstHold.dat"

// CDstDlg ダイアログ

CDstDlg::CDstDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CDstDlg::IDD, pParent)
	, m_iChannel(0)
	, m_iScaleMode(0)
{
	m_bInitialized = FALSE;
	m_pLeftData = NULL;
	m_pRightData = NULL;
	m_pSinTable = NULL;
	m_bStart = FALSE;
	m_pWaveIn = CWaveInEx::GetInstance();
	m_pWaveOut = CWaveOutEx::GetInstance();
	m_nTimeCount = 0;
	m_nWindowKind = WINDOW_DST;

	Create(IDD, g_oSetData.Dst.nPosLeft, g_oSetData.Dst.nPosTop, g_oSetData2.nFontSizeDst, pParent);
}

CDstDlg::~CDstDlg()
{
	m_pWaveIn->ReleaseInstance();
	m_pWaveOut->ReleaseInstance();
}

void CDstDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SAMPLING_RATE, m_cSamplingRate);
	DDX_Radio(pDX, IDC_CHANNEL_MONO, m_iChannel);
	DDX_Control(pDX, IDC_DST_TAB, m_cDstTab);
	DDX_Control(pDX, IDC_BTN_START, m_cBtnStart);
	DDX_Control(pDX, IDC_MAX_HARMONICS, m_cMaxHarmonics);
	DDX_Radio(pDX, IDC_SCALE_PERCENT, m_iScaleMode);
	DDX_Control(pDX, IDC_SCALE_MAX, m_cScaleMax);
	DDX_Control(pDX, IDC_SCALE_MIN, m_cScaleMin);
	DDX_Control(pDX, IDC_ELP_TIME, m_cElapseTime);
	DDX_Control(pDX, IDC_GUARD_TIME, m_cGuardTime);
	DDX_Control(pDX, IDC_CSV_OUTPUT, m_cCsvOutput);
	DDX_Control(pDX, IDC_ADD_DATA_HOLD, m_cAddDataHold);
	DDX_Control(pDX, IDC_DEL_DATA_HOLD, m_cDelDataHold);
	DDX_Control(pDX, IDC_SCREEN_SHOT, m_cScreenShot);
}

BEGIN_MESSAGE_MAP(CDstDlg, CBaseDlg)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_BTN_START, OnBnClickedStart)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_SAMPLING_RATE, OnCbnSelchangeSamplingRate)
	ON_BN_CLICKED(IDC_CHANNEL_MONO, OnBnClickedChannelMono)
	ON_BN_CLICKED(IDC_CHANNEL_STEREO, OnBnClickedChannelStereo)
	ON_CBN_SELCHANGE(IDC_MAX_HARMONICS, OnCbnSelchangeMaxHarmonics)
	ON_BN_CLICKED(IDC_SCALE_PERCENT, OnBnClickedScalePercent)
	ON_BN_CLICKED(IDC_SCALE_DB, OnBnClickedScaleDb)
	ON_CBN_SELCHANGE(IDC_SCALE_MAX, OnCbnSelchangeScaleMax)
	ON_CBN_SELCHANGE(IDC_SCALE_MIN, OnCbnSelchangeScaleMin)
	ON_NOTIFY(TCN_SELCHANGE, IDC_DST_TAB, OnTcnSelchangeDstTab)
	ON_CBN_SELCHANGE(IDC_GUARD_TIME, OnCbnSelchangeGuardTime)
	ON_BN_CLICKED(IDC_CSV_OUTPUT, OnBnClickedOutputFile)
	ON_BN_CLICKED(IDC_SCREEN_SHOT, &CDstDlg::OnBnClickedScreenShot)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_ADD_DATA_HOLD, &CDstDlg::OnBnClickedAddDataHold)
	ON_BN_CLICKED(IDC_DEL_DATA_HOLD, &CDstDlg::OnBnClickedDelDataHold)
END_MESSAGE_MAP()

// CDstDlg メッセージ ハンドラ

BOOL CDstDlg::OnInitDialog()
{
	CBaseDlg::OnInitDialog();

	CreateStatusBar();
	SetStatusBar(IDS_STATUS_STOP);

	SetSamplingRateList(m_cSamplingRate, WAVE_IN | WAVE_OUT, g_nWaveInDevice, g_nWaveOutDevice, g_oSetData.Dst.nSamplingRate);

	SetMaxHarmonics();
	SetGuardTime();

	m_iChannel = g_oSetData.Dst.nChannel;
	m_iScaleMode = g_oSetData.Dst.nScaleMode;

	SetScaleSelectAll();

	m_cDstTab.SetFontSize(g_oSetData2.nFontSizeDst);
	m_cDstTab.AddPage(&m_cDstManual, IDD_DST_MANUAL, IDS_MANUAL, TRUE);
	m_cDstTab.AddPage(&m_cDstFreq, IDD_DST_FREQ, IDS_FREQ_SWEEP, TRUE);
	m_cDstTab.AddPage(&m_cDstLevel, IDD_DST_LEVEL, IDS_LEVEL_SWEEP, TRUE);
	m_cDstTab.SetPage(g_oSetData.Dst.nMode);

	RestoreHoldData();

	EnableDataHoldButton();

	CWnd *pWnd = GetTopWindow();
	while (pWnd != NULL) {
		switch (pWnd->GetDlgCtrlID()) {
		case IDC_DST_TAB:
		case 0:
			SetCtlPosition(pWnd, 0, 0, 1, 1);
			break;
		default:
			SetCtlPosition(pWnd, 1, 0, 1, 0);
			break;
		}
		pWnd = pWnd->GetNextWindow();
	}

	UpdateData(FALSE);

	m_bInitialized = TRUE;

	return TRUE;
}

void CDstDlg::RestoreHoldData()
{
	CFile oFile;
	CString sDataDirectory;
	CString sPathName;

	GetDataDirectory(sDataDirectory);
	sPathName.Format("%s\\%s", (LPCSTR)sDataDirectory, FILENAME_HOLDDATA);

	if (oFile.Open(sPathName, CFile::modeRead | CFile::shareDenyNone)) {
		m_cDstFreq.LoadHoldData(oFile);
		m_cDstLevel.LoadHoldData(oFile);

		oFile.Close();
	}
}

void CDstDlg::SetMaxHarmonics()
{
	int i, nIndex;
	CString str;

	for (i = 2; i <= MAX_HARMONICS; i++) {
		str.Format("%d", i);
		nIndex = m_cMaxHarmonics.AddString(str);
		m_cMaxHarmonics.SetItemData(nIndex, i);
	}
	SelectListBox(m_cMaxHarmonics, g_oSetData.Dst.nMaxHarmonics);
}

void CDstDlg::SetGuardTime()
{
	int i, nIndex;
	CString str;

	for (i = 2; i <= MAX_GUARD_CNT; i += 2) {
		str.Format("%.1f sec", (double)i / 4);
		nIndex = m_cGuardTime.AddString(str);
		m_cGuardTime.SetItemData(nIndex, i);
	}
	SelectListBox(m_cGuardTime, g_oSetData.Dst.nGuardCnt);
}

void CDstDlg::SetScaleSelectAll()
{
	SetScaleSelect(m_cScaleMax, 0, -40, g_oSetData.Dst.nScaleMax);
	SetScaleSelect(m_cScaleMin, -40, -120, g_oSetData.Dst.nScaleMin);
}

void CDstDlg::SetScaleSelect(CComboBox &cCombo, int nMax, int nMin, int nDef)
{
	int i, nIndex;
	CString str;

	cCombo.ResetContent();

	for (i = nMax; i >= nMin; i -= 20) {
		if (g_oSetData.Dst.nScaleMode == 0)
			str.Format("%1g%%", pow(10.0, i / 20) * 100);
		else
			str.Format("%ddB", i);
		nIndex = cCombo.AddString(str);
		cCombo.SetItemData(nIndex, i);
	}
	SelectListBox(cCombo, nDef);
}

void CDstDlg::OnMove(int x, int y)
{
	CBaseDlg::OnMove(x, y);

	if (m_bInitialized && !IsIconic()) {
		CRect rect;
		GetWindowRect(rect);
		g_oSetData.Dst.nPosLeft = rect.left;
		g_oSetData.Dst.nPosTop = rect.top;
	}
}

void CDstDlg::ChangeWaveDevice(int /*nInOut*/)
{
	if (m_bStart)
		Stop();

	SetSamplingRateList(m_cSamplingRate, WAVE_IN | WAVE_OUT, g_nWaveInDevice, g_nWaveOutDevice, g_oSetData.Dst.nSamplingRate);
}

void CDstDlg::OnBnClickedStart()
{
	if (m_bStart)
		Stop();
	else {
		if (!Start())
			m_cBtnStart.SetCheck(FALSE);
	}
}

void CDstDlg::Stop()
{
	m_pWaveOut->Close();
	m_pWaveIn->Close();
}

BOOL CDstDlg::Start()
{
	FreeBuffers();

	m_nSample = g_oSetData.Dst.nSamplingRate;
	m_nChannel = g_oSetData.Dst.nChannel + 1;
	m_nBufSize = g_oSetData.Dst.nSamplingRate / 5;
	m_nGuardCnt = g_oSetData.Dst.nGuardCnt;
	m_nFreq = 0;
	m_fLevel = 0;
	m_nWaveOutOffset = 0;
	m_nWaveInOffset = 0;
	m_nTimeCount = 0;
	m_bRegist = (BOOL)CHECK_LICENSE2(APP_RA);
	m_bNoRegist = FALSE;

	switch (g_oSetData.Dst.nMode) {
	case MODE_MANUAL:
		m_cDstManual.Initialize();
		break;
	case MODE_FREQ_SWEEP:
		m_cDstFreq.Initialize();
		break;
	case MODE_LEVEL_SWEEP:
		m_cDstLevel.Initialize();
		break;
	}

	if (!m_pWaveOut->Open(g_nWaveOutDevice, this, 1, g_oSetData.Dst.nSamplingRate, m_nBufSize, 4, TRUE, TRUE))
		return FALSE;

	if (!m_pWaveIn->Open(g_nWaveInDevice, this, m_nChannel, g_oSetData.Dst.nSamplingRate, m_nBufSize, 4, FALSE)) {
		m_pWaveOut->Close();
		::AfxMessageBox(IDS_MSG_SOUNDINOUT);
		return FALSE;
	}

	MakeSinTable();

	m_pLeftData = new double[m_nSample];
	if (m_nChannel == 2)
		m_pRightData = new double[m_nSample];

	memset(m_pLeftDst, 0, sizeof(m_pLeftDst));
	memset(m_pRightDst, 0, sizeof(m_pRightDst));

	StartWaveInOut(m_pWaveIn, m_pWaveOut, FALSE);

	return TRUE;
}

void CDstDlg::FreeBuffers()
{
	if (m_pLeftData != NULL) {
		delete [] m_pLeftData;
		m_pLeftData = NULL;
	}

	if (m_pRightData != NULL) {
		delete [] m_pRightData;
		m_pRightData = NULL;
	}

	if (m_pSinTable != NULL) {
		delete [] m_pSinTable;
		m_pSinTable = NULL;
	}
}

void CDstDlg::MakeSinTable()
{
	int i;

	m_pSinTable = new double[m_nSample];

	for (i = 0; i < m_nSample; i++)
		m_pSinTable[i] = sin(2 * M_PI * i / m_nSample);
}

int CDstDlg::OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify)
{
	int nTimeCount;

	switch (nCode) {
	case WAVEOUT_OPEN:
		m_bStart = TRUE;
		m_cBtnStart.SetCheck(m_bStart);
		m_cBtnStart.SetWindowText(GetString(IDS_STOP));
		pWaveNotify->nWindow = WINDOW_DST;
		break;
	case WAVEOUT_CLOSE:
		m_pWaveIn->Close();
		m_bStart = FALSE;
		m_cBtnStart.SetCheck(m_bStart);
		m_cBtnStart.SetWindowText(GetString(IDS_START));
		EnableOutputFile();
		EnableDataHoldButton();
		FreeBuffers();
		break;
	case WAVEOUT_DATA:
		return WaveOutData(pWaveNotify->pSamplesData);
	case WAVEIN_OPEN:
		SetStatusBar(GetRunningStatus(IDS_STATUS_RUNNING, m_pWaveIn->GetBitsPerSample(), m_pWaveOut->GetBitsPerSample(), 0));
		pWaveNotify->nWindow = WINDOW_DST;
		break;
	case WAVEIN_DATA:
		nTimeCount = ++m_nTimeCount / 5;
		m_cElapseTime.Format("%d:%02d:%02d", nTimeCount / 3600, nTimeCount / 60 % 60, nTimeCount % 60);

		// 未登録時の動作制限
		if (nTimeCount >= NOREG_LIMIT_TIME && !m_bRegist) {
			m_bNoRegist = TRUE;
			return FALSE;
		}

		return WaveInData(pWaveNotify->pSamplesData);
	case WAVEIN_CLOSE:
		m_pWaveOut->Close();

		// 未登録時のダイアログ
		if (m_bNoRegist) {
			m_bNoRegist = FALSE;
			CHECK_LICENSE1(APP_RA);
		}

		SetStatusBar(IDS_STATUS_STOP);
		break;
	}

	return 0;
}

int CDstDlg::WaveOutData(double *pData)
{
	BOOL bContinue = TRUE;
	int i;
	int nFreq = 0;
	double fLevel = 0;

	switch (g_oSetData.Dst.nMode) {
	case MODE_MANUAL:
		bContinue = m_cDstManual.WaveOutData(nFreq, fLevel);
		break;
	case MODE_FREQ_SWEEP:
		bContinue = m_cDstFreq.WaveOutData(nFreq, fLevel);
		break;
	case MODE_LEVEL_SWEEP:
		bContinue = m_cDstLevel.WaveOutData(nFreq, fLevel);
		break;
	}

	if (!bContinue)
		return 0;

	if (nFreq != m_nFreq || fLevel != m_fLevel) {
		m_nFreq = nFreq;
		m_fLevel = fLevel;
		m_nGuardCnt = g_oSetData.Dst.nGuardCnt;
		m_nWaveInOffset = 0;
		m_pWaveOut->Reset();
	}

	const int nBitsPerSample = m_pWaveOut->GetBitsPerSample();
	const double fTmp = pow(10.0, fLevel / 20.0);
	for (i = 0; i < m_nBufSize; i++) {
		*pData++ = m_pSinTable[m_nWaveOutOffset] * fTmp + GetDither(nBitsPerSample);
		m_nWaveOutOffset = (m_nWaveOutOffset + nFreq) % m_nSample;
	}

	return i;
}

BOOL CDstDlg::WaveInData(const double *pData)
{
	BOOL bContinue = TRUE;
	int i;

	if (m_nGuardCnt > 0) {
		m_nGuardCnt--;
		return TRUE;
	}

	for (i = 0; i < m_nBufSize; i++) {
		m_pLeftData[m_nWaveInOffset] = *pData++;
		if (m_nChannel == 2)
			m_pRightData[m_nWaveInOffset] = *pData++;

		m_nWaveInOffset++;
	}

	if (m_nWaveInOffset == m_nSample) {
		CalcTHD();
		m_nWaveInOffset = 0;

		switch (g_oSetData.Dst.nMode) {
		case MODE_MANUAL:
			bContinue = m_cDstManual.NotifyTHD(m_pLeftDst, m_pRightDst);
			break;
		case MODE_FREQ_SWEEP:
			bContinue = m_cDstFreq.NotifyTHD(m_pLeftDst, m_pRightDst);
			break;
		case MODE_LEVEL_SWEEP:
			bContinue = m_cDstLevel.NotifyTHD(m_pLeftDst, m_pRightDst);
			break;
		}
	}

	return bContinue;
}

void CDstDlg::CalcTHD()
{
	double fLeftHarmonic = 0;
	double fRightHarmonic = 0;
	double fLeftTotal;
	double fRightTotal = 0;
	int i;

	for (i = 0; i < g_oSetData.Dst.nMaxHarmonics; i++) {
		m_pLeftDst[i] = GetHarmonic(m_pLeftData, i + 1);
		if (m_nChannel == 2)
			m_pRightDst[i] = GetHarmonic(m_pRightData, i + 1);

		if (i != 0) {
			fLeftHarmonic += m_pLeftDst[i];
			if (m_nChannel == 2)
				fRightHarmonic += m_pRightDst[i];
		}
	}

	fLeftTotal = m_pLeftDst[0] + fLeftHarmonic;
	m_pLeftDst[0] = fLeftHarmonic;
	if (m_nChannel == 2) {
		fRightTotal = m_pRightDst[0] + fRightHarmonic;
		m_pRightDst[0] = fRightHarmonic;
	}

	for (i = 0; i < g_oSetData.Dst.nMaxHarmonics; i++) {
		m_pLeftDst[i] = m_pLeftDst[i] / fLeftTotal;
		if (m_nChannel == 2)
			m_pRightDst[i] = m_pRightDst[i] / fRightTotal;
	}
}

double CDstDlg::GetHarmonic(const double *pData, int nHarmonic)
{
	int i;
	const int nStep = m_nFreq * nHarmonic;
	int nSinOffset = 0;
	int nCosOffset = m_nSample / 4;
	double fSinSum = 0;
	double fCosSum = 0;

	if (nStep >= m_nSample / 2)
		return 0;

	for (i = 0; i < m_nSample; i++) {
		fSinSum += pData[i] * m_pSinTable[nSinOffset];
		fCosSum += pData[i] * m_pSinTable[nCosOffset];

		nSinOffset = (nSinOffset + nStep) % m_nSample;
		nCosOffset = (nCosOffset + nStep) % m_nSample;
	}

	return fSinSum * fSinSum + fCosSum * fCosSum;
}

void CDstDlg::OnDestroy()
{
	if (m_bStart)
		Stop();

	CBaseDlg::OnDestroy();
}

void CDstDlg::OnCbnSelchangeSamplingRate()
{
	int nIndex;

	if ((nIndex = m_cSamplingRate.GetCurSel()) != CB_ERR) {
		BOOL bStart = m_bStart;

		if (bStart)
			Stop();

		g_oSetData.Dst.nSamplingRate = (int)m_cSamplingRate.GetItemData(nIndex);

		if (bStart)
			Start();
	}
}

void CDstDlg::OnBnClickedChannelMono()
{
	if (g_oSetData.Dst.nChannel != 0) {
		BOOL bStart = m_bStart;

		if (bStart)
			Stop();

		g_oSetData.Dst.nChannel = 0;

		if (bStart)
			Start();
	}
}

void CDstDlg::OnBnClickedChannelStereo()
{
	if (g_oSetData.Dst.nChannel != 1) {
		BOOL bStart = m_bStart;

		if (bStart)
			Stop();

		g_oSetData.Dst.nChannel = 1;

		if (bStart)
			Start();
	}
}

void CDstDlg::OnCbnSelchangeMaxHarmonics()
{
	int nIndex;

	if ((nIndex = m_cMaxHarmonics.GetCurSel()) != CB_ERR) {
		g_oSetData.Dst.nMaxHarmonics = (int)m_cMaxHarmonics.GetItemData(nIndex);
		RedrawGraph();
	}
}

void CDstDlg::OnBnClickedScalePercent()
{
	if (g_oSetData.Dst.nScaleMode != 0) {
		g_oSetData.Dst.nScaleMode = 0;
		SetScaleSelectAll();
		RedrawGraph();
	}
}

void CDstDlg::OnBnClickedScaleDb()
{
	if (g_oSetData.Dst.nScaleMode != 1) {
		g_oSetData.Dst.nScaleMode = 1;
		SetScaleSelectAll();
		RedrawGraph();
	}
}

void CDstDlg::OnCbnSelchangeScaleMax()
{
	const int nIndex = m_cScaleMax.GetCurSel();
	if (nIndex != CB_ERR) {
		g_oSetData.Dst.nScaleMax = (int)m_cScaleMax.GetItemData(nIndex);
		RedrawGraph();
	}
}

void CDstDlg::OnCbnSelchangeScaleMin()
{
	const int nIndex = m_cScaleMin.GetCurSel();
	if (nIndex != CB_ERR) {
		g_oSetData.Dst.nScaleMin = (int)m_cScaleMin.GetItemData(nIndex);
		RedrawGraph();
	}
}

void CDstDlg::RedrawGraph()
{
	switch (g_oSetData.Dst.nMode) {
	case MODE_MANUAL:
		m_cDstManual.Redraw();
		break;
	case MODE_FREQ_SWEEP:
		m_cDstFreq.Redraw();
		break;
	case MODE_LEVEL_SWEEP:
		m_cDstLevel.Redraw();
		break;
	}
}

void CDstDlg::OnTcnSelchangeDstTab(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	int	n;

	if ((n = m_cDstTab.GetCurSel()) != -1) {
		g_oSetData.Dst.nMode = n;
		Stop();
		EnableOutputFile();
		EnableDataHoldButton();
	}

	*pResult = 0;
}

void CDstDlg::OnCbnSelchangeGuardTime()
{
	const int nIndex = m_cGuardTime.GetCurSel();
	if (nIndex != CB_ERR)
		g_oSetData.Dst.nGuardCnt = (int)m_cGuardTime.GetItemData(nIndex);
}

void CDstDlg::OnBnClickedOutputFile()
{
	CFileDialog fileDlg(FALSE, "csv", NULL,
			OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
			"CSV File (*.csv)|*.csv|Text File (*.txt)|*.txt|All Files (*.*)|*.*||", this);

	if (fileDlg.DoModal() == IDOK) {
		switch (g_oSetData.Dst.nMode) {
		case MODE_MANUAL:
			m_cDstManual.CsvOutput(fileDlg.GetPathName());
			break;
		case MODE_FREQ_SWEEP:
			m_cDstFreq.CsvOutput(fileDlg.GetPathName());
			break;
		case MODE_LEVEL_SWEEP:
			m_cDstLevel.CsvOutput(fileDlg.GetPathName());
			break;
		}
	}
}

void CDstDlg::EnableOutputFile()
{
	BOOL bDataExist = CheckDataExist();

	m_cCsvOutput.EnableWindow(bDataExist);
	m_cScreenShot.EnableWindow(bDataExist);
}

BOOL CDstDlg::CheckDataExist()
{
	BOOL bDataExist = FALSE;

	switch (g_oSetData.Dst.nMode) {
	case MODE_MANUAL:
		bDataExist = m_cDstManual.CheckDataExist();
		break;
	case MODE_FREQ_SWEEP:
		bDataExist = m_cDstFreq.CheckDataExist();
		break;
	case MODE_LEVEL_SWEEP:
		bDataExist = m_cDstLevel.CheckDataExist();
		break;
	}

	return bDataExist;
}

BOOL CDstDlg::CheckDataHold()
{
	BOOL bDataHold = FALSE;

	switch (g_oSetData.Dst.nMode) {
	case MODE_FREQ_SWEEP:
		bDataHold = m_cDstFreq.CheckDataHold();
		break;
	case MODE_LEVEL_SWEEP:
		bDataHold = m_cDstLevel.CheckDataHold();
		break;
	}

	return bDataHold;
}

void CDstDlg::OnBnClickedScreenShot()
{
	CString sTitle;
	CString sTab;
	HBITMAP hBmp = NULL;
	m_cDstTab.GetCurrentTitle(sTab);
	const int nTimeCount = m_nTimeCount / 5;
	sTitle.Format("%s - %d:%02d:%02d", (LPCSTR)sTab, nTimeCount / 3600, nTimeCount / 60 % 60, nTimeCount % 60);

	switch (g_oSetData.Dst.nMode) {
	case MODE_MANUAL:
		hBmp = m_cDstManual.GetBitmap();
		break;
	case MODE_FREQ_SWEEP:
		hBmp = m_cDstFreq.GetBitmap();
		break;
	case MODE_LEVEL_SWEEP:
		hBmp = m_cDstLevel.GetBitmap();
		break;
	}

	m_oScreenShot.ShowScreenShot(sTitle, hBmp, TRUE);
}

BOOL CDstDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDC_BTN_START, IDH_DST_START,
		IDC_ELP_TIME, IDH_DST_ELP_TIME,
		IDC_SAMPLING_RATE, IDH_DST_SAMPLING_RATE,
		IDC_CHANNEL_MONO, IDH_DST_CHANNEL_MONO,
		IDC_CHANNEL_STEREO, IDH_DST_CHANNEL_STEREO,
		IDC_MAX_HARMONICS, IDH_DST_MAX_HARMONICS,
		IDC_GUARD_TIME, IDH_DST_GUARD_TIME,
		IDC_SCALE_PERCENT, IDH_DST_SCALE_PERCENT,
		IDC_SCALE_DB, IDH_DST_SCALE_DB,
		IDC_SCALE_MAX, IDH_DST_SCALE_MAX,
		IDC_SCALE_MIN, IDH_DST_SCALE_MIN,
		IDC_SCREEN_SHOT, IDH_DST_SCREEN_SHOT,
		IDC_CSV_OUTPUT, IDH_DST_CSV_OUTPUT,
		IDC_ADD_DATA_HOLD, IDH_DST_ADD_DATA_HOLD,
		IDC_DEL_DATA_HOLD, IDH_DST_DEL_DATA_HOLD,
		IDCANCEL, IDH_DST_CLOSE,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

void CDstDlg::OnBnClickedAddDataHold()
{
	CColorDialog oColorDlg;

	COLORREF cr = RGB(0, 0, 255);
	oColorDlg.m_cc.rgbResult = cr;
	oColorDlg.m_cc.Flags |= CC_RGBINIT;
	if (oColorDlg.DoModal() == IDOK) {
		COLORREF colorLeft = oColorDlg.m_cc.rgbResult;
		COLORREF colorRight = colorLeft ^ RGB(255, 0, 255);

		switch (g_oSetData.Dst.nMode) {
		case MODE_FREQ_SWEEP:
			m_cDstFreq.AddDataHold(colorLeft, colorRight);
			break;
		case MODE_LEVEL_SWEEP:
			m_cDstLevel.AddDataHold(colorLeft, colorRight);
			break;
		}

		RedrawGraph();

		EnableOutputFile();
		EnableDataHoldButton();
	}
}

void CDstDlg::OnBnClickedDelDataHold()
{
	switch (g_oSetData.Dst.nMode) {
	case MODE_FREQ_SWEEP:
		m_cDstFreq.DelDataHold();
		break;
	case MODE_LEVEL_SWEEP:
		m_cDstLevel.DelDataHold();
		break;
	}

	RedrawGraph();

	EnableDataHoldButton();
}

void CDstDlg::EnableDataHoldButton()
{
	m_cAddDataHold.EnableWindow(g_oSetData.Dst.nMode != 0 && CheckDataExist());
	m_cDelDataHold.EnableWindow(g_oSetData.Dst.nMode != 0 && CheckDataHold());
}

BOOL CDstDlg::DestroyWindow()
{
	CFile oFile;
	CString sDataDirectory;
	CString sPathName;

	GetDataDirectory(sDataDirectory);
	sPathName.Format("%s\\%s", (LPCSTR)sDataDirectory, FILENAME_HOLDDATA);

	if (oFile.Open(sPathName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone)) {
		m_cDstFreq.SaveHoldData(oFile);
		m_cDstLevel.SaveHoldData(oFile);

		oFile.Close();
	}

	return CBaseDlg::DestroyWindow();
}
