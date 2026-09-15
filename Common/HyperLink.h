#pragma once

class CHyperLink : public CStatic
{
public:
	CHyperLink();

	void SetHyperLinkURL(LPCTSTR pLinkURL, LPCTSTR pFontName = NULL, int nFontSize = 0);

protected:
	CString m_sLinkURL;
	CFont m_Font;
	CBrush m_brushBG;

	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnClicked();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};
