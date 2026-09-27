// HyperLink.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "HyperLink.h"
#include "comres.h"

/////////////////////////////////////////////////////////////////////////////
// CHyperLink

CHyperLink::CHyperLink()
{
	m_brushBG.CreateSysColorBrush(COLOR_3DFACE);
}

BEGIN_MESSAGE_MAP(CHyperLink, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHyperLink メッセージ ハンドラ

HBRUSH CHyperLink::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	pDC->SetTextColor(RGB(0, 0, 208));
	pDC->SetBkMode(TRANSPARENT);

	return m_brushBG;
}

void CHyperLink::OnClicked()
{
	if (!m_sLinkURL.IsEmpty())
	    ::ShellExecute(NULL, "open", m_sLinkURL, NULL, NULL, SW_SHOWNORMAL);
}

BOOL CHyperLink::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/)
{
    HCURSOR hCursor = ::AfxGetApp()->LoadCursor(IDC_FINGER);
    ::SetCursor(hCursor);
    return TRUE;
}

void CHyperLink::SetHyperLinkURL(LPCTSTR pLinkURL, LPCTSTR pFontName, int nFontSize)
{
	LOGFONT font{};

	if (pLinkURL == NULL)
		GetWindowText(m_sLinkURL);
	else
		m_sLinkURL = pLinkURL;

	if (pFontName == NULL) {
		CFont *pFont = GetFont();
		pFont->GetLogFont(&font);
		font.lfUnderline = 1;
		m_Font.CreateFontIndirect(&font);
	} else {
		font.lfHeight = nFontSize * 10;
		font.lfUnderline = 1;
		font.lfCharSet = DEFAULT_CHARSET;
		strcpy_s(font.lfFaceName, pFontName);
		m_Font.CreatePointFontIndirect(&font, NULL);
	}

	SetFont(&m_Font);
}
