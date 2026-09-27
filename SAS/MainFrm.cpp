// MainFrm.cpp : CMainFrame クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "Sas.h"
#include "MainFrm.h"
#include "LeftView.h"
#include "SasView.h"
#include "ImpFactor.h"
#include "AcfFactor.h"
#include "NmsFactor.h"
#include "Regist.h"
#include "VerRes.h"

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnUpdateViewStyles)
	ON_COMMAND_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnViewStyle)
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

BOOL CMainFrame::OnCreateClient( LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	// 分割ウィンドウを作成します
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(setData.MainWindow.width2, 0), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CSasView), CSize(0, 0), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (setData.MainWindow.width != 0 && setData.MainWindow.height != 0) {
		cs.x = setData.MainWindow.left;
		cs.y = setData.MainWindow.top;
		cs.cx = setData.MainWindow.width;
		cs.cy = setData.MainWindow.height;
	}

	cs.style &= ~FWS_ADDTOTITLE;

	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame メッセージ ハンドラ

CSasView* CMainFrame::GetRightPane() const
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
	CSasView* pView = DYNAMIC_DOWNCAST(CSasView, pWnd);
	return pView;
}

void CMainFrame::OnUpdateViewStyles(CCmdUI* pCmdUI)
{
	CSasView* pView = GetRightPane();

	if (pView == NULL)
		pCmdUI->Enable(FALSE);
	else
	{
		DWORD dwStyle = pView->GetStyle() & LVS_TYPEMASK;

		pCmdUI->Enable();
		BOOL bChecked = FALSE;

		switch (pCmdUI->m_nID)
		{
		case ID_VIEW_DETAILS:
			bChecked = (dwStyle == LVS_REPORT);
			break;
		case ID_VIEW_LIST:
			bChecked = (dwStyle == LVS_LIST);
			break;
		default:
			bChecked = FALSE;
			break;
		}

		pCmdUI->SetRadio(bChecked ? 1 : 0);
	}
}

void CMainFrame::OnViewStyle(UINT nCommandID)
{
	CSasView* pView = GetRightPane();

	if (pView != NULL) {
		DWORD dwStyle = (DWORD)-1;

		switch (nCommandID) {
		case ID_VIEW_DETAILS:
			dwStyle = LVS_REPORT;
			break;
		case ID_VIEW_LIST:
			dwStyle = LVS_LIST;
			break;
		}

		if (dwStyle != -1)
			pView->ModifyStyle(LVS_TYPEMASK, dwStyle);
	}
}

void CMainFrame::OnClose()
{
	CImpFactor::CloseAll();
	CAcfFactor::CloseAll();
	CNmsFactor::CloseAll();

	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	setData.MainWindow.left = wp.rcNormalPosition.left;
	setData.MainWindow.top = wp.rcNormalPosition.top;
	setData.MainWindow.width = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
	setData.MainWindow.height = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;

	int cxCur, cxMin;
	m_wndSplitter.GetColumnInfo(0, cxCur, cxMin);
	setData.MainWindow.width2 = cxCur;

	CFrameWnd::OnClose();
}

CView *CMainFrame::GetTreeView() const
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 0);
	CView* pView = DYNAMIC_DOWNCAST(CView, pWnd);
	return pView;
}

CView *CMainFrame::GetListView() const
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
	CView* pView = DYNAMIC_DOWNCAST(CView, pWnd);
	return pView;
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
		return RegistLicense(g_sSystemName, g_sRegistryName, PRODUCT_CODE, res.GetProductVersion() + "/SA", APP_SA << 8);
	} else
		return 0;
}
