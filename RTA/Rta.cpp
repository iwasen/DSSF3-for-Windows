// Rta.cpp : アプリケーション用クラスの定義を行います。
//

#include "stdafx.h"
#include "Rta.h"
#include "RtaDlg.h"
#include "ParentWnd.h"
#include "WaveInEx.h"
#include "WaveOutEx.h"
#include "UpdateAp.h"
#include "Mrnd.h"
#include "FileIO.h"
#include "AsioManager.h"
#include "WasapiManager.h"
#include "Regist.h"
#include "VerRes.h"

/////////////////////////////////////////////////////////////////////////////
// CRtaApp

BEGIN_MESSAGE_MAP(CRtaApp, CWinApp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRtaApp クラスの構築

CRtaApp::CRtaApp()
{
	m_nCheckData = GET_CHECK_DATA;

	if (GetCheckData2() == m_nCheckData)
		m_pRegistryName = g_sRegistryName;
}

/////////////////////////////////////////////////////////////////////////////
// 唯一の CRtaApp オブジェクト

CRtaApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRtaApp クラスの初期化

BOOL CRtaApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls{
		.dwSize = sizeof(InitCtrls),
		.dwICC = ICC_WIN95_CLASSES
	};
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	CDpi::InitDpi();

	g_nWaveInDevice = 0;
	g_nWaveOutDevice = 0;
	m_nPresetID = -1;

	BOOL bAutoRecFile = FALSE;
	CString sAutoRecFile;
	CString argv;
	for (int i = 1; i < __argc; i++) {
		argv = __argv[i];

		if (bAutoRecFile) {
			sAutoRecFile = argv;
			bAutoRecFile = FALSE;
		} else if (_strnicmp(argv, "/WD=", 4) == 0) {
			g_nWaveInDevice = argv[4] - '0';
			if (argv[5] == ',')
				g_nWaveOutDevice = argv[6] - '0';
			else
				g_nWaveOutDevice = g_nWaveInDevice;
		} else if (_strnicmp(argv, "/S=", 3) == 0) {
			m_nPresetID = atoi(argv.Mid(3));
		} else if (_stricmp(argv, "/A") == 0) {
			bAutoRecFile = TRUE;
		}
	}

	if (!sAutoRecFile.IsEmpty()) {
		CRtaDlg *pWndRta = (CRtaDlg *)CWnd::FindWindow(NULL, "Realtime Analyzer [RA]");
		if (pWndRta != NULL) {
			if (OpenClipboard(NULL)) {
				EmptyClipboard();

				DWORD dwCount = (DWORD)sAutoRecFile.GetLength() + 1;
				HGLOBAL hMem = ::GlobalAlloc(GHND | GMEM_SHARE , dwCount);

				PTSTR pMem = (PTSTR)::GlobalLock(hMem);
				lstrcpy(pMem , sAutoRecFile);
				::GlobalUnlock(hMem);

				SetClipboardData(CF_TEXT, hMem);
				CloseClipboard();

				pWndRta->SendMessage(WM_AUTO_REC_FILE);

				return FALSE;
			}
		}
	}

	g_hIcon = LoadIcon(IDR_MAINFRAME);
	CVerRes res;
	if (!CHECK_LICENSE0(APP_RAL, res.GetProductVersion() + "/RA"))
		return FALSE;

	// WASAPIに必要
	::CoInitialize(NULL);

	g_oPcserve.Initialize();

	init_rnd(123456789);

	CWaveInEx::Init();
	CWaveOutEx::Init();

	CWaveIn::m_b16BitOnly = g_oSetData2.bInput16BitOnly;
	CWaveOut::m_b16BitOnly = g_oSetData2.bOutput16BitOnly;

	g_cAsioManager.Init();
	g_cWasapiManager.Init();

	m_pMainWnd = new CParentWnd;
	m_pWndRta = new CRtaDlg;

	if (!sAutoRecFile.IsEmpty())
		m_pWndRta->OpenAutoStartFile(sAutoRecFile);

	char path[_MAX_PATH];
	GetModuleFileName(NULL, path, _MAX_PATH);
	CString sCommand;
	sCommand.Format("\"%s\" /A \"%%1\"", path);
	SetFileType(".wavdssf", "Dssf3.AutoRec", "Dssf3 Automatic Recorded file", sCommand);

	return TRUE;
}

int CRtaApp::ExitInstance()
{
	CWaveInEx::Exit();
	CWaveOutEx::Exit();

	g_cAsioManager.Exit();
	g_cWasapiManager.Exit();

	::CoUninitialize();

	return CWinApp::ExitInstance();
}

LRESULT CRtaApp::ProcessWndProcException(CException* e, const MSG* /*pMsg*/)
{
	e->ReportError(MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
	exit(1);
//	return CWinApp::ProcessWndProcException(e, pMsg);
}

void CRtaApp::OnOnlineUpdate()
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
	pOnlineUpdate(sOnlineUpdate, exeFileName, m_lpCmdLine, LoadIcon(IDR_MAINFRAME), "Realtime\nAnalyzer", g_sRegistryName);
}

void CRtaApp::CloseFuncWindow(CWnd *pWnd)
{
	((CRtaDlg *)m_pWndRta)->CloseRaWindow(pWnd);
}

BOOL CRtaApp::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd != NULL && pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_F9) {
			ScreenShot(pMsg->hwnd);
		}
	}

	return CWinApp::PreTranslateMessage(pMsg);
}
