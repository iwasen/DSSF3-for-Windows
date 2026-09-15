// Nms.cpp : アプリケーション用クラスの機能定義を行います。
//

#include "stdafx.h"
#include "Nms.h"
#include "MainFrm.h"
#include "NmsDoc.h"
#include "NmsView.h"
#include "UpdateAp.h"
#include "SplWnd.h"
#include "PeakLevelWnd.h"
#include "NmsFactor.h"
#include "VerRes.h"
#include "HyperLink.h"
#include "SetData.h"
#include "Regist.h"
#include "Directory.h"

/////////////////////////////////////////////////////////////////////////////
// アプリケーションのバージョン情報で使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogAF
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

public:
	CHyperLink m_cProductURL;
	CHyperLink m_cYmecStoreURL;
	CHyperLink m_cCompanyURL;
	CHyperLink m_cEmailAddr;

protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV のサポート
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
	sVersion += "d";
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
// CNmsApp

BEGIN_MESSAGE_MAP(CNmsApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(IDM_ONLINE_UPDATE, OnOnlineUpdate)
	// 標準のファイル基本ドキュメント コマンド
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// 標準の印刷セットアップ コマンド
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(IDM_ONLINE_MANUAL, OnOnlineManual)
	ON_COMMAND(IDM_TECHNICAL_SUPPORT, OnTechnicalSupport)
	ON_COMMAND(IDM_DOCUMENT_QA, OnDocumentQa)
	ON_COMMAND(IDM_README, OnReadme)
	ON_COMMAND(IDM_YMEC_STORE, OnYmecStore)
	ON_COMMAND(IDM_LICENSE_REGIST, OnLicenseRegist)
	ON_UPDATE_COMMAND_UI(IDM_LICENSE_REGIST, OnUpdateLicenseRegist)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNmsApp クラスの構築

CNmsApp::CNmsApp()
{
	m_nCheckData = GET_CHECK_DATA;
	if (GetCheckData2() == m_nCheckData)
		m_pRegistryName = g_sRegistryName;
}

/////////////////////////////////////////////////////////////////////////////
// 唯一の CNmsApp オブジェクト

CNmsApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CNmsApp クラスの初期化

BOOL CNmsApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	AfxEnableControlContainer();

	// 標準的な初期化処理
	// もしこれらの機能を使用せず、実行ファイルのサイズを小さく
	// したければ以下の特定の初期化ルーチンの中から不必要なもの
	// を削除してください。

	CDpi::InitDpi();

	// 設定が保存される下のレジストリ キーを変更します。
	// TODO: この文字列を、会社名または所属など適切なものに
	// 変更してください。
//	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings(0);  // 標準の INI ファイルのオプションをローﾄﾞします (MRU を含む)

	g_oPcserve.Initialize();

//	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	::GetTempPath(MAX_PATH, g_sTempDir.GetBuffer(MAX_PATH));
	g_sTempDir.ReleaseBuffer();
	g_sTempDir += "\\NMS";
	DeleteDirectoryAll(g_sTempDir);
	CreateDirectoryAll(g_sTempDir);

	g_hIcon = LoadIcon(IDR_MAINFRAME);
	CVerRes res;
	if (!CHECK_LICENSE0(APP_EA, res.GetProductVersion() + "/EA"))
		return FALSE;

	// アプリケーション用のドキュメント テンプレートを登録します。ドキュメント テンプレート
	//  はドキュメント、フレーム ウィンドウとビューを結合するために機能します。

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CNmsDoc),
		RUNTIME_CLASS(CMainFrame),       // メイン SDI フレーム ウィンドウ
		RUNTIME_CLASS(CNmsView));
	AddDocTemplate(pDocTemplate);

	// DDE、file open など標準のシェル コマンドのコマンドラインを解析します。
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// コマンドラインでディスパッチ コマンドを指定します。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// メイン ウィンドウが初期化されたので、表示と更新を行います。
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

// ダイアログを実行するためのアプリケーション コマンド
void CNmsApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CNmsApp メッセージ ハンドラ

int CNmsApp::ExitInstance()
{
	g_oSetData.SplWindow.bShow = (g_pSplWnd != NULL) ? TRUE : FALSE;
	if (g_pSplWnd != NULL)
		g_pSplWnd->DestroyWindow();

	g_oSetData.PeakLevelWindow.bShow = (g_pPeakLevelWnd != NULL) ? TRUE : FALSE;
	if (g_pPeakLevelWnd != NULL)
		g_pPeakLevelWnd->DestroyWindow();

	CNmsFactor::CloseFactorWnd();

	if (!g_sTempDir.IsEmpty())
		DeleteDirectoryAll(g_sTempDir);

	return CWinApp::ExitInstance();
}

void CNmsApp::OnOnlineUpdate()
{
	OnlineUpdateFunc pOnlineUpdate;
	HINSTANCE hModule;
	CString exeFileName;

#ifdef _DEBUG
	if ((hModule = ::LoadLibrary("YMUPDATED.DLL")) == NULL) {
#else
	if ((hModule = ::LoadLibrary("YMUPDATE.DLL")) == NULL) {
#endif
		::AfxMessageBox(IDS_ERR_YMUPDATE);
		return;
	}

	if ((pOnlineUpdate = (OnlineUpdateFunc)::GetProcAddress(hModule, "OnlineUpdate")) == NULL) {
		::AfxMessageBox(IDS_ERR_ONLINEUPDATE);
		::FreeLibrary(hModule);
		return;
	}

	::GetModuleFileName(m_hInstance, exeFileName.GetBuffer(MAX_PATH), MAX_PATH);
	exeFileName.ReleaseBuffer();

	CString sOnlineUpdate;
	sOnlineUpdate.LoadString(IDS_ONLINEUPDATE);
#ifdef _WIN64
	sOnlineUpdate += "64";
#else
	sOnlineUpdate += "32";
#endif
	pOnlineUpdate(sOnlineUpdate, exeFileName, m_lpCmdLine, LoadIcon(IDR_MAINFRAME), "Noise\nAnalyzer", g_sRegistryName);
}

void CNmsApp::OnOnlineManual()
{
	OpenURL(IDS_ONLINE_MANUAL_CMD);
}

void CNmsApp::OnTechnicalSupport()
{
	OpenURL(IDS_TECHNICAL_SUPPORT_CMD);
}

void CNmsApp::OnDocumentQa()
{
	OpenURL(IDS_DOCUMENT_QA_CMD);
}

void CNmsApp::OnReadme()
{
	CString sReadmeFile;

	sReadmeFile.Format("%s\\ReadMe.txt", (LPCSTR)GetExeDir());
	OpenURL(sReadmeFile);
}

void CNmsApp::OnYmecStore()
{
	OpenURL(IDS_YMEC_STORE);
}

void CNmsApp::OnLicenseRegist()
{
	REGIST_LICENSE();
}

void CNmsApp::OnUpdateLicenseRegist(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(CHECK_LICENSE2(APP_EA) != LICENSE_REGIST);
}

BOOL CNmsApp::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd != NULL && pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_F9) {
			ScreenShot(pMsg->hwnd);
		}
	}

	return CWinApp::PreTranslateMessage(pMsg);
}
