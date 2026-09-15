// ParentWnd.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "ParentWnd.h"
#include "Common.h"
#include "Regist.h"
#include "VerRes.h"

/////////////////////////////////////////////////////////////////////////////
// CParentWnd

CParentWnd::CParentWnd()
{
	m_nCheckData = GET_CHECK_DATA;

	CreateEx(0, ::AfxRegisterWndClass(0, 0, 0, ::AfxGetApp()->LoadIcon(IDR_MAINFRAME)), "Realtime Analyzer", WS_POPUP | WS_VISIBLE | WS_SYSMENU, 0, 0, 0, 0, NULL, NULL);
}

BEGIN_MESSAGE_MAP(CParentWnd, CWnd)
	ON_MESSAGE(WM_CHECK_LICENSE1, OnGetCheckLicense1)
	ON_MESSAGE(WM_CHECK_LICENSE2, OnGetCheckLicense2)
	ON_MESSAGE(WM_REGIST_LICENSE, OnRegistLicense)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParentWnd メッセージ ハンドラ

void CParentWnd::PostNcDestroy()
{
	CWnd::PostNcDestroy();

	delete this;
}

LRESULT CParentWnd::OnGetCheckLicense1(WPARAM wParam, LPARAM /*lParam*/)
{
	const char *pRegistryName;

	if (m_nCheckData == GetCheckData2())
		pRegistryName = g_sRegistryName;
	else
		pRegistryName = NULL;

	return CheckLicense(g_sSystemName, g_hIcon, pRegistryName, (int)wParam, FALSE, PRODUCT_CODE, NULL);
}

LRESULT CParentWnd::OnGetCheckLicense2(WPARAM wParam, LPARAM /*lParam*/)
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

LRESULT CParentWnd::OnRegistLicense(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	if (m_nCheckData == GetCheckData2()) {
		CVerRes res;
		return RegistLicense(g_sSystemName, g_sRegistryName, PRODUCT_CODE, res.GetProductVersion() + "/RA", APP_RA << 8);
	} else
		return 0;
}
