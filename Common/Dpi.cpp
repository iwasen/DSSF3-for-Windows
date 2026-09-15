// DialogAF.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Dpi.h"

int CDpi::m_nDpi;

void CDpi::InitDpi()
{
	typedef int (__stdcall *GetDpiForSystemFunc)();

	HINSTANCE hModule;
	m_nDpi = 96;
	if ((hModule = ::LoadLibrary("User32.dll")) != NULL) {
		GetDpiForSystemFunc pGetDpiForSystem;
		if ((pGetDpiForSystem = (GetDpiForSystemFunc)::GetProcAddress(hModule, "GetDpiForSystem")) != NULL) {
			m_nDpi = pGetDpiForSystem();
		}
		::FreeLibrary(hModule);
	}
}

int CDpi::GetDpi()
{
	return m_nDpi;
}

int CDpi::AdjustDpi(int n, int nAdjust)
{
	return m_nDpi == 96 ? n : n * m_nDpi * nAdjust / (96 * 100);
}

int CDpi::OriginalDpi(int n, int nAdjust)
{
	return m_nDpi == 96 ? n : n * (96 * 100) / (m_nDpi * nAdjust);
}

// CDialogAF ダイアログ

IMPLEMENT_DYNAMIC(CDialogAF, CDialog)

CDialogAF::CDialogAF()
{
}

CDialogAF::CDialogAF(UINT nID, CWnd* pParent /*=NULL*/)
	: CDialog(nID, pParent)
{

}

void CDialogAF::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogAF, CDialog)
END_MESSAGE_MAP()

// CDialogAF メッセージ ハンドラ

BOOL CDialogAF::OnInitDialog()
{
	CDialog::OnInitDialog();

#ifdef _LANG_JPN
	if (CDpi::GetDpi() > 96) {
		CFont *pFont = GetFont();
		LOGFONT lf;
		pFont->GetLogFont(&lf);

		if (lf.lfHeight < 0) {
			// フォントの作成
			lf.lfHeight += 1;
			m_oFont.CreateFontIndirect(&lf);

			// ダイアログ自身およびすべての子コントロールにフォントを設定
//			SendMessageToDescendants(WM_SETFONT, (WPARAM)(HFONT)m_oFont.GetSafeHandle(), MAKELPARAM(0, 0), FALSE, TRUE);
			CWnd* pChild = GetWindow(GW_CHILD);
			while (pChild)
			{
				pChild->SetFont(&m_oFont, FALSE);
				pChild = pChild->GetWindow(GW_HWNDNEXT);
			}
			SetFont(&m_oFont, FALSE); // ダイアログ自身にも設定
		}
	}
#endif

	return TRUE;
}
