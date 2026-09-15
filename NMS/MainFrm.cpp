// MainFrm.cpp : CMainFrame クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "Nms.h"
#include "MainFrm.h"
#include "SetData.h"
#include "Regist.h"
#include "VerRes.h"

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_CHECK_LICENSE1, OnGetCheckLicense1)
	ON_MESSAGE(WM_CHECK_LICENSE2, OnGetCheckLicense2)
	ON_MESSAGE(WM_REGIST_LICENSE, OnRegistLicense)
END_MESSAGE_MAP()

static const UINT indicators[] =
{
	ID_SEPARATOR,           // ステータス ライン インジケータ
	ID_INDICATOR_KANA,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame クラスの構築/消滅

CMainFrame::CMainFrame()
{
	m_nCheckData = GET_CHECK_DATA;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // 作成に失敗
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // 作成に失敗
	}

	// TODO: ツール バーをドッキング可能にしない場合は以下の３行を削除
	//       してください。
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (g_oSetData.MainWindow.width != 0 && g_oSetData.MainWindow.height != 0) {
		cs.x = g_oSetData.MainWindow.left;
		cs.y = g_oSetData.MainWindow.top;
		cs.cx = g_oSetData.MainWindow.width;
		cs.cy = g_oSetData.MainWindow.height;
	}

	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame メッセージ ハンドラ

void CMainFrame::OnClose()
{
	WINDOWPLACEMENT wp;

	GetWindowPlacement(&wp);
	g_oSetData.MainWindow.left = wp.rcNormalPosition.left;
	g_oSetData.MainWindow.top = wp.rcNormalPosition.top;
	g_oSetData.MainWindow.width = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
	g_oSetData.MainWindow.height = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;

	CFrameWnd::OnClose();
}

LRESULT CMainFrame::OnGetCheckLicense1(WPARAM wParam, LPARAM /*lParam*/)
{
	const char *pRegistryName;

	if (m_nCheckData == GetCheckData2())
		pRegistryName = g_sRegistryName;
	else
		pRegistryName = NULL;

	return CheckLicense(g_sSystemName, g_hIcon, pRegistryName, (int)wParam, FALSE, PRODUCT_CODE, NULL);
}

LRESULT CMainFrame::OnGetCheckLicense2(WPARAM wParam, LPARAM /*lParam*/)
{
	const char *pRegistryName;

	if (m_nCheckData == GetCheckData2())
		pRegistryName = g_sRegistryName;
	else
		pRegistryName = NULL;

	switch (CheckLicense3(pRegistryName, (int)wParam) - GET_CHECK_DATA) {
	case LICENSE_TRIAL:
		return LICENSE_TRIAL;
	case LICENSE_REGIST:
		return LICENSE_REGIST;
	}
	return 0;
}

LRESULT CMainFrame::OnRegistLicense(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	if (m_nCheckData == GetCheckData2()) {
		CVerRes res;
		return RegistLicense(g_sSystemName, g_sRegistryName, PRODUCT_CODE, res.GetProductVersion() + "/EA", APP_EA << 8);
	} else
		return 0;
}
