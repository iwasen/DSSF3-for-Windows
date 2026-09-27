// RtaDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "PresetDlg.h"
#include "RtaDlg.h"
#include "Mixer.h"
#include "WaveInEx.h"
#include "WaveOutEx.h"
#include "VerRes.h"
#include "SgDlg.h"
#include "FftDlg.h"
#include "OsDlg.h"
#include "FreDlg.h"
#include "DstDlg.h"
#include "ImpulseDlg.h"
#include "AcfDlg.h"
#include "RecDlg.h"
#include "FilterDlg.h"
#include "HyperLink.h"
#include "Setting.h"
#include "Regist.h"
#include "DataBackupDlg.h"
#include "DataRestoreDlg.h"
#include "AutoRecDlg.h"
#include "WasapiManager.h"
#include "Help\ContextHelp.h"
#include <dwmapi.h>

/////////////////////////////////////////////////////////////////////////////
// アプリケーションのバージョン情報で使われている CAboutDlg ダイアログ

class CAboutDlg : public CDialogAF
{
public:
	CAboutDlg();

protected:
	enum { IDD = IDD_ABOUTBOX };

	CHyperLink m_cProductURL;
	CHyperLink m_cCompanyURL;
	CHyperLink m_cEmailAddr;
	CHyperLink m_cYmecStoreURL;

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV のサポート
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialogAF(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PRODUCT_URL, m_cProductURL);
	DDX_Control(pDX, IDC_COMPANY_URL, m_cCompanyURL);
	DDX_Control(pDX, IDC_EMAIL_ADDR, m_cEmailAddr);
	DDX_Control(pDX, IDC_YMEC_STORE, m_cYmecStoreURL);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogAF)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	CVerRes		res;
	CString sVersion;
	sVersion = res.GetProductName() + "  Version " + res.GetProductVersion();
#ifdef BUILD_DEV
	sVersion += ", dev";
#endif
#ifdef _WIN64
	sVersion += " (64bit)";
#else
	sVersion += " (32bit)";
#endif
	SetDlgItemText(IDC_PRODUCT, sVersion);
	SetDlgItemText(IDC_COPYRIGHT, res.GetLegalCopyright());

	m_cProductURL.SetHyperLinkURL(NULL);
	m_cYmecStoreURL.SetHyperLinkURL(NULL);
	m_cCompanyURL.SetHyperLinkURL(NULL);
	m_cEmailAddr.SetHyperLinkURL("mailto:ymec@ymec.com");

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CRtaDlg ダイアログ

CRtaDlg::CRtaDlg(CWnd* pParent /*=NULL*/)
	: CDialogAF(CRtaDlg::IDD, pParent)
{
	CWinApp *pWinApp = ::AfxGetApp();

	m_hIcon = pWinApp->LoadIcon(IDR_MAINFRAME);
	m_hIcon16 = (HICON)LoadImage(pWinApp->m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

	m_pWndSg = NULL;
	m_pWndFft = NULL;
	m_pWndOs = NULL;
	m_pWndFre = NULL;
	m_pWndDst = NULL;
	m_pWndImp = NULL;
	m_pWndAcf = NULL;
	m_pWndRec = NULL;
	m_pWndFilter = NULL;
	m_pWndTime = NULL;

	Create(IDD, pParent);
}

void CRtaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogAF::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RA_OUTPUT_LEVEL, m_cOutputLevel);
	DDX_Control(pDX, IDC_RA_INPUT_LEVEL, m_cInputLevel);
	DDX_Control(pDX, IDC_RA_INPUT_VOLUME, m_cInputVolume);
	DDX_Control(pDX, IDC_RA_INPUT_SELECT, m_cInputSelect);
	DDX_Control(pDX, IDC_RA_INPUT_DEVICE, m_cInputDevice);
	DDX_Control(pDX, IDC_RA_OUTPUT_DEVICE, m_cOutputDevice);
	DDX_Control(pDX, IDC_RA_SG, m_cRaSg);
	DDX_Control(pDX, IDC_RA_FFT, m_cRaFft);
	DDX_Control(pDX, IDC_RA_OS, m_cRaOs);
	DDX_Control(pDX, IDC_RA_FRE, m_cRaFre);
	DDX_Control(pDX, IDC_RA_DST, m_cRaDst);
	DDX_Control(pDX, IDC_RA_IMP, m_cRaImp);
	DDX_Control(pDX, IDC_RA_ACF, m_cRaAcf);
	DDX_Control(pDX, IDC_RA_REC, m_cRaRec);
//	DDX_Control(pDX, IDC_RA_FILTER, m_cRaFilter);
}

BEGIN_MESSAGE_MAP(CRtaDlg, CDialogAF)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_RA_SG, OnRaSg)
	ON_BN_CLICKED(IDC_RA_FFT, OnRaFft)
	ON_BN_CLICKED(IDC_RA_OS, OnRaOs)
	ON_BN_CLICKED(IDC_RA_FRE, OnRaFre)
	ON_BN_CLICKED(IDC_RA_DST, OnRaDst)
	ON_BN_CLICKED(IDC_RA_IMP, OnRaImp)
	ON_BN_CLICKED(IDC_RA_ACF, OnRaAcf)
	ON_BN_CLICKED(IDC_RA_REC, OnRaRec)
//	ON_BN_CLICKED(IDC_RA_FILTER, OnRaFilter)
	ON_BN_CLICKED(IDC_RA_PRESET, OnRaPreset)
	ON_CBN_SELCHANGE(IDC_RA_INPUT_SELECT, OnSelchangeRaInputSelect)
	ON_WM_HSCROLL()
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_RA_INPUT_DEVICE, OnCbnSelchangeRaInputDevice)
	ON_CBN_SELCHANGE(IDC_RA_OUTPUT_DEVICE, OnCbnSelchangeRaOutputDevice)
	ON_COMMAND(IDM_ABOUTBOX, OnAboutbox)
	ON_COMMAND(IDM_DOCUMENT_QA, OnDocumentQa)
	ON_COMMAND(IDM_TECHNICAL_SUPPORT, OnTechnicalSupport)
	ON_COMMAND(IDM_ONLINE_MANUAL, OnOnlineManual)
	ON_COMMAND(IDM_ONLINE_UPDATE, OnOnlineUpdate)
	ON_COMMAND(IDM_RA_SG, OnRaSg)
	ON_COMMAND(IDM_RA_FFT, OnRaFft)
	ON_COMMAND(IDM_RA_OS, OnRaOs)
	ON_COMMAND(IDM_RA_FRE, OnRaFre)
	ON_COMMAND(IDM_RA_DST, OnRaDst)
	ON_COMMAND(IDM_RA_IMP, OnRaImp)
	ON_COMMAND(IDM_RA_ACF, OnRaAcf)
	ON_COMMAND(IDM_RA_REC, OnRaRec)
	ON_COMMAND(IDM_RA_AUTOREC, OnRaAutorec)
	ON_COMMAND(IDM_RA_FILTER, OnRaFilter)
	ON_COMMAND(IDM_RA_PRESET, OnRaPreset)
	ON_COMMAND(IDM_README, OnReadme)
	ON_COMMAND(IDM_YMEC_STORE, OnYmecStore)
	ON_COMMAND(IDM_LICENSE_REGIST, OnLicenseRegist)
	ON_UPDATE_COMMAND_UI(IDM_LICENSE_REGIST, OnUpdateLicenseRegist)
	ON_WM_INITMENU()
	ON_COMMAND(IDM_RA_TIME, OnRaTime)
	ON_COMMAND(IDM_SETTING, OnSetting)
	ON_WM_SETCURSOR()
	ON_WM_HELPINFO()
	ON_COMMAND(IDM_HELP_FILE, &CRtaDlg::OnHelpFile)
	ON_COMMAND(IDM_PDF_FILE, &CRtaDlg::OnPdfFile)
	ON_COMMAND(IDM_DATA_BACKUP, &CRtaDlg::OnDataBackup)
	ON_COMMAND(IDM_DATA_RESTORE, &CRtaDlg::OnDataRestore)
	ON_MESSAGE(WM_CHANGE_FONT_SIZE, OnChangeFontSize)
	ON_COMMAND(IDM_TRANSFER_MMLIB, &CRtaDlg::OnTransferMmlib)
	ON_BN_CLICKED(IDC_RA_AUTOREC, &CRtaDlg::OnBnClickedRaAutorec)
	ON_MESSAGE(WM_AUTO_REC_FILE, OnAutoRecFile)
//	ON_MESSAGE(MM_MIXM_LINE_CHANGE, OnMixmLineChange)
//	ON_MESSAGE(MM_MIXM_CONTROL_CHANGE , OnMixmControlChange)
	ON_MESSAGE(WM_NOTIFY_DEVICE_CHANGE, OnNotifyDeviceChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRtaDlg メッセージ ハンドラ

BOOL CRtaDlg::OnInitDialog()
{
	CDialogAF::OnInitDialog();

	if (m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC)) {
		m_wndToolBar.LoadToolBar(IDR_TOOLBAR_MAIN);
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

		CSize sizeToolBar = m_wndToolBar.CalcFixedLayout(FALSE, TRUE);

		// ダイアログの左上にツールバー表示
		m_wndToolBar.MoveWindow(0, 0, sizeToolBar.cx, sizeToolBar.cy);

	}

	// IDM_ABOUTBOX はコマンド メニューの範囲でなければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	CString strAboutMenu;
	strAboutMenu.LoadString(IDS_ABOUTBOX);
	if (!strAboutMenu.IsEmpty())
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	}

	CString strOnlineUpdate;
	strOnlineUpdate.LoadString(IDS_ONLINE_UPDATE);
	if (!strOnlineUpdate.IsEmpty())
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ONLINE_UPDATE, strOnlineUpdate);
	}

	// このダイアログ用のアイコンを設定します。フレームワークはアプリケーションのメイン
	// ウィンドウがダイアログでない時は自動的に設定しません。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンを設定
	SetIcon(m_hIcon16, FALSE);		// 小さいアイコンを設定

	CRect rect;
	GetWindowRect(rect);
	SetWindowPos(0, (GetSystemMetrics(SM_CXSCREEN) - rect.Width()) / 2, 4, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	m_cInputVolume.SetRange(0, 100);
	m_cInputVolume.SetTicFreq(10);
	m_cInputVolume.SetPageSize(10);

	SetSoundDevice();

	SelectSoundDevice();

	m_cInputLevel.Initialize();
	m_cOutputLevel.Initialize();

	CWaveInEx::SetMonitor(this);
	CWaveOutEx::SetMonitor(this);

	CRtaApp *pWinApp = (CRtaApp *)::AfxGetApp();
	if (pWinApp->m_nPresetID != -1)
		OpenPreset(pWinApp->m_nPresetID);

//	HMIXER hMixer;
//	MMRESULT result = mixerOpen(&hMixer, dwMixerInDevice, (DWORD_PTR)m_hWnd, 0, CALLBACK_WINDOW);
//	result = 0;

	g_cWasapiManager.SetNorifyDeviceChange(m_hWnd);

	return TRUE;
}

void CRtaDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	switch (nID & 0xFFF0) {
	case IDM_ABOUTBOX:
		{
			CAboutDlg dlgAbout;
			dlgAbout.DoModal();
		}
		break;
	case IDM_ONLINE_UPDATE:
		((CRtaApp *)::AfxGetApp())->OnOnlineUpdate();
		break;
	case SC_MINIMIZE:
		::AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);
		break;
	default:
		CDialogAF::OnSysCommand(nID, lParam);
		break;
	}
}

// もしダイアログボックスに最小化ボタンを追加するならば、アイコンを描画する
// コードを以下に記述する必要があります。MFC アプリケーションは document/view
// モデルを使っているので、この処理はフレームワークにより自動的に処理されます。

void CRtaDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画用のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// クライアントの矩形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンを描画します。
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this); // 描画用のデバイス コンテキスト
		CFont *pFont;
		CString sText;
		CSize size;
		CRect rect;
		int x;
		int i;
		CPen pen1(PS_SOLID, 0, RGB(128, 128, 128));
		CPen *pPen;

		WINDOWPLACEMENT wp;
		m_cOutputLevel.GetWindowPlacement(&wp);
		m_cOutputLevel.GetClientRect(rect);
		pFont = dc.SelectObject(GetFont());
		pPen = dc.SelectObject(&pen1);
		dc.SetBkMode(TRANSPARENT);
		for (i = 0; i <= 8; i++) {
			x = wp.rcNormalPosition.left + rect.Width() * i / 8;
			dc.MoveTo(x, wp.rcNormalPosition.bottom);
			dc.LineTo(x, wp.rcNormalPosition.bottom + 4);
			sText.Format("%d", i * 10 - 80);
			size = dc.GetOutputTextExtent(sText);
			dc.TextOut(x - size.cx / 2, wp.rcNormalPosition.bottom + 5, sText);
		}
		dc.SelectObject(pFont);
		dc.SelectObject(pPen);
	}
}

void CRtaDlg::OnRaSg()
{
	if (m_pWndSg == NULL)
		OpenRaWindow(WINDOW_SG);
	else
		CloseRaWindow(WINDOW_SG);
}

void CRtaDlg::OnRaFft()
{
	if (m_pWndFft == NULL)
		OpenRaWindow(WINDOW_FFT);
	else
		CloseRaWindow(WINDOW_FFT);
}

void CRtaDlg::OnRaOs()
{
	if (m_pWndOs == NULL)
		OpenRaWindow(WINDOW_OS);
	else
		CloseRaWindow(WINDOW_OS);
}

void CRtaDlg::OnRaFre()
{
	if (m_pWndFre == NULL)
		OpenRaWindow(WINDOW_FRE);
	else
		CloseRaWindow(WINDOW_FRE);
}

void CRtaDlg::OnRaDst()
{
	if (m_pWndDst == NULL)
		OpenRaWindow(WINDOW_DST);
	else
		CloseRaWindow(WINDOW_DST);
}

void CRtaDlg::OnRaImp()
{
	if (m_pWndImp == NULL)
		OpenRaWindow(WINDOW_IMP);
	else
		CloseRaWindow(WINDOW_IMP);
}

void CRtaDlg::OnRaAcf()
{
	if (m_pWndAcf == NULL)
		OpenRaWindow(WINDOW_ACF);
	else
		CloseRaWindow(WINDOW_ACF);
}

void CRtaDlg::OnRaRec()
{
	if (m_pWndRec == NULL)
		OpenRaWindow(WINDOW_REC);
	else
		CloseRaWindow(WINDOW_REC);
}

void CRtaDlg::OnRaAutorec()
{
	CAutoRecDlg dlg(this);
	dlg.DoModal();
}

void CRtaDlg::OnRaFilter()
{
	if (m_pWndFilter == NULL)
		OpenRaWindow(WINDOW_FLT);
	else
		CloseRaWindow(WINDOW_FLT);
}

void CRtaDlg::OnRaPreset()
{
	CPresetDlg dlg(this);

	if (dlg.DoModal() == IDOK) {
		if (dlg.m_nPresetID == 0)
			InitialPreset();
		else
			OpenPreset(dlg.m_nPresetID);
	}
}

void CRtaDlg::InitialPreset()
{
	CloseRaWindow(WINDOW_ALL);

	::g_oSetData.Init();
}

void CRtaDlg::OpenPreset(int nPresetID)
{
	CDbPreset dbPreset;
	DbPresetRec dbPresetRec;
	long dataSize;

	if (!dbPreset.Open())
		return;

	if (!dbPreset.ReadRecID(nPresetID, &dbPresetRec))
		return;

	CloseRaWindow(WINDOW_ALL);

	if (dbPresetRec.nOpenWindow & WINDOW_SG) {
//		memset(&g_oSetData.Sg, 0, sizeof(g_oSetData.Sg));
		dbPreset.DBReadBinary("SG", &g_oSetData.Sg, sizeof(g_oSetData.Sg), &dataSize);
	}
	if (dbPresetRec.nOpenWindow & WINDOW_FFT) {
//		memset(&g_oSetData.Fft, 0, sizeof(g_oSetData.Fft));
		dbPreset.DBReadBinary("FFT", &g_oSetData.Fft, sizeof(g_oSetData.Fft), &dataSize);
	}
	if (dbPresetRec.nOpenWindow & WINDOW_OS) {
//		memset(&g_oSetData.Os, 0, sizeof(g_oSetData.Os));
		dbPreset.DBReadBinary("OS", &g_oSetData.Os, sizeof(g_oSetData.Os), &dataSize);
	}
	if (dbPresetRec.nOpenWindow & WINDOW_FRE) {
//		memset(&g_oSetData.Fre, 0, sizeof(g_oSetData.Fre));
		dbPreset.DBReadBinary("FRE", &g_oSetData.Fre, sizeof(g_oSetData.Fre), &dataSize);
	}
	if (dbPresetRec.nOpenWindow & WINDOW_DST) {
//		memset(&g_oSetData.Dst, 0, sizeof(g_oSetData.Dst));
		dbPreset.DBReadBinary("DST", &g_oSetData.Dst, sizeof(g_oSetData.Dst), &dataSize);
	}
	if (dbPresetRec.nOpenWindow & WINDOW_IMP) {
//		memset(&g_oSetData.Imp, 0, sizeof(g_oSetData.Imp));
		dbPreset.DBReadBinary("IMP", &g_oSetData.Imp, sizeof(g_oSetData.Imp), &dataSize);
	}
	if (dbPresetRec.nOpenWindow & WINDOW_ACF) {
//		memset(&g_oSetData.Acf, 0, sizeof(g_oSetData.Acf));
		dbPreset.DBReadBinary("ACF", &g_oSetData.Acf, sizeof(g_oSetData.Acf), &dataSize);
	}

	OpenRaWindow(dbPresetRec.nOpenWindow);
}

void CRtaDlg::OnSelchangeRaInputSelect()
{
	int i;
	MIXERCONTROLDETAILS_BOOLEAN mxcdb[MAX_CONTROL_ID];

	m_nSelectInput = m_cInputSelect.GetCurSel();

	for (i = 0; i < (int)m_dwSrcItems; i++) {
		if (m_dwSrcRecMuteID[i] != -1)
			MixerSetBoolControl(dwMixerInDevice, m_dwSrcRecMuteID[i], (i == m_nSelectInput) ? 0 : 1);
	}

	if (m_dwMultipleItems != 0) {
		for (i = 0; i < (int)m_dwMultipleItems; i++)
			mxcdb[i].fValue = (i == m_nSelectInput) ? 1 : 0;

		MixerSetControlValue(dwMixerInDevice, m_dwDstControlID, m_dwMultipleItems, mxcdb, sizeof(MIXERCONTROLDETAILS_BOOLEAN));
	}

	if (m_nSelectInput >= 0)
		m_cInputVolume.SetPos(GetInputVolume(m_dwSrcRecVolID[m_nSelectInput]) * 100 / 65535);
}

void CRtaDlg::SetSoundDevice()
{
	int i, n;
	WAVEINCAPS wic;
	WAVEOUTCAPS woc;
	int nDevice;

	// Wave入力デバイス
//	n = m_cInputDevice.AddString("Wave Mapper");
//	m_cInputDevice.SetItemData(n, WAVE_MAPPER);
	nDevice = waveInGetNumDevs();
	for (i = 0; i < nDevice; i++) {
		waveInGetDevCaps(i, &wic, sizeof(wic));
		n = m_cInputDevice.AddString(wic.szPname);
		m_cInputDevice.SetItemData(n, i);
	}

	// Wave出力デバイス
//	n = m_cOutputDevice.AddString("Wave Mapper");
//	m_cOutputDevice.SetItemData(n, WAVE_MAPPER);
	nDevice = waveOutGetNumDevs();
	for (i = 0; i < nDevice; i++) {
		waveOutGetDevCaps(i, &woc, sizeof(woc));
		n = m_cOutputDevice.AddString(woc.szPname);
		m_cOutputDevice.SetItemData(n, i);
	}

	// ASIOデバイス
	CStringArray aDriverNames;
	nDevice = CAsioDriver::getDriverNames(aDriverNames);
	for (i = 0; i < nDevice; i++) {
		int nId = -(i + 1);
		CString sDriverNames("[ASIO]" + aDriverNames[i]);

		n = m_cInputDevice.AddString(sDriverNames);
		m_cInputDevice.SetItemData(n, nId);

		n = m_cOutputDevice.AddString(sDriverNames);
		m_cOutputDevice.SetItemData(n, nId);
	}

	// WASAPI入力デバイス
	CStringArray aWasapiDevices;
	if (g_cWasapiManager.GetDevicesIn(aWasapiDevices)) {
		for (i = 0; i < aWasapiDevices.GetCount(); i++) {
			int nId = -(i + 100);
			CString sWasapiDevice("[WASAPI]" + aWasapiDevices[i]);

			n = m_cInputDevice.AddString(sWasapiDevice);
			m_cInputDevice.SetItemData(n, nId);
		}
	}

	// WASAPI出力デバイス
	if (g_cWasapiManager.GetDevicesOut(aWasapiDevices)) {
		for (i = 0; i < aWasapiDevices.GetCount(); i++) {
			int nId = -(i + 100);
			CString sWasapiDevice("[WASAPI]" + aWasapiDevices[i]);

			n = m_cOutputDevice.AddString(sWasapiDevice);
			m_cOutputDevice.SetItemData(n, nId);
		}
	}

	// 入力デバイス選択
	if (!SelectListBox(m_cInputDevice, g_oSetData.Pc.nInputDevice)) {
		g_oSetData.Pc.nInputDevice = 0;
		m_cInputDevice.SetCurSel(0);
	}

	// 出力デバイス選択
	if (!SelectListBox(m_cOutputDevice, g_oSetData.Pc.nOutputDevice)) {
		g_oSetData.Pc.nOutputDevice = 0;
		m_cOutputDevice.SetCurSel(0);
	}
}

BOOL CRtaDlg::SetInputSelect()
{
	if (g_nWaveInDevice >= 0) {
		// WAVE
		MIXERCONTROL mxc;
		MIXERCONTROLDETAILS_BOOLEAN mxcdb[MAX_CONTROL_ID];
		MIXERCONTROLDETAILS_LISTTEXT mxcdl[MAX_CONTROL_ID];
		DWORD i, dst, src;

		m_cInputSelect.EnableWindow(TRUE);
		m_cInputVolume.EnableWindow(FALSE);

		m_cInputSelect.ResetContent();

		// 録音マスターの LineID を取得
		MIXERLINE mxl{};
		mxl.cbStruct = sizeof(mxl);
		mxl.dwComponentType	= MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
		if (mixerGetLineInfo((HMIXEROBJ)dwMixerInDevice, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
			return FALSE;

		// 録音入力セレクタ
		if (MixerGetControlsByType(dwMixerInDevice, mxl.dwLineID, MIXERCONTROL_CONTROLTYPE_MUX, mxc) == MMSYSERR_NOERROR ||
				MixerGetControlsByType(dwMixerInDevice, mxl.dwLineID, MIXERCONTROL_CONTROLTYPE_MIXER, mxc) == MMSYSERR_NOERROR) {
			m_dwSrcItems = 0;
			m_dwDstControlID = mxc.dwControlID;
			m_dwMultipleItems = mxc.cMultipleItems;
			if (m_dwMultipleItems > MAX_CONTROL_ID)
				m_dwMultipleItems = MAX_CONTROL_ID;

			// セレクタリスト取得
			if (MixerGetControlList(dwMixerInDevice, m_dwDstControlID, m_dwMultipleItems, mxcdl, sizeof(MIXERCONTROLDETAILS_LISTTEXT)) != MMSYSERR_NOERROR)
				return FALSE;

			// セレクト状態取得
			if (MixerGetControlValue(dwMixerInDevice, m_dwDstControlID, m_dwMultipleItems, mxcdb, sizeof(MIXERCONTROLDETAILS_BOOLEAN)) != MMSYSERR_NOERROR)
				return FALSE;

			// 入力機器のボリュームの ControlID 取得
			for (i = 0; i < m_dwMultipleItems; i++) {
				if ((m_dwSrcRecVolID[i] = MixerGetControlID(dwMixerInDevice, mxcdl[i].dwParam1, MIXERCONTROL_CONTROLTYPE_VOLUME)) == -1) {
					m_dwSrcRecVolID[i] = MixerGetControlID(dwMixerInDevice, dwMixerInMasterLineID, MIXERCONTROL_CONTROLTYPE_VOLUME);
					DisableInputMasterVolume();
				}
				m_dwSrcRecMuteID[i] = (DWORD)-1;

				m_cInputSelect.AddString(mxcdl[i].szName);

				if (mxcdb[i].fValue != 0)
					m_nSelectInput = i;
			}
			m_nInputDevice = m_dwMultipleItems;
		} else {
			m_dwMultipleItems = 0;
			m_dwSrcItems = mxl.cConnections;
			if (m_dwSrcItems > MAX_CONTROL_ID)
				m_dwSrcItems = MAX_CONTROL_ID;
			dst = mxl.dwDestination;
			m_nSelectInput = 0;
			for (src = 0; src < m_dwSrcItems; src++) {
				// 入力機器の LineID 取得
				MIXERLINE mxl{};
				mxl.cbStruct = sizeof(mxl);
				mxl.dwDestination = dst;
				mxl.dwSource = src;
				if (mixerGetLineInfo((HMIXEROBJ)dwMixerInDevice, &mxl, MIXER_GETLINEINFOF_SOURCE) != MMSYSERR_NOERROR)
					return FALSE;

				// 入力機器のボリュームの ControlID 取得
				m_dwSrcRecVolID[src] = MixerGetControlID(dwMixerInDevice, mxl.dwLineID, MIXERCONTROL_CONTROLTYPE_VOLUME);
				if ((m_dwSrcRecVolID[src] = MixerGetControlID(dwMixerInDevice, mxl.dwLineID, MIXERCONTROL_CONTROLTYPE_VOLUME)) == -1) {
					m_dwSrcRecVolID[src] = MixerGetControlID(dwMixerInDevice, dwMixerInMasterLineID, MIXERCONTROL_CONTROLTYPE_VOLUME);
					DisableInputMasterVolume();
				}
				m_dwSrcRecMuteID[src] = MixerGetControlID(dwMixerInDevice, mxl.dwLineID, MIXERCONTROL_CONTROLTYPE_MUTE);

				// 入力機器のミュート状態取得
				if (m_dwSrcRecMuteID[src] != -1) {
					if (MixerGetBoolControl(dwMixerInDevice, m_dwSrcRecMuteID[src]) == 0)
						m_nSelectInput = src;
				}

				m_cInputSelect.AddString(mxl.szName);
			}
			m_nInputDevice = m_dwSrcItems;
		}

		if (m_nInputDevice > 0) {
			m_cInputSelect.SetCurSel(m_nSelectInput);
			m_cInputVolume.SetPos(GetInputVolume(m_dwSrcRecVolID[m_nSelectInput]) * 100 / 65535);
			m_cInputVolume.EnableWindow(TRUE);
		}
		OnSelchangeRaInputSelect();
	} else {
		// ASIO or WASAPI
		m_nInputDevice = -1;
		m_nSelectInput = -1;

		m_cInputSelect.ResetContent();
		m_cInputSelect.EnableWindow(FALSE);
		m_cInputVolume.EnableWindow(FALSE);
	}

	return TRUE;
}
/*
void CRtaDlg::MuteMicAndLineIn()
{
	DWORD dst, src;
	DWORD cSrcItems;
	DWORD dwSrcRecMuteID;
	static char *name[] = {"ﾏｲｸ", "マイク", "ﾗｲﾝ", "ライン", "Mic", "Line", NULL};
	char **pName;

	if (g_nWaveOutDevice < 0)
		return;

	// 再生マスターの LineID を取得
	MIXERLINE mxl{};
	mxl.cbStruct = sizeof(mxl);
	mxl.dwComponentType	= MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	if (mixerGetLineInfo((HMIXEROBJ)dwMixerOutDevice, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
		return;

	// 再生入力セレクタ
	cSrcItems = mxl.cConnections;
	dst = mxl.dwDestination;
	for (src = 0; src < cSrcItems; src++) {
		// 出力機器の LineID 取得
		MIXERLINE mxl{};
		mxl.cbStruct = sizeof(mxl);
		mxl.dwDestination = dst;
		mxl.dwSource = src;
		if (mixerGetLineInfo((HMIXEROBJ)g_nWaveOutDevice, &mxl, MIXER_GETLINEINFOF_SOURCE) != MMSYSERR_NOERROR)
			return;

		// 出力機器のミュートの ControlID 取得
		dwSrcRecMuteID = MixerGetControlID(dwMixerOutDevice, mxl.dwLineID, MIXERCONTROL_CONTROLTYPE_MUTE);

		// 出力機器のミュート状態取得
		if (dwSrcRecMuteID != -1) {
			for (pName = name; *pName != NULL; pName++) {
				if (_strnicmp(mxl.szName, *pName, strlen(*pName)) == 0) {
					if (MixerGetBoolControl(dwMixerOutDevice, dwSrcRecMuteID) == 0)
						MixerSetBoolControl(dwMixerOutDevice, dwSrcRecMuteID, 1);
					break;
				}
			}
		}
	}
}
*/
DWORD CRtaDlg::GetInputVolume(DWORD dwControlID)
{
	return MixerGetUnsignedControl(dwMixerInDevice, dwControlID);
}

void CRtaDlg::SetInputVolume(DWORD dwControlID, DWORD dwLevel)
{
	MixerSetUnsignedControl(dwMixerInDevice, dwControlID, dwLevel);
}

void CRtaDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (pScrollBar->GetDlgCtrlID()) {
	case IDC_RA_INPUT_VOLUME:
		if (m_nSelectInput >= 0)
			SetInputVolume(m_dwSrcRecVolID[m_nSelectInput], ((CSliderCtrl *)pScrollBar)->GetPos() * 65535 / 100);
		break;
	}

	CDialogAF::OnHScroll(nSBCode, nPos, pScrollBar);
}

int CRtaDlg::OnWaveNotify(int nCode, LPWAVENOTIFY pWaveNotify)
{
	switch (nCode) {
	case WAVEIN_OPEN:
		break;
	case WAVEIN_CLOSE:
		m_cInputLevel.Clear();
		break;
	case WAVEIN_DATA:
		m_cInputLevel.WaveData(pWaveNotify);
		return TRUE;
	case WAVEOUT_OPEN:
		break;
	case WAVEOUT_CLOSE:
		m_cOutputLevel.Clear();
		break;
	case WAVEOUT_DATA:
		m_cOutputLevel.WaveData(pWaveNotify);
		return TRUE;
	}

	return 0;
}

void CRtaDlg::OnDestroy()
{
	CDialogAF::OnDestroy();

	CWaveInEx::SetMonitor(NULL);
	CWaveOutEx::SetMonitor(NULL);

	CloseRaWindow(WINDOW_ALL);
}

void CRtaDlg::GetInputDevice(int *pInputDevice, int *pInputVolume)
{
	*pInputDevice = m_nSelectInput;
	if (m_nSelectInput >= 0) {
		*pInputVolume = GetInputVolume(m_dwSrcRecVolID[m_nSelectInput]);
	} else {
		*pInputVolume = 0;
	}
}

void CRtaDlg::SetInputDevice(int nInputDevice, int nInputVolume)
{
	if (nInputDevice >= 0 && nInputDevice < m_nInputDevice) {
		m_cInputSelect.SetCurSel(nInputDevice);
		OnSelchangeRaInputSelect();

		if (m_nSelectInput >= 0) {
			SetInputVolume(m_dwSrcRecVolID[m_nSelectInput], nInputVolume);
			m_cInputVolume.SetPos(GetInputVolume(m_dwSrcRecVolID[m_nSelectInput]) * 100 / 65535);
		}
	}
}

void CRtaDlg::UpdownInputVolume(int nUpdown)
{
	if (m_nSelectInput >= 0) {
		int nInputVolume = GetInputVolume(m_dwSrcRecVolID[m_nSelectInput]);

		nInputVolume += nUpdown;
		if (nInputVolume < 0)
			nInputVolume = 0;
		else if (nInputVolume > 65535)
			nInputVolume = 65535;

		SetInputVolume(m_dwSrcRecVolID[m_nSelectInput], nInputVolume);
		m_cInputVolume.SetPos(GetInputVolume(m_dwSrcRecVolID[m_nSelectInput]) * 100 / 65535);
	}
}

void CRtaDlg::OnOK()
{
	((CRtaApp *)::AfxGetApp())->m_pMainWnd->DestroyWindow();
}

void CRtaDlg::OnCancel()
{
	((CRtaApp *)::AfxGetApp())->m_pMainWnd->DestroyWindow();
}

void CRtaDlg::PostNcDestroy()
{
	CDialogAF::PostNcDestroy();

	delete this;
}

void CRtaDlg::OnCbnSelchangeRaInputDevice()
{
	int nIndex = m_cInputDevice.GetCurSel();
	if (nIndex != CB_ERR) {
		int nInOut = WAVE_IN;

		CWaveInEx::CloseAll();
		g_oSetData.Pc.nInputDevice = (int)m_cInputDevice.GetItemData(nIndex);

		// ASIOの場合は入力デバイスと出力デバイスは同じものにする
		if (g_oSetData.Pc.nInputDevice < 0 && g_oSetData.Pc.nInputDevice > -100 && g_oSetData.Pc.nOutputDevice < 0 && g_oSetData.Pc.nOutputDevice > -100 && g_oSetData.Pc.nInputDevice != g_oSetData.Pc.nOutputDevice) {
			CWaveOutEx::CloseAll();
			g_oSetData.Pc.nOutputDevice = g_oSetData.Pc.nInputDevice;
			SelectListBox(m_cOutputDevice, g_oSetData.Pc.nOutputDevice);
			nInOut |= WAVE_OUT;
		}

		SelectSoundDevice();
		ChangeWaveDevice(nInOut);
	}
}

void CRtaDlg::OnCbnSelchangeRaOutputDevice()
{
	int nIndex = m_cOutputDevice.GetCurSel();
	if (nIndex != CB_ERR) {
		int nInOut = WAVE_OUT;

		CWaveOutEx::CloseAll();
		g_oSetData.Pc.nOutputDevice = (int)m_cOutputDevice.GetItemData(nIndex);

		// ASIOの場合は入力デバイスと出力デバイスは同じものにする
		if (g_oSetData.Pc.nInputDevice < 0 && g_oSetData.Pc.nInputDevice > -100 && g_oSetData.Pc.nOutputDevice < 0 && g_oSetData.Pc.nOutputDevice > -100 && g_oSetData.Pc.nInputDevice != g_oSetData.Pc.nOutputDevice) {
			CWaveInEx::CloseAll();
			g_oSetData.Pc.nInputDevice = g_oSetData.Pc.nOutputDevice;
			SelectListBox(m_cInputDevice, g_oSetData.Pc.nInputDevice);
			nInOut |= WAVE_IN;
		}

		SelectSoundDevice();
		ChangeWaveDevice(nInOut);
	}
}

void CRtaDlg::SelectSoundDevice()
{
	g_nWaveInDevice = g_oSetData.Pc.nInputDevice;
	g_nWaveOutDevice = g_oSetData.Pc.nOutputDevice;

	MixerInitialize(g_nWaveInDevice, g_nWaveOutDevice);
//	MuteMicAndLineIn();
	SetInputSelect();
	ResetSamplingRate();
}

void CRtaDlg::ChangeWaveDevice(int nInOut)
{
	if (m_pWndSg != NULL)
		m_pWndSg->ChangeWaveDevice(nInOut);

	if (m_pWndFft != NULL)
		m_pWndFft->ChangeWaveDevice(nInOut);

	if (m_pWndOs != NULL)
		m_pWndOs->ChangeWaveDevice(nInOut);

	if (m_pWndFre != NULL)
		m_pWndFre->ChangeWaveDevice(nInOut);

	if (m_pWndDst != NULL)
		m_pWndDst->ChangeWaveDevice(nInOut);

	if (m_pWndImp != NULL)
		m_pWndImp->ChangeWaveDevice(nInOut);

	if (m_pWndAcf != NULL)
		m_pWndAcf->ChangeWaveDevice(nInOut);
}

void CRtaDlg::OpenRaWindow(int nWindow)
{
	CMenu* pMenu = GetMenu()->GetSubMenu(2);

	if (nWindow & WINDOW_SG) {
		if (m_pWndSg == NULL) {
			m_cRaSg.SetCheck(1);
			pMenu->CheckMenuItem(IDM_RA_SG, MF_BYCOMMAND | MF_CHECKED);
			m_pWndSg = new CSgDlg;
		} else
			m_pWndSg->SetActiveWindow();
	}

	if (nWindow & WINDOW_FFT) {
		if (m_pWndFft == NULL) {
			m_cRaFft.SetCheck(1);
			pMenu->CheckMenuItem(IDM_RA_FFT, MF_BYCOMMAND | MF_CHECKED);
			m_pWndFft = new CFftDlg;
		} else
			m_pWndFft->SetActiveWindow();
	}

	if (nWindow & WINDOW_OS) {
		if (m_pWndOs == NULL) {
			m_cRaOs.SetCheck(1);
			pMenu->CheckMenuItem(IDM_RA_OS, MF_BYCOMMAND | MF_CHECKED);
			m_pWndOs = new COsDlg;
		} else
			m_pWndOs->SetActiveWindow();
	}

	if (nWindow & WINDOW_FRE) {
		if (m_pWndFre == NULL) {
			m_cRaFre.SetCheck(1);
			pMenu->CheckMenuItem(IDM_RA_FRE, MF_BYCOMMAND | MF_CHECKED);
			m_pWndFre = new CFreDlg;
		} else
			m_pWndFre->SetActiveWindow();
	}

	if (nWindow & WINDOW_DST) {
		if (m_pWndDst == NULL) {
			m_cRaDst.SetCheck(1);
			pMenu->CheckMenuItem(IDM_RA_DST, MF_BYCOMMAND | MF_CHECKED);
			m_pWndDst = new CDstDlg;
		} else
			m_pWndDst->SetActiveWindow();
	}

	if (nWindow & WINDOW_IMP) {
		if (m_pWndImp == NULL) {
			m_cRaImp.SetCheck(1);
			pMenu->CheckMenuItem(IDM_RA_IMP, MF_BYCOMMAND | MF_CHECKED);
			m_pWndImp = new CImpulseDlg;
		} else
			m_pWndImp->SetActiveWindow();
	}

	if (nWindow & WINDOW_ACF) {
		if (m_pWndAcf == NULL) {
			m_cRaAcf.SetCheck(1);
			pMenu->CheckMenuItem(IDM_RA_ACF, MF_BYCOMMAND | MF_CHECKED);
			m_pWndAcf = new CAcfDlg;
		} else
			m_pWndAcf->SetActiveWindow();
	}

	if (nWindow & WINDOW_REC) {
		if (m_pWndRec == NULL) {
			m_cRaRec.SetCheck(1);
			pMenu->CheckMenuItem(IDM_RA_REC, MF_BYCOMMAND | MF_CHECKED);
			m_pWndRec = new CRecDlg;
		} else
			m_pWndRec->SetActiveWindow();
	}

	if (nWindow & WINDOW_FLT) {
		if (m_pWndFilter == NULL) {
//			m_cRaFilter.SetCheck(1);
			pMenu->CheckMenuItem(IDM_RA_FILTER, MF_BYCOMMAND | MF_CHECKED);
			m_pWndFilter = new CFilterDlg;
		} else
			m_pWndFilter->SetActiveWindow();
	}

	if (nWindow & WINDOW_TIME) {
		if (m_pWndTime == NULL) {
			pMenu->CheckMenuItem(IDM_RA_TIME, MF_BYCOMMAND | MF_CHECKED);
			m_pWndTime = new CTimeWnd;
		} else
			m_pWndTime->SetActiveWindow();
	}
}

void CRtaDlg::CloseRaWindow(int nWindow)
{
	if (nWindow & WINDOW_SG) {
		if (m_pWndSg != NULL)
			m_pWndSg->DestroyWindow();
	}

	if (nWindow & WINDOW_FFT) {
		if (m_pWndFft != NULL)
			m_pWndFft->DestroyWindow();
	}

	if (nWindow & WINDOW_OS) {
		if (m_pWndOs != NULL)
			m_pWndOs->DestroyWindow();
	}

	if (nWindow & WINDOW_FRE) {
		if (m_pWndFre != NULL)
			m_pWndFre->DestroyWindow();
	}

	if (nWindow & WINDOW_DST) {
		if (m_pWndDst != NULL)
			m_pWndDst->DestroyWindow();
	}

	if (nWindow & WINDOW_IMP) {
		if (m_pWndImp != NULL)
			m_pWndImp->DestroyWindow();
	}

	if (nWindow & WINDOW_ACF) {
		if (m_pWndAcf != NULL)
			m_pWndAcf->DestroyWindow();
	}

	if (nWindow & WINDOW_REC) {
		if (m_pWndRec != NULL)
			m_pWndRec->DestroyWindow();
	}

	if (nWindow & WINDOW_FLT) {
		if (m_pWndFilter != NULL)
			m_pWndFilter->DestroyWindow();
	}

	if (nWindow & WINDOW_TIME) {
		if (m_pWndTime != NULL)
			m_pWndTime->DestroyWindow();
	}
}

void CRtaDlg::CloseRaWindow(CWnd *pWnd)
{
	CMenu* pMenu = GetMenu()->GetSubMenu(2);

	if (pWnd == m_pWndSg) {
		m_cRaSg.SetCheck(0);
		pMenu->CheckMenuItem(IDM_RA_SG, MF_BYCOMMAND | MF_UNCHECKED);
		m_pWndSg = NULL;
	}

	if (pWnd == m_pWndFft) {
		m_cRaFft.SetCheck(0);
		pMenu->CheckMenuItem(IDM_RA_FFT, MF_BYCOMMAND | MF_UNCHECKED);
		m_pWndFft = NULL;
	}

	if (pWnd == m_pWndOs) {
		m_cRaOs.SetCheck(0);
		pMenu->CheckMenuItem(IDM_RA_OS, MF_BYCOMMAND | MF_UNCHECKED);
		m_pWndOs = NULL;
	}

	if (pWnd == m_pWndFre) {
		m_cRaFre.SetCheck(0);
		pMenu->CheckMenuItem(IDM_RA_FRE, MF_BYCOMMAND | MF_UNCHECKED);
		m_pWndFre = NULL;
	}

	if (pWnd == m_pWndDst) {
		m_cRaDst.SetCheck(0);
		pMenu->CheckMenuItem(IDM_RA_DST, MF_BYCOMMAND | MF_UNCHECKED);
		m_pWndDst = NULL;
	}

	if (pWnd == m_pWndImp) {
		m_cRaImp.SetCheck(0);
		pMenu->CheckMenuItem(IDM_RA_IMP, MF_BYCOMMAND | MF_UNCHECKED);
		m_pWndImp = NULL;
	}

	if (pWnd == m_pWndAcf) {
		m_cRaAcf.SetCheck(0);
		pMenu->CheckMenuItem(IDM_RA_ACF, MF_BYCOMMAND | MF_UNCHECKED);
		m_pWndAcf = NULL;
	}

	if (pWnd == m_pWndRec) {
		m_cRaRec.SetCheck(0);
		pMenu->CheckMenuItem(IDM_RA_REC, MF_BYCOMMAND | MF_UNCHECKED);
		m_pWndRec = NULL;
	}

	if (pWnd == m_pWndFilter) {
//		m_cRaFilter.SetCheck(0);
		pMenu->CheckMenuItem(IDM_RA_FILTER, MF_BYCOMMAND | MF_UNCHECKED);
		m_pWndFilter = NULL;
	}

	if (pWnd == m_pWndTime) {
		pMenu->CheckMenuItem(IDM_RA_TIME, MF_BYCOMMAND | MF_UNCHECKED);
		m_pWndTime = NULL;
	}
}

void CRtaDlg::OnAboutbox()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CRtaDlg::OnDocumentQa()
{
	OpenURL(IDS_DOCUMENT_QA_CMD);
}

void CRtaDlg::OnTechnicalSupport()
{
	OpenURL(IDS_TECHNICAL_SUPPORT_CMD);
}

void CRtaDlg::OnOnlineManual()
{
	OpenURL(IDS_ONLINE_MANUAL_CMD);
}

void CRtaDlg::OnOnlineUpdate()
{
	((CRtaApp *)::AfxGetApp())->OnOnlineUpdate();
}

void CRtaDlg::OnReadme()
{
	CString sReadmeFile;

	sReadmeFile.Format("%s\\ReadMe.txt", (LPCSTR)GetExeDir());
	OpenURL(sReadmeFile);
}

void CRtaDlg::OnYmecStore()
{
	OpenURL(IDS_YMEC_STORE);
}

void CRtaDlg::OnLicenseRegist()
{
	REGIST_LICENSE();
}

void CRtaDlg::OnUpdateLicenseRegist(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(CHECK_LICENSE2(APP_RA) != LICENSE_REGIST);
}

void CRtaDlg::OnInitMenu(CMenu* pMenu)
{
	CDialogAF::OnInitMenu(pMenu);

	CMenu *pHelpMenu = pMenu->GetSubMenu(3);
	if (pHelpMenu != NULL) {
		UINT nState = CHECK_LICENSE2(APP_RA) == LICENSE_REGIST ? MF_GRAYED : MF_ENABLED;
		pHelpMenu->EnableMenuItem(IDM_LICENSE_REGIST, MF_BYCOMMAND | nState);
	}
}

void CRtaDlg::OnRaTime()
{
	if (m_pWndTime == NULL)
		OpenRaWindow(WINDOW_TIME);
	else
		CloseRaWindow(WINDOW_TIME);
}

void CRtaDlg::OnSetting()
{
	CSetting dlg;

	if (dlg.DoModal() == IDOK) {
		CWaveIn::m_b16BitOnly = g_oSetData2.bInput16BitOnly;
		CWaveOut::m_b16BitOnly = g_oSetData2.bOutput16BitOnly;
	}
}

BOOL CRtaDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (message == WM_MOUSEMOVE) {
		HWND hActiveWnd = CWnd::GetActiveWindow()->GetSafeHwnd();
		HWND hWnd = pWnd->m_hWnd;

		if (hWnd == m_cRaSg.m_hWnd) {
			if (m_pWndSg != NULL && hActiveWnd != m_pWndSg->m_hWnd)
				m_pWndSg->SetActiveWindow();
		} else if (hWnd == m_cRaFft.m_hWnd) {
			if (m_pWndFft != NULL && hActiveWnd != m_pWndFft->m_hWnd)
				m_pWndFft->SetActiveWindow();
		} else if (hWnd == m_cRaOs.m_hWnd) {
			if (m_pWndOs != NULL && hActiveWnd != m_pWndOs->m_hWnd)
				m_pWndOs->SetActiveWindow();
		} else if (hWnd == m_cRaFre.m_hWnd) {
			if (m_pWndFre != NULL && hActiveWnd != m_pWndFre->m_hWnd)
				m_pWndFre->SetActiveWindow();
		} else if (hWnd == m_cRaDst.m_hWnd) {
			if (m_pWndDst != NULL && hActiveWnd != m_pWndDst->m_hWnd)
				m_pWndDst->SetActiveWindow();
		} else if (hWnd == m_cRaImp.m_hWnd) {
			if (m_pWndImp != NULL && hActiveWnd != m_pWndImp->m_hWnd)
				m_pWndImp->SetActiveWindow();
		} else if (hWnd == m_cRaAcf.m_hWnd) {
			if (m_pWndAcf != NULL && hActiveWnd != m_pWndAcf->m_hWnd)
				m_pWndAcf->SetActiveWindow();
		} else if (hWnd == m_cRaRec.m_hWnd) {
			if (m_pWndRec != NULL && hActiveWnd != m_pWndRec->m_hWnd)
				m_pWndRec->SetActiveWindow();
		} else if (hWnd == m_cRaFilter.m_hWnd) {
			if (m_pWndFilter != NULL && hActiveWnd != m_pWndFilter->m_hWnd)
				m_pWndFilter->SetActiveWindow();
		}
	}

	return CDialogAF::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CRtaDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	static const UINT aIDs[] = {
		IDC_RA_SG, IDH_RTA_SG,
		IDC_RA_FFT, IDH_RTA_FFT,
		IDC_RA_OS, IDH_RTA_OS,
		IDC_RA_FRE, IDH_RTA_FRE,
		IDC_RA_DST, IDH_RTA_DST,
		IDC_RA_IMP, IDH_RTA_IMP,
		IDC_RA_ACF, IDH_RTA_ACF,
		IDC_RA_REC, IDH_RTA_REC,
		IDC_RA_AUTOREC, IDH_RTA_AUTOREC,
		IDC_RA_PRESET, IDH_RTA_PRESET,
		IDOK, IDH_RTA_EXIT,
		IDC_RA_INPUT_DEVICE, IDH_RTA_INPUT_DEVICE,
		IDC_RA_OUTPUT_DEVICE, IDH_RTA_OUTPUT_DEVICE,
		IDC_RA_INPUT_SELECT, IDH_RTA_INPUT_SELECT,
		IDC_RA_INPUT_VOLUME, IDH_RTA_INPUT_VOLUME,
		IDC_RA_INPUT_LEVEL, IDH_RTA_INPUT_LEVEL,
		IDC_RA_OUTPUT_LEVEL, IDH_RTA_OUTPUT_LEVEL,
		0
	};

	DispContextHelp(pHelpInfo, aIDs);

	return TRUE;
}

void CRtaDlg::OnHelpFile()
{
	CString sHelpFile;

	sHelpFile.Format("%s\\%s", (LPCSTR)GetExeDir(), (LPCSTR)GetString(IDS_HELP_FILE));
	::HtmlHelp((HWND)m_hWnd, sHelpFile, HH_DISPLAY_TOC , NULL);
}

void CRtaDlg::OnPdfFile()
{
	CString sPdfFile;

	sPdfFile.Format("%s\\%s", (LPCSTR)GetExeDir(), (LPCSTR)GetString(IDS_PDF_FILE));
	::ShellExecute(NULL, "open", sPdfFile, NULL, NULL, SW_SHOWNORMAL);
}

void CRtaDlg::OnDataBackup()
{
	CDataBackupDlg dlg;

	dlg.DoModal();
}

void CRtaDlg::OnDataRestore()
{
	CDataRestoreDlg dlg;

	dlg.DoModal();
}

LRESULT CRtaDlg::OnChangeFontSize(WPARAM wParam, LPARAM lParam)
{
	int nWindowKind = (int)wParam;
	int nFontSize = (int)lParam;

	CloseRaWindow(nWindowKind);

	switch (nWindowKind) {
	case WINDOW_SG:
		g_oSetData2.nFontSizeSg = nFontSize;
		break;
	case WINDOW_FFT:
		g_oSetData2.nFontSizeFft = nFontSize;
		break;
	case WINDOW_OS:
		g_oSetData2.nFontSizeOs = nFontSize;
		break;
	case WINDOW_IMP:
		g_oSetData2.nFontSizeImp = nFontSize;
		break;
	case WINDOW_ACF:
		g_oSetData2.nFontSizeAcf = nFontSize;
		break;
	case WINDOW_DST:
		g_oSetData2.nFontSizeDst = nFontSize;
		break;
	case WINDOW_FRE:
		g_oSetData2.nFontSizeFre = nFontSize;
		break;
	}

	OpenRaWindow(nWindowKind);

	return 0;
}

void CRtaDlg::OnTransferMmlib()
{
	if (!CheckMMLib()) {
		if (!ExecMMLib(this))
			return;
	}

	CString sFolderTitle = CTime::GetCurrentTime().Format("DSSF3-%Y%m%d-%H%M%S");

	TransferToMmlib(this, sFolderTitle, "Realtime Analyzer");

	if (m_pWndSg != NULL)
		TransferToMmlib(m_pWndSg, sFolderTitle, "Signal Generator");

	if (m_pWndFft != NULL)
		TransferToMmlib(m_pWndFft, sFolderTitle, "FFT Analyzer");

	if (m_pWndOs != NULL)
		TransferToMmlib(m_pWndOs, sFolderTitle, "Oscilloscope");

	if (m_pWndFre != NULL)
		TransferToMmlib(m_pWndFre, sFolderTitle, "Frequency Response");

	if (m_pWndDst != NULL)
		TransferToMmlib(m_pWndDst, sFolderTitle, "THD Analyzer");

	if (m_pWndImp != NULL)
		TransferToMmlib(m_pWndImp, sFolderTitle, "Impulse Response");

	if (m_pWndAcf != NULL)
		TransferToMmlib(m_pWndAcf, sFolderTitle, "Running ACF");
}

void CRtaDlg::TransferToMmlib(CWnd *pWnd, const CString &sFolderTitle, const CString &sDataTitle)
{
	HDIB hDIB = CreateDIB(pWnd);
	SendMMLib(this, sFolderTitle, "", sDataTitle, "", "", hDIB);
	::GlobalFree(hDIB);
}

// Source - https://stackoverflow.com/a/76957105
// Posted by gil123, modified by community. See post 'Timeline' for change history
// Retrieved 2026-08-25, License - CC BY-SA 4.0
/*
BOOL CRtaDlg::GetWindowRectNoInvisibleBorders(HWND hWnd, RECT* rect)
{
    // Get the physical coordinates of the window (this is without the additional offsets)
    RECT dwmRect;
    HRESULT hresult = DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &dwmRect, sizeof(RECT));
    // Return false in such case. error handling was done only here because after this check, the rest of the code
    // use the API correctly so error is unlikely 
    if (hresult != S_OK)
        return FALSE;

    // Get information from the monitor where the window located.
    // We need it for getting its RECT in logical coordinates (rcMonitor.*)
    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFOEX monInfo;
    monInfo.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(monitor, &monInfo);

    // Get additional information from this monitor. we need it for getting the physical
    // coordinates of its position (dmPosition.x and dmPosition.y) and its physical coordinates
    // of its size (dmPelsWidth)
    DEVMODE monDeviceConfig;
    monDeviceConfig.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(monInfo.szDevice, ENUM_CURRENT_SETTINGS, &monDeviceConfig);

    // Calculate the ratio between the logical size and the physical size of the monitor (part of math to handle DPI changes)
    double scalingRatio = (monInfo.rcMonitor.right - monInfo.rcMonitor.left) / (double)monDeviceConfig.dmPelsWidth;

    // Calculate the final answer in logical coordinates
    rect->left = (LONG)((dwmRect.left - monDeviceConfig.dmPosition.x) * scalingRatio + monInfo.rcMonitor.left);
    rect->right = (LONG)((dwmRect.right - monDeviceConfig.dmPosition.x) * scalingRatio + monInfo.rcMonitor.left);
    rect->top = (LONG)((dwmRect.top - monDeviceConfig.dmPosition.y) * scalingRatio + monInfo.rcMonitor.top);
    rect->bottom = (LONG)((dwmRect.bottom - monDeviceConfig.dmPosition.y) * scalingRatio + monInfo.rcMonitor.top);

    return TRUE; // success
}
*/

HDIB CRtaDlg::CreateDIB(CWnd *pWnd)
{
	pWnd->SetForegroundWindow();
	pWnd->SetFocus();
//	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW);	// 非クライアント領域を再描画（特に効果なし）
	pWnd->UpdateWindow();
	::Sleep(1000);		// ウィンドウが重なっていると下のウィンドウが透けてしまうので、それが消えるまで待つ

	// エアロ環境だと左右、下にマージンができてしまうので、それを除去したrectを取得
	CRect rect;
	BOOL bDwmEnable; 
	HRESULT hResult = DwmIsCompositionEnabled(&bDwmEnable);
	if (S_OK == hResult){
		if (bDwmEnable){
			// エアロ環境
			DwmGetWindowAttribute(pWnd->GetSafeHwnd(), DWMWA_EXTENDED_FRAME_BOUNDS, rect, sizeof(rect));
		} else {
			// 非エアロ環境
			pWnd->GetWindowRect(rect);
		}
	}

	CRect rect2;
	pWnd->GetWindowRect(rect2);

	int nOffsetX = rect.left - rect2.left;
	int nOffsetY = rect.top - rect2.top;
	int nWidth = rect.Width();
	int nHeight = rect.Height();

	HMONITOR monitor = MonitorFromWindow(pWnd->GetSafeHwnd(), MONITOR_DEFAULTTONEAREST);
	MONITORINFOEX monInfo;
	monInfo.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(monitor, &monInfo);

	DEVMODE monDeviceConfig{
		.dmSize = sizeof(DEVMODE)
	};
	EnumDisplaySettings(monInfo.szDevice, ENUM_CURRENT_SETTINGS, &monDeviceConfig);

	// スケーリングが100%の時はそのウィンドウ、それ以外はタイトルバーが正常に取れないのでデスクトップウィンドウから転送
	if (monInfo.rcMonitor.right - monInfo.rcMonitor.left == (LONG)monDeviceConfig.dmPelsWidth) {
		// スケーリングが100%の時
		rect2.SetRect(0, 0, 0, 0);
	} else {
		// スケーリングが100%以外の時
		pWnd = CWnd::GetDesktopWindow();
	}
	CDC *pDC = pWnd->GetWindowDC();

	CDC DCMem;
	DCMem.CreateCompatibleDC(pDC);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, nWidth, nHeight);

	CGdiObject *pOrg = DCMem.SelectObject(&bmp);
	DCMem.BitBlt(0, 0, nWidth, nHeight, pDC, rect2.left + nOffsetX, rect2.top + nOffsetY, SRCCOPY);
	DCMem.SelectObject(pOrg);
	pWnd->ReleaseDC(pDC);

	HDIB hDIB;
	MMPictureGetBitmap(&hDIB, bmp);

	return hDIB;
}

void CRtaDlg::OnBnClickedRaAutorec()
{
	CAutoRecDlg dlg(this);
	dlg.DoModal();
}

void CRtaDlg::OpenAutoStartFile(LPCTSTR pAutoRecFile)
{
	OpenRaWindow(WINDOW_REC);
	((CRecDlg *)m_pWndRec)->LoadWavDssfFile(pAutoRecFile);
}

LRESULT CRtaDlg::OnAutoRecFile(WPARAM, LPARAM)
{
	HANDLE hMem;
	LPCTSTR pMem;

	if (OpenClipboard()) {
		if ((hMem = GetClipboardData(CF_TEXT)) != NULL) {
			if ((pMem = (LPTSTR)::GlobalLock(hMem)) != NULL) {
				OpenAutoStartFile(pMem);
				::GlobalUnlock(hMem);
			}
		}
		CloseClipboard();
	}

	return 0;
}
/*
LRESULT CRtaDlg::OnMixmLineChange(WPARAM, LPARAM)
{
	return 0;
}

LRESULT CRtaDlg::OnMixmControlChange(WPARAM, LPARAM)
{
	return 0;
}
*/

LRESULT CRtaDlg::OnNotifyDeviceChange(WPARAM, LPARAM)
{
	CWaveInEx::CloseAll();
	CWaveOutEx::CloseAll();

	g_cWasapiManager.ResetInterface();

	m_cInputDevice.ResetContent();
	m_cOutputDevice.ResetContent();

	SetSoundDevice();
	SelectSoundDevice();

	return 0;
}
