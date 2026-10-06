// ScreenCopyBtn.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "ScreenCopyBtn.h"
#include "comres.h"

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

/////////////////////////////////////////////////////////////////////////////
// CScreenCopyBtn

CScreenCopyBtn::CScreenCopyBtn()
{
	m_bNoProcess = FALSE;
}

CScreenCopyBtn::~CScreenCopyBtn()
{
	if (!m_TempFile.IsEmpty())
		::DeleteFile(m_TempFile);
}

BEGIN_MESSAGE_MAP(CScreenCopyBtn, CButton)
	ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

CString CScreenCopyBtn::m_TempFile;

/////////////////////////////////////////////////////////////////////////////
// CScreenCopyBtn メッセージ ハンドラ

BOOL CScreenCopyBtn::OnClicked()
{
	if (m_bNoProcess)
		return FALSE;
	else {
		CString filter;
		CString title;

		filter.LoadString(IDS_IMAGE_FILE);
		filter += " (*.png)|*.png||";
		CFileDialog dlg(FALSE, "png", NULL, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, filter, this, 0);

		title.LoadString(IDS_SAVE_IMAGE_FILE);
		dlg.m_ofn.lpstrTitle = title;
		if (dlg.DoModal() == IDOK) {
			CWnd *pParentWnd = GetParent();
			pParentWnd->UpdateWindow();
			HDIB hDIB = CreateDIB(pParentWnd);
			MMPictureWriteFile(hDIB, (LPSTR)(LPCSTR)dlg.GetPathName());
			::GlobalFree(hDIB);
		}

		return TRUE;
	}
}

void CScreenCopyBtn::OnRButtonDown(UINT /*nFlags*/, CPoint /*point*/)
{
	COleDataSource *pods = new COleDataSource;
	DROPFILES *pDropFiles;
	HANDLE hGlobal;
	LPSTR pFileName;
	SYSTEMTIME st;
	char tempPath[MAX_PATH];

	if (!m_TempFile.IsEmpty())
		::DeleteFile(m_TempFile);

	::GetTempPath(sizeof(tempPath), tempPath);
	size_t len = strlen(tempPath);
	if (tempPath[len - 1] != '\\')
		strcat_s(tempPath, "\\");

	GetLocalTime(&st);
	m_TempFile.Format("%s%02d%02d%02d%02d%02d%02d.png", tempPath, st.wYear % 100, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	HDIB hDIB = CreateDIB(GetParent());
	MMPictureWriteFile(hDIB, (LPSTR)(LPCSTR)m_TempFile);
	::GlobalFree(hDIB);

	const int nLength = m_TempFile.GetLength();
	if ((hGlobal = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(DROPFILES) + nLength + 2)) != NULL) {
		pDropFiles = (DROPFILES *)::GlobalLock(hGlobal);
		pDropFiles->pFiles = sizeof(DROPFILES);
		pDropFiles->fWide = FALSE;
		pFileName = (char *)pDropFiles + sizeof(DROPFILES);
		strcpy_s(pFileName, nLength + 1, m_TempFile);
		pFileName[nLength + 1] = '\0';

		::GlobalUnlock(hGlobal);
	}

	pods->CacheGlobalData(CF_HDROP, hGlobal);
	pods->DoDragDrop(DROPEFFECT_COPY);

	delete pods;
}

HDIB CScreenCopyBtn::CreateDIB(CWnd *pWnd)
{
	CRect rect;
	pWnd->GetWindowRect(&rect);
	const int nWidth = rect.Width();
	const int nHeight = rect.Height();

	CDC *pDC = pWnd->GetWindowDC();
	CDC DCMem;
	DCMem.CreateCompatibleDC(pDC);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, nWidth, nHeight);

	CGdiObject *pOrg = DCMem.SelectObject(&bmp);
	DCMem.BitBlt(0, 0, nWidth, nHeight, pDC, 0, 0, SRCCOPY);
	DCMem.SelectObject(pOrg);
	pWnd->ReleaseDC(pDC);

	HDIB hDIB;
	MMPictureGetBitmap(&hDIB, bmp);

	return hDIB;
}
