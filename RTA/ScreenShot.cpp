// ScreenShot.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Rta.h"
#include "ScreenShot.h"

// CScreenShot

void CScreenShot::ShowScreenShot(LPCSTR pTitle, HBITMAP hBmp, BOOL bCopy)
{
	CScreenShotWnd *pWnd = new CScreenShotWnd;

	m_ItemList.AddTail(pWnd);

	if (bCopy) {
		HDC hDC = ::GetDC(NULL);
		HDC hDC1 = ::CreateCompatibleDC(hDC);
		HDC hDC2 = ::CreateCompatibleDC(hDC);
		BITMAP bmp;
		::GetObject(hBmp, sizeof(bmp) , &bmp);
		HBITMAP hBmp2 = ::CreateCompatibleBitmap(hDC, bmp.bmWidth, bmp.bmHeight);

		HBITMAP hBitmapOrg1 = (HBITMAP)::SelectObject(hDC1, hBmp);
		HBITMAP hBitmapOrg2 = (HBITMAP)::SelectObject(hDC2, hBmp2);
		::BitBlt(hDC2, 0, 0, bmp.bmWidth, bmp.bmHeight, hDC1, 0, 0, SRCCOPY);
		::SelectObject(hDC1, hBitmapOrg1);
		::SelectObject(hDC2, hBitmapOrg2);

		::ReleaseDC(NULL, hDC);
		::DeleteDC(hDC1);
		::DeleteDC(hDC2);

		hBmp = hBmp2;
	}

	pWnd->ShowBitmap(pTitle, hBmp, this);
}

void CScreenShot::CloseScreenShot(CScreenShotWnd *pWnd)
{
	POSITION pos;

	if ((pos = m_ItemList.Find(pWnd)) != NULL)
		m_ItemList.RemoveAt(pos);

	delete pWnd;
}

CScreenShot::~CScreenShot()
{
	CWnd *pWnd;
	while (!m_ItemList.IsEmpty()) {
		pWnd = (CWnd *)m_ItemList.GetHead();
		pWnd->DestroyWindow();
	}
}

void CScreenShot::OutputImageFile(CWnd *pWnd, CBitmap &bitmap)
{
	CFileDialog dlg(FALSE, "png", "", OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, "PNG|*.png||", pWnd);

	//dlg.m_ofn.lpstrTitle = GetString(IDS_EXPORT_FILE);
	if (dlg.DoModal() == IDOK) {
		HANDLE hDIB;
		MMPictureGetBitmap(&hDIB, (HBITMAP)bitmap.m_hObject);
		MMPictureWriteFile(hDIB, (LPSTR)(LPCSTR)dlg.GetPathName());
	}
}

// CScreenShotWnd

BEGIN_MESSAGE_MAP(CScreenShotWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDM_POPUP_FILE_SAVE, &CScreenShotWnd::OnPopupFileSave)
END_MESSAGE_MAP()

// CScreenShotWnd メッセージ ハンドラ

void CScreenShotWnd::ShowBitmap(LPCSTR pTitle, HBITMAP hBmp, CScreenShot *pScreenShot)
{
	const DWORD dwStyle = WS_TILED | WS_VISIBLE | WS_CAPTION | WS_SYSMENU;
	const DWORD dwExStyle = 0;
	BITMAP bm;

	m_pScreenShot = pScreenShot;

	m_bitmap.Attach(hBmp);
	m_bitmap.GetBitmap(&bm);
	m_nWidth = bm.bmWidth;
	m_nHeight = bm.bmHeight;
	CRect rect(0, 0, bm.bmWidth, bm.bmHeight);
	AdjustWindowRectEx(rect, dwStyle, FALSE, dwExStyle);

	CreateEx(dwExStyle, ::AfxRegisterWndClass(0, ::AfxGetApp()->LoadStandardCursor(IDC_ARROW), 0, ::AfxGetApp()->LoadIcon(IDI_SCREEN_SHOT)),
				pTitle, dwStyle,
				CW_USEDEFAULT, CW_USEDEFAULT, rect.Width(), rect.Height(),
				::AfxGetMainWnd()->m_hWnd, 0);
}

void CScreenShotWnd::OnPaint()
{
	CPaintDC dc(this);

	CDC dcSrc;
	dcSrc.CreateCompatibleDC(&dc);
	CBitmap *pBitmapOrg = dcSrc.SelectObject(&m_bitmap);
	dc.BitBlt(0, 0, m_nWidth, m_nHeight, &dcSrc, 0, 0, SRCCOPY);
	dcSrc.SelectObject(pBitmapOrg);
}

void CScreenShotWnd::PostNcDestroy()
{
	CWnd::PostNcDestroy();

	m_pScreenShot->CloseScreenShot(this);
}

void CScreenShotWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_SCREEN_SHOT_MENU);
	CMenu* pPopup = menu.GetSubMenu(0);
	ClientToScreen(&point);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);

	CWnd::OnRButtonDown(nFlags, point);
}

void CScreenShotWnd::OnPopupFileSave()
{
	CScreenShot::OutputImageFile(this, m_bitmap);
}
