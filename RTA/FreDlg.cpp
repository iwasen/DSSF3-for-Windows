// FreDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "RTA.h"
#include "RtaDlg.h"
#include "FreDlg.h"
#include "WaveInEx.h"
#include "WaveOutEx.h"
#include "Regist.h"
#include "MicCalDlg.h"
#include "Directory.h"
#include "Help\ContextHelp.h"

#define MODE_SWEEP	0
#define MODE_SPOT	1
#define MODE_NOISE	2

#define FILENAME_HOLDDATA	"FreHold.dat"

// CFreDlg ダイアログ

CFreDlg::CFreDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CFreDlg::IDD, pParent)
{
	m_bInitialized = NULL;
	m_pWaveIn = CWaveInEx::GetInstance();
	m_pWaveOut = CWaveOutEx::GetInstance();
	m_bStart = FALSE;
	m_fTimeCount = 0;
	m_nWindowKind = WINDOW_FRE;

	Create(IDD, g_oSetData.Fre.nPosLeft, g_oSetData.Fre.nPosTop, g_oSetData2.nFontSizeFre, pParent);
}

CFreDlg::~CFreDlg()
{
	m_pWaveIn->ReleaseInstance();
	m_pWaveOut->ReleaseInstance();
}

void CFreDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SAMPLING_RATE, m_cSamplingRate);
	DDX_Control(pDX, IDC_FRE_TAB, m_cFreTab);
	DDX_Control(pDX, IDC_BTN_START, m_cBtnStart);
	DDX_Control(pDX, IDC_ELP_TIME, m_cElapseTime);
	DDX_Control(pDX, IDC_MAX_LEVEL, m_cMaxLevel);
	DDX_Control(pDX, IDC_MIN_LEVEL, m_cMinLevel);
	DDX_Radio(pDX, IDC_CHANNEL_MONO, m_iChannel);
	DDX_Control(pDX, IDC_CSV_OUTPUT, m_cCsvOutput);
	DDX_Control(pDX, IDC_MAX_LEVEL_SPIN, m_cMaxLevelSpin);
	DDX_Control(pDX, IDC_MIN_LEVEL_SPIN, m_cMinLevelSpin);
	DDX_Control(pDX, IDC_CAL_MESSAGE, m_cCalMessage);
	DDX_Control(pDX, IDC_ADD_DATA_HOLD, m_cAddDataHold);
	DDX_Control(pDX, IDC_DEL_DATA_HOLD, m_cDelDataHold);
	DDX_Control(pDX, IDC_SCREEN_SHOT, m_cScreenShot);
}

BEGIN_MESSAGE_MAP(CFreDlg, CBaseDlg)
	ON_CBN_SELCHANGE(IDC_SAMPLING_RATE, &CFreDlg::OnCbnSelchangeSamplingRate)
	ON_BN_CLICKED(IDC_CHANNEL_MONO, &CFreDlg::OnBnClickedChannelMono)
	ON_BN_CLICKED(IDC_CHANNEL_STEREO, &CFreDlg::OnBnClickedChannelStereo)
	ON_EN_CHANGE(IDC_MAX_LEVEL, &CFreDlg::OnEnChangeMaxLevel)
	ON_EN_CHANGE(IDC_MIN_LEVEL, &CFreDlg::OnEnChangeMinLevel)
	ON_NOTIFY(TCN_SELCHANGE, IDC_FRE_TAB, &CFreDlg::OnTcnSelchangeFreTab)
	ON_BN_CLICKED(IDC_BTN_START, &CFreDlg::OnBnClickedStart)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CSV_OUTPUT, &CFreDlg::OnBnClickedCsvOutput)
	ON_BN_CLICKED(IDC_SCREEN_SHOT, &CFreDlg::OnBnClickedScreenShot)
	ON_WM_HELPINFO()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_SET_CALIBRATION, &CFreDlg::OnBnClickedSetCalibration)
	ON_BN_CLICKED(IDC_ADD_DATA_HOLD, &CFreDlg::OnBnClickedAddDataHold)
	ON_BN_CLICKED(IDC_DEL_DATA_HOLD, &CFreDlg::OnBnClickedDelDataHold)
	ON_BN_CLICKED(IDC_CSV_INPUT, &CFreDlg::OnBnClickedCsvInput)
END_MESSAGE_MAP()

// CFreDlg メッセージ ハンドラ

BOOL CFreDlg::OnInitDialog()
{
	CBaseDlg::OnInitDialog();

	CreateStatusBar();
	SetStatusBar(IDS_STATUS_STOP);

	m_cFreTab.SetFontSize(g_oSetData2.nFontSizeFre);
	m_cFreTab.AddPage(&m_oFreSweep, IDD_FRE_SWEEP, IDS_FREQ_SWEEP, TRUE);
	m_cFreTab.AddPage(&m_oFreSpot, IDD_FRE_SPOT, IDS_SPOT_FREQ, TRUE);
	m_cFreTab.AddPage(&m_oFreNoise, IDD_FRE_NOISE, IDS_PINK_NOISE, TRUE);
	m_cFreTab.SetPage(g_oSetData.Fre.nMode);

	RestoreHoldData();

	EnableDataHoldButton();

	CWnd *pWnd = GetTopWindow();
	while (pWnd != NULL) {
		switch (pWnd->GetDlgCtrlID()) {
		case IDC_FRE_TAB:
		case 0:
			SetCtlPosition(pWnd, 0, 0, 1, 1);
			break;
		default:
			SetCtlPosition(pWnd, 1, 0, 1, 0);
			break;
		}
		pWnd = pWnd->GetNextWindow();
	}

	SetSamplingRateList(m_cSamplingRate, WAVE_IN | WAVE_OUT, g_nWaveInDevice, g_nWaveOutDevice, g_oSetData.Fre.nSamplingRate);

	m_iChannel = g_oSetData.Fre.nChannel;

	m_cMaxLevel = g_oSetData.Fre.nMaxLevel;
	m_cMinLevel = g_oSetData.Fre.nMinLevel;

	m_cMaxLevelSpin.SetRange(-999, 999);
	m_cMinLevelSpin.SetRange(-999, 999);

	SetMicCalData();

	UpdateData(FALSE);

	m_bInitialized = TRUE;

	return TRUE;
}

void CFreDlg::RestoreHoldData()
{
	CString sDataDirectory;
	CString sPathName;

	GetDataDirectory(sDataDirectory);
	sPathName.Format("%s\\%s", (LPCSTR)sDataDirectory, FILENAME_HOLDDATA);

	CFile oFile;
	if (oFile.Open(sPathName, CFile::modeRead | CFile::shareDenyNone)) {
		m_oFreSweep.LoadHoldData(oFile);
		m_oFreSpot.LoadHoldData(oFile);
		m_oFreNoise.LoadHoldData(oFile);

		oFile.Close();
	}
}

void CFreDlg::ChangeWaveDevice(int /*nInOut*/)
{
	if (m_bStart)
		Stop();

	SetSamplingRateList(m_cSamplingRate, WAVE_IN | WAVE_OUT, g_nWaveInDevice, g_nWaveOutDevice, g_oSetData.Fre.nSamplingRate);
	OnCbnSelchangeSamplingRate();
}

int CFreDlg::OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify)
{
	int nTimeCount;

	switch (nCode) {
	case WAVEOUT_OPEN:
		m_bStart = TRUE;
		m_cBtnStart.SetCheck(m_bStart);
		m_cBtnStart.SetWindowText(GetString(IDS_STOP));
		pWaveNotify->nWindow = WINDOW_FRE;
		break;
	case WAVEOUT_CLOSE:
		m_pWaveIn->Close();
		m_bStart = FALSE;
		m_cBtnStart.SetCheck(m_bStart);
		m_cBtnStart.SetWindowText(GetString(IDS_START));
		EnableOutputFile();
		EnableDataHoldButton();
		break;
	case WAVEOUT_DATA:
		return WaveOutData(pWaveNotify->pSamplesData);
	case WAVEIN_OPEN:
		SetStatusBar(GetRunningStatus(IDS_STATUS_RUNNING, m_pWaveIn->GetBitsPerSample(), m_pWaveOut->GetBitsPerSample(), 0));
		pWaveNotify->nWindow = WINDOW_FRE;
		break;
	case WAVEIN_DATA:
		m_fTimeCount += (double)m_nBufSize / m_nSamplingRate;
		nTimeCount = (int)m_fTimeCount;
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

void CFreDlg::OnBnClickedStart()
{
	if (m_bStart)
		Stop();
	else {
		if (!Start())
			m_cBtnStart.SetCheck(FALSE);
	}
}

void CFreDlg::OnCbnSelchangeSamplingRate()
{
	int nIndex;

	if ((nIndex = m_cSamplingRate.GetCurSel()) != CB_ERR) {
		BOOL bStart = m_bStart;

		if (bStart)
			Stop();

		g_oSetData.Fre.nSamplingRate = (int)m_cSamplingRate.GetItemData(nIndex);

		m_oFreNoise.SetResolutionList();

		if (bStart)
			Start();
	}
}

void CFreDlg::OnBnClickedChannelMono()
{
	if (g_oSetData.Fre.nChannel != 0) {
		BOOL bStart = m_bStart;

		if (bStart)
			Stop();

		g_oSetData.Fre.nChannel = 0;

		if (bStart)
			Start();
	}
}

void CFreDlg::OnBnClickedChannelStereo()
{
	if (g_oSetData.Fre.nChannel != 1) {
		BOOL bStart = m_bStart;

		if (bStart)
			Stop();

		g_oSetData.Fre.nChannel = 1;

		if (bStart)
			Start();
	}
}

void CFreDlg::OnEnChangeMaxLevel()
{
	if (m_cMaxLevel.m_hWnd) {
		const int nMaxLevel = m_cMaxLevel;
		if (nMaxLevel != g_oSetData.Fre.nMaxLevel) {
			g_oSetData.Fre.nMaxLevel = nMaxLevel;
			RedrawGraph();
		}
	}
}

void CFreDlg::OnEnChangeMinLevel()
{
	if (m_cMinLevel.m_hWnd) {
		const int nMinLevel = m_cMinLevel;
		if (nMinLevel != g_oSetData.Fre.nMinLevel) {
			g_oSetData.Fre.nMinLevel = nMinLevel;
			RedrawGraph();
		}
	}
}

void CFreDlg::OnTcnSelchangeFreTab(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	int	n;

	if ((n = m_cFreTab.GetCurSel()) != -1) {
		g_oSetData.Fre.nMode = n;
		Stop();
		EnableOutputFile();
		EnableDataHoldButton();
	}

	*pResult = 0;
}

void CFreDlg::OnBnClickedScreenShot()
{
	CString sTitle;
	CString sTab;
	HBITMAP hBmp = NULL;
	m_cFreTab.GetCurrentTitle(sTab);
	const int nTimeCount = (int)m_fTimeCount;
	sTitle.Format("%s - %d:%02d:%02d", (LPCSTR)sTab, nTimeCount / 3600, nTimeCount / 60 % 60, nTimeCount % 60);

	switch (g_oSetData.Fre.nMode) {
	case MODE_SWEEP:
		hBmp = m_oFreSweep.GetBitmap();
		break;
	case MODE_SPOT:
		hBmp = m_oFreSpot.GetBitmap();
		break;
	case MODE_NOISE:
		hBmp = m_oFreNoise.GetBitmap();
		break;
	}

	m_oScreenShot.ShowScreenShot(sTitle, hBmp, TRUE);
}

void CFreDlg::OnBnClickedCsvOutput()
{
	CFileDialog fileDlg(FALSE, "csv", NULL,
			OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
			"CSV File (*.csv)|*.csv|Text File (*.txt)|*.txt|All Files (*.*)|*.*||", this);

	if (fileDlg.DoModal() == IDOK) {
		switch (g_oSetData.Fre.nMode) {
		case MODE_SWEEP:

			m_oFreSweep.CsvOutput(fileDlg.GetPathName());
			break;
		case MODE_SPOT:
			m_oFreSpot.CsvOutput(fileDlg.GetPathName());
			break;
		case MODE_NOISE:
			m_oFreNoise.CsvOutput(fileDlg.GetPathName());
			break;
		}
	}
}

void CFreDlg::OnBnClickedCsvInput()
{
	CFileDialog fileDlg(TRUE, "csv", NULL,
			OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
			"CSV File (*.csv)|*.csv|Text File (*.txt)|*.txt|All Files (*.*)|*.*||", this);

	if (fileDlg.DoModal() == IDOK) {
		CCsvFile oCsvFile;

		if (oCsvFile.Open(fileDlg.GetPathName(), CFile::modeRead | CFile::shareDenyNone)) {
			oCsvFile.Input();

			CString sHeader = oCsvFile.GetString(0);
			if (sHeader != "Frequency Response") {
				::AfxMessageBox(IDS_ERR_CSV_FORMAT, MB_OK | MB_ICONEXCLAMATION);
				return;
			}

			sHeader = oCsvFile.GetString(1);
			if (sHeader == "Sweep") {
				g_oSetData.Fre.nMode = MODE_SWEEP;
				m_oFreSweep.ReadCsv(oCsvFile);
			} else if (sHeader == "Spot") {
				g_oSetData.Fre.nMode = MODE_SPOT;
				m_oFreSpot.ReadCsv(oCsvFile);
			} else if (sHeader == "Pink Noise") {
				g_oSetData.Fre.nMode = MODE_NOISE;
				m_oFreNoise.ReadCsv(oCsvFile);
			} else {
				::AfxMessageBox(IDS_ERR_CSV_FORMAT, MB_OK | MB_ICONEXCLAMATION);
				return;
			}

			m_cFreTab.SetPage(g_oSetData.Fre.nMode);
			EnableDataHoldButton();
		}
	}
}

void CFreDlg::OnDestroy()
{
	if (m_bStart)
		Stop();

	CBaseDlg::OnDestroy();
}

BOOL CFreDlg::Start()
{
	m_nSamplingRate = g_oSetData.Fre.nSamplingRate;
	m_nChannel = g_oSetData.Fre.nChannel + 1;
	m_fTimeCount = 0;
	m_bRegist = (BOOL)CHECK_LICENSE2(APP_RA);
	m_bNoRegist = FALSE;

	switch (g_oSetData.Fre.nMode) {
	case MODE_SWEEP:
		m_nBufSize = m_oFreSweep.Initialize();
		break;
	case MODE_SPOT:
		m_nBufSize = m_oFreSpot.Initialize();
		break;
	case MODE_NOISE:
		m_nBufSize = m_oFreNoise.Initialize();
		break;
	}

	if (!m_pWaveOut->Open(g_nWaveOutDevice, this, 1, g_oSetData.Fre.nSamplingRate, m_nBufSize, 3, TRUE, TRUE))
		return FALSE;

	if (!m_pWaveIn->Open(g_nWaveInDevice, this, m_nChannel, g_oSetData.Fre.nSamplingRate, m_nBufSize, 3, FALSE)) {
		m_pWaveOut->Close();
		::AfxMessageBox(IDS_MSG_SOUNDINOUT);
		return FALSE;
	}

	StartWaveInOut(m_pWaveIn, m_pWaveOut, FALSE);

	return TRUE;
}

void CFreDlg::Stop()
{
	m_pWaveOut->Close();
	m_pWaveIn->Close();
}

void CFreDlg::Restart()
{
	if (m_bStart) {
		Stop();
		Start();
	}
}

void CFreDlg::RedrawGraph()
{
	switch (g_oSetData.Fre.nMode) {
	case MODE_SWEEP:
		m_oFreSweep.Redraw();
		break;
	case MODE_SPOT:
		m_oFreSpot.Redraw();
		break;
	case MODE_NOISE:
		m_oFreNoise.Redraw();
		break;
	}
}

int CFreDlg::WaveOutData(double *pData)
{
	BOOL bReset = FALSE;

	switch (g_oSetData.Fre.nMode) {
	case MODE_SWEEP:
		bReset = m_oFreSweep.WaveOutData(pData, m_pWaveOut->GetBitsPerSample());
		break;
	case MODE_SPOT:
		bReset = m_oFreSpot.WaveOutData(pData, m_pWaveOut->GetBitsPerSample());
		break;
	case MODE_NOISE:
		bReset = m_oFreNoise.WaveOutData(pData);
		break;
	}

	if (bReset)
		m_pWaveOut->Reset();

	return m_nBufSize;
}

BOOL CFreDlg::WaveInData(const double *pData)
{
	BOOL bContinue = FALSE;

	switch (g_oSetData.Fre.nMode) {
	case MODE_SWEEP:
		bContinue = m_oFreSweep.WaveInData(pData);
		break;
	case MODE_SPOT:
		bContinue = m_oFreSpot.WaveInData(pData);
		break;
	case MODE_NOISE:
		bContinue = m_oFreNoise.WaveInData(pData);
		break;
	}

	return bContinue;
}

void CFreDlg::EnableOutputFile()
{
	BOOL bDataExist = CheckDataExist();

	m_cCsvOutput.EnableWindow(bDataExist);
	m_cScreenShot.EnableWindow(bDataExist);
}

BOOL CFreDlg::CheckDataExist()
{
	BOOL bDataExist = FALSE;

	switch (g_oSetData.Fre.nMode) {
	case MODE_SWEEP:
		bDataExist = m_oFreSweep.CheckDataExist();
		break;
	case MODE_SPOT:
		bDataExist = m_oFreSpot.CheckDataExist();
		break;
	case MODE_NOISE:
		bDataExist = m_oFreNoise.CheckDataExist();
		break;
	}

	return bDataExist;
}

BOOL CFreDlg::CheckDataHold()
{
	BOOL bDataHold = FALSE;

	switch (g_oSetData.Fre.nMode) {
	case MODE_SWEEP:
		bDataHold = m_oFreSweep.CheckDataHold();
		break;
	case MODE_SPOT:
		bDataHold = m_oFreSpot.CheckDataHold();
		break;
	case MODE_NOISE:
		bDataHold = m_oFreNoise.CheckDataHold();
		break;
	}

	return bDataHold;
}

BOOL CFreDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static constexpr UINT aIDs[] = {
		IDC_BTN_START, IDH_FRE_START,
		IDC_ELP_TIME, IDH_FRE_ELP_TIME,
		IDC_SAMPLING_RATE, IDH_FRE_SAMPLING_RATE,
		IDC_CHANNEL_MONO, IDH_FRE_CHANNEL_MONO,
		IDC_CHANNEL_STEREO, IDH_FRE_CHANNEL_STEREO,
		IDC_MAX_LEVEL, IDH_FRE_MAX_LEVEL,
		IDC_MIN_LEVEL, IDH_FRE_MIN_LEVEL,
		IDC_SET_CALIBRATION, IDH_FFT_CALIBRATION,
		IDC_CAL_MESSAGE, IDH_FFT_MESSAGE,
		IDC_SCREEN_SHOT, IDH_FRE_SCREEN_SHOT,
		IDC_CSV_OUTPUT, IDH_FRE_CSV_OUTPUT,
		IDC_CSV_INPUT, IDH_FRE_CSV_INPUT,
		IDC_ADD_DATA_HOLD, IDH_FRE_ADD_DATA_HOLD,
		IDC_DEL_DATA_HOLD, IDH_FRE_DEL_DATA_HOLD,
		IDCANCEL, IDH_FRE_EXIT,
		0
	};

	::DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

void CFreDlg::OnMove(int x, int y)
{
	CBaseDlg::OnMove(x, y);

	if (m_bInitialized && !IsIconic()) {
		CRect rect;
		GetWindowRect(rect);
		g_oSetData.Fre.nPosLeft = rect.left;
		g_oSetData.Fre.nPosTop = rect.top;
	}
}

void CFreDlg::OnBnClickedSetCalibration()
{
	int nInputDevice, nInputVolume;
	CMicCalDlg dlg(this);

	GetInputDevice(&nInputDevice, &nInputVolume);

	dlg.m_MicCalID = g_oSetData.Fre.nMicCalID;
	if (dlg.DoModal() == IDOK) {
		g_oSetData.Fre.nMicCalID = dlg.m_MicCalID;
		SetMicCalData();

		if (g_oSetData.Fre.nMicCalID == -1) {
			if (g_oSetData.Fre.nMaxLevel > 0) {
				m_cMaxLevel = 0;
				m_cMinLevel = -50;
				//RedrawGraph();
			}
		} else{
			if (g_oSetData.Fre.nMaxLevel <= 0) {
				m_cMaxLevel = 100;
				m_cMinLevel = 0;
				//RedrawGraph();
			}
		}
	}

	SetInputDevice(nInputDevice, nInputVolume);
}

void CFreDlg::GetInputDevice(int *pInputDevice, int *pInputVolume)
{
	CRtaApp *pWinApp = (CRtaApp *)::AfxGetApp();
	pWinApp->m_pWndRta->GetInputDevice(pInputDevice, pInputVolume);
}

void CFreDlg::SetInputDevice(int nInputDevice, int nInputVolume)
{
	CRtaApp *pWinApp = (CRtaApp *)::AfxGetApp();
	pWinApp->m_pWndRta->SetInputDevice(nInputDevice, nInputVolume);
}

void CFreDlg::SetMicCalData()
{
	if (g_oSetData.Fre.nMicCalID != -1) {
		CDbMicCal dbMicCal;

		if (dbMicCal.Open()) {
			if (dbMicCal.ReadRecID(g_oSetData.Fre.nMicCalID, &m_oMicCalDataL)) {
				if (dbMicCal.SearchRecID(-g_oSetData.Fre.nMicCalID - 1))
					dbMicCal.ReadRecID(-g_oSetData.Fre.nMicCalID - 1, &m_oMicCalDataR);
				else
					m_oMicCalDataR = m_oMicCalDataL;

				m_cCalMessage = m_oMicCalDataL.sMicInfo;

				return;
			}
		}
	}

	m_oMicCalDataL.fInputSens = 0;
	m_oMicCalDataL.nFreqData = 0;
	m_oMicCalDataR.fInputSens = 0;
	m_oMicCalDataR.nFreqData = 0;

	m_cCalMessage.Blank();
}

void CFreDlg::OnBnClickedAddDataHold()
{
	CColorDialog oColorDlg;

	COLORREF cr = RGB(0, 0, 255);
	oColorDlg.m_cc.rgbResult = cr;
	oColorDlg.m_cc.Flags |= CC_RGBINIT;
	if (oColorDlg.DoModal() == IDOK) {
		COLORREF colorLeft = oColorDlg.m_cc.rgbResult;
		COLORREF colorRight = colorLeft ^ RGB(255, 0, 255);

		switch (g_oSetData.Fre.nMode) {
		case MODE_SWEEP:
			m_oFreSweep.AddDataHold(colorLeft, colorRight);
			break;
		case MODE_SPOT:
			m_oFreSpot.AddDataHold(colorLeft, colorRight);
			break;
		case MODE_NOISE:
			m_oFreNoise.AddDataHold(colorLeft, colorRight);
			break;
		}

		RedrawGraph();

		EnableOutputFile();
		EnableDataHoldButton();
	}
}

void CFreDlg::OnBnClickedDelDataHold()
{
	switch (g_oSetData.Fre.nMode) {
	case MODE_SWEEP:
		m_oFreSweep.DelDataHold();
		break;
	case MODE_SPOT:
		m_oFreSpot.DelDataHold();
		break;
	case MODE_NOISE:
		m_oFreNoise.DelDataHold();
		break;
	}

	RedrawGraph();

	EnableDataHoldButton();
}

void CFreDlg::EnableDataHoldButton()
{
	m_cAddDataHold.EnableWindow(CheckDataExist());
	m_cDelDataHold.EnableWindow(CheckDataHold());
}

BOOL CFreDlg::DestroyWindow()
{
	CString sDataDirectory;
	CString sPathName;

	GetDataDirectory(sDataDirectory);
	sPathName.Format("%s\\%s", (LPCSTR)sDataDirectory, FILENAME_HOLDDATA);

	CFile oFile;
	if (oFile.Open(sPathName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone)) {
		m_oFreSweep.SaveHoldData(oFile);
		m_oFreSpot.SaveHoldData(oFile);
		m_oFreNoise.SaveHoldData(oFile);

		oFile.Close();
	}

	return CBaseDlg::DestroyWindow();
}
