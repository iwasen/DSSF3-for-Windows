// Sas.cpp : アプリケーション用クラスの機能定義を行います。
//

#include "stdafx.h"
#include "Sas.h"

#include "MainFrm.h"
#include "SasDoc.h"
#include "SasView.h"
#include "UpdateAp.h"
#include "VerRes.h"
#include "HyperLink.h"
#include "Regist.h"
#include "ImpFactor.h"
#include "AcfFactor.h"

CImageList imageList;

/////////////////////////////////////////////////////////////////////////////
// アプリケーションのバージョン情報で使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogAF
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

public:
	CHyperLink m_cProductURL;
	CHyperLink m_cCompanyURL;
	CHyperLink m_cEmailAddr;
	CHyperLink m_cYmecStoreURL;

protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // // DDX/DDV のサポート
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
// SasApp

BEGIN_MESSAGE_MAP(SasApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(IDM_ONLINE_UPDATE, OnOnlineUpdate)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(IDM_ONLINE_MANUAL, OnOnlineManual)
	ON_COMMAND(IDM_TECHNICAL_SUPPORT, OnTechnicalSupport)
	ON_COMMAND(IDM_DOCUMENT_QA, OnDocumentQa)
	ON_COMMAND(IDM_README, OnReadme)
	ON_COMMAND(IDM_YMEC_STORE, OnYmecStore)
	ON_COMMAND(IDM_LICENSE_REGIST, OnLicenseRegist)
	ON_UPDATE_COMMAND_UI(IDM_LICENSE_REGIST, OnUpdateLicenseRegist)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SasApp クラスの構築

SasApp::SasApp()
{
	m_nCheckData = GET_CHECK_DATA;
	if (GetCheckData2() == m_nCheckData)
		m_pRegistryName = g_sRegistryName;
}

/////////////////////////////////////////////////////////////////////////////
// 唯一の SasApp オブジェクト

SasApp theApp;

/////////////////////////////////////////////////////////////////////////////
// SasApp クラスの初期化

BOOL SasApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls{
		.dwSize = sizeof(InitCtrls),
		.dwICC = ICC_WIN95_CLASSES
	};
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	AfxEnableControlContainer();
	AfxOleInit();

	// 標準的な初期化処理
	// もしこれらの機能を使用せず、実行ファイルのサイズを小さく
	//  したければ以下の特定の初期化ルーチンの中から不必要なもの
	//  を削除してください。
	CDpi::InitDpi();

	g_hIcon = LoadIcon(IDR_MAINFRAME);
	CVerRes res;
	if (!CHECK_LICENSE0(APP_SA, res.GetProductVersion() + "/SA"))
		return FALSE;

	imageList.Create(IDB_IMAGE_LIST, 16, 1, RGB(255, 255, 255));

	// 設定が保存される下のレジストリ キーを変更します。
	// 会社名または所属など、適切な文字列に
	// 変更してください。
//	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

//	AdjustDialogSize();

	LoadStdProfileSettings(0);  // 標準の INI ファイルのオプションをローﾄﾞします (MRU を含む)

	g_oPcserve.Initialize();

	BOOL bImp = FALSE;
	BOOL bAcf = FALSE;
	CString argv;
	for (int i = 1; i < __argc; i++) {
		argv = __argv[i];

		if (argv == "/IMP")
			bImp = TRUE;

		if (argv == "/ACF")
			bAcf = TRUE;
	}

	// アプリケーション用のドキュメント テンプレートを登録します。ドキュメント テンプレート
	//  はドキュメント、フレーム ウィンドウとビューを結合するために機能します。

	if (bImp) {
		CImpFactor *pDlg = new CImpFactor((CWnd *)&CWnd::wndTop);
		pDlg->m_nFolderID = -2;
		pDlg->m_nImpulseID = -2;
		pDlg->Create();
		m_pMainWnd = pDlg;
	} else if (bAcf) {
		CAcfFactor *pDlg = new CAcfFactor((CWnd *)&CWnd::wndTop);
		pDlg->m_nFolderID = -2;
		pDlg->m_nAcfID = -2;
		pDlg->Create();
		m_pMainWnd = pDlg;
	} else {
		CSingleDocTemplate* pDocTemplate;
		pDocTemplate = new CSingleDocTemplate(
			IDR_MAINFRAME,
			RUNTIME_CLASS(CSasDoc),
			RUNTIME_CLASS(CMainFrame),       // メイン SDI フレーム ウィンドウ
			RUNTIME_CLASS(CSasView));
		AddDocTemplate(pDocTemplate);

		// DDE、file open など標準のシェル コマンドのコマンドラインを解析します。
		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);

		// コマンドラインでディスパッチ コマンドを指定します。
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;

		// エクスポートファイルアイコン登録
		RegistExportFileIcon();

		// メイン ウィンドウが初期化されたので、表示と更新を行います。
		m_pMainWnd->ShowWindow(SW_SHOW);
		m_pMainWnd->UpdateWindow();
	}

	return TRUE;
}

// ダイアログを実行するためのアプリケーション コマンド
void SasApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// SasApp コマンド

LRESULT SasApp::ProcessWndProcException(CException* e, const MSG* /*pMsg*/)
{
	e->ReportError(MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
	exit(1);
//	return CWinApp::ProcessWndProcException(e, pMsg);
}

void SasApp::OnOnlineUpdate()
{
	HINSTANCE hModule;

#ifdef _DEBUG
	if ((hModule = ::LoadLibrary("YMUPDATED.DLL")) == NULL) {
#else
	if ((hModule = ::LoadLibrary("YMUPDATE.DLL")) == NULL) {
#endif
		::AfxMessageBox(IDS_ERR_YMUPDATE);
		return;
	}

	OnlineUpdateFunc pOnlineUpdate;
	if ((pOnlineUpdate = (OnlineUpdateFunc)::GetProcAddress(hModule, "OnlineUpdate")) == NULL) {
		::AfxMessageBox(IDS_ERR_ONLINEUPDATE);
		::FreeLibrary(hModule);
		return;
	}

	CString exeFileName;
	::GetModuleFileName(m_hInstance, exeFileName.GetBuffer(MAX_PATH), MAX_PATH);
	exeFileName.ReleaseBuffer();

	CString sOnlineUpdate;
	sOnlineUpdate.LoadString(IDS_ONLINEUPDATE);
#ifdef _WIN64
	sOnlineUpdate += "64";
#else
	sOnlineUpdate += "32";
#endif
	pOnlineUpdate(sOnlineUpdate, exeFileName, m_lpCmdLine, LoadIcon(IDR_MAINFRAME), "Sound\nAnalyzer", g_sRegistryName);
}

void SasApp::AdjustDialogSize()
{
	HDC dc = ::GetDC(NULL);
	const int nLogPixel = GetDeviceCaps(dc, LOGPIXELSX);
	::ReleaseDC(NULL, dc);

	if (nLogPixel <= 96)
		return;

	const int nFontSize = 9 * 96 / nLogPixel;

	SetDialogFontSize(IDD_ACF_FACTOR, nFontSize);
	SetDialogFontSize(IDD_NMS_FACTOR, nFontSize);
	SetDialogFontSize(IDD_IMP_FACTOR, nFontSize);

	SetDialogFontSize(IDD_GRAPH_ACF, nFontSize);
	SetDialogFontSize(IDD_GRAPH_ECHO, nFontSize);
	SetDialogFontSize(IDD_GRAPH_FACTOR, nFontSize);
	SetDialogFontSize(IDD_GRAPH_FREQ, nFontSize);
	SetDialogFontSize(IDD_GRAPH_IACC, nFontSize);
	SetDialogFontSize(IDD_GRAPH_IACF, nFontSize);
	SetDialogFontSize(IDD_GRAPH_IR, nFontSize);
	SetDialogFontSize(IDD_GRAPH_PARAM, nFontSize);
	SetDialogFontSize(IDD_GRAPH_PREF, nFontSize);
}

void SasApp::SetDialogFontSize(UINT nIDTemplate, int nFontSize)
{
#pragma pack(push, 1)
	typedef struct {
		WORD   dlgVer;
		WORD   signature;
		DWORD  helpID;
		DWORD  exStyle;
		DWORD  style;
		WORD   cDlgItems;
		short  x;
		short  y;
		short  cx;
		short  cy;
	//    sz_Or_Ord menu;         // name or ordinal of a menu resource
	//    sz_Or_Ord windowClass;  // name or ordinal of a window class
	//    WCHAR  title[titleLen]; // title string of the dialog box
	//    short  pointsize;       // only if DS_SETFONT flag is set
	//    short  weight;          // only if DS_SETFONT flag is set
	//    short  bItalic;         // only if DS_SETFONT flag is set
	//    WCHAR  font[fontLen];   // typeface name, if DS_SETFONT is set
} DLGTEMPLATEEX, *LPDLGTEMPLATEEX;
#pragma pack(pop)
	DWORD dwStyle;
	LPWORD lpDlgTmp;

	HINSTANCE hInst = ::AfxGetResourceHandle();
	HRSRC hResInfo = ::FindResource(hInst, MAKEINTRESOURCE(nIDTemplate), RT_DIALOG);
	HGLOBAL hRes = ::LoadResource(hInst, hResInfo);
	LPDLGTEMPLATEEX lpDlgTemplateEx = (LPDLGTEMPLATEEX)::LockResource(hRes);
	if (lpDlgTemplateEx->signature == 0xffff) {
		dwStyle = lpDlgTemplateEx->style;
		lpDlgTmp = (LPWORD)((char *)lpDlgTemplateEx + sizeof(DLGTEMPLATEEX));
	} else {
		LPDLGTEMPLATE lpDlgTemplate = (LPDLGTEMPLATE)lpDlgTemplateEx;
		dwStyle = lpDlgTemplate->style;
		lpDlgTmp = (LPWORD)((char *)lpDlgTemplate + sizeof(DLGTEMPLATE));
	}

	if (dwStyle & DS_SETFONT) {
		if (*lpDlgTmp == 0xffff)
			lpDlgTmp += 2;
		else {
			while (*lpDlgTmp++ != 0)
				;
		}

		if (*lpDlgTmp == 0xffff)
			lpDlgTmp += 2;
		else {
			while (*lpDlgTmp++ != 0)
				;
		}
		while (*lpDlgTmp++ != 0)
			;

		*lpDlgTmp = (WORD)nFontSize;
	}
}

void SasApp::OnOnlineManual()
{
	OpenURL(IDS_ONLINE_MANUAL_CMD);
}

void SasApp::OnTechnicalSupport()
{
	OpenURL(IDS_TECHNICAL_SUPPORT_CMD);
}

void SasApp::OnDocumentQa()
{
	OpenURL(IDS_DOCUMENT_QA_CMD);
}

void SasApp::OnReadme()
{
	CString sReadmeFile;

	sReadmeFile.Format("%s\\ReadMe.txt", (LPCSTR)GetExeDir());
	OpenURL(sReadmeFile);
}

void SasApp::OnYmecStore()
{
	OpenURL(IDS_YMEC_STORE);
}

void SasApp::OnLicenseRegist()
{
	REGIST_LICENSE();
}

void SasApp::OnUpdateLicenseRegist(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(CHECK_LICENSE2(APP_SA) != LICENSE_REGIST);
}

BOOL SasApp::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd != NULL && pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_F9) {
			ScreenShot(pMsg->hwnd);
		}
	}

	return CWinApp::PreTranslateMessage(pMsg);
}

void SasApp::RegistExportFileIcon()
{
	HKEY hKey;
	DWORD dwDisposition;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, "SoftWare\\Classes\\.dssf", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition) != ERROR_SUCCESS)
		return;

//	if (dwDisposition == REG_OPENED_EXISTING_KEY) {
//		CloseHandle(hKey);
//		return;
//	}

	if (RegSetValueEx(hKey, "", 0, REG_SZ, (BYTE *)"DSSF.File", 10) != ERROR_SUCCESS) {
		CloseHandle(hKey);
		return;
	}

	CloseHandle(hKey);

	if (RegCreateKeyEx(HKEY_CURRENT_USER, "SoftWare\\Classes\\DSSF.File", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition) != ERROR_SUCCESS)
		return;

	if (RegSetValueEx(hKey, "", 0, REG_SZ, (BYTE *)"DSSF3 Export file", 18) != ERROR_SUCCESS) {
		CloseHandle(hKey);
		return;
	}

	CloseHandle(hKey);

	CString sExeFileName;
	::GetModuleFileName(m_hInstance, sExeFileName.GetBuffer(MAX_PATH), MAX_PATH);
	sExeFileName.ReleaseBuffer();

	if (RegCreateKeyEx(HKEY_CURRENT_USER, "SoftWare\\Classes\\DSSF.File\\DefaultIcon", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition) != ERROR_SUCCESS) {
		CloseHandle(hKey);
		return;
	}

	if (RegSetValueEx(hKey, "", 0, REG_SZ, (BYTE *)(LPCSTR)(sExeFileName + ",1"), sExeFileName.GetLength() + 3) != ERROR_SUCCESS) {
		CloseHandle(hKey);
		return;
	}

	CloseHandle(hKey);
}
